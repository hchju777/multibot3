"""비곱 인스턴스 헌팅 시험 — 곱꼴 판별기 자체와 최소 목격자.

🔴 **여기서 통과하는 것은 판정이 아니다.** 시험을 쓴 사람과 승인하는 사람이 다르다.

이 파일이 박아 두려는 것 둘:
1. **최소 비곱 목격자**(환상 통로 4정점·로봇 2대)가 회귀로 남는다 — «D가 M을 넘는 자리»의
   유일한 실험 재료이므로 코드가 바뀌어 이 성질이 사라지면 즉시 드러나야 한다.
2. **가지치기 뒤에 재면 언제나 곱꼴**이라는 사실 — 측정 절차가 결론을 만들어내는 자리다.
"""

from __future__ import annotations

from assignment_core import AssignmentModel, GoalItem, RobotPlan
from nonproduct_hunt import (
    antiparallel_diagnosis,
    build_loop_roadmap,
    joint_analysis,
    loop_robot_specs,
)
from plan_core import PlanParams, plan_instance
from protoscale import load_scale
from sadg_core import PlanView, compile_graph, plan_view_of, prune_cyclic_alternatives


def loop_view(loop_nodes: int = 4, robots: int = 2, one_way: bool = True) -> PlanView:
    """환상 통로 인스턴스 하나를 계획까지 돌려 뷰로 만든다."""
    cfg = load_scale()
    specs = loop_robot_specs(loop_nodes, robots)
    model = build_loop_roadmap(cfg, loop_nodes, one_way, 1, specs)
    assignment = AssignmentModel(
        instance_id=model.instance_id,
        revision=0,
        robots=[
            RobotPlan(name, f"S{name}", [GoalItem(f"g-{name}", f"t{name}", f"G{name}")])
            for name, _, _ in specs
        ],
        unassigned_tasks=[],
    )
    plan = plan_instance(model, assignment, PlanParams(horizon=96))
    assert not plan.unplanned
    return plan_view_of(plan)


def test_minimal_loop_instance_is_non_product():
    """최소 목격자 — 그룹 2개 × 대안 2개, 비순환 3개, 사영의 곱 4개."""
    view = loop_view()
    graph = compile_graph(view, alt_generator="lift")
    report = joint_analysis(graph)

    assert report["groups"] == 2
    assert report["alternatives_per_group"] == [2, 2]
    assert report["total_combinations"] == 4
    assert report["acyclic_combinations"] == 3
    assert report["projection_sizes"] == [2, 2]
    assert report["projection_product_size"] == 4
    assert report["non_product"] is True
    assert report["solo_cyclic_alternatives"] == 0
    # 혼자 순환하는 대안이 하나도 없으므로 순환의 최소 지지는 2개 그룹이다
    assert report["min_forcing_support"] == 2
    assert report["non_product_witness"]["minimal_forcing_support"] == [0, 1]


def test_minimal_loop_instance_has_an_antiparallel_pair():
    """진단 지표 — 두 로봇이 두 그룹 위치를 **반대 순서로** 지난다."""
    view = loop_view()
    graph = compile_graph(view, alt_generator="lift")
    diagnosis = antiparallel_diagnosis(view, graph)
    assert diagnosis["antiparallel_group_pairs"] >= 1


def test_pruning_makes_the_space_product_by_construction():
    """🔴 **가지치기 뒤에 재면 언제나 곱꼴이다** — 그래서 곱꼴 판별은 가지치기 전에 해야 한다."""
    view = loop_view()
    graph = compile_graph(view, alt_generator="lift")
    assert joint_analysis(graph)["non_product"] is True

    pruned = compile_graph(view, alt_generator="lift")
    removed, _log = prune_cyclic_alternatives(pruned)
    assert removed >= 1
    after = joint_analysis(pruned)
    assert after["non_product"] is False
    # 가지치기는 «구멍»을 지우는 대신 **대안 자체**를 지운다 — 방이 줄어든다
    assert after.get("acyclic_combinations", 0) < 3


def test_joint_analysis_reports_product_form_when_groups_are_independent():
    """두 그룹이 서로 다른 로봇 쌍을 쓰면 결합이 없어 곱꼴이다 — 판별기가 참을 참이라 하는가."""
    view = PlanView(
        instance_id="independent",
        revision=0,
        steps={
            "a": ["a0", "v", "a1"],
            "b": ["b0", "v", "b1"],
            "c": ["c0", "w", "c1"],
            "d": ["d0", "w", "d1"],
        },
        visit_order=[
            ("v", [("a", 1), ("b", 1)]),
            ("w", [("c", 1), ("d", 1)]),
        ],
    )
    report = joint_analysis(compile_graph(view, alt_generator="lift"))
    assert report["groups"] == 2
    assert report["non_product"] is False
    assert report["acyclic_combinations"] == report["projection_product_size"]


def test_joint_analysis_marks_oversized_instances_instead_of_sampling():
    """상한을 넘으면 «곱꼴이다»라고 적지 않고 `exhaustive=False`로 판정을 비운다."""
    view = loop_view()
    report = joint_analysis(compile_graph(view, alt_generator="lift"), max_combinations=2)
    assert report["exhaustive"] is False
    assert "non_product" not in report
