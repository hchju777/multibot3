"""S2 계획 코어 시험 — 충돌 0 · 정적 제약 · 절단의 정직성 · 결정론."""

import pytest

from assignment_core import AssignParams, build_assignment
from plan_core import PRIORITY_RULES, TERMINALS, PlanParams, fold_timeline, plan_instance, validate_plan
from protoscale import load_scale
from roadmap_core import GenParams, generate_roadmap

CFG = load_scale()


def _case(seed=0, robots=6, tasks=8, cap=2, priority="most_goals", horizon=96, **gen):
    model = generate_roadmap(CFG, GenParams(seed=seed, **gen))
    assignment = build_assignment(
        model, AssignParams(robots=robots, tasks=tasks, max_tasks_per_robot=cap, seed=seed)
    )
    result = plan_instance(model, assignment, PlanParams(priority=priority, horizon=horizon))
    return model, assignment, result


def test_same_params_same_plan():
    a = _case(seed=2)[2]
    b = _case(seed=2)[2]
    assert [(p.robot, p.timeline, p.terminal) for p in a.paths] == [
        (p.robot, p.timeline, p.terminal) for p in b.paths
    ]
    assert a.visit_order == b.visit_order


@pytest.mark.parametrize("seed", [0, 1, 2, 3, 4, 5])
def test_no_conflicts_and_static_constraints(seed):
    """정점·간선·following·회전 충돌 0 + traversal·용량·교행 불가 통로 준수."""
    model, assignment, result = _case(seed=seed)
    assert validate_plan(model, assignment, result) == []


def test_every_robot_gets_a_plan():
    """계약 §17-3-1(8): 인스턴스 로봇 전부. 유휴 로봇도 길이 1 이상 계획으로 등장한다."""
    _, assignment, result = _case(seed=1, robots=8, tasks=4, cap=1)
    assert {p.robot for p in result.paths} == {p.robot for p in assignment.robots}
    assert all(len(fold_timeline(p.timeline)[0]) >= 1 for p in result.paths)


def test_terminal_vocabulary_and_honesty():
    """terminal은 닫힌 어휘 3값이고, goal_reached는 목표를 다 덮었을 때만 붙는다."""
    for seed in range(6):
        _, _, result = _case(seed=seed)
        for path in result.paths:
            assert path.terminal in TERMINALS
            if path.terminal == "goal_reached":
                assert path.goals_covered == path.goals_total
            else:
                assert path.goals_covered < path.goals_total
        # 실패를 성공으로 포장하지 않는다
        assert result.ok == all(p.terminal == "goal_reached" for p in result.paths)


def test_visit_order_rules():
    """항목 수 >= 2만 싣고, index는 접은 뒤 순번이며, 그 자리에 그 정점이 있다."""
    _, _, result = _case(seed=3)
    assert result.visit_order, "이 인스턴스에서는 공유 위치가 나와야 시험이 성립한다"
    for location, seq in result.visit_order:
        assert len(seq) >= 2
        for robot, index in seq:
            steps, _ = fold_timeline(result.path_of(robot).timeline)
            assert steps[index] == location


def test_visit_order_counts_single_robot_revisit():
    """«둘 이상의 로봇»이 아니라 «항목 수 >= 2»다 — 한 로봇의 재방문도 실린다(§1-2(3))."""
    _, _, result = _case(seed=0)
    revisits = [
        (loc, seq)
        for loc, seq in result.visit_order
        if len({r for r, _ in seq}) < len(seq)  # 같은 로봇이 두 번 이상 들어 있다
    ]
    assert revisits, "한 로봇이 같은 정점을 두 번 방문하는 사례가 없어 시험이 성립하지 않는다"
    for _, seq in revisits:  # 재방문 두 항목의 index가 서로 다르다
        for robot in {r for r, _ in seq}:
            mine = [i for r, i in seq if r == robot]
            assert len(set(mine)) == len(mine)


def test_steps_fold_consecutive_waits():
    """접은 열에는 연속한 동일 정점이 없다(§1-2(1))."""
    _, _, result = _case(seed=4)
    for path in result.paths:
        steps, _ = fold_timeline(path.timeline)
        assert all(a != b for a, b in zip(steps, steps[1:]))


def test_priority_rule_changes_the_result():
    """우선순위 규칙이 실제로 결과를 바꾼다 — 인자가 살아 있다는 확인."""
    shapes = set()
    for rule in PRIORITY_RULES:
        _, _, result = _case(seed=1, priority=rule)
        shapes.add(tuple((p.robot, tuple(p.timeline)) for p in result.paths))
    assert len(shapes) > 1


def test_unknown_priority_rule_rejected():
    model, assignment, _ = _case(seed=0)
    with pytest.raises(ValueError):
        plan_instance(model, assignment, PlanParams(priority="no_such_rule"))


def test_small_horizon_truncates_and_says_so():
    """지평을 줄이면 절단이 늘고, 그 사실이 terminal과 failures에 나타난다."""
    _, _, tight = _case(seed=0, horizon=4)
    assert any(p.terminal != "goal_reached" for p in tight.paths)
    assert tight.failures and not tight.ok


def test_capacity_one_edges_carry_one_robot_per_step():
    """용량 1 간선을 같은 타임스텝에 둘이 쓰지 않는다."""
    model, assignment, result = _case(seed=5)
    caps = {}
    for e in model.edges:
        caps[(e.u, e.v)] = caps[(e.v, e.u)] = e.capacity_robots
    use: dict[tuple[int, frozenset], int] = {}
    for path in result.paths:
        for t in range(len(path.timeline) - 1):
            u, v = path.timeline[t], path.timeline[t + 1]
            if u == v:
                continue
            key = (t, frozenset((u, v)))
            use[key] = use.get(key, 0) + 1
            assert use[key] <= caps[(u, v)]


def test_broken_plan_is_detected():
    """자체 점검이 실제로 충돌을 잡는가 — 두 로봇을 같은 정점에 세운다."""
    model, assignment, result = _case(seed=0)
    victim = result.paths[1]
    victim.timeline = list(result.paths[0].timeline)
    problems = validate_plan(model, assignment, result)
    assert any("정점 충돌" in p for p in problems)
