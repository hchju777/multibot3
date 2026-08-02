"""태스크 배정 코어 — 내부 표현만 다룬다.

경계 스키마(mrs.assignment)를 모른다. 변환은 boundary_adapter.py에만 있다.

배정 방식: **가장 가까운 것부터**(greedy nearest). 배정 품질이 목적이 아니라
파이프라인 다음 단계(계획기)에 먹일 입력을 만드는 것이 목적이다.

🔴 탐색용 시제다.
"""

from __future__ import annotations

import math
import random
from dataclasses import dataclass, field

from roadmap_core import RoadmapModel, shortest_path_len


@dataclass(frozen=True)
class GoalItem:
    """목표 «원소» 하나. goal_id는 불투명하며 위치의 함수가 아니다."""

    goal_id: str
    task: str
    location: str


@dataclass
class RobotPlan:
    robot: str
    start: str
    goals: list[GoalItem] = field(default_factory=list)


@dataclass
class AssignmentModel:
    instance_id: str
    revision: int
    robots: list[RobotPlan]
    unassigned_tasks: list[str]


@dataclass(frozen=True)
class AssignParams:
    robots: int = 6
    tasks: int = 8
    max_tasks_per_robot: int = 2
    seed: int = 0
    revision: int = 0


@dataclass(frozen=True)
class _Task:
    """내부 태스크. 픽업 -> 배달 두 목표 원소로 펼쳐진다."""

    task_id: str
    pickup: str
    dropoff: str


def _make_tasks(model: RoadmapModel, params: AssignParams, rng: random.Random) -> list[_Task]:
    eps = sorted(model.endpoints)
    if len(eps) < 2:
        raise ValueError("엔드포인트가 2개 미만이라 픽업·배달 태스크를 만들 수 없다")
    tasks: list[_Task] = []
    for i in range(params.tasks):
        pickup, dropoff = rng.sample(eps, 2)
        tasks.append(_Task(f"t{i:03d}", pickup, dropoff))
    return tasks


def _choose_starts(model: RoadmapModel, params: AssignParams, rng: random.Random) -> list[str]:
    """로봇 시작 정점. 엔드포인트가 아닌 교차점을 우선 쓴다(엔드포인트는 목표 자리다)."""
    ep = set(model.endpoints)
    pool = sorted(n.key for n in model.nodes if n.key not in ep)
    if len(pool) < params.robots:
        pool = sorted(n.key for n in model.nodes)
    if len(pool) < params.robots:
        raise ValueError("정점 수가 로봇 수보다 적다")
    return sorted(rng.sample(pool, params.robots))


def build_assignment(model: RoadmapModel, params: AssignParams) -> AssignmentModel:
    """가장 가까운 로봇에게 태스크를 하나씩 붙인다.

    - 태스크는 식별자 오름차순으로 처리한다(결정론).
    - 각 태스크는 픽업·배달 두 목표 «원소»가 되고 둘 다 같은 로봇에 붙는다.
    - 로봇당 상한에 걸리거나 도달 불가면 미배정으로 남긴다.
    - 동점은 로봇 식별자 오름차순으로 깬다.
    """
    rng = random.Random(params.seed)
    starts = _choose_starts(model, params, rng)
    tasks = _make_tasks(model, params, rng)

    plans = [RobotPlan(robot=f"r{i}", start=starts[i]) for i in range(params.robots)]
    cursor = {p.robot: p.start for p in plans}
    load = {p.robot: 0 for p in plans}
    unassigned: list[str] = []
    goal_counter = 0

    for task in tasks:
        best_robot, best_cost = None, math.inf
        for plan in plans:
            if load[plan.robot] >= params.max_tasks_per_robot:
                continue
            cost = shortest_path_len(model, cursor[plan.robot], task.pickup)
            if cost < best_cost:
                best_robot, best_cost = plan.robot, cost
        if best_robot is None or math.isinf(best_cost):
            unassigned.append(task.task_id)
            continue
        plan = next(p for p in plans if p.robot == best_robot)
        for loc in (task.pickup, task.dropoff):
            plan.goals.append(
                GoalItem(goal_id=f"g-{goal_counter:06d}", task=task.task_id, location=loc)
            )
            goal_counter += 1
        load[best_robot] += 1
        cursor[best_robot] = task.dropoff

    return AssignmentModel(
        instance_id=model.instance_id,
        revision=params.revision,
        robots=plans,
        unassigned_tasks=sorted(unassigned),
    )


def validate_assignment(model: RoadmapModel, plan: AssignmentModel) -> list[str]:
    """내부 배정의 구조 불변식을 점검한다. 층 1 검증기의 대체물이 아니다."""
    problems: list[str] = []
    node_keys = {n.key for n in model.nodes}
    endpoints = set(model.endpoints)

    if plan.instance_id != model.instance_id:
        problems.append("instance_id가 로드맵과 다르다")
    if plan.revision < 0:
        problems.append("revision이 음수다")

    robots = [p.robot for p in plan.robots]
    if len(set(robots)) != len(robots):
        problems.append("robot 중복 배정 항목")

    goal_ids: list[str] = []
    assigned_tasks: set[str] = set()
    for p in plan.robots:
        if p.start not in node_keys:
            problems.append(f"start가 로드맵 정점이 아니다: {p.robot} -> {p.start}")
        for g in p.goals:
            goal_ids.append(g.goal_id)
            assigned_tasks.add(g.task)
            if g.location not in endpoints:
                problems.append(f"goal location이 엔드포인트가 아니다: {g.location}")
    if len(set(goal_ids)) != len(goal_ids):
        problems.append("goal_id가 전 로봇을 통틀어 유일하지 않다")

    if len(set(plan.unassigned_tasks)) != len(plan.unassigned_tasks):
        problems.append("unassigned_tasks 중복")
    if plan.unassigned_tasks != sorted(plan.unassigned_tasks):
        problems.append("unassigned_tasks가 오름차순이 아니다")
    overlap = assigned_tasks & set(plan.unassigned_tasks)
    if overlap:
        problems.append(f"태스크가 배정·미배정 양쪽에 있다: {sorted(overlap)}")
    return problems
