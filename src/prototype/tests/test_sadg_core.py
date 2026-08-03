"""S3 알고리즘 코어 시험 — 세그먼트·의존·스위치 그룹·전수 비순환·음성 시험.

🔴 **여기서 통과하는 것은 판정이 아니다.** 시험을 쓴 사람과 승인하는 사람이 다르다.
"""

import json
import os

import pytest
from jsonschema import Draft202012Validator

from assignment_core import AssignParams, build_assignment
from boundary_adapter import (
    constraints_from_boundary,
    constraints_to_boundary,
    plan_from_boundary,
    plan_to_boundary,
)
from plan_core import PlanParams, plan_instance
from protoscale import load_scale
from roadmap_core import GenParams, generate_roadmap
from sadg_core import (
    INTER_ROBOT,
    _kahn,
    arriving_segment,
    departing_segment,
    INTRA_ROBOT,
    ExecGraph,
    PlanView,
    Precedence,
    Segment,
    build_intra_deps,
    build_segments,
    check_all_switch_combinations,
    compile_graph,
    critical_path_segments,
    first_occurrence_indices,
    gen_adjacent_swap,
    gen_head_lift,
    heads_of,
    inject_cycle,
    is_acyclic,
    nominal_edges,
    plan_view_of,
    preserves_intra_robot_order,
    prune_cyclic_alternatives,
    self_check,
)

HERE = os.path.dirname(os.path.abspath(__file__))
SCHEMA_DIR = os.path.normpath(os.path.join(HERE, "..", "..", "mrs_msgs", "schema"))
CFG = load_scale()


# --------------------------------------------------------------------- 합성 입력


def _crossing_view():
    """세 로봇이 공유 정점 V를 하나씩 지난다. **고정된 항목이 하나도 없다.**"""
    return PlanView(
        instance_id="synthetic_crossing",
        revision=0,
        steps={
            "r0": ["A0", "V", "B0"],
            "r1": ["A1", "V", "B1"],
            "r2": ["A2", "V", "B2"],
        },
        visit_order=[("V", [("r0", 1), ("r1", 1), ("r2", 1)])],
    )


def _revisit_view():
    """r0이 V를 두 번 방문한다 — 계약 §1-2(3)이 명시로 허용한 자리."""
    return PlanView(
        instance_id="synthetic_revisit",
        revision=0,
        steps={"r0": ["A", "V", "B", "V", "C"], "r1": ["D", "V", "E"]},
        visit_order=[("V", [("r0", 1), ("r1", 1), ("r0", 3)])],
    )


def _pipeline(seed=0, robots=6, tasks=8):
    model = generate_roadmap(CFG, GenParams(seed=seed))
    assignment = build_assignment(model, AssignParams(robots=robots, tasks=tasks, seed=seed))
    return model, plan_instance(model, assignment, PlanParams())


# --------------------------------------------------------------------- 세그먼트


def test_segment_count_is_steps_minus_one():
    """계약 §1-2(6). 단계 1개 = 세그먼트 0개도 합법이다."""
    view = _crossing_view()
    view.steps["r3"] = ["Z"]  # 세그먼트 0개 로봇
    segments = build_segments(view)
    for robot, nodes in view.steps.items():
        assert sum(1 for s in segments if s.robot == robot) == len(nodes) - 1
    assert not [s for s in segments if s.robot == "r3"]


def test_segment_endpoints_follow_the_step_sequence():
    segments = [s for s in build_segments(_revisit_view()) if s.robot == "r0"]
    assert [(s.number, s.src_node, s.dst_node) for s in segments] == [
        (0, "A", "V"), (1, "V", "B"), (2, "B", "V"), (3, "V", "C")
    ]


def test_intra_robot_deps_chain_consecutive_segments():
    deps = build_intra_deps(_revisit_view())
    assert all(d.kind == INTRA_ROBOT for d in deps)
    assert [(d.src, d.dst) for d in deps if d.src[0] == "r0"] == [
        (("r0", 0), ("r0", 1)), (("r0", 1), ("r0", 2)), (("r0", 2), ("r0", 3))
    ]


def test_inter_robot_deps_come_only_from_visit_order():
    """Type-2의 근거는 `visit_order`뿐이다 — 공유 위치를 지우면 로봇 간 의존이 사라진다."""
    view = _crossing_view()
    view.visit_order = []
    graph = compile_graph(view)
    assert graph.groups == []
    assert all(d.kind == INTRA_ROBOT for d in graph.fixed)


