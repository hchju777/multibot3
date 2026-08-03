#!/usr/bin/env python3
"""태스크 배정 생성기 (탐색용 시제).

로드맵 아티팩트를 읽어 mrs.assignment 아티팩트를 만든다.
배정 방식은 **가장 가까운 것부터**(greedy nearest)이며 품질이 목적이 아니다.

사용 예:
    python3 gen_assignment.py --roadmap out/roadmap.json --robots 6 --tasks 8 \
        --out out/assignment.json --schema-check

🔴 탐색용 시제이며 증거가 아니다.
"""

from __future__ import annotations

import argparse
import json
import os
import sys

from assignment_core import AssignParams, build_assignment, validate_assignment
from boundary_adapter import assignment_to_boundary, roadmap_from_boundary
from gen_roadmap import structural_schema_check

HERE = os.path.dirname(os.path.abspath(__file__))
ASSIGNMENT_SCHEMA_PATH = os.path.normpath(
    os.path.join(HERE, "..", "mrs_msgs", "schema", "assignment.schema.json")
)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="태스크 배정 생성기 (탐색용 시제)")
    ap.add_argument("--roadmap", default=os.path.join(HERE, "out", "roadmap.json"))
    ap.add_argument("--robots", type=int, default=6, help="로봇 수 (4~8 규모를 본다)")
    ap.add_argument("--tasks", type=int, default=8, help="태스크 수")
    ap.add_argument("--max-tasks-per-robot", type=int, default=2)
    ap.add_argument("--seed", type=int, default=0)
    ap.add_argument("--revision", type=int, default=0)
    ap.add_argument("--out", default=os.path.join(HERE, "out", "assignment.json"))
    ap.add_argument("--schema-check", action="store_true")
    args = ap.parse_args(argv)

    with open(args.roadmap, "r", encoding="utf-8") as handle:
        model = roadmap_from_boundary(json.load(handle))

    params = AssignParams(
        robots=args.robots,
        tasks=args.tasks,
        max_tasks_per_robot=args.max_tasks_per_robot,
        seed=args.seed,
        revision=args.revision,
    )
    plan = build_assignment(model, params)

    problems = validate_assignment(model, plan)
    for p in problems:
        print(f"  [self-check] {p}", file=sys.stderr)

    doc = assignment_to_boundary(plan)
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    with open(args.out, "w", encoding="utf-8") as handle:
        json.dump(doc, handle, indent=2, ensure_ascii=False, sort_keys=False)
        handle.write("\n")

    goals = sum(len(p.goals) for p in plan.robots)
    idle = sum(1 for p in plan.robots if not p.goals)
    print(f"instance_id      : {plan.instance_id}")
    print(f"robots           : {len(plan.robots)} (목표 없는 로봇 {idle}대)")
    print(f"goal 원소        : {goals}")
    print(f"unassigned_tasks : {len(plan.unassigned_tasks)} {plan.unassigned_tasks}")
    print(f"self-check 위반  : {len(problems)}")
    print(f"wrote            : {os.path.abspath(args.out)}")

    schema_errors = 0
    if args.schema_check:
        schema_errors = structural_schema_check(doc, ASSIGNMENT_SCHEMA_PATH)
        print(f"schema errors    : {schema_errors} (구조만. 확장 규칙 미검사)")

    return 1 if (problems or schema_errors) else 0


if __name__ == "__main__":
    raise SystemExit(main())
