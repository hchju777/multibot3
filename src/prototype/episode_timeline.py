#!/usr/bin/env python3
"""M2 — 에피소드 시계열: 완료 사건마다 **그 시점**의 재순서 여지를 잰다 (탐색용 시제).

`_workspace/60_session9_decisions.md` §60-1의 수치(평균 2.00조합 · 접기 후 0.35그룹)는
전부 **시각 0의 컴파일 1회**다. 이 스크립트는 릴리스 재생(`tick_core.run_tick` +
`exec_sim.segment_durations`)을 **직접 돌리며**(기존 `release_sim.run_loop`를 고치지 않고
같은 재료로 새로 짠 루프다 — 계측 훅이 없어서 재사용할 수 없었다), 각 **완료 사건**
처리 직후 두 가지를 계산해 기록한다:

  - `uncommitted_groups`/`acyclic_combinations` : **고정 컴파일**(t=0에 한 번 컴파일한 그래프)
    안에서 미커밋 그룹만 자유롭게 뒀을 때 전수 비순환 조합 수. M2(1차)의 정의 그대로다.
  - `recompiled_groups`/`recompiled_acyclic_combinations` : 🔴 **M2′ 추가(§65-8)** — 완료된
    접두를 뗀 **잔여 계획**(각 로봇의 현재 위치를 새 시작으로)으로 `compile_graph`를 **다시**
    돌린 그래프(가지치기도 정본과 동일하게 적용)에서 낸 같은 값. 「그 시점에서 다시 컴파일하면
    방이 열리는가」(`59` §59-1-6의 교란 요인 C2)를 직접 겨눈다 — 고정 컴파일은 커밋이 소비만
    하므로 거의 구조적으로 비증가이고, C2가 진짜 묻는 것은 재컴파일 쪽이다.

경계 아티팩트(로드맵·이산 계획)와 실행 상태(`tick_core.ExecutionState`)만 읽는다.
스키마·검사기·`sadg_core.py`·`tick_core.py`·`release_sim.py`를 고치지 않는다.

🔴 시각은 지어낸 것이다(공칭 속도 x 간선 길이). 어떤 값도 사전등록 상수가 아니다.
출력은 `stats/`에 둔다 — 경계 디렉터리(`out/`)가 아니다.
"""

from __future__ import annotations

import argparse
import json
import os
import statistics
from dataclasses import dataclass, field
from itertools import product

from assignment_core import AssignParams, build_assignment
from exec_sim import Blockage, segment_durations
from plan_core import PlanParams, plan_instance
from protoscale import load_scale
from roadmap_core import GenParams, generate_roadmap
from sadg_core import PlanView, compile_graph, is_acyclic, plan_view_of, prune_cyclic_alternatives
from tick_core import (
    DELTA_T_T0_S,
    CompletionEvent,
    EntryEvent,
    ExecutionState,
    SegKey,
    TickInput,
    committed_edges,
    cross_check_pend,
    make_state,
    run_tick,
    take_republish,
)

HERE = os.path.dirname(os.path.abspath(__file__))

# 🔴 시제용 임시값 — release_sim.py의 ENTRY_FRACTION과 같은 자리, 사전등록 상수가 아니다.
ENTRY_FRACTION = 0.8
MAX_EXHAUSTIVE_COMBINATIONS = 200_000


@dataclass
class RobotPhysical:
    robot: str
    at_node: str
    segment: SegKey | None = None
    started_s: float = 0.0
    duration_s: float = 0.0
    entry_sent: bool = False
    seq: int = 0


@dataclass
class TimelineRow:
    tick_index: int
    sim_time_s: float
    completed_segment: str
    uncommitted_groups: int
    acyclic_combinations: int
    total_combinations: int
    exhaustive: bool
    kind: str = "completion"  # "completion" | "entry" (§65-8 M2′ 4번 — 진입 사건도 재는 확장)
    # 🔴 M2′(§65-8) — 재컴파일 값. `view`를 주지 않고 부르면 None으로 남는다(하위 호환).
    recompiled_groups: int | None = None
    recompiled_acyclic_combinations: int | None = None
    recompiled_total_combinations: int | None = None
    recompiled_exhaustive: bool | None = None


