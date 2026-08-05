"""M2 — 에피소드 시계열 측정 시험. `remaining_reorder_room` · `run_episode` ·
M2′(§65-8) `recompiled_remaining_room`·`_tail_view`.

🔴 여기서 통과하는 것은 판정이 아니다. 시험을 쓴 사람과 승인하는 사람이 다르다.
"""

from __future__ import annotations

from types import SimpleNamespace

import pytest

from assignment_core import AssignParams, build_assignment
from episode_timeline import (
    _tail_view,
    exhaustive_acyclic_scan,
    recompiled_remaining_room,
    remaining_reorder_room,
    run_episode,
)
from exec_sim import Blockage
from plan_core import PlanParams, plan_instance
from protoscale import load_scale
from roadmap_core import GenParams, generate_roadmap
from sadg_core import PlanView, compile_graph, is_acyclic, plan_view_of, prune_cyclic_alternatives
from tick_core import make_state

CFG = load_scale()


def _pruned_graph(index=2, robots=6, tasks=8):
    """지정 인자에서 스위치 그룹이 살아남는 인스턴스. §60 스윕과 같은 생성 규칙."""
    model = generate_roadmap(CFG, GenParams(seed=index))
    assignment = build_assignment(model, AssignParams(robots=robots, tasks=tasks, seed=index))
    result = plan_instance(model, assignment, PlanParams())
    assert not result.unplanned
    graph = compile_graph(plan_view_of(result), alt_generator="lift")
    prune_cyclic_alternatives(graph)
    return model, graph


def test_remaining_reorder_room_matches_exhaustive_bruteforce_at_t0():
    """t=0(커밋 0건)의 acyclic_combinations이 **전수** 브루트포스와 같아야 한다."""
    model, graph = _pruned_graph()
    state = make_state(graph, {})
    n_groups, n_acyclic, n_total, exhaustive = remaining_reorder_room(state)
    assert exhaustive
    assert n_groups == len(graph.groups)

    # 브루트포스 — 검사 대상 코드와 다른 경로로 다시 센다.
    from itertools import product

    nodes = [s.key for s in graph.segments]
    per_group = [[a.edges for a in g.alternatives] for g in graph.groups]
    counts = [len(a) for a in per_group]
    total = 1
    for c in counts:
        total *= c
    acyclic = 0
    iterator = product(*[range(c) for c in counts]) if counts else iter([()])
    for combo in iterator:
        edges = list(graph.fixed)
        for gi, ai in enumerate(combo):
            edges.extend(per_group[gi][ai])
        if is_acyclic(nodes, edges):
            acyclic += 1
    assert n_total == total
    assert n_acyclic == acyclic


def test_remaining_reorder_room_shrinks_after_a_group_is_committed():
    """스위치 그룹이 하나라도 있는 인스턴스에서, 그 그룹을 손으로 커밋하면 미커밋 그룹 수가 준다."""
    model, graph = None, None
    for index in range(20):
        model, g = _pruned_graph(index=index)
        if g.groups:
            graph = g
            break
    assert graph is not None, "이 스윕 규모(0..19)에서 스위치 그룹이 있는 인스턴스를 못 찾았다"

    state = make_state(graph, {})
    n_groups_before, _, total_before, _ = remaining_reorder_room(state)
    assert n_groups_before >= 1

    group = graph.groups[0]
    original_idx = next(i for i, a in enumerate(group.alternatives) if a.is_original)
    state.committed_choice[group.location] = original_idx
    from tick_core import _reindex

    _reindex(state)
    n_groups_after, _, total_after, _ = remaining_reorder_room(state)
    assert n_groups_after == n_groups_before - 1
    assert total_after <= total_before