# --------------------------------------------------------------------- 대안 생성


def test_first_occurrence_picks_each_robot_once():
    items = (("r0", 1), ("r1", 1), ("r0", 3), ("r2", 2))
    assert first_occurrence_indices(items) == [0, 1, 3]


def test_head_lift_keeps_original_order_in_the_set():
    """`lift_0 = pi_v` — 원 순서는 **항상** A_g에 있다. C4의 충족 가능성이 여기 걸린다."""
    items = (("r0", 1), ("r1", 1), ("r2", 1))
    orders = [order for _, order in gen_head_lift(items)]
    assert items in orders


def test_head_lift_preserves_intra_robot_relative_order():
    """정리 H1 / 불변식 I-9 — 필터가 아니라 **생성**으로 성립한다."""
    items = (("r0", 1), ("r1", 1), ("r0", 3), ("r2", 2), ("r1", 4))
    for _, order in gen_head_lift(items):
        assert preserves_intra_robot_order(items, order)


def test_head_lift_size_equals_distinct_robot_count_when_nothing_is_pinned():
    """|A_g| = k_d(v) (정리 H2). **고정된 항목이 없을 때만 성립한다** — 아래 시험을 보라."""
    graph = compile_graph(_crossing_view(), alt_generator="lift")
    assert len(graph.groups) == 1
    assert len(graph.groups[0].alternatives) == 3  # k_d = 3


def test_heads_are_complete_when_nothing_is_pinned():
    """정리 H3 — Heads(A_g)가 그룹 구성원 전체다."""
    graph = compile_graph(_crossing_view(), alt_generator="lift")
    assert heads_of(graph.groups[0]) == {"r0", "r1", "r2"}


def test_start_item_pins_the_group_to_a_single_alternative():
    """🔴 순번 0 항목은 **들어오는 세그먼트가 없다** — 그 항목이 선두면 다른 순서를 표현할 수 없다.

    이것은 표현의 결함이 아니라 물리적 사실이다(로봇이 이미 그 정점에 서 있다).
    그러나 `|A_g| = k_d(v)`와 정리 H3(Heads 완비)이 이 자리에서 성립하지 않는다.
    """
    view = PlanView(
        instance_id="pinned_start",
        revision=0,
        steps={"r0": ["V", "B0"], "r1": ["A1", "V", "B1"]},
        visit_order=[("V", [("r0", 0), ("r1", 1)])],
    )
    graph = compile_graph(view, alt_generator="lift")
    assert graph.groups == []  # |A_g| = 1 이라 그룹으로 발행하지 않는다
    assert graph.stats.groups_folded_single == 1
    assert graph.stats.dropped_unrepresentable == 1


def test_terminal_item_cannot_be_lifted_to_the_front():
    """🔴 마지막 단계 항목은 **떠나는 세그먼트가 없다**(계약 §1-2(7): 마지막 정점은 해제되지 않는다)."""
    view = PlanView(
        instance_id="pinned_terminal",
        revision=0,
        steps={"r0": ["A0", "V"], "r1": ["A1", "V", "B1"]},
        visit_order=[("V", [("r1", 1), ("r0", 1)])],
    )
    graph = compile_graph(view, alt_generator="lift")
    assert graph.groups == []
    assert graph.stats.pinned_terminal_items == 1
    assert graph.stats.dropped_unrepresentable == 1


def test_adjacent_swap_filters_same_robot_pairs():
    """구판(`11c_r2` §R2-1) — 인접 두 항목이 같은 로봇이면 교환하지 않는다."""
    items = (("r0", 1), ("r0", 3), ("r1", 1))
    orders = [order for _, order in gen_adjacent_swap(items)]
    assert items in orders
    assert (("r0", 3), ("r0", 1), ("r1", 1)) not in orders
    assert len(orders) == 2


def test_two_generators_are_swapped_by_one_argument():
    """확장점 E3 — 인자 한 줄로 갈리고 **경계에 나가는 필드 집합이 달라지지 않는다**."""
    view = _revisit_view()
    a = constraints_to_boundary(compile_graph(view, alt_generator="lift"))
    b = constraints_to_boundary(compile_graph(view, alt_generator="swap"))
    assert set(a) == set(b)

    def _keys(doc):
        out = set()
        stack = [doc]
        while stack:
            node = stack.pop()
            if isinstance(node, dict):
                out.update(node)
                stack.extend(node.values())
            elif isinstance(node, list):
                stack.extend(node)
        return out

    assert _keys(a) == _keys(b)


