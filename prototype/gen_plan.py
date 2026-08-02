#!/usr/bin/env python3
"""S2 — 우선순위 기반 시공간 A* 경로 계획기 CLI (탐색용 시제).

로드맵·배정 아티팩트를 읽어 `mrs.discrete_plan` 2.0.0 아티팩트를 만든다.

    python3 gen_plan.py --priority most_goals --horizon 96 --schema-check --table

🔴 우선순위 기반은 **완전하지 않다.** 못 푸는 인스턴스가 나오는 것이 정상이고,
이 CLI는 실패를 사유와 함께 그대로 출력한다. 종료 코드 1은 «계획이 완전하지 않았다»를 뜻한다.
"""

from __future__ import annotations

import argparse
import json
import os
import sys

from boundary_adapter import (
    assignment_from_boundary,
    plan_to_boundary,
    roadmap_from_boundary,
)
from gen_roadmap import structural_schema_check
from plan_core import PRIORITY_RULES, PlanParams, plan_instance, validate_plan

HERE = os.path.dirname(os.path.abspath(__file__))
PLAN_SCHEMA_PATH = os.path.normpath(
    os.path.join(HERE, "..", "mrs_msgs", "schema", "discrete_plan.schema.json")
)


def load_inputs(roadmap_path: str, assignment_path: str):
    with open(roadmap_path, "r", encoding="utf-8") as handle:
        model = roadmap_from_boundary(json.load(handle))
    with open(assignment_path, "r", encoding="utf-8") as handle:
        assignment = assignment_from_boundary(json.load(handle))
    if model.instance_id != assignment.instance_id:
        raise ValueError(
            f"instance_id가 다르다: 로드맵 {model.instance_id} vs 배정 {assignment.instance_id}"
        )
    return model, assignment


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="우선순위 기반 시공간 A* 계획기 (탐색용 시제)")
    ap.add_argument("--roadmap", default=os.path.join(HERE, "out", "roadmap.json"))
    ap.add_argument("--assignment", default=os.path.join(HERE, "out", "assignment.json"))
    ap.add_argument(
        "--priority", default="most_goals", choices=list(PRIORITY_RULES),
        help="무엇을 먼저 계획하는가. 기본 most_goals",
    )
    ap.add_argument("--horizon", type=int, default=96, help="내부 타임스텝 상한(경계로 안 나간다)")
    ap.add_argument("--seed", type=int, default=0, help="priority=random의 시드")
    ap.add_argument("--plan-revision", type=int, default=0)
    ap.add_argument("--out", default=os.path.join(HERE, "out", "discrete_plan.json"))
    ap.add_argument("--schema-check", action="store_true")
    ap.add_argument("--table", action="store_true", help="로봇별 노드 순서표를 함께 낸다")
    ap.add_argument("--table-out", default=os.path.join(HERE, "out", "plan_table.md"))
    args = ap.parse_args(argv)

    model, assignment = load_inputs(args.roadmap, args.assignment)
    params = PlanParams(
        priority=args.priority,
        horizon=args.horizon,
        seed=args.seed,
        plan_revision=args.plan_revision,
    )
    result = plan_instance(model, assignment, params)

    if not result.roster_complete:
        # 🔴 계약을 만족하는 아티팩트를 만들 수 없다. **쓰지 않는다.**
        print(f"instance_id      : {result.instance_id}")
        print(f"우선순위 규칙    : {args.priority} -> {result.priority_order}")
        print("🔴 인스턴스 미해결 — 계획을 주지 못한 로봇이 있다:", result.unplanned)
        for robot, reason in result.failures:
            print(f"   - {robot}: {reason}")
        print("   계약 §17-3-1(8)은 로봇 전부를 요구하므로 산출물을 쓰지 않았다.")
        print("   우선순위 기반은 완전하지 않다 — 이것이 실패의 정직한 모습이다.")
        return 3

    problems = validate_plan(model, assignment, result)
    for p in problems:
        print(f"  [self-check] {p}", file=sys.stderr)

    doc = plan_to_boundary(result)
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    with open(args.out, "w", encoding="utf-8") as handle:
        json.dump(doc, handle, indent=2, ensure_ascii=False)
        handle.write("\n")

    counts: dict[str, int] = {}
    for path in result.paths:
        counts[path.terminal] = counts.get(path.terminal, 0) + 1
    covered = sum(p.goals_covered for p in result.paths)
    total = sum(p.goals_total for p in result.paths)
    print(f"instance_id      : {result.instance_id}")
    print(f"우선순위 규칙    : {args.priority} -> {result.priority_order}")
    print(f"계획된 로봇      : {len(result.paths)} / {len(assignment.robots)}")
    print(f"terminal 분포    : {dict(sorted(counts.items()))}")
    print(f"덮은 목표        : {covered}/{total}")
    print(f"공유 위치        : {len(result.visit_order)}개 (visit_order)")
    print(f"self-check 위반  : {len(problems)}")
    if result.failures:
        print(f"🔴 완전하지 않음 : {len(result.failures)}건")
        for robot, reason in result.failures:
            print(f"   - {robot}: {reason}")
    print(f"wrote            : {os.path.abspath(args.out)}")

    schema_errors = 0
    if args.schema_check:
        schema_errors = structural_schema_check(doc, PLAN_SCHEMA_PATH)
        print(f"schema errors    : {schema_errors} (구조만. 확장 규칙 미검사)")

    if args.table:
        from plan_table import write_plan_table

        text = write_plan_table(model, assignment, result, args.table_out)
        print()
        print(text)
        print(f"wrote            : {os.path.abspath(args.table_out)}")

    if problems or schema_errors:
        return 2  # 산출물 자체가 깨졌다 — 계획 실패와 구별한다
    return 1 if result.failures else 0


if __name__ == "__main__":
    raise SystemExit(main())
