#!/usr/bin/env python3
"""S4 — **릴리스 판정 루프**의 닫힌 고리 재생 + CLI (탐색용 시제).

    python3 release_sim.py --block-robot r0 --block-at 4 --block-seconds 45 --drop-entry-nth 6

S3(`exec_sim.py`)는 max-plus로 **시각을 한 번에 풀었다.** S4는 그러지 않는다 —
**틱마다 릴리스 판정을 돌리고, 릴리스된 세그먼트만 물리 계층이 시작한다.** 즉 이제
*"세그먼트를 언제 놓아 주는가"*가 코드의 결정이지 방정식의 해가 아니다.

🔴 못 박는 것 넷.

1. **타이밍은 우리가 지어낸 것이다.** 공칭 속도 × 간선 길이. 사전등록 상수가 아니다.
2. **재선택(스위칭)이 없다.** 순서는 고정이며 T1 제안을 만드는 선택기가 **존재하지 않는다.**
3. **계측이 아니다.** 링버퍼·분위수·예산 측정이 없다. 여기 나오는 시각은 **시뮬 시각**이다.
4. **theta_esc가 값 부재**이므로 승급 트리거가 없다. `D_r`은 **보고만** 된다.
"""

from __future__ import annotations

import argparse
import json
import os
from dataclasses import dataclass, field

from boundary_adapter import constraints_from_boundary, roadmap_from_boundary
from exec_sim import Blockage, segment_durations
from protoscale import load_scale
from sadg_core import ExecGraph, SegKey, nominal_edges
from tick_core import (
    DELTA_T_T0_S,
    CompletionEvent,
    EntryEvent,
    ExecutionState,
    TickInput,
    cross_check_pend,
    invariant_check,
    make_state,
    run_tick,
    take_republish,
)

HERE = os.path.dirname(os.path.abspath(__file__))

# 🔴 **시제용 임시값 — 사전등록 상수가 아니다.** 정점 점유 영역 N(v)의 기하(cl(v))가 값 부재이므로
# 「세그먼트를 얼마나 지났을 때 그 정점에 진입한 것으로 보는가」를 비율로 근사했다.
# 계약 §1-3은 «진입 판정은 저수준의 것이고 실행 계층은 결과를 받는다»이므로 이 값은 저수준 대역이다.
ENTRY_FRACTION = 0.8


@dataclass
class RobotPhysical:
    robot: str
    at_node: str
    segment: SegKey | None = None  # 지금 지나고 있는 세그먼트
    started_s: float = 0.0
    duration_s: float = 0.0
    entry_sent: bool = False
    seq: int = 0
    finished: bool = False


@dataclass
class Frame:
    t_s: float
    tick_seq: int
    positions: dict[str, tuple[float, float, str]]
    released: list[SegKey]
    waiting: dict[str, tuple[SegKey, SegKey]]  # 로봇 -> (프런티어, 기다리는 선행)
    deficits: dict[str, float]


@dataclass
class LoopResult:
    frames: list[Frame] = field(default_factory=list)
    makespan_s: float = 0.0
    ticks: int = 0
    state: ExecutionState | None = None
    problems: list[str] = field(default_factory=list)
    vertex_conflicts: list = field(default_factory=list)
    republish_log: list[tuple[int, str]] = field(default_factory=list)
    dropped_entries: list[tuple[str, int]] = field(default_factory=list)


def nominal_schedule(graph: ExecGraph, durations: dict[SegKey, float]) -> dict[SegKey, float]:
    """공칭 완료 시각 tau^nom — `D_r`의 유일한 입력이다. **경계에 나가지 않는다.**"""
    from sadg_core import _kahn

    nodes = [s.key for s in graph.segments]
    edges = nominal_edges(graph)
    ok, order = _kahn(nodes, edges)
    if not ok:
        raise ValueError("공칭 그래프가 순환한다 — 공칭 스케줄이 정의되지 않는다")
    preds: dict[SegKey, list[SegKey]] = {n: [] for n in nodes}
    for e in edges:
        preds[e.dst].append(e.src)
    tau: dict[SegKey, float] = {}
    for n in order:
        tau[n] = max((tau[p] for p in preds[n]), default=0.0) + durations[n]
    return tau


