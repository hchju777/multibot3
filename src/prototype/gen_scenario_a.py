#!/usr/bin/env python3
"""S-A — 손으로 설계한 마주보기(head-on) 시나리오 생성기 (탐색용 시제).

`_workspace/62_session10_decisions.md` §62-6의 사용자 시나리오를 시제 인스턴스로 만든다:
교행 가능한 통로에 로봇 4대가 2대씩 마주 보고 진입한다. 왼쪽에서 A 뒤로 B(둘 다 왼->오),
오른쪽에서 C 뒤로 D(둘 다 오->왼).

이 스크립트는 `roadmap_core.py`·`assignment_core.py`·`boundary_adapter.py`를 **고치지 않고**
그 dataclass·변환 함수만 재사용해 로드맵·배정을 손으로 조립한다 — README가 허용한 대체 경로다
("손으로 쓰기 어려우면 별도 생성 스크립트를 prototype 디렉터리에 추가해도 된다").

토폴로지:

    SB -- SA -- L -- [체인] -- R -- SC -- SD
                |                    |
               GC,GD                GA,GB   (목표 지선)

- SB(B 시작, 멀리) -> SA(A 시작, 가까이) -> L : 좌측 단일열 접근(항상 A가 앞선다 — 물리적 구조)
- SD(D 시작, 멀리) -> SC(C 시작, 가까이) -> R : 우측 단일열 접근(항상 C가 앞선다)
- A,B의 목표(GA,GB)는 R 너머, C,D의 목표(GC,GD)는 L 너머 — 그래서 A,B는 좌->우, C,D는 우->좌로
  통로 전체를 가로지르고, 통로 안에서 두 방향이 마주친다.

변형 넷 — `--topology {single,chain}` x `--capacity {1,2}`:

| 토폴로지 | 통로 구조 | capacity=2 | capacity=1 |
|----------|----------|:----------:|:----------:|
| single   | L-R 간선 하나, 중간 노드 없음 | corridor_key="" | corridor_key="" |
| chain    | L-M1-M2-R 간선 셋(중간 노드 2개) | corridor_key="" | corridor_key="q_main"(통로 묶음) |

🔴 **corridor 묶음(비 어 있지 않은 corridor_key)의 의미**: `plan_core._Reservations.can_move`가
`(t, corridor)`로 방향을 잠근다 — 통로에 속한 **어느 간선**이든 같은 시각에 반대 방향 이동이
있으면 막는다. 간선이 하나뿐인 `single` 토폴로지에서는 이 묶음이 있으나 없으나 같다(swap 검사가
이미 그 간선의 반대 방향 동시 이동을 막는다) — **그래서 `single`의 capacity 두 값은 사실상 같은
차단 양상을 보일 것으로 예상되고, 이 예상 자체가 관찰 대상이다.**

🔴 탐색용 시제이며 증거가 아니다. 어떤 수치도 사전등록 상수가 아니다.
"""

from __future__ import annotations

import argparse
import json
import os
import sys

from assignment_core import AssignmentModel, GoalItem, RobotPlan, validate_assignment
from boundary_adapter import assignment_to_boundary, roadmap_to_boundary
from gen_roadmap import structural_schema_check
from protoscale import DEFAULT_SCALE_PATH, load_scale
from roadmap_core import Corridor, Edge, Node, RoadmapModel, validate_model

HERE = os.path.dirname(os.path.abspath(__file__))
ROADMAP_SCHEMA_PATH = os.path.normpath(
    os.path.join(HERE, "..", "mrs_msgs", "schema", "roadmap.schema.json")
)
ASSIGNMENT_SCHEMA_PATH = os.path.normpath(
    os.path.join(HERE, "..", "mrs_msgs", "schema", "assignment.schema.json")
)

TOPOLOGIES = ("single", "chain")
CAPACITIES = (1, 2)
GENERATOR_VERSION = "prototype-gen_scenario_a-0.1.0"


def instance_id(topology: str, capacity: int) -> str:
    return f"scenA_{topology}_cap{capacity}"


