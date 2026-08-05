#!/usr/bin/env python3
"""재컴파일 비용 측정 — RECOMP-LOCK 적용 전/후 (탐색용 시제).

🔴 **탐색용 시제이며 증거가 아니다. 어떤 값도 사전등록 상수가 아니다. 사전등록을 발동시키지 않는다.**
🔴 **여기 나오는 벽시계 시간은 파이썬 시제의 시간이다.** C++ 서버의 T0 예산(제어 주기)과
직접 비교할 수 없고, 이 문서는 예산 적합 여부를 **판정하지 않는다.** 기계 독립적인 규모 지표
(세그먼트·간선·그룹·조합 수)를 시간과 **나란히** 싣는 이유가 그것이다.

## 무엇을 재는가

`65_p2` §65-8이 «꼬리를 다시 컴파일하면 방이 열린다»를 보였고 §65-8-7-1이 «비용은 재지
않았다»를 한계로 남겼다. 이 스크립트가 그 자리를 잰다.

재컴파일 1회 = `_tail_view`(절단) + `compile_graph`(대안 생성) + 잠금 앵커링 +
`prune_cyclic_alternatives`(가지치기). **전수 비순환 스캔은 계측이지 기전이 아니므로 시간에서
뺀다** — 넣으면 «측정 비용»이 «기전 비용»으로 보고된다.

## 다섯 가지 모드 — 잠금의 **층위**와 **강도**가 갈린다

| 모드 | 잠금 단위 | 잠그는 것 | 근거 |
|------|----------|----------|------|
| `no_lock` | — | 없음 | `65_p2`가 잰 것과 같은 절차. **안전 조건을 무시한 값**이라 그대로 쓸 수 없다 |
| `lock_release` | **위치 통째** | 릴리스가 나간 세그먼트의 떠난 정점 ∪ 들어간 정점 | T-REL-IRREV(`68_p2` §R7-5-2)의 **보수적** 읽기 |
| `lock_depart` | 위치 통째 | 떠난 정점만 | 같은 정리의 느슨한 읽기. 감도 확인용 |
| `lock_start` | **항목** | 꼬리 시작 항목만 | A-LOCK(개시 선점 불가)만 반영. T-REL-IRREV는 미반영 |
| `lock_rel_item` | **항목** | 진입 릴리스가 나간 항목 + 시작 항목 | 🔴 **T-REL-IRREV를 항목 층위로 그대로 옮긴 것.** 이 라운드의 본안 |

잠긴 것은 스위치 그룹으로 발행하지 않고 **실현된 순서를 고정 의존으로 앵커링**한다 —
`68_p2` §R7-4-3의 처방 (b)와 같은 기전이며 필드 신설이 없다.
"""

from __future__ import annotations

import argparse
import json
import os
import statistics
import time

from episode_timeline import (
    _tail_view,
    build_instance,
    exhaustive_acyclic_scan,
    run_episode,
)
from exec_sim import Blockage
from protoscale import load_scale
from sadg_core import (
    PlanView,
    compile_graph,
    pair_edge,
    plan_view_of,
    prune_cyclic_alternatives,
)

HERE = os.path.dirname(os.path.abspath(__file__))

MODES = ("no_lock", "lock_release", "lock_depart", "lock_start", "lock_rel_item")


def realized_order(state, location: str, view: PlanView) -> tuple:
    """잠긴 위치의 **실현된 통행 순서**. 커밋된 대안이 있으면 그것, 없으면 공칭 순서.

    진입 커밋(`tick_core._apply_entry_commits`)은 «물리적으로 먼저 들어간 로봇»을 기록하므로
    커밋된 대안의 순서가 실현된 순서다. 커밋 전이면 아직 뒤집힌 적이 없으므로 공칭이 실현이다.
    """
    idx = state.committed_choice.get(location)
    if idx is not None:
        for group in state.graph.groups:
            if group.location == location:
                return group.alternatives[idx].order
    for loc, items in view.visit_order:
        if loc == location:
            return tuple(items)
    return ()


def anchor_edges(tail: PlanView, order_by_location: dict[str, tuple]) -> tuple[list, int]:
    """잠긴 위치의 실현 순서를 **고정 의존**으로 옮긴다 (`68_p2` §R7-4-3 처방 (b)).

    인접 쌍만 발행한다 — 로봇 내부 Type-1 사슬이 다리를 놓아 비인접 쌍의 도달 관계가 따라온다.
    표현 불가(출발 항목의 진입 세그먼트 부재 등)인 쌍은 **버리고 센다**. 버려도 안전한 이유는
    그 순서가 **이미 물리적으로 일어났기 때문**이다(A-LOCK) — 다시 강제할 대상이 아니다.
    """
    edges: list = []
    skipped = 0
    for _location, order in sorted(order_by_location.items()):
        for a, b in zip(order, order[1:]):
            edge = pair_edge(tail, a, b)
            if edge is None:
                skipped += 1
                continue
            edges.append(edge)
    return edges, skipped


