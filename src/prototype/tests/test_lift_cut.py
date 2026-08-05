"""LIFT-CUT(1단 컴파일 사후 단계) 시험 — 반례 W1·W3 · 조건 LC-4 · 표현 보존.

🔴 **여기서 통과하는 것은 판정이 아니다.** 시험을 쓴 사람과 승인하는 사람이 다르다.

이 파일이 박아 두려는 것 하나: **`Alternative.edges`(커버 관계)를 교집합의 재료로 쓰면
종점 항목이 안 막히는데 비순환 검사는 그대로 통과한다** — 조용히 깨지는 결함이므로
시험이 없으면 회귀가 안 잡힌다(`68b` §68b-4b).
"""

from __future__ import annotations

import lift_cut_check
from assignment_core import AssignParams, build_assignment
from plan_core import PlanParams, plan_instance
from protoscale import load_scale
from roadmap_core import GenParams, generate_roadmap
from sadg_core import (
    COVER_RELATION,
    TOTAL_ORDER,
    PlanView,
    apply_lift_cut,
    arriving_segment,
    compile_graph,
    cover_pairs,
    forced_pairs,
    head_items,
    is_acyclic,
    minimal_items,
    plan_view_of,
    total_order_pairs,
    transitive_closure,
)


def w1_view() -> PlanView:
    """반례 W1을 **실제 계획 뷰로** 만든다.

    r3가 `v`를 **종점**으로 삼으므로 `lift_2`(r3를 선두로)가 표현 불가로 걸러지고,
    남는 대안이 정확히 $\\{(x_1,x_2,x_3),(x_2,x_1,x_3)\\}$ = W1이다. 손으로 대안을 지우지
    않고도 W1이 나온다는 것 자체가 «W1이 모서리 사례가 아니다»의 한 사례다.
    """
    return PlanView(
        instance_id="w1",
        revision=0,
        steps={"r1": ["n1", "v", "m1"], "r2": ["n2", "v", "m2"], "r3": ["n3", "v"]},
        visit_order=[("v", [("r1", 1), ("r2", 1), ("r3", 1)])],
    )


def test_cover_pairs_are_a_subset_of_total_order_pairs():
    order = (("a", 1), ("b", 1), ("c", 1))
    assert cover_pairs(order) < total_order_pairs(order)
    assert len(total_order_pairs(order)) == 3
    assert len(cover_pairs(order)) == 2


def test_forced_pairs_reproduces_w1_witness():
    """$I_g=\\{13,23\\}$, $\\mathrm{tc}(J_g)=\\varnothing$ — `68` §R7-1-1의 W1 그대로."""
    graph = compile_graph(w1_view(), alt_generator="lift")
    group = graph.groups[0]
    assert len(group.alternatives) == 2
    assert graph.stats.dropped_unrepresentable == 1

    i_g = forced_pairs(group.alternatives, TOTAL_ORDER)
    j_g = forced_pairs(group.alternatives, COVER_RELATION)
    assert i_g == {(("r1", 1), ("r3", 1)), (("r2", 1), ("r3", 1))}
    assert j_g == set()
    assert j_g < i_g


def test_forced_pairs_is_subset_of_the_original_alternative_order():
    """따름정리 LC-3의 전제 $I_g\\subseteq\\prec_{a_0(g)}$ — 원 순서가 대안 집합에 있으므로."""
    graph = compile_graph(w1_view(), alt_generator="lift")
    group = graph.groups[0]
    original = next(a for a in group.alternatives if a.is_original)
    assert forced_pairs(group.alternatives, TOTAL_ORDER) <= total_order_pairs(original.order)


def test_lift_cut_with_total_order_blocks_the_terminal_item_arrival():
    """정리 LIFT-CUT 결론 (i) — 종점 항목의 **들어오는 세그먼트**가 $t{=}0$부터 막힌다."""
    view = w1_view()
    graph = compile_graph(view, alt_generator="lift")
    stats = apply_lift_cut(graph, view, representation=TOTAL_ORDER)

    terminal_arrival = arriving_segment(view, ("r3", 1))
    blocked = {e.dst for e in graph.fixed}
    assert terminal_arrival in blocked
    assert stats.terminal_items_blocked == 1
    assert stats.groups_with_terminal_item == 1


def test_lift_cut_with_cover_relation_does_not_block_the_terminal_item():
    """🔴 회귀 방지 — `Alternative.edges`(커버 관계) 교집합을 쓰면 아무것도 승격되지 않는다."""
    view = w1_view()
    graph = compile_graph(view, alt_generator="lift")
    stats = apply_lift_cut(graph, view, representation=COVER_RELATION)

    terminal_arrival = arriving_segment(view, ("r3", 1))
    blocked = {e.dst for e in graph.fixed}
    assert terminal_arrival not in blocked
    assert stats.promoted_edges == 0
    assert stats.terminal_items_blocked == 0