def build_roadmap(cfg, topology: str, capacity: int, chain_nodes: int) -> RoadmapModel:
    """S-A 로드맵을 손으로 조립한다. 물리 규모는 전부 `cfg`(scale.yaml)에서 온다."""
    if topology not in TOPOLOGIES:
        raise ValueError(f"알 수 없는 토폴로지: {topology}")
    if capacity not in CAPACITIES:
        raise ValueError(f"알 수 없는 용량: {capacity}")

    seg = cfg.corridor_segment_length_m
    stub = cfg.endpoint_stub_length_m
    wide = cfg.aisle_width_wide_m
    narrow = cfg.aisle_width_narrow_m
    approach_cap = cfg.capacity_rule_a(wide)  # 접근 단일열은 항상 넓은 통로로 둔다
    corridor_width = wide if capacity >= 2 else narrow

    # 🔴 채택 사유: corridor_key(통로 묶음)는 「간선이 둘 이상 있고(chain) + 교행 불가(capacity=1)」
    # 일 때만 준다. single 토폴로지는 간선이 하나뿐이라 묶어도 의미가 없고(swap 검사가 이미
    # 그 간선의 반대 방향 동시 진입을 막는다), chain+capacity=2는 각 간선을 독립으로 둬야
    # 「다른 간선을 각자 쓰는 교행」(패싱 베이)이 가능해진다.
    corridor_key = "q_main" if (topology == "chain" and capacity == 1) else ""

    coords: dict[str, tuple[float, float]] = {}
    edges: list[Edge] = []
    corridor_members: list[tuple[str, str]] = []

    def add_edge(u: str, v: str, length: float, width: float, cap: int, ckey: str, kind: str) -> None:
        edges.append(Edge(u, v, float(length), float(width), int(cap), ckey, kind))

    x = 0.0
    coords["SB"] = (x, 0.0)
    x += seg
    coords["SA"] = (x, 0.0)
    x += seg
    coords["L"] = (x, 0.0)
    add_edge("SB", "SA", seg, wide, approach_cap, "", "aisle")
    add_edge("SA", "L", seg, wide, approach_cap, "", "aisle")

    chain_keys = [f"M{i + 1}" for i in range(chain_nodes)] if topology == "chain" else []
    prev = "L"
    for mk in chain_keys:
        x += seg
        coords[mk] = (x, 0.0)
        add_edge(prev, mk, seg, corridor_width, capacity, corridor_key, "aisle")
        if corridor_key:
            corridor_members.append((prev, mk))
        prev = mk
    x += seg
    coords["R"] = (x, 0.0)
    add_edge(prev, "R", seg, corridor_width, capacity, corridor_key, "aisle")
    if corridor_key:
        corridor_members.append((prev, "R"))

    x += seg
    coords["SC"] = (x, 0.0)
    add_edge("R", "SC", seg, wide, approach_cap, "", "aisle")
    x += seg
    coords["SD"] = (x, 0.0)
    add_edge("SC", "SD", seg, wide, approach_cap, "", "aisle")

    rx, ry = coords["R"]
    lx, ly = coords["L"]
    stub_cap = cfg.capacity_rule_a(cfg.stub_width_m)
    coords["GA"] = (rx + stub, ry + stub)
    coords["GB"] = (rx + stub, ry - stub)
    add_edge("R", "GA", stub, cfg.stub_width_m, stub_cap, "", "stub")
    add_edge("R", "GB", stub, cfg.stub_width_m, stub_cap, "", "stub")
    coords["GC"] = (lx - stub, ly + stub)
    coords["GD"] = (lx - stub, ly - stub)
    add_edge("L", "GC", stub, cfg.stub_width_m, stub_cap, "", "stub")
    add_edge("L", "GD", stub, cfg.stub_width_m, stub_cap, "", "stub")

    widest: dict[str, float] = {}
    for e in edges:
        widest[e.u] = max(widest.get(e.u, 0.0), e.width_m)
        widest[e.v] = max(widest.get(e.v, 0.0), e.width_m)
    nodes = [Node(k, coords[k][0], coords[k][1], widest[k] / 2.0) for k in sorted(coords)]

    corridors = []
    if corridor_key and corridor_members:
        corridors.append(
            Corridor(corridor_key, tuple(sorted(corridor_members)), seg * len(corridor_members))
        )

    return RoadmapModel(
        instance_id=instance_id(topology, capacity),
        nodes=nodes,
        edges=edges,
        corridors=corridors,
        endpoints=sorted(["GA", "GB", "GC", "GD"]),
        min_separation_m=cfg.min_separation_m,
        wall_inflation_m=cfg.wall_inflation_m,
        generator_version=GENERATOR_VERSION,
    )