def start_item_anchor(tail: PlanView) -> tuple[PlanView, list, int]:
    """**항목 층위 잠금** — 꼬리 시작 항목(index 0)을 앞에 못 박고 그룹에서 뺀다.

    `68_p2` §R7-4-3의 처방 (b)를 위치 통째가 아니라 **항목 단위**로 적용한 것이다. 시작 항목은
    이미 그 정점을 떠나는 중(릴리스 발행 완료)이므로 순서가 물리적으로 확정됐고, 뒤에 오는
    항목들 사이의 순서는 아직 자유롭다. 이렇게 하면
    (i) 시작 항목이 둘 겹쳐도 **표현 불가가 생기지 않고**(RECOMP-WF 위반이 소멸),
    (ii) 위치 통째 잠금보다 **자유부가 더 남는다.**

    돌려주는 것: (시작 항목을 뺀 뷰, 앵커 간선, 표현 불가로 버린 앵커 쌍 수).
    """
    edges: list = []
    skipped = 0
    reduced: list[tuple[str, list]] = []
    for location, items in tail.visit_order:
        starts = [it for it in items if it[1] == 0]
        rest = [it for it in items if it[1] != 0]
        for i, a in enumerate(starts):
            for b in starts[i + 1 :] + rest:
                edge = pair_edge(tail, a, b)
                if edge is None:
                    skipped += 1
                    continue
                edges.append(edge)
        if len(rest) >= 2:
            reduced.append((location, rest))
    return (
        PlanView(tail.instance_id, tail.revision, tail.steps, reduced),
        edges,
        skipped,
    )


def released_item_anchor(
    tail: PlanView, frontier_start: dict[str, int], released_segments: frozenset
) -> tuple[PlanView, list, int, int]:
    """🔴 **RECOMP-LOCK 본체 — 항목 층위.** T-REL-IRREV(`68_p2` §R7-5-2)를 그대로 옮긴다.

    꼬리 위치 $v$의 항목 $(r,\\cdot)$가 **잠긴다**는 것은 둘 중 하나다:
    (i) $r$의 **$v$로 들어가는** 세그먼트에 릴리스 비트가 이미 나갔다 — 서버는 그 시점 이후
        $r$의 $v$에서의 상대 순서를 뒤집을 수 없다,
    (ii) 항목이 꼬리 시작 항목이고 그 **떠나는** 세그먼트에 릴리스가 나갔다 — A-LOCK.

    잠긴 항목들은 **실현된 상대 순서대로 앞에** 고정 의존으로 박고, 나머지 항목만 스위치
    그룹으로 남긴다. 잠긴 항목이 하나도 없는 위치는 그대로 자유롭다.
    **릴리스가 아직 안 나간 시작 항목은 잠기지 않는다** — 그래서 그런 항목이 한 위치에 둘
    겹치면 표현 불가(RECOMP-WF 위반)가 그대로 남고, 이 함수는 그것을 삼키지 않는다.
    """
    edges: list = []
    skipped = 0
    locked_items = 0
    reduced: list[tuple[str, list]] = []
    for location, items in tail.visit_order:
        locked: list = []
        free: list = []
        for robot, tidx in items:
            origin = tidx + frontier_start.get(robot, 0)
            arriving_released = origin >= 1 and (robot, origin - 1) in released_segments
            departing_released = tidx == 0 and (robot, origin) in released_segments
            (locked if (arriving_released or departing_released) else free).append((robot, tidx))
        locked_items += len(locked)
        for a, b in zip(locked, locked[1:]):
            edge = pair_edge(tail, a, b)
            if edge is None:
                skipped += 1
                continue
            edges.append(edge)
        if locked:
            for b in free:
                edge = pair_edge(tail, locked[-1], b)
                if edge is None:
                    skipped += 1
                    continue
                edges.append(edge)
        if len(free) >= 2:
            reduced.append((location, free))
    return (
        PlanView(tail.instance_id, tail.revision, tail.steps, reduced),
        edges,
        skipped,
        locked_items,
    )


