"""어댑터 — 내부 표현 <-> 경계 스키마. **경계 JSON 키가 나오는 유일한 파일이다.**

경계 스키마 정본: src/mrs_msgs/schema/roadmap.schema.json (mrs.roadmap 2.0.0)
                 src/mrs_msgs/schema/assignment.schema.json (mrs.assignment 2.0.0)
                 src/mrs_msgs/schema/discrete_plan.schema.json (mrs.discrete_plan 3.0.0)
                 src/mrs_msgs/schema/execution_constraints.schema.json (2.0.0)
정정분: _workspace/48_contract_amendment.md §18 (앞 전부를 이긴다)

여기서 지키는 것:
  - edges[].corridor의 «없음»은 빈 문자열 ""이며 null이 아니다 (§18-3).
  - corridors[].edges는 [from, to] 쌍 목록이고 각 쌍이 edges에 실재해야 한다 (§18-6, A15).
  - 알고리즘 파라미터(시드·narrow_ratio·격자 크기·내부 kind 분류)는 경계로 나가지 않는다.
  - 세그먼트 식별자 {robot}#{index}의 **문자열 인코딩이 여기서만 일어난다** (A10).
  - 대안 생성 규칙(lift/FirstOcc)·Heads(A_g)·lambda*·tau^nom은 경계에 **싣지 않는다**
    (execution_constraints.schema.json의 x-forbidden-note).
"""

from __future__ import annotations

from assignment_core import AssignmentModel, GoalItem, RobotPlan
from plan_core import PlanResult, RobotPath, fold_timeline
from roadmap_core import Corridor, Edge, Node, RoadmapModel
from sadg_core import (
    INTER_ROBOT,
    INTRA_ROBOT,
    Alternative,
    ExecGraph,
    Precedence,
    Segment,
    SwitchGroup,
)

ROADMAP_SCHEMA = "mrs.roadmap"
ROADMAP_SCHEMA_VERSION = "2.0.0"
ASSIGNMENT_SCHEMA = "mrs.assignment"
# 2026-08-13: 1.1.0 -> 2.0.0. 스키마 const 가 2.0.0 인데 여기가 안 따라와 구조 검증 5건이
# 실패하고 있었다(`175`·`177` 게이트). 구조 자체는 이미 2.0.0 을 낸다 — 실패하던 단언이
# schema_version 하나뿐이었다. 계약 §0-1 표도 같은 날 2.0.0 으로 맞췄다(`177` D-6 처분, 계약 §0-1-2).
ASSIGNMENT_SCHEMA_VERSION = "2.0.0"
DISCRETE_PLAN_SCHEMA = "mrs.discrete_plan"
# 2026-08-13: 2.0.0 -> 3.0.0. visit_order 동점 해소 규칙 + ［P］ 참조가 들어가 MAJOR (160 §160-3-3).
DISCRETE_PLAN_SCHEMA_VERSION = "3.0.0"
EXEC_CONSTRAINTS_SCHEMA = "mrs.execution_constraints"
# 2026-08-13: 1.1.0 -> 2.0.0. $defs.dependency 에 ［D］(퇴거→진입) 신설 + ［D2］［D3］ (162a §162a-1-2).
EXEC_CONSTRAINTS_SCHEMA_VERSION = "2.0.0"

NO_CORRIDOR = ""  # 센티널. corridors[].id는 minLength:1이므로 값 공간 밖이다.

# 내부 유래 어휘 <-> 경계의 닫힌 어휘. **이 표가 어댑터의 존재 이유다.**
_DEP_TYPE_TO_BOUNDARY = {INTRA_ROBOT: "sequential", INTER_ROBOT: "ordering"}
_DEP_TYPE_FROM_BOUNDARY = {v: k for k, v in _DEP_TYPE_TO_BOUNDARY.items()}


def _segment_id(key) -> str:
    """(로봇, 번호) -> `{robot}#{index}`. 상류 steps[].index의 재사용이다 (§2-1)."""
    robot, number = key
    return f"{robot}#{number}"


def _segment_key(text: str):
    """`{robot}#{index}` -> (로봇, 번호). `#`는 로봇 이름에 나올 수 없다(스키마 pattern)."""
    robot, _, number = text.rpartition("#")
    return (robot, int(number))


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