def build_assignment(topology: str, capacity: int) -> AssignmentModel:
    """A,B,C,D 각각 목표 하나. 우선순위 규칙(most_goals, 동점은 robot 오름차순)이
    로봇 이름 오름차순 A,B,C,D로 계획 순서를 정하게 만든다 — 「A가 B보다 먼저,
    C가 D보다 먼저」를 계획기 인자를 하나도 바꾸지 않고 얻는다.
    """
    robots = [
        RobotPlan("A", "SA", [GoalItem("g-A0", "tA", "GA")]),
        RobotPlan("B", "SB", [GoalItem("g-B0", "tB", "GB")]),
        RobotPlan("C", "SC", [GoalItem("g-C0", "tC", "GC")]),
        RobotPlan("D", "SD", [GoalItem("g-D0", "tD", "GD")]),
    ]
    return AssignmentModel(
        instance_id=instance_id(topology, capacity),
        revision=0,
        robots=robots,
        unassigned_tasks=[],
    )


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="S-A 마주보기 시나리오 생성기 (탐색용 시제)")
    ap.add_argument("--topology", choices=TOPOLOGIES, required=True)
    ap.add_argument("--capacity", type=int, choices=CAPACITIES, required=True)
    ap.add_argument(
        "--chain-nodes", type=int, default=2,
        help="chain 토폴로지의 중간 노드 개수 (2 또는 3). single이면 무시된다",
    )
    ap.add_argument("--scale", default=DEFAULT_SCALE_PATH)
    ap.add_argument("--out-dir", default=None, help="기본은 out/scenA_{topology}_cap{capacity}")
    ap.add_argument("--schema-check", action="store_true")
    args = ap.parse_args(argv)

    if args.topology == "chain" and args.chain_nodes not in (2, 3):
        print("chain-nodes는 2 또는 3이어야 한다", file=sys.stderr)
        return 2

    out_dir = args.out_dir or os.path.join(
        HERE, "out", f"scenA_{args.topology}_cap{args.capacity}"
    )
    os.makedirs(out_dir, exist_ok=True)

    cfg = load_scale(args.scale)
    model = build_roadmap(cfg, args.topology, args.capacity, args.chain_nodes)
    assignment = build_assignment(args.topology, args.capacity)

    problems = validate_model(model) + validate_assignment(model, assignment)
    for p in problems:
        print(f"  [self-check] {p}", file=sys.stderr)

    roadmap_doc = roadmap_to_boundary(model)
    assignment_doc = assignment_to_boundary(assignment)
    with open(os.path.join(out_dir, "roadmap.json"), "w", encoding="utf-8") as handle:
        json.dump(roadmap_doc, handle, indent=2, ensure_ascii=False)
        handle.write("\n")
    with open(os.path.join(out_dir, "assignment.json"), "w", encoding="utf-8") as handle:
        json.dump(assignment_doc, handle, indent=2, ensure_ascii=False)
        handle.write("\n")

    print(f"instance_id      : {model.instance_id}")
    print(f"topology/capacity: {args.topology} / {args.capacity}")
    print(f"nodes / edges    : {len(model.nodes)} / {len(model.edges)}")
    print(f"corridors        : {[c.key for c in model.corridors]}")
    print(f"self-check 위반  : {len(problems)}")
    print(f"wrote            : {os.path.abspath(out_dir)}/{{roadmap,assignment}}.json")

    schema_errors = 0
    if args.schema_check:
        e1 = structural_schema_check(roadmap_doc, ROADMAP_SCHEMA_PATH)
        e2 = structural_schema_check(assignment_doc, ASSIGNMENT_SCHEMA_PATH)
        schema_errors = e1 + e2
        print(f"schema errors    : roadmap {e1} / assignment {e2} (구조만. 확장 규칙 미검사)")

    return 1 if (problems or schema_errors) else 0


if __name__ == "__main__":
    raise SystemExit(main())