@dataclass
class EpisodeResult:
    rows: list[TimelineRow] = field(default_factory=list)
    entry_rows: list[TimelineRow] = field(default_factory=list)  # §65-8 4번
    baseline_acyclic_combinations: int = 0
    baseline_total_combinations: int = 0
    baseline_groups: int = 0
    makespan_s: float = 0.0
    ticks: int = 0
    problems: list[str] = field(default_factory=list)
    completed_events: int = 0
    recompile_unrepresentable_count: int = 0  # §65-8 M2′ — 꼬리 절단이 표현 불가를 낸 사건 수


def remaining_reorder_room(state: ExecutionState) -> tuple[int, int, int, bool]:
    """(미커밋 그룹 수, 전수 비순환 조합 수, 전체 조합 수, 전수인가)를 **다시 계산해** 낸다.

    🔴 이것은 그 시점의 `state`를 다시 읽어 계산하는 자체 점검이며 정본 릴리스 판정
    경로(`run_tick`)가 쓰는 값이 아니다 — E1 원자성(에포크가 닫힌 뒤에만 계산)과 무관하게
    호출자가 에포크 밖에서 부르는 것이 이 함수의 계약이다.
    """
    base = committed_edges(state)
    nodes = [s.key for s in state.graph.segments]
    remaining_groups = [g for g in state.graph.groups if g.location not in state.committed_choice]
    counts = [len(g.alternatives) for g in remaining_groups]
    total = 1
    for c in counts:
        total *= c

    if not is_acyclic(nodes, base):
        # 이미 커밋된 것만으로 순환한다 — prune 뒤에는 나오면 안 되는 값. 지어내지 않는다.
        return len(remaining_groups), 0, total, True

    if total > MAX_EXHAUSTIVE_COMBINATIONS:
        return len(remaining_groups), -1, total, False  # 표본 강등 — 이 스윕 규모에서는 실측상 발생하지 않았다(아래 로그 참고)

    acyclic_n = 0
    iterator = product(*[range(c) for c in counts]) if counts else iter([()])
    for combo in iterator:
        edges = list(base)
        for g, idx in zip(remaining_groups, combo):
            edges.extend(g.alternatives[idx].edges)
        if is_acyclic(nodes, edges):
            acyclic_n += 1
    return len(remaining_groups), acyclic_n, total, True


def exhaustive_acyclic_scan(graph, max_combinations: int = MAX_EXHAUSTIVE_COMBINATIONS
                             ) -> tuple[int, int, bool]:
    """(전수 비순환 조합 수, 전체 조합 수, 전수인가) — 그래프 **전체**(그룹 전부가 자유)를 본다.

    `remaining_reorder_room`과 자매 함수다 — 저것은 «지금 상태에서 미커밋 그룹만」을 보고,
    이것은 «방금 재컴파일한 새 그래프 전체」를 본다(재컴파일 그래프에는 커밋 개념이 없다).
    """
    nodes = [s.key for s in graph.segments]
    base = list(graph.fixed)
    per_group = [[a.edges for a in g.alternatives] for g in graph.groups]
    counts = [len(a) for a in per_group]
    total = 1
    for c in counts:
        total *= c
    if not is_acyclic(nodes, base):
        return 0, total, True
    if total > max_combinations:
        return -1, total, False
    acyclic_n = 0
    iterator = product(*[range(c) for c in counts]) if counts else iter([()])
    for combo in iterator:
        edges = list(base)
        for gi, ai in enumerate(combo):
            edges.extend(per_group[gi][ai])
        if is_acyclic(nodes, edges):
            acyclic_n += 1
    return acyclic_n, total, True