def run_loop(
    graph: ExecGraph,
    model,
    nominal_speed_mps: float,
    blockages: list[Blockage] | None = None,
    drop_entry_nth: int | None = None,
    max_ticks: int = 20000,
    cross_check_every: int = 20,
    entry_fraction: float = ENTRY_FRACTION,
) -> LoopResult:
    """🔴 **닫힌 고리다** — 릴리스 판정이 물리 계층을 움직이고 물리 계층이 사건을 되돌려 준다."""
    nodes = model.node_map()
    durations = segment_durations(graph, model, nominal_speed_mps)
    extra: dict[SegKey, float] = {s.key: 0.0 for s in graph.segments}
    for b in blockages or []:
        if (b.robot, b.segment_number) in extra:
            extra[(b.robot, b.segment_number)] += b.seconds

    state = make_state(graph, nominal_schedule(graph, durations))
    take_republish(state)  # 초기 컴파일분을 소비한다 (틱 밖)

    segment_by_key = {s.key: s for s in graph.segments}
    robots = sorted({s.robot for s in graph.segments})
    phys = {
        r: RobotPhysical(r, segment_by_key[(r, 0)].src_node)
        for r in robots
        if (r, 0) in segment_by_key
    }
    result = LoopResult(state=state)
    entry_counter = 0

    for tick_seq in range(max_ticks):
        now = tick_seq * DELTA_T_T0_S
        entries: list[EntryEvent] = []
        completions: list[CompletionEvent] = []

        # ---- 물리 계층이 낸 사건을 모은다 (T0가 관측하는 것은 이것뿐이다) ----
        for p in phys.values():
            if p.segment is None:
                continue
            elapsed = now - p.started_s
            if not p.entry_sent and elapsed >= p.duration_s * entry_fraction:
                p.entry_sent = True
                entry_counter += 1
                if drop_entry_nth is not None and entry_counter == drop_entry_nth:
                    result.dropped_entries.append((p.robot, p.seq))  # 🔴 유실 주입
                else:
                    entries.append(EntryEvent(p.robot, p.segment, p.seq))
                p.seq += 1
            if elapsed >= p.duration_s:
                completions.append(CompletionEvent(p.segment))
                p.at_node = segment_by_key[p.segment].dst_node
                p.segment = None
                p.entry_sent = False

        out = run_tick(state, TickInput(tick_seq, now, entries, completions))
        cause = take_republish(state)  # 🔴 틱 밖. 산출은 어댑터의 몫이고 여기서 만들지 않는다
        if cause:
            result.republish_log.append((tick_seq, cause))

        # ---- 릴리스된 세그먼트만 시작한다 ----
        for key in out.released:
            p = phys.get(key[0])
            if p is None or p.segment is not None:
                continue
            p.segment = key
            p.started_s = now
            p.duration_s = durations[key] + extra[key]

        # ---- 프레임 기록 ----
        positions = {}
        for r in robots:
            p = phys.get(r)
            if p is None:
                continue
            if p.segment is None:
                n = nodes[p.at_node]
                mode = "done" if state.frontier[r] is None else "wait"
                positions[r] = (n.x_m, n.y_m, mode)
            else:
                seg = segment_by_key[p.segment]
                a, b = nodes[seg.src_node], nodes[seg.dst_node]
                span = p.duration_s or 1.0
                ratio = min(1.0, (now - p.started_s) / span)
                mode = "block" if extra[p.segment] > 0 else "move"
                positions[r] = (a.x_m + (b.x_m - a.x_m) * ratio,
                                a.y_m + (b.y_m - a.y_m) * ratio, mode)
        waiting = {
            s[0]: (s, out.blocked_reason[s]) for s in out.blocked_reason
        }
        result.frames.append(Frame(now, tick_seq, positions, list(out.released), waiting,
                                   dict(out.deficits)))

        # ---- 자체 점검 (판정이 아니다) ----
        if cross_check_every and tick_seq % cross_check_every == 0:
            result.problems.extend(cross_check_pend(state))
        result.problems.extend(invariant_check(state))
        here: dict[str, str] = {}
        for r, (x, y, mode) in positions.items():
            if mode in ("move", "block"):
                continue
            node = phys[r].at_node
            if node in here:
                result.vertex_conflicts.append((round(now, 3), node, here[node], r))
            else:
                here[node] = r

        if all(state.frontier[r] is None for r in robots):
            result.makespan_s = now
            result.ticks = tick_seq + 1
            break
    else:
        result.problems.append(f"틱 상한 {max_ticks}에 도달했다 — 완료하지 못했다")
        result.ticks = max_ticks
    result.problems = sorted(set(result.problems))
    return result


