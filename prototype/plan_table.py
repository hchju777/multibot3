"""로봇별 «시작 → 도착» 노드 순서표.

사용자가 명시로 요청한 것: **한 곳에서 「r0: A → B → C → …」를 눈으로 읽을 수 있어야 한다.**
화면과 파일(`out/plan_table.md`) 둘 다에 낸다.

여기 적히는 index는 전부 **경계와 같은 순서 인덱스**다 — 타임스텝이 아니다.
"""

from __future__ import annotations

import os

from assignment_core import AssignmentModel
from plan_core import PlanResult, fold_timeline
from roadmap_core import RoadmapModel

TERMINAL_KO = {
    "goal_reached": "goal_reached (목표 열을 다 덮고 정지)",
    "plan_truncated": "plan_truncated (목표가 남았으나 계획이 여기서 끝난다)",
    "no_progress": "no_progress (전진 계획을 만들지 못했다)",
}


def build_plan_table(
    model: RoadmapModel, assignment: AssignmentModel, result: PlanResult
) -> str:
    """로봇별 블록 + 공유 위치 통행 순서표를 마크다운 문자열로 만든다."""
    endpoints = set(model.endpoints)
    goals_of = {p.robot: p.goals for p in assignment.robots}
    start_of = {p.robot: p.start for p in assignment.robots}
    lines: list[str] = []

    lines.append(f"# 계획 노드 순서표 — `{result.instance_id}` (plan_revision {result.plan_revision})")
    lines.append("")
    lines.append("> 🔴 **탐색용 시제이며 증거가 아니다.** 아래 index는 `mrs.discrete_plan`의")
    lines.append("> **순서 인덱스**이며 타임스텝도 시각도 아니다(계약 §1-2(1)).")
    lines.append(f"> 우선순위 순서: {' > '.join(result.priority_order)}")
    lines.append("")

    # --- 요약표 (로봇당 한 줄)
    lines.append("## 1. 한눈에 — 로봇별 시작 → 도착")
    lines.append("")
    lines.append("| 로봇 | 시작 | 도착 | 단계 수 | 세그먼트 수 | 덮은 목표 | terminal |")
    lines.append("|------|------|------|:------:|:----------:|:--------:|----------|")
    for path in sorted(result.paths, key=lambda p: p.robot):
        steps, _ = fold_timeline(path.timeline)
        lines.append(
            f"| `{path.robot}` | `{steps[0]}` | `{steps[-1]}` | {len(steps)} | "
            f"{len(steps) - 1} | {path.goals_covered}/{path.goals_total} | `{path.terminal}` |"
        )
    lines.append("")

    # --- 로봇별 블록
    lines.append("## 2. 로봇별 노드 순서")
    lines.append("")
    for path in sorted(result.paths, key=lambda p: p.robot):
        steps, _ = fold_timeline(path.timeline)
        goal_steps = {v: k for k, v in path.goal_at_step.items()}
        lines.append(f"### `{path.robot}` — {TERMINAL_KO.get(path.terminal, path.terminal)}")
        lines.append("")
        lines.append(f"- 시작 정점: `{start_of[path.robot]}` · 도착 정점: `{steps[-1]}`")
        lines.append(f"- 단계 {len(steps)}개 · 세그먼트 {len(steps) - 1}개")
        chunks = []
        for i, node in enumerate(steps):
            mark = ""
            if i in goal_steps:
                mark = "★"
            elif node in endpoints:
                mark = "·"
            chunks.append(f"{node}{mark}[{i}]")
        lines.append("")
        lines.append("```")
        for row in range(0, len(chunks), 6):
            lines.append("  " + " -> ".join(chunks[row : row + 6]) + (" ->" if row + 6 < len(chunks) else ""))
        lines.append("```")
        lines.append("")
        lines.append("★ = 그 단계에서 목표를 덮는다 · · = 엔드포인트이지만 이 로봇의 목표는 아니다")
        lines.append("")
        my_goals = goals_of[path.robot]
        if my_goals:
            lines.append("| 목표 순번 | goal_id | task | 위치 | 덮은 단계 index |")
            lines.append("|:--------:|---------|------|------|:--------------:|")
            for k, g in enumerate(my_goals):
                where = path.goal_at_step.get(g.goal_id)
                cell = str(where) if where is not None else "**미도달**"
                lines.append(
                    f"| {k} | `{g.goal_id}` | `{g.task}` | `{g.location}` | {cell} |"
                )
        else:
            lines.append("목표가 없는 로봇이다(`goals: []`). 계약은 이 경우에도 계획을 요구한다.")
        lines.append("")

    # --- 공유 위치 통행 순서
    lines.append("## 3. 공유 위치의 통행 순서 (`visit_order`)")
    lines.append("")
    lines.append("**누가 먼저 그 정점을 지나는가.** 이것이 실행 계층(SADG) Type-2 의존의 유일한 근거다.")
    lines.append("싣는 기준은 «항목 수 ≥ 2»이며 한 로봇의 재방문도 항목 둘로 센다(계약 §1-2(3)).")
    lines.append("")
    if result.visit_order:
        lines.append("| 위치 | 통행 순서 (앞이 먼저) | 항목 수 |")
        lines.append("|------|----------------------|:------:|")
        for location, seq in result.visit_order:
            chain = " → ".join(f"`{robot}`#{index}" for robot, index in seq)
            lines.append(f"| `{location}` | {chain} | {len(seq)} |")
    else:
        lines.append("공유 위치가 없다 — `visit_order`가 빈 배열이며 계약상 합법이다.")
    lines.append("")

    if result.failures:
        lines.append("## 4. 🔴 완전하지 않은 것")
        lines.append("")
        lines.append("| 로봇 | 사유 |")
        lines.append("|------|------|")
        for robot, reason in result.failures:
            lines.append(f"| `{robot}` | {reason} |")
        lines.append("")
        lines.append("**우선순위 기반 계획은 완전하지 않다.** 이 표가 비지 않는 것이 정상이며,")
        lines.append("이것이 다음 단계(실행 계층·재계획)가 왜 필요한지의 자료다.")
        lines.append("")
    return "\n".join(lines)


def write_plan_table(
    model: RoadmapModel, assignment: AssignmentModel, result: PlanResult, out_path: str
) -> str:
    """표를 파일로 쓰고 같은 문자열을 돌려준다(화면 출력용)."""
    text = build_plan_table(model, assignment, result)
    os.makedirs(os.path.dirname(os.path.abspath(out_path)), exist_ok=True)
    with open(out_path, "w", encoding="utf-8") as handle:
        handle.write(text + "\n")
    return text