def _tail_view(view: PlanView, frontier_start: dict[str, int]) -> PlanView:
    """§65-8 M2′ — 완료된 접두를 뗀 **잔여 계획** 뷰.

    `frontier_start[robot]`은 그 로봇의 «완료되지 않은 첫 세그먼트의 번호」(= 아직 남은
    부분의 folded 시작 index). 로봇이 이미 다 끝났으면 마지막 index(정지 지점)를 쓴다.
    `view.steps`/`view.visit_order`는 원본(잘리지 않은) 것을 넘긴다 — `compile_graph`가
    이 잘린 뷰를 **완전히 새 인스턴스**로 다루므로 원본은 훼손되지 않는다.
    """
    steps: dict[str, list[str]] = {}
    for robot, nodes in view.steps.items():
        start = min(frontier_start.get(robot, len(nodes) - 1), len(nodes) - 1)
        steps[robot] = nodes[start:]
    visit_order: list[tuple[str, list[tuple[str, int]]]] = []
    for location, items in view.visit_order:
        remapped = []
        for robot, index in items:
            start = min(frontier_start.get(robot, len(view.steps[robot]) - 1),
                        len(view.steps[robot]) - 1)
            if index >= start:
                remapped.append((robot, index - start))
        if len(remapped) >= 2:  # 계약 §1-2(3)과 같은 기준 — build_visit_order를 다시 쓴다
            visit_order.append((location, remapped))
    return PlanView(instance_id=view.instance_id, revision=view.revision, steps=steps,
                     visit_order=visit_order)


def recompiled_remaining_room(view: PlanView, state: ExecutionState
                               ) -> tuple[int, int, int, bool, str | None]:
    """§65-8 M2′ — `state`가 가리키는 시점의 **잔여 계획을 새로 컴파일**해 재순서 여지를 낸다.

    가지치기(prune)는 정본(`out/`)과 동일하게 적용한다. 돌려주는 값은
    (재컴파일 그룹 수, 전수 비순환 조합 수, 전체 조합 수, 전수인가, 표현불가 사유) —
    앞 넷은 `remaining_reorder_room`과 같은 모양이라 바로 비교할 수 있다.

    🔴 **실측으로 드러난 경계 사례**: 두 로봇의 「잔여 계획 시작점」(꼬리 절단 지점)이 **같은
    위치로 동시에** 떨어지면 `compile_graph`가 `ValueError`를 던진다 — 그 위치의 원 통행 순서를
    표현할 대안이 하나도 없기 때문이다(사유: 시작 항목은 정의상 `arriving_segment`가 없고,
    시작 항목이 둘이면 어느 것도 순서열의 맨 앞이 아닌 자리에 놓일 수 없다). 원본(잘리지 않은)
    계획에서는 로봇마다 시작 정점이 다르므로 이 경우가 생기지 않지만, **꼬리 절단은 실시간
    재생(서로 다른 세그먼트 소요)에서 나온 스냅샷**이라 두 로봇이 우연히 같은 위치에서 동시에
    「대기 중」인 상태가 될 수 있다 — 원 이산 계획이 보장하던 동시 점유 배제가 재생 시각축에서는
    자동으로 옮겨지지 않는다는 뜻이다. **지어내지 않는다** — 이 값은 `(-1, -1, -1, False, 사유)`로
    돌려주고 호출자가 「표현 불가」로 센다.
    """
    frontier_start: dict[str, int] = {}
    for robot, seg_key in state.frontier.items():
        if seg_key is None:
            frontier_start[robot] = len(view.steps[robot]) - 1
        else:
            frontier_start[robot] = seg_key[1]
    tail = _tail_view(view, frontier_start)
    try:
        graph2 = compile_graph(tail, alt_generator="lift", revision=0)
    except ValueError as exc:
        return -1, -1, -1, False, str(exc)
    prune_cyclic_alternatives(graph2)
    acyclic_n, total, exhaustive = exhaustive_acyclic_scan(graph2)
    return len(graph2.groups), acyclic_n, total, exhaustive, None


