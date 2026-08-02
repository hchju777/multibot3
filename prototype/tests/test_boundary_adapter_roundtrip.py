"""어댑터 왕복 시험 — 내부 -> 경계 -> 내부에서 의미가 보존되는가."""

import json

from assignment_core import AssignParams, build_assignment
from boundary_adapter import (
    NO_CORRIDOR,
    assignment_from_boundary,
    assignment_to_boundary,
    plan_from_boundary,
    plan_to_boundary,
    roadmap_from_boundary,
    roadmap_to_boundary,
)
from plan_core import PlanParams, fold_timeline, plan_instance
from protoscale import load_scale
from roadmap_core import GenParams, generate_roadmap

CFG = load_scale()


def _model(seed=0):
    return generate_roadmap(CFG, GenParams(seed=seed))


def test_roadmap_roundtrip_preserves_meaning():
    model = _model(7)
    back = roadmap_from_boundary(roadmap_to_boundary(model))
    assert back.instance_id == model.instance_id
    assert back.nodes == model.nodes
    assert back.endpoints == model.endpoints
    assert back.corridors == model.corridors
    assert back.min_separation_m == model.min_separation_m
    assert back.capacity_rule_id == model.capacity_rule_id
    # 간선은 내부 전용 분류(kind) 하나만 잃는다 — 알고리즘 코어가 쓰지 않는 값이다.
    assert [
        (e.u, e.v, e.length_m, e.width_m, e.capacity_robots, e.corridor_key) for e in back.edges
    ] == [
        (e.u, e.v, e.length_m, e.width_m, e.capacity_robots, e.corridor_key) for e in model.edges
    ]


def test_roadmap_roundtrip_is_idempotent_on_the_boundary_side():
    doc = roadmap_to_boundary(_model(2))
    assert roadmap_to_boundary(roadmap_from_boundary(doc)) == doc


def test_assignment_roundtrip_preserves_meaning():
    model = _model(1)
    plan = build_assignment(model, AssignParams(robots=6, tasks=10, seed=1))
    back = assignment_from_boundary(assignment_to_boundary(plan))
    assert back == plan


def test_no_corridor_is_empty_string_not_null():
    """§18-3: «없음»은 빈 문자열이며 null이 아니다. 왕복 사상이 항등이다."""
    doc = roadmap_to_boundary(_model(0))
    assert any(e["corridor"] == NO_CORRIDOR for e in doc["edges"])
    assert all(e["corridor"] is not None for e in doc["edges"])
    assert all(c["id"] != "" for c in doc["corridors"])


def test_boundary_carries_no_internal_or_algorithm_values():
    """시드·격자 인자·내부 분류·타임스텝이 경계로 새지 않는다."""
    doc = roadmap_to_boundary(_model(3))
    doc2 = assignment_to_boundary(build_assignment(_model(3), AssignParams(seed=3)))
    blob = json.dumps([doc, doc2], ensure_ascii=False).lower()
    for banned in (
        "seed", "narrow_ratio", "timestep", "timeout", "horizon",
        "suboptimality", "solver", "kind", "aisle", "cross", "stamp", "time",
    ):
        assert banned not in blob, f"경계 아티팩트에 내부 값이 샜다: {banned}"


def test_corridor_edge_refs_exist_in_edges():
    """x-ref 복합 키(A15)가 요구하는 것 — 통로가 참조하는 쌍이 edges에 실재한다."""
    doc = roadmap_to_boundary(_model(5))
    pairs = {(e["from"], e["to"]) for e in doc["edges"]}
    for c in doc["corridors"]:
        for from_id, to_id in c["edges"]:
            assert (from_id, to_id) in pairs


def test_edge_endpoints_and_goal_locations_resolve():
    """x-ref 단일 키가 요구하는 것 — 모든 참조가 실재 대상을 가리킨다."""
    model = _model(4)
    rdoc = roadmap_to_boundary(model)
    adoc = assignment_to_boundary(build_assignment(model, AssignParams(seed=4)))
    node_ids = {n["id"] for n in rdoc["nodes"]}
    endpoints = set(rdoc["endpoints"])
    assert endpoints <= node_ids
    for e in rdoc["edges"]:
        assert e["from"] in node_ids and e["to"] in node_ids
    for a in adoc["assignments"]:
        assert a["start"] in node_ids
        for g in a["goals"]:
            assert g["location"] in endpoints
    assert adoc["instance_id"] == rdoc["instance_id"]


# ---------------------------------------------------------------- mrs.discrete_plan


def _planned(seed=0):
    model = _model(seed)
    assignment = build_assignment(model, AssignParams(robots=6, tasks=8, seed=seed))
    return model, assignment, plan_instance(model, assignment, PlanParams())


def test_plan_boundary_is_idempotent_on_the_boundary_side():
    """왕복은 내부에서 항등이 아니다(타임스텝이 사라진다). 경계 쪽에서는 항등이다."""
    doc = plan_to_boundary(_planned(1)[2])
    assert plan_to_boundary(plan_from_boundary(doc)) == doc


def test_plan_boundary_loses_waits_by_design():
    """대기가 접혀 사라진다 — 계약이 요구하는 손실이며 숨기지 않는다(§1-2(1))."""
    _, _, result = _planned(0)
    waiting = [p for p in result.paths if len(fold_timeline(p.timeline)[0]) < len(p.timeline)]
    assert waiting, "대기가 있는 로봇이 없어 시험이 성립하지 않는다"
    doc = plan_to_boundary(result)
    back = plan_from_boundary(doc)
    for path in waiting:
        assert len(back.path_of(path.robot).timeline) < len(path.timeline)


def test_plan_step_index_equals_array_position():
    """x-derived-from(A1): index는 자기 배열 위치와 같다."""
    doc = plan_to_boundary(_planned(2)[2])
    for plan in doc["plans"]:
        assert [s["index"] for s in plan["steps"]] == list(range(len(plan["steps"])))


def test_plan_robot_set_equals_assignment_robot_set():
    """x-set-equal: 계획의 로봇 집합 == 배정의 로봇 집합(§17-3-1(8))."""
    _, assignment, result = _planned(3)
    pdoc = plan_to_boundary(result)
    adoc = assignment_to_boundary(assignment)
    assert {p["robot"] for p in pdoc["plans"]} == {a["robot"] for a in adoc["assignments"]}
    assert pdoc["instance_id"] == adoc["instance_id"]


def test_plan_boundary_has_no_time_and_no_algorithm_parameters():
    """🔴 상류 경계에 절대 시각·타임스텝·알고리즘 파라미터가 없다."""
    doc = plan_to_boundary(_planned(4)[2])
    blob = json.dumps(doc, ensure_ascii=False).lower()
    for banned in ("time", "timestep", "sigma", "horizon", "priority", "seed", "wait",
                   "cost", "makespan", "solver", "fallback"):
        assert banned not in blob, f"경계로 새면 안 되는 것이 실렸다: {banned}"


def test_plan_locations_resolve_to_roadmap_nodes():
    """x-ref: steps[].location과 visit_order[].location이 로드맵 정점이다."""
    model, _, result = _planned(5)
    doc = plan_to_boundary(result)
    node_ids = {n.key for n in model.nodes}
    for plan in doc["plans"]:
        for step in plan["steps"]:
            assert step["location"] in node_ids
    for entry in doc["visit_order"]:
        assert entry["location"] in node_ids