def test_cover_relation_variant_still_passes_the_acyclicity_check():
    """🔴 **조용히 깨진다는 것**을 시험으로 고정한다 — 비순환 검사는 두 구현을 구별하지 못한다."""
    view = w1_view()
    results = {}
    for representation in (TOTAL_ORDER, COVER_RELATION):
        graph = compile_graph(view, alt_generator="lift")
        apply_lift_cut(graph, view, representation=representation)
        nodes = [s.key for s in graph.segments]
        ok = []
        for alt in graph.groups[0].alternatives if graph.groups else []:
            ok.append(is_acyclic(nodes, list(graph.fixed) + list(alt.edges)))
        results[representation] = all(ok) if ok else is_acyclic(nodes, list(graph.fixed))
    assert results[TOTAL_ORDER] is True
    assert results[COVER_RELATION] is True  # 검사기가 둘을 가르지 못한다


def _acyclic_combination_set(graph):
    from itertools import product

    nodes = [s.key for s in graph.segments]
    base = list(graph.fixed)
    per_group = [[a.edges for a in g.alternatives] for g in graph.groups]
    out = set()
    for combo in product(*[range(len(a)) for a in per_group]) if per_group else [()]:
        edges = list(base)
        for gi, ai in enumerate(combo):
            edges.extend(per_group[gi][ai])
        if is_acyclic(nodes, edges):
            out.add(combo)
    return out


def test_lift_cut_preserves_the_acyclic_combination_set_on_a_real_instance():
    """승격은 **도달 관계를 바꾸지 않아야** 한다 — 같은 조합이 같은 판정을 받는가."""
    cfg = load_scale()
    checked = 0
    for index in range(6):
        model = generate_roadmap(cfg, GenParams(seed=index))
        assignment = build_assignment(model, AssignParams(robots=4, tasks=6, seed=index))
        plan = plan_instance(model, assignment, PlanParams(horizon=96))
        if plan.unplanned:
            continue
        view = plan_view_of(plan)
        before = compile_graph(view, alt_generator="lift")
        after = compile_graph(view, alt_generator="lift")
        apply_lift_cut(after, view, representation=TOTAL_ORDER, fold_degenerate=False)
        assert [len(g.alternatives) for g in before.groups] == [
            len(g.alternatives) for g in after.groups
        ]
        assert _acyclic_combination_set(before) == _acyclic_combination_set(after)
        checked += 1
    assert checked >= 3


def test_transitive_closure_is_idempotent_on_an_intersection():
    """보조정리 LC-1 — 교집합은 **이미 이행적**이므로 폐포가 값을 바꾸지 않는다."""
    graph = compile_graph(w1_view(), alt_generator="lift")
    i_g = forced_pairs(graph.groups[0].alternatives, TOTAL_ORDER)
    assert transitive_closure(i_g) == i_g


def test_w3_family_violates_lc4_but_the_canonical_generator_does_not():
    """반례 W3은 **임의 순열족**의 성질이다 — 정본 생성기 부분족에서는 $n\\le7$ 전수 위반 0."""
    w3 = [("x1", "x2", "x3"), ("x3", "x2", "x1")]
    i_g = lift_cut_check.intersect(w3, TOTAL_ORDER)
    assert lift_cut_check.minima(i_g, w3[0]) == {"x1", "x2", "x3"}
    assert lift_cut_check.heads(w3) == {"x1", "x3"}

    for n in range(3, 8):
        for family in lift_cut_check.canonical_subfamilies(n, "A"):
            pairs = lift_cut_check.intersect(family, TOTAL_ORDER)
            assert lift_cut_check.minima(pairs, family[0]) == lift_cut_check.heads(family)


def test_reading_b_also_satisfies_lc4_exhaustively():
    """읽기 B(자유 구간 선두)도 같은 범위에서 위반 0 — «앞»의 모호성이 답을 바꾸지 않는다."""
    for n in range(4, 8):
        for fixed in range(1, n - 1):
            for family in lift_cut_check.canonical_subfamilies(n, "B", fixed):
                pairs = lift_cut_check.intersect(family, TOTAL_ORDER)
                assert lift_cut_check.minima(pairs, family[0]) == lift_cut_check.heads(family)


def test_arbitrary_permutation_pairs_do_violate_lc4():
    """대조군에서는 실제로 깨진다 — 위 두 시험이 공허참이 아님을 확인한다."""
    violations = 0
    for family in lift_cut_check.arbitrary_pairs(3):
        pairs = lift_cut_check.intersect(family, TOTAL_ORDER)
        if lift_cut_check.minima(pairs, family[0]) != lift_cut_check.heads(family):
            violations += 1
    assert violations > 0


def test_head_items_and_minimal_items_agree_on_the_w1_family():
    graph = compile_graph(w1_view(), alt_generator="lift")
    group = graph.groups[0]
    i_g = forced_pairs(group.alternatives, TOTAL_ORDER)
    assert minimal_items(i_g, group.original) == head_items(group.alternatives)