def run_episode(graph, model, nominal_speed_mps: float, blockage: Blockage | None,
                 max_ticks: int = 20000, view: PlanView | None = None,
                 record_entries: bool = False, probe=None) -> EpisodeResult:
    """릴리스 재생 + 완료 사건마다 재순서 여지를 잰다. `release_sim.run_loop`와 같은 재료,
    다른 계측 지점 — 그 함수를 고치지 않고 새로 짰다.

    🔴 `probe`(§72 재컴파일 비용 측정에서 추가) — 완료 사건마다
    `probe(state, released_segments, now, completed_segment)`를 부른다. **기본값 `None`이면
    호출되지 않으므로 기존 호출자의 동작이 바뀌지 않는다.** `released_segments`는 «지금까지
    릴리스 비트가 나간 세그먼트 키»의 누적 집합이며, RECOMP-LOCK(`68_p2` §R7-4-4)과
    T-REL-IRREV(`68_p2` §R7-5-2)가 재컴파일 대상에서 빼야 한다고 말하는 자리다.
    """
    nodes_map = model.node_map()
    durations = segment_durations(graph, model, nominal_speed_mps)
    extra: dict[SegKey, float] = {s.key: 0.0 for s in graph.segments}
    if blockage is not None and (blockage.robot, blockage.segment_number) in extra:
        extra[(blockage.robot, blockage.segment_number)] += blockage.seconds

    state = make_state(graph, {})
    take_republish(state)  # 초기 컴파일분 소비 (틱 밖)

    base_groups, base_acyclic, base_total, base_exhaustive = remaining_reorder_room(state)
    result = EpisodeResult(
        baseline_groups=base_groups,
        baseline_acyclic_combinations=base_acyclic,
        baseline_total_combinations=base_total,
    )
    if not base_exhaustive:
        result.problems.append("t=0 조합 수가 상한을 넘어 표본으로 강등됐다 — exhaustive=False")
    if view is not None:
        # 🔴 t=0에는 완료된 접두가 없으므로 재컴파일 값은 고정 컴파일 값과 같아야 한다(자체 점검).
        base_r_groups, base_r_acyclic, _, base_r_exhaustive, base_r_reason = recompiled_remaining_room(
            view, state
        )
        if base_r_reason is not None:
            result.problems.append(f"t=0 재컴파일이 표현 불가였다 — {base_r_reason}")
        elif base_r_exhaustive and base_exhaustive and base_r_acyclic != base_acyclic:
            result.problems.append(
                f"t=0 재컴파일({base_r_acyclic})이 고정 컴파일({base_acyclic})과 다르다"
            )

    segment_by_key = {s.key: s for s in graph.segments}
    released_segments: set[SegKey] = set()  # 🔴 RECOMP-LOCK 대상 — 릴리스가 나간 세그먼트의 누적
    robots = sorted({s.robot for s in graph.segments})
    phys = {
        r: RobotPhysical(r, segment_by_key[(r, 0)].src_node)
        for r in robots
        if (r, 0) in segment_by_key
    }

    for tick_seq in range(max_ticks):
        now = tick_seq * DELTA_T_T0_S
        entries: list[EntryEvent] = []
        completions: list[CompletionEvent] = []

        for p in phys.values():
            if p.segment is None:
                continue
            elapsed = now - p.started_s
            if not p.entry_sent and elapsed >= p.duration_s * ENTRY_FRACTION:
                p.entry_sent = True
                entries.append(EntryEvent(p.robot, p.segment, p.seq))
                p.seq += 1
            if elapsed >= p.duration_s:
                completions.append(CompletionEvent(p.segment))

        out = run_tick(state, TickInput(tick_seq, now, entries, completions))
        take_republish(state)  # 산출은 만들지 않는다 — 플래그만 비운다(E2)

        for c in completions:
            p = phys.get(c.segment[0])
            if p is not None:
                p.at_node = segment_by_key[c.segment].dst_node
                p.segment = None
                p.entry_sent = False

        for key in out.released:
            # 릴리스 비트가 나간 세그먼트는 **떠나는 정점과 들어가는 정점** 양쪽의 통행 순서를
            # 확정한다(T-REL-IRREV, `68_p2` §R7-5-2). 세그먼트 키를 그대로 쌓고, 정점 단위·항목
            # 단위 중 어느 잠금을 쓸지는 **호출자가** 정한다.
            released_segments.add(key)
            p = phys.get(key[0])
            if p is None or p.segment is not None:
                continue
            p.segment = key
            p.started_s = now
            p.duration_s = durations[key] + extra[key]

        result.problems.extend(cross_check_pend(state))

        if completions:
            for c in completions:
                result.completed_events += 1
                if probe is not None:
                    probe(state, frozenset(released_segments), now, c.segment)
                n_groups, n_acyclic, n_total, exhaustive = remaining_reorder_room(state)
                row = TimelineRow(
                    tick_index=tick_seq,
                    sim_time_s=now,
                    completed_segment=f"{c.segment[0]}#{c.segment[1]}",
                    uncommitted_groups=n_groups,
                    acyclic_combinations=n_acyclic,
                    total_combinations=n_total,
                    exhaustive=exhaustive,
                    kind="completion",
                )
                if view is not None:
                    r_groups, r_acyclic, r_total, r_exhaustive, r_reason = recompiled_remaining_room(
                        view, state
                    )
                    row.recompiled_groups = r_groups
                    row.recompiled_acyclic_combinations = r_acyclic
                    row.recompiled_total_combinations = r_total
                    row.recompiled_exhaustive = r_exhaustive
                    if r_reason is not None:
                        result.recompile_unrepresentable_count += 1
                        result.problems.append(
                            "재컴파일 표현 불가 사건이 있었다(§65-8 — 두 로봇의 꼬리 시작점이 "
                            "같은 위치로 겹친 스냅샷)"
                        )
                result.rows.append(row)
                if not exhaustive:
                    result.problems.append(
                        f"tick {tick_seq} @ {c.segment}: 조합 상한 초과 — 표본 강등"
                    )

        # 🔴 §65-8 4번 — 진입 사건 시점도 잰다(여력이 되면 넣으라는 지시). `record_entries=True`일
        # 때만 돈다(기존 스윕 성능을 지키기 위한 옵트인 — 완료 사건 계측과 대칭이 아니다).
        if record_entries and entries and view is not None:
            for e in entries:
                n_groups, n_acyclic, n_total, exhaustive = remaining_reorder_room(state)
                r_groups, r_acyclic, r_total, r_exhaustive, r_reason = recompiled_remaining_room(
                    view, state
                )
                if r_reason is not None:
                    result.recompile_unrepresentable_count += 1
                    result.problems.append(
                        "재컴파일 표현 불가 사건이 있었다(§65-8 — 두 로봇의 꼬리 시작점이 "
                        "같은 위치로 겹친 스냅샷)"
                    )
                result.entry_rows.append(
                    TimelineRow(
                        tick_index=tick_seq,
                        sim_time_s=now,
                        completed_segment=f"{e.robot}#{e.segment[1]}",
                        uncommitted_groups=n_groups,
                        acyclic_combinations=n_acyclic,
                        total_combinations=n_total,
                        exhaustive=exhaustive,
                        kind="entry",
                        recompiled_groups=r_groups,
                        recompiled_acyclic_combinations=r_acyclic,
                        recompiled_total_combinations=r_total,
                        recompiled_exhaustive=r_exhaustive,
                    )
                )

        if all(state.frontier[r] is None for r in robots):
            result.makespan_s = now
            result.ticks = tick_seq + 1
            break
    else:
        result.problems.append(f"틱 상한 {max_ticks}에 도달했다 — 완료하지 못했다")
        result.ticks = max_ticks

    result.problems = sorted(set(result.problems))
    return result