def recompile_once(
    view: PlanView,
    state,
    locked: frozenset[str],
    anchor_starts: bool = False,
    released_segments: frozenset | None = None,
) -> dict:
    """재컴파일 1회. **시간은 기전만** 재고 전수 스캔은 시간 밖에서 돈다."""
    t0 = time.perf_counter()
    frontier_start: dict[str, int] = {}
    for robot, seg_key in state.frontier.items():
        if seg_key is None:
            frontier_start[robot] = len(view.steps[robot]) - 1
        else:
            frontier_start[robot] = seg_key[1]
    tail = _tail_view(view, frontier_start)

    start_edges: list = []
    start_skipped = 0
    locked_item_count = 0
    if released_segments is not None:
        tail_free, start_edges, start_skipped, locked_item_count = released_item_anchor(
            tail, frontier_start, released_segments
        )
        locked_here: set[str] = set()
    elif anchor_starts:
        tail_free, start_edges, start_skipped = start_item_anchor(tail)
        locked_here = set()
    else:
        locked_here = {loc for loc, _ in tail.visit_order if loc in locked}
        if locked_here:
            free_visit = [
                (loc, items) for loc, items in tail.visit_order if loc not in locked_here
            ]
            tail_free = PlanView(tail.instance_id, tail.revision, tail.steps, free_visit)
        else:
            tail_free = tail
    t_cut = time.perf_counter()

    try:
        graph2 = compile_graph(tail_free, alt_generator="lift", revision=0)
    except ValueError as exc:
        return {
            "unrepresentable": True,
            "reason": str(exc),
            "elapsed_s": time.perf_counter() - t0,
            "locked_locations": len(locked_here),
        }
    t_compile = time.perf_counter()

    anchor_skipped = start_skipped
    if start_edges:
        graph2.fixed.extend(start_edges)
    if locked_here:
        realized = {loc: _tail_items(tail, view, state, loc, frontier_start) for loc in locked_here}
        edges, skipped = anchor_edges(tail, realized)
        anchor_skipped += skipped
        graph2.fixed.extend(edges)
    t_anchor = time.perf_counter()

    prune_cyclic_alternatives(graph2)
    t_prune = time.perf_counter()

    acyclic_n, total, exhaustive = exhaustive_acyclic_scan(graph2)
    return {
        "unrepresentable": False,
        "elapsed_s": t_prune - t0,
        "cut_s": t_cut - t0,
        "compile_s": t_compile - t_cut,
        "anchor_s": t_anchor - t_compile,
        "prune_s": t_prune - t_anchor,
        "locked_locations": len(locked_here),
        "locked_items": locked_item_count,
        "anchor_pairs_skipped": anchor_skipped,
        "groups": len(graph2.groups),
        "segments": len(graph2.segments),
        "fixed_edges": len(graph2.fixed),
        "acyclic_combinations": acyclic_n,
        "total_combinations": total,
        "exhaustive": exhaustive,
    }


def _tail_items(tail: PlanView, view: PlanView, state, location: str, frontier_start) -> tuple:
    """잠긴 위치의 실현 순서를 **꼬리 인덱스로** 옮긴다. 잘려 나간 항목은 뺀다."""
    order = realized_order(state, location, view)
    out = []
    for robot, index in order:
        start = min(frontier_start.get(robot, len(view.steps[robot]) - 1),
                    len(view.steps[robot]) - 1)
        if index >= start:
            out.append((robot, index - start))
    return tuple(out)


def percentiles(values: list[float]) -> dict:
    """p50/p95/p99/max. **분모(시행 수)를 언제나 함께 낸다.**"""
    if not values:
        return {"n": 0}
    ordered = sorted(values)

    def at(q: float) -> float:
        k = min(len(ordered) - 1, max(0, int(round(q * (len(ordered) - 1)))))
        return ordered[k]

    return {
        "n": len(ordered),
        "p50_ms": round(statistics.median(ordered) * 1e3, 4),
        "p95_ms": round(at(0.95) * 1e3, 4),
        "p99_ms": round(at(0.99) * 1e3, 4),
        "max_ms": round(ordered[-1] * 1e3, 4),
    }


