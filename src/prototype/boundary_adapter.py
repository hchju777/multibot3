"""어댑터 — 내부 표현 <-> 경계 스키마. **경계 JSON 키가 나오는 유일한 파일이다.**

경계 스키마 정본: src/mrs_msgs/schema/roadmap.schema.json (mrs.roadmap 2.0.0)
                 src/mrs_msgs/schema/assignment.schema.json (mrs.assignment 1.1.0)
정정분: _workspace/48_contract_amendment.md §18 (앞 전부를 이긴다)

여기서 지키는 것:
  - edges[].corridor의 «없음»은 빈 문자열 ""이며 null이 아니다 (§18-3).
  - corridors[].edges는 [from, to] 쌍 목록이고 각 쌍이 edges에 실재해야 한다 (§18-6, A15).
  - 알고리즘 파라미터(시드·narrow_ratio·격자 크기·내부 kind 분류)는 경계로 나가지 않는다.
"""

from __future__ import annotations

from assignment_core import AssignmentModel, GoalItem, RobotPlan
from plan_core import PlanResult, RobotPath, fold_timeline
from roadmap_core import Corridor, Edge, Node, RoadmapModel

ROADMAP_SCHEMA = "mrs.roadmap"
ROADMAP_SCHEMA_VERSION = "2.0.0"
ASSIGNMENT_SCHEMA = "mrs.assignment"
ASSIGNMENT_SCHEMA_VERSION = "1.1.0"
DISCRETE_PLAN_SCHEMA = "mrs.discrete_plan"
DISCRETE_PLAN_SCHEMA_VERSION = "2.0.0"

NO_CORRIDOR = ""  # 센티널. corridors[].id는 minLength:1이므로 값 공간 밖이다.


# ---------------------------------------------------------------- 내부 -> 경계


def roadmap_to_boundary(model: RoadmapModel) -> dict:
    """내부 RoadmapModel을 mrs.roadmap 아티팩트로 옮긴다."""
    return {
        "schema": ROADMAP_SCHEMA,
        "schema_version": ROADMAP_SCHEMA_VERSION,
        "instance_id": model.instance_id,
        "nodes": [
            {
                "id": n.key,
                "x": n.x_m,
                "y": n.y_m,
                "clearance_m": n.clearance_m,
            }
            for n in model.nodes
        ],
        "edges": [
            {
                "from": e.u,
                "to": e.v,
                # 교행 불가는 traversal이 아니라 capacity_robots:1 + corridor로 표현한다.
                "traversal": e.traversal,
                "length_m": e.length_m,
                "width_m": e.width_m,
                "capacity_robots": int(e.capacity_robots),
                "corridor": e.corridor_key or NO_CORRIDOR,
            }
            for e in model.edges
        ],
        "endpoints": list(model.endpoints),
        "corridors": [
            {
                "id": c.key,
                "edges": [[u, v] for u, v in c.edge_keys],
                "length_m": c.length_m,
            }
            for c in model.corridors
        ],
        "provenance": {
            "min_separation_m": model.min_separation_m,
            "wall_inflation_m": model.wall_inflation_m,
            "capacity_rule_id": model.capacity_rule_id,
            "generator_version": model.generator_version,
        },
    }


def assignment_to_boundary(plan: AssignmentModel) -> dict:
    """내부 AssignmentModel을 mrs.assignment 아티팩트로 옮긴다."""
    return {
        "schema": ASSIGNMENT_SCHEMA,
        "schema_version": ASSIGNMENT_SCHEMA_VERSION,
        "instance_id": plan.instance_id,
        "revision": int(plan.revision),
        "assignments": [
            {
                "robot": p.robot,
                "start": p.start,
                "goals": [
                    {"goal_id": g.goal_id, "task": g.task, "location": g.location}
                    for g in p.goals
                ],
            }
            for p in plan.robots
        ],
        "unassigned_tasks": sorted(plan.unassigned_tasks),
    }


def plan_to_boundary(result: PlanResult) -> dict:
    """내부 PlanResult를 mrs.discrete_plan 아티팩트로 옮긴다.

    🔴 **여기서 내부 타임스텝이 사라진다.** 대기를 접어 순서 인덱스만 남기며(§1-2(1)),
    `visit_order`의 index는 **접은 뒤 순번**으로 이미 재매핑돼 있다(§1-2(2)).
    지평·시드·우선순위 규칙 같은 알고리즘 파라미터는 하나도 싣지 않는다.
    """
    plans = []
    for path in sorted(result.paths, key=lambda p: p.robot):
        steps, _ = fold_timeline(path.timeline)
        plans.append(
            {
                "robot": path.robot,
                "terminal": path.terminal,
                "steps": [{"index": i, "location": node} for i, node in enumerate(steps)],
            }
        )
    return {
        "schema": DISCRETE_PLAN_SCHEMA,
        "schema_version": DISCRETE_PLAN_SCHEMA_VERSION,
        "instance_id": result.instance_id,
        "plan_revision": int(result.plan_revision),
        "plans": plans,
        "visit_order": [
            {
                "location": location,
                "sequence": [{"robot": robot, "index": index} for robot, index in seq],
            }
            for location, seq in result.visit_order
        ],
    }