def build_instance(cfg, robots: int, tasks: int, priority: str, horizon: int, index: int):
    """창고형 인스턴스 하나. `index`가 결정론적 생성 인자다(내부 알고리즘 파라미터이며
    경계로 나가지 않는다 — 스키마 순수성 검사가 지키는 것과 같은 규율로 보고서 키 이름에서도
    'seed' 문자열을 쓰지 않는다).
    """
    model = generate_roadmap(cfg, GenParams(seed=index))
    assignment = build_assignment(model, AssignParams(robots=robots, tasks=tasks, seed=index))
    plan = plan_instance(model, assignment, PlanParams(priority=priority, horizon=horizon))
    return model, plan


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="M2 — 에피소드 시계열 측정 (탐색용 시제)")
    ap.add_argument("--robots", type=int, default=6)
    ap.add_argument("--tasks", type=int, default=8)
    ap.add_argument("--priority", default="most_goals")
    ap.add_argument("--horizon", type=int, default=96)
    ap.add_argument("--instances", type=int, default=20, help="생성 인자 0..instances-1 (창고형 스윕과 동일 규모)")
    ap.add_argument("--block-robot", default="r0")
    ap.add_argument("--block-at", type=int, default=1)
    ap.add_argument("--block-seconds", type=float, default=20.0)
    ap.add_argument("--max-ticks", type=int, default=20000)
    ap.add_argument("--record-entries", action="store_true",
                    help="§65-8 4번 — 진입 사건 시점도 잰다(완료 사건 계측에 옵트인으로 추가)")
    ap.add_argument("--out", default=os.path.join(HERE, "stats", "episode_timeline.json"))
    args = ap.parse_args(argv)

    cfg = load_scale()
    per_instance = []
    skipped_unplanned = 0

    def _row_dict(r):
        return {
            "kind": r.kind,
            "tick_index": r.tick_index,
            "sim_time_s": round(r.sim_time_s, 3),
            "completed_segment": r.completed_segment,
            "uncommitted_groups": r.uncommitted_groups,
            "acyclic_combinations": r.acyclic_combinations,
            "total_combinations": r.total_combinations,
            "exhaustive": r.exhaustive,
            "recompiled_groups": r.recompiled_groups,
            "recompiled_acyclic_combinations": r.recompiled_acyclic_combinations,
            "recompiled_total_combinations": r.recompiled_total_combinations,
            "recompiled_exhaustive": r.recompiled_exhaustive,
        }

    for index in range(args.instances):
        model, plan_result = build_instance(
            cfg, args.robots, args.tasks, args.priority, args.horizon, index
        )
        if plan_result.unplanned:
            skipped_unplanned += 1
            continue

        view = plan_view_of(plan_result)  # §65-8 M2′ — 재컴파일에 쓰는 원본(잘리지 않은) 뷰

        variants = {}
        for tag, blockage in (
            ("no_block", None),
            ("block", Blockage(args.block_robot, args.block_at, args.block_seconds)),
        ):
            # 그래프는 재생마다 커밋 상태가 바뀌므로 변형마다 새로 컴파일한다(같은 인자로 결정론).
            g2 = compile_graph(view, alt_generator="lift")
            prune_cyclic_alternatives(g2)  # out/ 정본과 같은 처리 — 배송 가능한 그래프로 맞춘다
            ep = run_episode(g2, model, cfg.nominal_speed_mps, blockage, max_ticks=args.max_ticks,
                             view=view, record_entries=args.record_entries)

            fixed_series = [r.acyclic_combinations for r in ep.rows
                             if r.exhaustive and r.acyclic_combinations >= 0]
            # §65-8 M2′ 핵심 물음 — 재컴파일 방이 (a) 그 시점 고정 컴파일 잔여 방을, (b) t=0 값을
            # 초과한 사건 수. `-1`(표본 강등)과 `None`(view 없이 부른 경우 — 여기서는 없다)은 뺀다.
            comparable = [
                r for r in ep.rows
                if r.exhaustive and r.recompiled_exhaustive
                and r.acyclic_combinations >= 0 and r.recompiled_acyclic_combinations is not None
                and r.recompiled_acyclic_combinations >= 0
            ]
            exceeds_fixed = sum(
                1 for r in comparable if r.recompiled_acyclic_combinations > r.acyclic_combinations
            )
            exceeds_t0 = sum(
                1 for r in comparable
                if r.recompiled_acyclic_combinations > ep.baseline_acyclic_combinations
            )
            recompiled_series = [r.recompiled_acyclic_combinations for r in comparable]

            variants[tag] = {
                "baseline_groups": ep.baseline_groups,
                "baseline_acyclic_combinations": ep.baseline_acyclic_combinations,
                "baseline_total_combinations": ep.baseline_total_combinations,
                "completed_events": ep.completed_events,
                "timeline_max_acyclic_combinations": (
                    max(fixed_series) if fixed_series else ep.baseline_acyclic_combinations
                ),
                "timeline_mean_acyclic_combinations": (
                    round(statistics.fmean(fixed_series), 4)
                    if fixed_series else float(ep.baseline_acyclic_combinations)
                ),
                "exceeds_baseline_event_count": sum(
                    1 for v in fixed_series if v > ep.baseline_acyclic_combinations
                ),
                # §65-8 M2′ — 재컴파일 쪽 집계
                "recompile_comparable_event_count": len(comparable),
                "recompile_exceeds_fixed_event_count": exceeds_fixed,
                "recompile_exceeds_baseline_event_count": exceeds_t0,
                "recompile_unrepresentable_count": ep.recompile_unrepresentable_count,
                "recompile_timeline_max_acyclic_combinations": (
                    max(recompiled_series) if recompiled_series else ep.baseline_acyclic_combinations
                ),
                "makespan_s": ep.makespan_s,
                "ticks": ep.ticks,
                "problems": ep.problems,
                "rows": [_row_dict(r) for r in ep.rows],
                "entry_rows": [_row_dict(r) for r in ep.entry_rows],
            }
        per_instance.append({"instance_tag": model.instance_id, "variants": variants})

    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    with open(args.out, "w", encoding="utf-8") as handle:
        json.dump(
            {
                "note": "내부 회계다. 경계 아티팩트가 아니며 논문 증거가 아니다. 탐색용 시제.",
                "robots": args.robots,
                "task_count": args.tasks,
                "priority_rule": args.priority,
                "instances_requested": args.instances,
                "instances_planned": len(per_instance),
                "instances_unplanned": skipped_unplanned,
                "record_entries": args.record_entries,
                "per_instance": per_instance,
            },
            handle,
            indent=2,
            ensure_ascii=False,
        )
        handle.write("\n")

    print(f"인스턴스 요청/계획됨/미해결: {args.instances}/{len(per_instance)}/{skipped_unplanned}")
    for tag in ("no_block", "block"):
        maxima = [row["variants"][tag]["timeline_max_acyclic_combinations"] for row in per_instance]
        baselines = [row["variants"][tag]["baseline_acyclic_combinations"] for row in per_instance]
        exceed = sum(row["variants"][tag]["exceeds_baseline_event_count"] > 0 for row in per_instance)
        events_total = sum(row["variants"][tag]["recompile_comparable_event_count"] for row in per_instance)
        events_exceed_fixed = sum(row["variants"][tag]["recompile_exceeds_fixed_event_count"] for row in per_instance)
        events_exceed_t0 = sum(row["variants"][tag]["recompile_exceeds_baseline_event_count"] for row in per_instance)
        print(
            f"[{tag}] 고정컴파일: t=0 평균 {statistics.fmean(baselines):.3f} / 시계열 최대의 평균 "
            f"{statistics.fmean(maxima):.3f} / t=0을 초과한 인스턴스 수 {exceed}/{len(per_instance)}"
        )
        print(
            f"[{tag}] 재컴파일(M2′): 비교가능 사건 {events_total} / "
            f"고정컴파일 잔여방을 초과한 사건 {events_exceed_fixed} / t=0을 초과한 사건 {events_exceed_t0}"
        )
    print(f"wrote: {os.path.abspath(args.out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