# --------------------------------------------------------------------- 중복 의존


def test_revisit_never_produces_a_same_robot_ordering_dependency():
    """🔴 §55-7-1 읽기 (다) — 같은 로봇 쌍에는 **아무것도 추가하지 않는다.**

    계약 §2-1의 술어(`ordering` ⟺ 두 세그먼트의 `robot`이 다르다)를 지키는 유일한 길이며,
    Type-1이 이미 그 순서를 주므로 도달 관계가 바뀌지 않는다.
    """
    view = _revisit_view()
    graph = compile_graph(view, alt_generator="lift")
    assert graph.stats.same_robot_ordering_edges == 0
    for g in graph.groups:
        for alt in g.alternatives:
            assert all(e.src[0] != e.dst[0] for e in alt.edges)
    assert check_all_switch_combinations(graph).ok


def test_type_predicate_holds_everywhere_in_the_boundary_artifact():
    """계약 §2-1의 **검사 가능한 술어**를 산출물 전체에 건다(`55` §55-6 S7)."""
    for seed in range(5):
        _, result = _pipeline(seed=seed)
        doc = constraints_to_boundary(compile_graph(plan_view_of(result)))
        robot = {s["id"]: s["robot"] for s in doc["segments"]}
        deps = list(doc["dependencies"])
        for g in doc["switch_groups"]:
            for alt in g["alternatives"]:
                deps.extend(alt["dependencies"])
        assert deps
        for d in deps:
            same = robot[d["from"]] == robot[d["to"]]
            assert (d["type"] == "sequential") == same, d


def test_dropping_same_robot_edges_preserves_reachability():
    """🔴 (다)가 «아무것도 바꾸지 않는다»는 논증을 도달 폐포로 확인한다.

    같은 로봇 $i$가 순번 $p<q$에서 $v$를 지날 때 필요한 간선은 `i#p -> i#(q-1)`이고,
    Type-1이 `i#p -> ... -> i#(q-1)`을 이미 준다. 정리 H1이 $p<q$를 모든 대안에서 보존한다.
    """
    view = _revisit_view()
    graph = compile_graph(view, alt_generator="lift")
    nodes = [s.key for s in graph.segments]

    def closure(edges):
        adjacency = {n: set() for n in nodes}
        for e in edges:
            adjacency[e.src].add(e.dst)
        ok, order = _kahn(nodes, edges)
        assert ok
        reach = {}
        for n in reversed(order):
            acc = set()
            for m in adjacency[n]:
                acc.add(m)
                acc |= reach[m]
            reach[n] = acc
        return reach

    # 모든 대안에 대해 «같은 로봇 쌍의 간선을 넣었다면» 무엇이 더해졌을지 직접 만들어 본다
    seen_extra = 0
    for g in graph.groups:
        for alt in g.alternatives:
            extra = []
            for a, b in zip(alt.order, alt.order[1:]):
                if a[0] != b[0]:
                    continue
                src, dst = departing_segment(view, a), arriving_segment(view, b)
                assert src is not None and dst is not None, "같은 로봇 쌍은 언제나 표현 가능하다"
                extra.append(Precedence(src, dst, INTER_ROBOT))
            if not extra:
                continue
            seen_extra += len(extra)
            base = list(graph.fixed) + list(alt.edges)
            assert closure(base) == closure(base + extra)
    assert seen_extra, "재방문 인스턴스이므로 넣었다면 생겼을 간선이 있어야 한다"


def test_self_check_catches_a_same_robot_ordering_dependency():
    """음성 시험 — 술어를 어긴 의존을 심으면 자체 점검이 잡는다."""
    view = _crossing_view()
    graph = compile_graph(view)
    graph.fixed.append(Precedence(("r0", 0), ("r0", 1), INTER_ROBOT))
    assert any("§2-1 술어 위반" in p for p in self_check(graph, view))


def test_self_check_catches_a_cross_robot_sequential_dependency():
    view = _crossing_view()
    graph = compile_graph(view)
    graph.fixed.append(Precedence(("r0", 0), ("r1", 1), INTRA_ROBOT))
    assert any("로봇 내부 의존이 서로 다른 로봇" in p for p in self_check(graph, view))