# ---------------------------------------------------------------- 경계 -> 내부


def roadmap_from_boundary(doc: dict) -> RoadmapModel:
    """mrs.roadmap 아티팩트를 내부 RoadmapModel로 되돌린다.

    내부 전용 필드(Edge.kind)는 경계에 없으므로 «미상»으로 복원된다 —
    왕복에서 잃는 것은 이것뿐이며 알고리즘 코어가 이 값에 의존하지 않는다.
    """
    if doc.get("schema") != ROADMAP_SCHEMA:
        raise ValueError(f"schema가 {ROADMAP_SCHEMA}가 아니다: {doc.get('schema')}")
    corridors = [
        Corridor(
            key=c["id"],
            edge_keys=tuple((pair[0], pair[1]) for pair in c["edges"]),
            length_m=float(c["length_m"]),
        )
        for c in doc["corridors"]
    ]
    return RoadmapModel(
        instance_id=doc["instance_id"],
        nodes=[
            Node(n["id"], float(n["x"]), float(n["y"]), float(n["clearance_m"]))
            for n in doc["nodes"]
        ],
        edges=[
            Edge(
                u=e["from"],
                v=e["to"],
                length_m=float(e["length_m"]),
                width_m=float(e["width_m"]),
                capacity_robots=int(e["capacity_robots"]),
                corridor_key=e["corridor"],
                kind="unknown",
                traversal=e["traversal"],
            )
            for e in doc["edges"]
        ],
        corridors=corridors,
        endpoints=list(doc["endpoints"]),
        min_separation_m=float(doc["provenance"]["min_separation_m"]),
        wall_inflation_m=float(doc["provenance"]["wall_inflation_m"]),
        capacity_rule_id=doc["provenance"]["capacity_rule_id"],
        generator_version=doc["provenance"]["generator_version"],
    )


def plan_from_boundary(doc: dict) -> PlanResult:
    """mrs.discrete_plan 아티팩트를 내부 PlanResult로 되돌린다.

    🔴 **왕복은 항등이 아니다. 그것이 계약이 요구하는 것이다** — 경계에 타임스텝이 없으므로
    되돌린 `timeline`은 «대기가 접힌 순서 열»이고 원래의 대기 시각은 복원되지 않는다.
    되돌린 값으로 다시 경계를 만들면 **경계 쪽에서는 항등**이며 시험이 그것을 고정한다.
    `ok`·`failures`·`priority_order`도 경계에 없다 — 알고리즘 상태이므로 실을 수 없다.
    """
    if doc.get("schema") != DISCRETE_PLAN_SCHEMA:
        raise ValueError(f"schema가 {DISCRETE_PLAN_SCHEMA}가 아니다: {doc.get('schema')}")
    paths = [
        RobotPath(
            robot=p["robot"],
            timeline=[s["location"] for s in p["steps"]],
            terminal=p["terminal"],
            goals_total=0,
            goals_covered=0,
        )
        for p in doc["plans"]
    ]
    return PlanResult(
        instance_id=doc["instance_id"],
        plan_revision=int(doc["plan_revision"]),
        paths=paths,
        visit_order=[
            (v["location"], [(s["robot"], int(s["index"])) for s in v["sequence"]])
            for v in doc["visit_order"]
        ],
        priority_order=[],
        ok=False,
        failures=[],
    )


def assignment_from_boundary(doc: dict) -> AssignmentModel:
    """mrs.assignment 아티팩트를 내부 AssignmentModel로 되돌린다."""
    if doc.get("schema") != ASSIGNMENT_SCHEMA:
        raise ValueError(f"schema가 {ASSIGNMENT_SCHEMA}가 아니다: {doc.get('schema')}")
    return AssignmentModel(
        instance_id=doc["instance_id"],
        revision=int(doc["revision"]),
        robots=[
            RobotPlan(
                robot=a["robot"],
                start=a["start"],
                goals=[
                    GoalItem(g["goal_id"], g["task"], g["location"]) for g in a["goals"]
                ],
            )
            for a in doc["assignments"]
        ],
        unassigned_tasks=list(doc["unassigned_tasks"]),
    )
