"""S-A 시나리오 생성기 시험 — 로드맵·배정 자체 점검 + 계획기와의 연동.

🔴 여기서 통과하는 것은 판정이 아니다. 시험을 쓴 사람과 승인하는 사람이 다르다.
"""

from __future__ import annotations

import itertools

import pytest

from assignment_core import validate_assignment
from gen_scenario_a import CAPACITIES, TOPOLOGIES, build_assignment, build_roadmap, instance_id
from plan_core import PlanParams, plan_instance
from protoscale import load_scale
from roadmap_core import validate_model
from sadg_core import compile_graph, plan_view_of


@pytest.fixture(scope="module")
def cfg():
    return load_scale()


@pytest.mark.parametrize("topology,capacity", list(itertools.product(TOPOLOGIES, CAPACITIES)))
def test_roadmap_and_assignment_self_check_clean(cfg, topology, capacity):
    model = build_roadmap(cfg, topology, capacity, chain_nodes=2)
    assignment = build_assignment(topology, capacity)
    assert validate_model(model) == []
    assert validate_assignment(model, assignment) == []


@pytest.mark.parametrize("topology,capacity", list(itertools.product(TOPOLOGIES, CAPACITIES)))
def test_instance_id_matches_between_roadmap_and_assignment(cfg, topology, capacity):
    model = build_roadmap(cfg, topology, capacity, chain_nodes=2)
    assignment = build_assignment(topology, capacity)
    assert model.instance_id == assignment.instance_id == instance_id(topology, capacity)


@pytest.mark.parametrize("topology,capacity", list(itertools.product(TOPOLOGIES, CAPACITIES)))
def test_all_four_robots_reach_their_goal(cfg, topology, capacity):
    """A,B,C,D 전부 goal_reached — 계획기를 갈아 끼우지 않고도 인스턴스가 풀린다."""
    model = build_roadmap(cfg, topology, capacity, chain_nodes=2)
    assignment = build_assignment(topology, capacity)
    result = plan_instance(model, assignment, PlanParams())
    assert not result.unplanned
    assert {p.robot for p in result.paths} == {"A", "B", "C", "D"}
    assert all(p.terminal == "goal_reached" for p in result.paths)


def test_priority_order_places_a_before_b_and_c_before_d(cfg):
    """기본 우선순위 규칙(most_goals, 동점은 robot 오름차순)이 A>B, C>D를 만든다.

    시나리오의 요구("같은 방향의 순서는 지켜져야 한다")를 계획기 인자를 바꾸지 않고 얻는다는
    주장의 근거 — 우선순위 순서 자체를 확인한다.
    """
    model = build_roadmap(cfg, "single", 2, chain_nodes=2)
    assignment = build_assignment("single", 2)
    result = plan_instance(model, assignment, PlanParams())
    order = result.priority_order
    assert order.index("A") < order.index("B")
    assert order.index("C") < order.index("D")


def test_left_queue_is_single_file_by_construction(cfg):
    """B의 시작(SB)에서 L로 가는 유일한 경로가 SA를 지난다 — 물리적으로 B가 A 뒤에 있다."""
    model = build_roadmap(cfg, "single", 2, chain_nodes=2)
    incident = {(e.u, e.v) for e in model.edges} | {(e.v, e.u) for e in model.edges}
    assert ("SB", "SA") in incident
    assert ("SA", "L") in incident
    # SB가 L에 직접 붙어 있지 않다(반드시 SA를 거친다)
    assert ("SB", "L") not in incident


def test_single_topology_head_lock_independent_of_capacity(cfg):
    """단일 간선 토폴로지에서는 capacity 값과 무관하게 마주보기가 같은 방식으로 막힌다
    (swap 검사가 간선 하나에서 반대 방향 동시 진입을 이미 막기 때문 — README의 채택 사유).
    두 capacity 변형의 스위치 그룹·대안 수가 같아야 이 주장이 성립한다.
    """
    graphs = {}
    for capacity in CAPACITIES:
        model = build_roadmap(cfg, "single", capacity, chain_nodes=2)
        assignment = build_assignment("single", capacity)
        result = plan_instance(model, assignment, PlanParams())
        graph = compile_graph(plan_view_of(result), alt_generator="lift")
        graphs[capacity] = (
            len(graph.groups),
            sorted(len(g.alternatives) for g in graph.groups),
        )
    assert graphs[1] == graphs[2]


def test_chain_topology_offers_more_shared_locations_than_single(cfg):
    """중간 노드가 있으면 공유 위치(스위치 그룹 후보 지점)가 늘어난다 — M1,M2도 A,B,C,D가 지난다."""
    model_single = build_roadmap(cfg, "single", 2, chain_nodes=2)
    model_chain = build_roadmap(cfg, "chain", 2, chain_nodes=2)
    assignment_single = build_assignment("single", 2)
    assignment_chain = build_assignment("chain", 2)
    r_single = plan_instance(model_single, assignment_single, PlanParams())
    r_chain = plan_instance(model_chain, assignment_chain, PlanParams())
    assert len(r_chain.visit_order) > len(r_single.visit_order)


@pytest.mark.parametrize("chain_nodes", [2, 3])
def test_chain_nodes_count_controls_intermediate_node_count(cfg, chain_nodes):
    model = build_roadmap(cfg, "chain", 1, chain_nodes=chain_nodes)
    m_keys = [n.key for n in model.nodes if n.key.startswith("M")]
    assert len(m_keys) == chain_nodes


def test_chain_capacity1_groups_edges_under_one_corridor(cfg):
    """chain + capacity=1에서만 corridor 묶음이 생긴다(README §설계 사유)."""
    model = build_roadmap(cfg, "chain", 1, chain_nodes=2)
    assert len(model.corridors) == 1
    assert model.corridors[0].key == "q_main"
    corridor_edges = set(model.corridors[0].edge_keys)
    all_corridor_labeled = {(e.u, e.v) for e in model.edges if e.corridor_key}
    assert corridor_edges == all_corridor_labeled

    model_wide = build_roadmap(cfg, "chain", 2, chain_nodes=2)
    assert model_wide.corridors == []
    model_single = build_roadmap(cfg, "single", 1, chain_nodes=2)
    assert model_single.corridors == []