def test_acyclicity_tolerates_exactly_duplicated_edges():
    nodes = [("r0", 0), ("r0", 1)]
    edge = Precedence(("r0", 0), ("r0", 1), INTRA_ROBOT)
    assert is_acyclic(nodes, [edge, edge, edge])


# --------------------------------------------------------------------- 전수 비순환


def test_all_switch_combinations_are_checked_exhaustively():
    graph = compile_graph(_crossing_view(), alt_generator="lift")
    report = check_all_switch_combinations(graph)
    assert report.exhaustive
    assert report.checked == report.total == 3
    assert report.ok


def test_sampling_downgrade_is_reported_as_not_exhaustive():
    """🔴 표본 통과를 증명으로 보고하지 않기 위해 강등 사실이 값으로 남아야 한다."""
    graph = compile_graph(_crossing_view(), alt_generator="lift")
    report = check_all_switch_combinations(graph, max_combinations=1, samples=2)
    assert not report.exhaustive
    assert report.checked < report.total


@pytest.mark.parametrize("gen", ["lift", "swap"])
def test_injected_cycle_is_caught(gen):
    """음성 시험 — 잡지 못하는 검사기의 «통과»는 아무것도 뜻하지 않는다."""
    graph = compile_graph(_crossing_view(), alt_generator=gen)
    assert check_all_switch_combinations(graph).ok
    inject_cycle(graph)
    report = check_all_switch_combinations(graph)
    assert not report.ok
    assert report.failing_combo is not None


def test_checking_only_the_nominal_combination_would_miss_the_cycle():
    """🔴 `src/CLAUDE.md` 불변식 4 — «현재 선택만 검사하는 시험»이 왜 무력한지를 값으로 보인다."""
    graph = compile_graph(_crossing_view(), alt_generator="lift")
    inject_cycle(graph)
    nodes = [s.key for s in graph.segments]
    assert is_acyclic(nodes, nominal_edges(graph))  # 공칭 조합만 보면 통과한다
    assert not check_all_switch_combinations(graph).ok  # 전수로 보면 걸린다


def test_pruning_makes_every_combination_acyclic_and_keeps_the_original_order():
    _, result = _pipeline()
    graph = compile_graph(plan_view_of(result), alt_generator="lift")
    prune_cyclic_alternatives(graph)
    report = check_all_switch_combinations(graph)
    assert report.ok and report.exhaustive
    for g in graph.groups:
        assert any(a.is_original for a in g.alternatives)


# --------------------------------------------------------------------- 임계 경로


def test_critical_path_counts_segments_on_the_longest_chain():
    view = PlanView(
        instance_id="chain", revision=0,
        steps={"r0": ["A", "B", "C", "D"]}, visit_order=[],
    )
    assert critical_path_segments(compile_graph(view)) == 3


def test_critical_path_is_undefined_on_a_cyclic_graph():
    """값을 지어내지 않는다 — 순환이면 −1을 돌려준다."""
    graph = compile_graph(_crossing_view())
    inject_cycle(graph)
    for g in graph.groups:  # 순환 대안을 공칭으로 만든다
        g.alternatives = [g.alternatives[-1]]
        object.__setattr__(g.alternatives[0], "is_original", True)
    assert critical_path_segments(graph) == -1


# --------------------------------------------------------------------- 자체 점검


def test_self_check_catches_a_dangling_dependency():
    graph = compile_graph(_crossing_view())
    graph.fixed.append(Precedence(("r9", 0), ("r0", 0), INTER_ROBOT))
    assert self_check(graph, _crossing_view())


def test_self_check_catches_a_broken_intra_robot_order():
    """불변식 I-9를 어긴 대안을 억지로 넣으면 잡힌다."""
    view = _revisit_view()
    graph = compile_graph(view, alt_generator="lift")
    group = graph.groups[0]
    bad = group.alternatives[-1]
    object.__setattr__(bad, "order", (("r0", 3), ("r1", 1), ("r0", 1)))
    assert any("I-9" in p for p in self_check(graph, view))


def test_self_check_is_clean_on_the_pipeline_output():
    model, result = _pipeline()
    view = plan_view_of(result)
    adjacency = set()
    for e in model.edges:
        adjacency.add((e.u, e.v))
        if e.traversal == "bidirectional":
            adjacency.add((e.v, e.u))
    assert self_check(compile_graph(view), view, adjacency) == []