def measure_scale_point(cfg, robots: int, tasks: int, instances: int, horizon: int,
                        with_blockage: bool) -> dict:
    """한 규모 점. 인스턴스마다 에피소드를 재생하며 완료 사건마다 세 모드를 **모두** 잰다."""
    samples: dict[str, list[float]] = {m: [] for m in MODES}
    rooms: dict[str, list[int]] = {m: [] for m in MODES}
    unrepresentable: dict[str, int] = {m: 0 for m in MODES}
    sizes: list[dict] = []
    planned = 0
    skipped = 0
    events = 0

    for index in range(instances):
        model, plan = build_instance(cfg, robots, tasks, "most_goals", horizon, index)
        if plan.unplanned:
            skipped += 1
            continue
        planned += 1
        view = plan_view_of(plan)
        graph = compile_graph(view, alt_generator="lift")
        prune_cyclic_alternatives(graph)

        def probe(state, released, _now, _seg):
            nonlocal events
            events += 1
            released_locations = frozenset(
                node
                for r, i in released
                for node in (view.steps[r][i], view.steps[r][i + 1])
                if i + 1 < len(view.steps[r])
            )
            for mode in MODES:
                anchor_starts = mode == "lock_start"
                rel_items = released if mode == "lock_rel_item" else None
                if mode in ("no_lock", "lock_start", "lock_rel_item"):
                    locked = frozenset()
                elif mode == "lock_release":
                    locked = released_locations
                else:
                    locked = _depart_locked(state, view)
                row = recompile_once(
                    view, state, locked, anchor_starts=anchor_starts,
                    released_segments=rel_items,
                )
                if row["unrepresentable"]:
                    unrepresentable[mode] += 1
                    continue
                samples[mode].append(row["elapsed_s"])
                rooms[mode].append(row["acyclic_combinations"])
                if mode == "no_lock" and len(sizes) < 200:
                    sizes.append(
                        {
                            "segments": row["segments"],
                            "groups": row["groups"],
                            "fixed_edges": row["fixed_edges"],
                            "total_combinations": row["total_combinations"],
                        }
                    )

        blockage = Blockage("r0", 1, 20.0) if with_blockage else None
        run_episode(graph, model, cfg.nominal_speed_mps, blockage, view=None, probe=probe)

    out = {
        "robots": robots,
        "tasks": tasks,
        "instances_requested": instances,
        "instances_planned": planned,
        "instances_unplanned": skipped,
        "completion_events": events,
        "blockage": with_blockage,
        "timing": {m: percentiles(samples[m]) for m in MODES},
        "unrepresentable_events": unrepresentable,
        "room_mean": {
            m: (round(statistics.fmean(rooms[m]), 4) if rooms[m] else None) for m in MODES
        },
        "room_max": {m: (max(rooms[m]) if rooms[m] else None) for m in MODES},
        "room_events_gt1": {m: sum(1 for v in rooms[m] if v > 1) for m in MODES},
        "room_total": {m: sum(rooms[m]) for m in MODES},
        "room_events_reduced_by_lock": sum(
            1 for a, b in zip(rooms["no_lock"], rooms["lock_release"]) if b < a
        ),
        "room_events_equal_under_lock": sum(
            1 for a, b in zip(rooms["no_lock"], rooms["lock_release"]) if b == a
        ),
        "size_samples": sizes[:20],
    }
    return out


def _depart_locked(state, view: PlanView) -> frozenset[str]:
    """느슨한 잠금 — 각 로봇이 **떠난** 정점만. frontier 이전 세그먼트의 출발 정점 전부."""
    locked: set[str] = set()
    for robot, seg in state.frontier.items():
        limit = len(view.steps[robot]) - 1 if seg is None else seg[1]
        for i in range(limit + 1):
            if i < len(view.steps[robot]):
                locked.add(view.steps[robot][i])
    return frozenset(locked)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="재컴파일 비용 측정 (탐색용 시제)")
    ap.add_argument("--robots", default="4,5,6,7,8", help="규모 스윕 점 (최소 4점)")
    ap.add_argument("--instances", type=int, default=8)
    ap.add_argument("--horizon", type=int, default=96)
    ap.add_argument("--blockage", action="store_true")
    ap.add_argument("--out", default=os.path.join(HERE, "stats", "recompile_cost.json"))
    args = ap.parse_args(argv)

    cfg = load_scale()
    points = []
    for robots in [int(v) for v in args.robots.split(",")]:
        points.append(
            measure_scale_point(
                cfg, robots, robots + 2, args.instances, args.horizon, args.blockage
            )
        )

    payload = {
        "note": "탐색용 시제. 파이썬 벽시계 시간이며 C++ 서버 예산과 직접 비교할 수 없다. 증거가 아니다.",
        "timing_excludes": "exhaustive_acyclic_scan (계측이며 기전이 아니다)",
        "modes": list(MODES),
        "scale_points": points,
    }
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    with open(args.out, "w", encoding="utf-8") as handle:
        json.dump(payload, handle, indent=2, ensure_ascii=False)
        handle.write("\n")

    for p in points:
        print(
            f"[로봇 {p['robots']}] 계획됨 {p['instances_planned']}/{p['instances_requested']}"
            f" · 완료사건 {p['completion_events']}"
        )
        for mode in MODES:
            t = p["timing"][mode]
            print(
                f"   {mode:<13} n={t.get('n')} p50={t.get('p50_ms')}ms p95={t.get('p95_ms')}ms"
                f" p99={t.get('p99_ms')}ms max={t.get('max_ms')}ms"
                f" · 표현불가 {p['unrepresentable_events'][mode]}"
                f" · 방 평균 {p['room_mean'][mode]} 최대 {p['room_max'][mode]}"
                f" · 방>1 사건 {p['room_events_gt1'][mode]}"
            )
    print(f"wrote: {os.path.abspath(args.out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