def load_bundle(roadmap_path: str, constraints_path: str):
    with open(roadmap_path, "r", encoding="utf-8") as handle:
        model = roadmap_from_boundary(json.load(handle))
    with open(constraints_path, "r", encoding="utf-8") as handle:
        graph = constraints_from_boundary(json.load(handle))
    return model, graph


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="S4 릴리스 판정 루프 (탐색용 시제)")
    ap.add_argument("--roadmap", default=os.path.join(HERE, "out", "roadmap.json"))
    ap.add_argument("--constraints", default=os.path.join(HERE, "out", "execution_constraints.json"))
    ap.add_argument("--block-robot", default=None)
    ap.add_argument("--block-at", type=int, default=0)
    ap.add_argument("--block-seconds", type=float, default=45.0)
    ap.add_argument("--drop-entry-nth", type=int, default=None,
                    help="🔴 n번째 진입 사건을 유실시킨다 — D7(진입 간주) 경로를 밟는다")
    ap.add_argument("--entry-fraction", type=float, default=ENTRY_FRACTION,
                    help="세그먼트의 몇 %를 지났을 때 진입 사건을 내는가. 🔴 사전등록 상수 아님")
    ap.add_argument("--scale", default=None)
    args = ap.parse_args(argv)

    cfg = load_scale(args.scale)
    model, graph = load_bundle(args.roadmap, args.constraints)
    blockages = (
        [Blockage(args.block_robot, args.block_at, args.block_seconds)] if args.block_robot else []
    )
    res = run_loop(graph, model, cfg.nominal_speed_mps, blockages,
                   drop_entry_nth=args.drop_entry_nth, entry_fraction=args.entry_fraction)
    st = res.state

    print(f"틱 주기            : {DELTA_T_T0_S * 1000:.0f} ms  [원문 11c_p2 §5-2] 🔴 동결 아님")
    print(f"세그먼트 · 로봇    : {len(graph.segments)}개 · {len({s.robot for s in graph.segments})}대")
    print(f"스위치 그룹        : {len(graph.groups)}개 (0개면 순서가 전부 고정이다)")
    print(f"완료               : {res.ticks}틱 · 시뮬 {res.makespan_s:.2f} s  🔴 지어낸 타이밍")
    if blockages:
        b = blockages[0]
        print(f"차단 주입          : {b.robot} 세그먼트 #{b.segment_number} 위에서 {b.seconds:.0f} s")
    if res.dropped_entries:
        print(f"🔴 진입 사건 유실  : {res.dropped_entries} -> D7(진입 간주) 경로")
    print(f"진입 사건 유실 판정: {st.counters.get('entry_event_gap_count', 0)}건")
    print(f"중복 진입/완료     : {st.counters.get('duplicate_entry_events', 0)} / "
          f"{st.counters.get('duplicate_completions', 0)}건 (멱등이어야 한다)")
    print(f"commit_seq         : {st.commit_seq} · commit_epoch: {st.commit_epoch}")
    print(f"재발행 발화        : {len(res.republish_log)}회 {res.republish_log[:6]}")
    print(f"게이트 폐기        : "
          f"{ {k: v for k, v in st.counters.items() if k.startswith('stale_discards')} }")
    print(f"교차검증·불변식 위반: {len(res.problems)}건 (증분 대 전체 재계산 · 자체 점검)")
    for p in res.problems[:5]:
        print(f"   - {p}")
    print(f"정점 점유 충돌     : {len(res.vertex_conflicts)}건 (자체 점검이며 판정이 아니다)")
    if res.vertex_conflicts:
        print(f"   예: {res.vertex_conflicts[:3]}")
    late = sorted(res.frames[-1].deficits.items(), key=lambda kv: -kv[1])[:3]
    print(f"D_r 최대 3 (마지막 틱): {[(r, round(v, 2)) for r, v in late]}  🔴 theta_esc 값 부재 — 트리거 없음")
    return 0 if not res.problems and not res.vertex_conflicts else 2


if __name__ == "__main__":
    raise SystemExit(main())