# --------------------------------------------------------------------- 결정론


@pytest.mark.parametrize("gen", ["lift", "swap"])
def test_same_arguments_give_the_same_artifact(gen):
    _, result = _pipeline(seed=3)
    view = plan_view_of(result)
    first = constraints_to_boundary(compile_graph(view, alt_generator=gen))
    second = constraints_to_boundary(compile_graph(view, alt_generator=gen))
    assert first == second


# --------------------------------------------------------------------- 경계 산출물


@pytest.mark.parametrize("seed", [0, 1, 2, 3, 4])
def test_execution_constraints_passes_structural_validation(seed):
    _, result = _pipeline(seed=seed)
    doc = constraints_to_boundary(compile_graph(plan_view_of(result)))
    with open(os.path.join(SCHEMA_DIR, "execution_constraints.schema.json"), encoding="utf-8") as h:
        schema = json.load(h)
    assert [
        f"{list(e.path)}: {e.message}" for e in Draft202012Validator(schema).iter_errors(doc)
    ] == []


def test_boundary_carries_no_algorithm_parameter():
    """🔴 대안 생성 규칙·Heads·lambda*·시드가 경계 문자열에 나타나면 SC-4가 무너진다."""
    _, result = _pipeline()
    graph = compile_graph(plan_view_of(result), alt_generator="lift")
    text = json.dumps(constraints_to_boundary(graph)).lower()
    for banned in ("lift", "firstocc", "head", "lambda", "tau_nom", "seed", "horizon",
                   "generator", "critical_path", "solver", "timeout", "z_g"):
        assert banned not in text


def test_alternative_ids_do_not_encode_the_generation_rule():
    _, result = _pipeline()
    a = constraints_to_boundary(compile_graph(plan_view_of(result), alt_generator="lift"))
    b = constraints_to_boundary(compile_graph(plan_view_of(result), alt_generator="swap"))

    def ids(doc):
        return {alt["id"] for g in doc["switch_groups"] for alt in g["alternatives"]}

    assert ids(a) <= {f"a{i}" for i in range(8)}
    assert ids(b) <= {f"a{i}" for i in range(8)}


def test_group_location_exists_in_upstream_visit_order():
    _, result = _pipeline()
    plan_doc = plan_to_boundary(result)
    graph = compile_graph(plan_view_of(result))
    locations = {v["location"] for v in plan_doc["visit_order"]}
    for g in constraints_to_boundary(graph)["switch_groups"]:
        assert g["location"] in locations


def test_emitted_groups_always_have_at_least_two_alternatives():
    """|A_g|=1이면 발행하지 않는다 — 하류가 «선택 대기»로 읽고 영원히 커밋되지 않는다."""
    for seed in range(5):
        _, result = _pipeline(seed=seed)
        doc = constraints_to_boundary(compile_graph(plan_view_of(result)))
        for g in doc["switch_groups"]:
            assert len(g["alternatives"]) >= 2


def test_segment_id_first_group_equals_the_robot_field():
    """A10 — `x-derived-from`이 층 1에서 잡는 자리를 산출 측에서도 지킨다."""
    _, result = _pipeline()
    for s in constraints_to_boundary(compile_graph(plan_view_of(result)))["segments"]:
        assert s["id"].rsplit("#", 1)[0] == s["robot"]


# --------------------------------------------------------------------- 어댑터 왕복


@pytest.mark.parametrize("gen", ["lift", "swap"])
def test_constraints_roundtrip_is_identity_on_the_boundary_side(gen):
    _, result = _pipeline(seed=1)
    doc = constraints_to_boundary(compile_graph(plan_view_of(result), alt_generator=gen))
    assert constraints_to_boundary(constraints_from_boundary(doc)) == doc


def test_constraints_roundtrip_preserves_the_graph_meaning():
    """왕복이 **그래프의 뜻**을 지키는가 — 세그먼트 집합·의존 집합·조합 비순환성."""
    _, result = _pipeline(seed=2)
    graph = compile_graph(plan_view_of(result))
    back = constraints_from_boundary(constraints_to_boundary(graph))
    assert back.segment_keys() == graph.segment_keys()
    assert {(d.src, d.dst, d.kind) for d in back.fixed} == {
        (d.src, d.dst, d.kind) for d in graph.fixed
    }
    before = check_all_switch_combinations(graph)
    after = check_all_switch_combinations(back)
    assert (before.ok, before.total) == (after.ok, after.total)


