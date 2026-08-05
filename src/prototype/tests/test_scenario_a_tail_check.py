"""§65-8 M2′ 3번 — S-A 체크포인트 스크립트 시험. `pick_checkpoints` + 실 아티팩트 통합.

🔴 여기서 통과하는 것은 판정이 아니다. 시험을 쓴 사람과 승인하는 사람이 다르다.
"""

from __future__ import annotations

import os

import pytest

from episode_timeline import run_episode
from protoscale import load_scale
from sadg_core import PlanView, Segment, compile_graph, plan_view_of, prune_cyclic_alternatives
from scenario_a_tail_check import VARIANTS, load_bundle, pick_checkpoints

HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.normpath(os.path.join(HERE, "..", "out"))
CFG = load_scale()


class _FakeSeg:
    def __init__(self, robot, number, dst_node):
        self.key = (robot, number)
        self.dst_node = dst_node


class _FakeGraph:
    def __init__(self, segments):
        self.segments = segments


class _FakeRow:
    def __init__(self, completed_segment):
        self.completed_segment = completed_segment


def test_pick_checkpoints_finds_first_last_and_a_crossing_row():
    graph = _FakeGraph([
        _FakeSeg("A", 0, "L"),
        _FakeSeg("A", 1, "R"),   # A가 R에 도착 — 통로를 다 지난 사건
        _FakeSeg("A", 2, "GA"),
        _FakeSeg("B", 0, "GB"),
    ])
    rows = [
        _FakeRow("A#0"), _FakeRow("B#0"), _FakeRow("A#1"), _FakeRow("A#2"),
    ]
    checkpoints = pick_checkpoints(graph, rows)
    names = [name for name, _ in checkpoints]
    assert names == ["첫 완료 사건", "A가 통로를 다 지난 직후(dst=R)", "마지막 완료 사건(전원 완료)"]
    assert checkpoints[0][1] is rows[0]
    assert checkpoints[1][1].completed_segment == "A#1"
    assert checkpoints[2][1] is rows[-1]


def test_pick_checkpoints_returns_empty_for_no_rows():
    assert pick_checkpoints(_FakeGraph([]), []) == []


def test_pick_checkpoints_omits_a_crossing_when_a_never_reaches_r():
    graph = _FakeGraph([_FakeSeg("A", 0, "L"), _FakeSeg("A", 1, "M1")])
    rows = [_FakeRow("A#0"), _FakeRow("A#1")]
    checkpoints = pick_checkpoints(graph, rows)
    names = [name for name, _ in checkpoints]
    assert "A가 통로를 다 지난 직후(dst=R)" not in names
    assert names == ["첫 완료 사건", "마지막 완료 사건(전원 완료)"]


@pytest.mark.parametrize("topology,capacity", VARIANTS)
def test_scenario_a_variant_recompiles_without_raising(topology, capacity):
    """실 S-A 아티팩트(§65-2)로 끝까지 돈다 — 존재하지 않으면 건너뛴다(기존 관례와 같다)."""
    out_dir = os.path.join(OUT, f"scenA_{topology}_cap{capacity}")
    if not os.path.exists(os.path.join(out_dir, "discrete_plan.json")):
        pytest.skip("S-A out/ 아티팩트가 없다 — gen_scenario_a.py + gen_plan.py를 먼저 돌린다")
    model, result = load_bundle(out_dir)
    view = plan_view_of(result)
    graph = compile_graph(view, alt_generator="lift")
    prune_cyclic_alternatives(graph)
    ep = run_episode(graph, model, CFG.nominal_speed_mps, blockage=None,
                      max_ticks=5000, view=view)
    assert ep.makespan_s > 0.0
    checkpoints = pick_checkpoints(graph, ep.rows)
    assert len(checkpoints) >= 2  # 최소 첫/마지막 사건은 항상 있다
    for _, row in checkpoints:
        assert row.recompiled_acyclic_combinations is not None
