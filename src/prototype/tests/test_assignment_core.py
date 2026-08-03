"""태스크 배정 코어 시험 — 계약이 확정한 성질을 내부 표현에서 먼저 본다."""

import pytest

from assignment_core import AssignParams, build_assignment, validate_assignment
from protoscale import load_scale
from roadmap_core import GenParams, generate_roadmap

CFG = load_scale()


def _pair(seed=0, robots=6, tasks=8, cap=2):
    model = generate_roadmap(CFG, GenParams(seed=seed))
    plan = build_assignment(
        model, AssignParams(robots=robots, tasks=tasks, max_tasks_per_robot=cap, seed=seed)
    )
    return model, plan


def test_same_seed_same_assignment():
    assert _pair(seed=5)[1] == _pair(seed=5)[1]


def test_structural_invariants_hold():
    for seed in range(6):
        model, plan = _pair(seed=seed)
        assert validate_assignment(model, plan) == []


def test_every_robot_appears_even_when_idle():
    """목표가 없는 로봇도 goals: []로 반드시 등장한다(계약 §17-3-1(7))."""
    model, plan = _pair(seed=0, robots=8, tasks=2, cap=1)
    assert [p.robot for p in plan.robots] == [f"r{i}" for i in range(8)]
    assert any(not p.goals for p in plan.robots)


def test_goal_ids_unique_across_robots():
    _, plan = _pair(seed=1, robots=6, tasks=10, cap=2)
    ids = [g.goal_id for p in plan.robots for g in p.goals]
    assert len(set(ids)) == len(ids)


def test_goal_id_is_not_a_function_of_location():
    """같은 위치를 가리키는 두 목표 원소는 서로 다른 goal_id를 갖는다."""
    _, plan = _pair(seed=2, robots=4, tasks=12, cap=3)
    by_loc: dict[str, set] = {}
    for p in plan.robots:
        for g in p.goals:
            by_loc.setdefault(g.location, set()).add(g.goal_id)
    repeated = [loc for loc, ids in by_loc.items() if len(ids) > 1]
    assert repeated, "같은 위치가 두 번 이상 목표로 나오는 사례가 없어 시험이 성립하지 않는다"


def test_task_is_in_exactly_one_side():
    """태스크는 배정 목록과 미배정 목록 중 정확히 한쪽에만 있다(x-exclusive-with)."""
    _, plan = _pair(seed=3, robots=4, tasks=14, cap=2)
    assigned = {g.task for p in plan.robots for g in p.goals}
    unassigned = set(plan.unassigned_tasks)
    assert unassigned, "이 설정에서는 미배정이 남아야 시험이 성립한다"
    assert assigned & unassigned == set()


def test_unassigned_is_sorted_and_unique():
    _, plan = _pair(seed=3, robots=4, tasks=14, cap=2)
    assert plan.unassigned_tasks == sorted(set(plan.unassigned_tasks))


def test_one_task_yields_two_goal_elements_with_same_task_id():
    """픽업·배달이 한 태스크의 두 목표 «원소»다(계약 §17-3-2: task 중복은 위반이 아니다)."""
    _, plan = _pair(seed=0)
    counts: dict[str, int] = {}
    for p in plan.robots:
        for g in p.goals:
            counts[g.task] = counts.get(g.task, 0) + 1
    assert counts and all(v == 2 for v in counts.values())


def test_goals_of_a_task_stay_on_one_robot():
    _, plan = _pair(seed=4, robots=5, tasks=9, cap=2)
    owner: dict[str, str] = {}
    for p in plan.robots:
        for g in p.goals:
            assert owner.setdefault(g.task, p.robot) == p.robot


def test_greedy_prefers_the_nearer_robot():
    """첫 태스크는 픽업 지점에 가장 가까운 로봇이 받는다."""
    from roadmap_core import shortest_path_len

    model, plan = _pair(seed=6, robots=6, tasks=6, cap=1)
    first = min(g.task for p in plan.robots for g in p.goals)
    holder = next(p for p in plan.robots if any(g.task == first for g in p.goals))
    pickup = next(g.location for g in holder.goals if g.task == first)
    mine = shortest_path_len(model, holder.start, pickup)
    for p in plan.robots:
        assert mine <= shortest_path_len(model, p.start, pickup) + 1e-9


def test_broken_assignment_is_detected():
    from assignment_core import GoalItem

    model, plan = _pair(seed=0)
    plan.robots[0].goals.append(GoalItem("g-999999", "tXXX", "NO_SUCH_NODE"))
    plan.unassigned_tasks = sorted(plan.unassigned_tasks + ["tXXX"])
    problems = validate_assignment(model, plan)
    assert any("엔드포인트가 아니다" in p for p in problems)
    assert any("양쪽에 있다" in p for p in problems)


def test_too_many_robots_rejected():
    model = generate_roadmap(CFG, GenParams(seed=0))
    with pytest.raises(ValueError):
        build_assignment(model, AssignParams(robots=999, tasks=1))