def test_roundtrip_loses_internal_accounting_and_that_is_the_point():
    """🔴 왕복은 항등이 아니다 — `lambda*`·생성기·«어느 것이 원 순서인가»가 경계를 건너지 않는다."""
    _, result = _pipeline()
    graph = compile_graph(plan_view_of(result))
    back = constraints_from_boundary(constraints_to_boundary(graph))
    assert graph.stats.critical_path_segments > 0
    assert back.stats.critical_path_segments == 0
    assert all(not a.is_original for g in back.groups for a in g.alternatives)


def test_from_boundary_rejects_a_foreign_envelope():
    with pytest.raises(ValueError):
        constraints_from_boundary({"schema": "mrs.discrete_plan"})


def test_segment_id_encoding_survives_a_robot_name_with_digits():
    graph = ExecGraph(
        instance_id="x", revision=0,
        segments=[Segment("robot12", 7, "A", "B")],
        fixed=[], groups=[],
    )
    doc = constraints_to_boundary(graph)
    assert doc["segments"][0]["id"] == "robot12#7"
    assert constraints_from_boundary(doc).segments[0].key == ("robot12", 7)


# --------------------------------------------------------------------- 재생(막힘·회복)


def _sim_setup(seed=0):
    from exec_sim import segment_durations

    model, result = _pipeline(seed=seed)
    graph = constraints_from_boundary(
        constraints_to_boundary(compile_graph(plan_view_of(result)))
    )
    return model, graph, segment_durations(graph, model, CFG.nominal_speed_mps)


def test_replay_uses_the_boundary_artifact_only():
    """🔴 S2와 다른 점 — 계획기를 다시 돌리지 않고 경계 아티팩트만으로 재생한다."""
    from exec_sim import simulate, vertex_occupancy_conflicts

    _, graph, durations = _sim_setup()
    sim = simulate(graph, durations)
    assert sim.makespan > 0
    assert vertex_occupancy_conflicts(graph, sim) == []


def test_makespan_of_a_single_chain_is_the_sum_of_durations():
    from exec_sim import simulate

    view = PlanView(instance_id="chain", revision=0,
                    steps={"r0": ["A", "B", "C"]}, visit_order=[])
    graph = compile_graph(view)
    durations = {("r0", 0): 2.0, ("r0", 1): 3.0}
    assert simulate(graph, durations).makespan == pytest.approx(5.0)


def test_blockage_propagates_only_along_the_dependency_graph():
    """차단이 **순서를 따라** 전파된다 — 의존이 없는 로봇은 늦지 않는다."""
    from exec_sim import Blockage, simulate

    _, graph, durations = _sim_setup()
    free = simulate(graph, durations)
    blocked = simulate(graph, durations, [Blockage("r0", 4, 45.0)])
    assert blocked.makespan > free.makespan
    delayed, untouched = 0, 0
    for robot in sorted({s.robot for s in graph.segments}):
        keys = [s.key for s in graph.segments if s.robot == robot]
        gap = max(blocked.end[k] for k in keys) - max(free.end[k] for k in keys)
        delayed += gap > 0
        untouched += gap == 0
    assert delayed >= 1 and untouched >= 1


def test_blocked_replay_still_has_no_vertex_conflict():
    """🔴 **막혀도 순서를 지킨다** — 이것이 이 그림이 보여 주려는 전부다."""
    from exec_sim import Blockage, simulate, vertex_occupancy_conflicts

    _, graph, durations = _sim_setup()
    sim = simulate(graph, durations, [Blockage("r0", 4, 45.0)])
    assert vertex_occupancy_conflicts(graph, sim) == []


def test_replay_refuses_a_cyclic_graph():
    """순환하면 재생하지 않는다 — 그것이 데드락의 정의다. 조용히 계속하지 않는다."""
    from exec_sim import segment_durations, simulate

    model, result = _pipeline()
    graph = compile_graph(plan_view_of(result))
    durations = segment_durations(graph, model, CFG.nominal_speed_mps)
    graph.fixed.append(Precedence(graph.segments[1].key, graph.segments[0].key, INTER_ROBOT))
    with pytest.raises(ValueError):
        simulate(graph, durations)