def test_run_episode_baseline_matches_t0_and_series_never_exceeds_it_for_pruned_graphs():
    """가지치기(prune)된 그래프에서 시계열이 t=0 값을 넘지 않는다(이 스윕 규모의 관찰 — 일반 정리가 아니다)."""
    model, graph = _pruned_graph(index=3)
    ep = run_episode(graph, model, CFG.nominal_speed_mps, blockage=None, max_ticks=5000)
    assert ep.baseline_acyclic_combinations >= 1
    for row in ep.rows:
        if row.exhaustive:
            assert row.acyclic_combinations <= ep.baseline_acyclic_combinations
    assert ep.makespan_s > 0.0
    assert not ep.problems


def test_run_episode_with_blockage_still_completes_and_stays_within_baseline():
    model, graph = _pruned_graph(index=3)
    ep = run_episode(
        graph, model, CFG.nominal_speed_mps,
        blockage=Blockage("r0", 1, 20.0), max_ticks=5000,
    )
    assert ep.makespan_s > 0.0
    for row in ep.rows:
        if row.exhaustive:
            assert row.acyclic_combinations <= ep.baseline_acyclic_combinations


def test_run_episode_records_one_row_per_completion_event():
    model, graph = _pruned_graph(index=3)
    ep = run_episode(graph, model, CFG.nominal_speed_mps, blockage=None, max_ticks=5000)
    assert len(ep.rows) == ep.completed_events
    assert ep.completed_events == len(graph.segments)


# --------------------------------------------------------------------------- §65-8 M2′


def _bundle_view_and_graph(index=3, robots=6, tasks=8):
    """`_pruned_graph`와 같은 인스턴스이되 재컴파일에 필요한 `view`도 함께 돌려준다."""
    model = generate_roadmap(CFG, GenParams(seed=index))
    assignment = build_assignment(model, AssignParams(robots=robots, tasks=tasks, seed=index))
    result = plan_instance(model, assignment, PlanParams())
    assert not result.unplanned
    view = plan_view_of(result)
    graph = compile_graph(view, alt_generator="lift")
    prune_cyclic_alternatives(graph)
    return model, view, graph


def test_tail_view_truncates_steps_and_remaps_visit_order_indices():
    view = PlanView(
        instance_id="t", revision=0,
        steps={"X": ["A", "B", "C"], "Y": ["D", "B", "E"]},
        visit_order=[("B", [("X", 1), ("Y", 1)])],
    )
    tail = _tail_view(view, frontier_start={"X": 1, "Y": 1})
    assert tail.steps == {"X": ["B", "C"], "Y": ["B", "E"]}
    # 두 로봇 다 index 0으로 재매핑된다 — 둘 다 새 시작점이므로 시험 자체가 아래 「표현 불가」
    # 시험이 겨누는 바로 그 모양을 만든다는 것을 확인한다.
    assert tail.visit_order == [("B", [("X", 0), ("Y", 0)])]


def test_tail_view_drops_locations_with_fewer_than_two_remaining_items():
    view = PlanView(
        instance_id="t", revision=0,
        steps={"X": ["A", "B", "C"], "Y": ["D", "B", "E"]},
        visit_order=[("B", [("X", 1), ("Y", 1)])],
    )
    # Y는 이미 B를 지났다(현재 위치가 E) — 잘린 뷰에는 X 혼자만 B를 갖는다 -> 그룹 기준(>=2) 미달
    tail = _tail_view(view, frontier_start={"X": 1, "Y": 2})
    assert tail.visit_order == []


def test_recompiled_remaining_room_matches_bruteforce_on_a_pruned_instance():
    """t=0(완료된 접두가 0)에서는 재컴파일 값이 고정 컴파일 값과 같아야 한다(자체 점검과 같은 성질)."""
    model, view, graph = _bundle_view_and_graph()
    state = make_state(graph, {})
    fixed_groups, fixed_acyclic, _, fixed_exhaustive = remaining_reorder_room(state)
    r_groups, r_acyclic, r_total, r_exhaustive, reason = recompiled_remaining_room(view, state)
    assert reason is None
    assert r_exhaustive and fixed_exhaustive
    assert r_groups == fixed_groups
    assert r_acyclic == fixed_acyclic


