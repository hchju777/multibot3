"""재컴파일 비용·RECOMP-LOCK 시험.

🔴 **여기서 통과하는 것은 판정이 아니다.** 시험을 쓴 사람과 승인하는 사람이 다르다.
🔴 **시간 값은 시험하지 않는다** — 벽시계 값은 기계에 따라 달라지므로 시험으로 고정하면
    다른 기계에서 거짓 실패가 난다. 시험은 **구조**(무엇이 잠기는가·표현 불가가 사라지는가)만 본다.
"""

from __future__ import annotations

from assignment_core import AssignParams, build_assignment
from episode_timeline import _tail_view
from plan_core import PlanParams, plan_instance
from protoscale import load_scale
from recompile_cost import (
    MODES,
    percentiles,
    recompile_once,
    released_item_anchor,
    start_item_anchor,
)
from roadmap_core import GenParams, generate_roadmap
from sadg_core import PlanView, compile_graph, plan_view_of, prune_cyclic_alternatives


class _FakeState:
    """`recompile_once`가 읽는 것만 갖춘 최소 상태. `tick_core`를 띄우지 않고 격리한다."""

    def __init__(self, frontier, graph=None, committed=None):
        self.frontier = frontier
        self.graph = graph
        self.committed_choice = committed or {}


def overlapping_tail_view() -> PlanView:
    """두 로봇의 **꼬리 시작점이 같은 위치**로 떨어지는 뷰 — RECOMP-WF (WF-1) 위반 상황."""
    return PlanView(
        instance_id="overlap",
        revision=0,
        steps={"r1": ["v", "a1", "a2"], "r2": ["v", "b1", "b2"]},
        visit_order=[("v", [("r1", 0), ("r2", 0)])],
    )


def test_start_item_anchor_removes_the_unrepresentable_location():
    """시작 항목이 둘이면 그 위치는 그룹에서 빠진다 — 표현할 것이 남지 않는다."""
    tail = overlapping_tail_view()
    reduced, edges, skipped = start_item_anchor(tail)
    assert reduced.visit_order == []
    # r1의 떠나는 세그먼트는 있지만 r2의 들어오는 세그먼트가 없다 — 앵커 쌍이 버려진다
    assert skipped == 1
    assert edges == []
    compile_graph(reduced, alt_generator="lift")  # ValueError가 나지 않는다


def test_compile_of_the_overlapping_tail_raises_without_anchoring():
    """앵커링 없이는 `65_p2` §65-8-3의 표현 불가가 그대로 난다 — 대조군."""
    tail = overlapping_tail_view()
    try:
        compile_graph(tail, alt_generator="lift")
    except ValueError:
        return
    raise AssertionError("표현 불가가 나야 하는 자리에서 나지 않았다")


def test_recompile_once_returns_a_sentinel_instead_of_raising():
    """표현 불가는 **삼키지도 죽지도 않고** 센티넬로 돌아온다(`65_p2`와 같은 규율)."""
    view = overlapping_tail_view()
    state = _FakeState({"r1": ("r1", 0), "r2": ("r2", 0)})
    row = recompile_once(view, state, frozenset())
    assert row["unrepresentable"] is True
    assert "elapsed_s" in row


def test_released_item_anchor_pins_locked_items_ahead_of_free_items():
    """진입 릴리스가 나간 항목은 앞에 박히고 그룹에서 빠진다 — T-REL-IRREV의 구현."""
    tail = PlanView(
        instance_id="anchor",
        revision=0,
        steps={"a": ["a0", "v", "a1"], "b": ["b0", "v", "b1"], "c": ["c0", "v", "c1"]},
        visit_order=[("v", [("a", 1), ("b", 1), ("c", 1)])],
    )
    frontier_start = {"a": 0, "b": 0, "c": 0}
    released = frozenset({("a", 0)})  # a의 v로 들어가는 세그먼트만 릴리스됨
    reduced, edges, skipped, locked = released_item_anchor(tail, frontier_start, released)

    assert locked == 1
    assert skipped == 0
    assert reduced.visit_order == [("v", [("b", 1), ("c", 1)])]
    # a -> b, a -> c 두 간선이 고정으로 박힌다
    assert {(e.src, e.dst) for e in edges} == {
        (("a", 1), ("b", 0)),
        (("a", 1), ("c", 0)),
    }


def test_released_item_anchor_leaves_untouched_locations_free():
    """릴리스가 하나도 안 나간 위치는 그대로 자유롭다 — 잠금이 과잉 적용되지 않는다."""
    tail = PlanView(
        instance_id="free",
        revision=0,
        steps={"a": ["a0", "v", "a1"], "b": ["b0", "v", "b1"]},
        visit_order=[("v", [("a", 1), ("b", 1)])],
    )
    reduced, edges, _skipped, locked = released_item_anchor(
        tail, {"a": 0, "b": 0}, frozenset()
    )
    assert locked == 0
    assert edges == []
    assert reduced.visit_order == tail.visit_order


def test_item_level_lock_keeps_at_least_as_much_room_as_location_level_lock():
    """항목 층위 잠금이 위치 통째 잠금보다 자유부를 덜 지운다 — 실 인스턴스에서 확인."""
    cfg = load_scale()
    model = generate_roadmap(cfg, GenParams(seed=0))
    assignment = build_assignment(model, AssignParams(robots=4, tasks=6, seed=0))
    plan = plan_instance(model, assignment, PlanParams(horizon=96))
    assert not plan.unplanned
    view = plan_view_of(plan)

    graph = compile_graph(view, alt_generator="lift")
    prune_cyclic_alternatives(graph)
    frontier = {r: (r, 1) for r in sorted(view.steps) if len(view.steps[r]) > 2}
    for r in sorted(view.steps):
        frontier.setdefault(r, None)
    state = _FakeState(frontier, graph=graph)

    released = frozenset({(r, 0) for r in frontier if frontier[r] is not None})
    locations = frozenset(
        node
        for r, i in released
        for node in (view.steps[r][i], view.steps[r][i + 1])
    )
    coarse = recompile_once(view, state, locations)
    fine = recompile_once(view, state, frozenset(), released_segments=released)
    if coarse["unrepresentable"] or fine["unrepresentable"]:
        return  # 표현 불가면 비교 대상이 없다 — 지어내지 않는다
    assert fine["acyclic_combinations"] >= coarse["acyclic_combinations"]


def test_tail_view_and_recompile_agree_on_the_untruncated_case():
    """절단이 없으면(모든 frontier가 0) 꼬리 뷰가 원본과 같은 위치 집합을 준다."""
    view = PlanView(
        instance_id="notrunc",
        revision=0,
        steps={"a": ["a0", "v", "a1"], "b": ["b0", "v", "b1"]},
        visit_order=[("v", [("a", 1), ("b", 1)])],
    )
    tail = _tail_view(view, {"a": 0, "b": 0})
    assert tail.steps == view.steps
    assert tail.visit_order == view.visit_order


def test_percentiles_always_reports_the_denominator():
    stats = percentiles([0.001, 0.002, 0.003, 0.004])
    assert stats["n"] == 4
    assert stats["p50_ms"] > 0
    assert stats["max_ms"] == 4.0
    assert percentiles([]) == {"n": 0}


def test_mode_list_is_stable():
    """모드 이름은 원자료 키이므로 조용히 바뀌면 안 된다."""
    assert MODES == (
        "no_lock",
        "lock_release",
        "lock_depart",
        "lock_start",
        "lock_rel_item",
    )