def constraints_to_boundary(graph: ExecGraph) -> dict:
    """내부 ExecGraph를 mrs.execution_constraints 2.0.0 아티팩트로 옮긴다.

    🔴 **여기서 막는 것**(스키마 x-forbidden-note): Heads(A_g) · lift/FirstOcc ·
    lambda*/tau^nom · 대안 생성기 이름 · 조합 수 · 표본 시드. 전부 `graph.stats`에 남고
    이 함수는 그 필드를 **하나도 읽지 않는다** — 어댑터에 생성기 핸들이 없는 것과 같은 규율이다.

    대안 id는 **위치 인덱스**(`a0`, `a1`, …)다. `lift2` 같은 이름을 쓰면 **대안 생성 규칙이
    id 문자열로 새어 나가** SC-4가 무너진다.
    """
    return {
        "schema": EXEC_CONSTRAINTS_SCHEMA,
        "schema_version": EXEC_CONSTRAINTS_SCHEMA_VERSION,
        "instance_id": graph.instance_id,
        "commit_seq": int(graph.revision),
        "segments": [
            {
                "id": _segment_id(s.key),
                "robot": s.robot,
                "from": s.src_node,
                "to": s.dst_node,
            }
            for s in graph.segments
        ],
        "dependencies": [_dep_to_boundary(d) for d in graph.fixed],
        "switch_groups": [
            {
                "id": f"g{gi}",
                "location": g.location,
                "alternatives": [
                    {
                        "id": f"a{ai}",
                        "dependencies": [_dep_to_boundary(d) for d in alt.edges],
                    }
                    for ai, alt in enumerate(g.alternatives)
                ],
            }
            for gi, g in enumerate(graph.groups)
        ],
    }


def _dep_to_boundary(dep: Precedence) -> dict:
    return {
        "from": _segment_id(dep.src),
        "to": _segment_id(dep.dst),
        "type": _DEP_TYPE_TO_BOUNDARY[dep.kind],
    }


def _dep_from_boundary(doc: dict) -> Precedence:
    return Precedence(
        _segment_key(doc["from"]), _segment_key(doc["to"]), _DEP_TYPE_FROM_BOUNDARY[doc["type"]]
    )


# ---------------------------------------------------------------- 경계 -> 내부


def constraints_from_boundary(doc: dict) -> ExecGraph:
    """mrs.execution_constraints 아티팩트를 내부 ExecGraph로 되돌린다.

    🔴 **왕복은 항등이 아니다 — 그리고 그것이 계약이 요구하는 것이다.** 되돌린 그래프의
    `stats`는 비어 있고(`critical_path_segments`·생성기 이름·조합 수가 경계에 없다),
    `is_original`·`source_index`도 복원되지 않는다. **대안 생성 규칙은 경계를 건너지 않으므로
    소비자는 「어느 것이 원 순서인가」를 알 수 없다** — 알 필요도 없다(§2-2(3): 대안은 내용으로
    실리고 상호 배타·완전이다). **경계 쪽에서는 왕복이 항등**이며 시험이 그것을 고정한다.
    """
    if doc.get("schema") != EXEC_CONSTRAINTS_SCHEMA:
        raise ValueError(f"schema가 {EXEC_CONSTRAINTS_SCHEMA}가 아니다: {doc.get('schema')}")
    segments = [
        Segment(s["robot"], _segment_key(s["id"])[1], s["from"], s["to"]) for s in doc["segments"]
    ]
    groups = []
    for g in doc["switch_groups"]:
        alternatives = [
            Alternative(
                order=(),  # 항목 열은 의존에서 유도되는 값이며 경계 필드가 아니다
                edges=tuple(_dep_from_boundary(d) for d in a["dependencies"]),
                is_original=False,
                source_index=-1,
            )
            for a in g["alternatives"]
        ]
        groups.append(SwitchGroup(g["location"], (), alternatives))
    return ExecGraph(
        instance_id=doc["instance_id"],
        revision=int(doc["commit_seq"]),
        segments=segments,
        fixed=[_dep_from_boundary(d) for d in doc["dependencies"]],
        groups=groups,
    )


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