def test_recompiled_remaining_room_handles_two_robots_sharing_a_tail_start_location():
    """§65-8 실측으로 드러난 경계 사례 — 두 로봇의 꼬리 시작점이 같은 위치로 겹치면
    `compile_graph`가 `ValueError`를 던진다. `recompiled_remaining_room`은 이것을 삼키고
    `(-1, -1, -1, False, 사유)`로 정직하게 돌려줘야 한다(죽지 않는다).
    """
    view = PlanView(
        instance_id="t", revision=0,
        steps={"X": ["V", "W"], "Y": ["V", "Z"]},
        visit_order=[("V", [("X", 0), ("Y", 0)])],
    )
    # `state.frontier`만 읽으므로 최소 스텁으로 충분하다.
    state = SimpleNamespace(frontier={"X": ("X", 0), "Y": ("Y", 0)})
    groups, acyclic, total, exhaustive, reason = recompiled_remaining_room(view, state)
    assert reason is not None
    assert (groups, acyclic, total, exhaustive) == (-1, -1, -1, False)


def test_run_episode_with_view_populates_recompiled_fields_and_never_raises():
    """재컴파일을 켠 재생이 예외 없이 끝까지 돌고, 모든 행에 재컴파일 값이 채워진다
    (표현 불가 사건이 있어도 -1로 채워지지 「없음」으로 비지 않는다)."""
    for index in range(6):
        model, view, graph = _bundle_view_and_graph(index=index)
        ep = run_episode(graph, model, CFG.nominal_speed_mps, blockage=None,
                          max_ticks=5000, view=view)
        assert ep.makespan_s > 0.0
        for row in ep.rows:
            assert row.recompiled_acyclic_combinations is not None
            assert row.recompiled_groups is not None


def test_run_episode_recompiled_room_can_exceed_fixed_room_and_t0_baseline():
    """§65-8의 핵심 관찰 — **고정 컴파일과 달리 재컴파일 방은 t=0과 그 시점 고정 잔여방을
    넘을 수 있다.** 실측으로 이 조건을 만족하는 인스턴스(로봇 6·태스크 8, 인자 1)를 회귀
    시험으로 고정한다 — 반례가 나오면 이 시험이 아니라 §65-8이 갱신 대상이다.
    """
    model, view, graph = _bundle_view_and_graph(index=1)
    ep = run_episode(graph, model, CFG.nominal_speed_mps, blockage=None,
                      max_ticks=5000, view=view)
    comparable = [
        r for r in ep.rows
        if r.exhaustive and r.recompiled_exhaustive
        and r.acyclic_combinations >= 0 and (r.recompiled_acyclic_combinations or -1) >= 0
    ]
    assert any(r.recompiled_acyclic_combinations > r.acyclic_combinations for r in comparable)
    assert any(r.recompiled_acyclic_combinations > ep.baseline_acyclic_combinations for r in comparable)


def test_exhaustive_acyclic_scan_matches_bruteforce():
    _, _, graph = _bundle_view_and_graph(index=2)
    acyclic_n, total, exhaustive = exhaustive_acyclic_scan(graph)
    assert exhaustive

    from itertools import product

    nodes = [s.key for s in graph.segments]
    per_group = [[a.edges for a in g.alternatives] for g in graph.groups]
    counts = [len(a) for a in per_group]
    brute_total = 1
    for c in counts:
        brute_total *= c
    brute_acyclic = 0
    iterator = product(*[range(c) for c in counts]) if counts else iter([()])
    for combo in iterator:
        edges = list(graph.fixed)
        for gi, ai in enumerate(combo):
            edges.extend(per_group[gi][ai])
        if is_acyclic(nodes, edges):
            brute_acyclic += 1
    assert total == brute_total
    assert acyclic_n == brute_acyclic
