"""증분 삽입 시제(265d B부) 시험 — 불변식(비순환·H-cons) · 합류 발화 · 결정론.

🔴 벽시계 수치를 판정하지 않는다 — 탐침이 재려는 것을 재고 있는지만 고정한다.
"""

import pytest

from gen_corridor_roadmap import CorridorGenParams, generate_corridor_roadmap
from insertion_spike import SpikeState, run_sweep
from protoscale import load_scale
from roadmap_core import Corridor, Edge, Node, RoadmapModel

CFG = load_scale()


def _chain_model() -> RoadmapModel:
    """JA—JB=(통로 30 m)=JC—JD 사슬 — 합류·H-cons를 손으로 셀 수 있는 최소 지도."""
    nodes = [
        Node("JA", 0.0, 0.0, 1.0),
        Node("JB", 10.0, 0.0, 1.0),
        Node("JC", 40.0, 0.0, 1.0),
        Node("JD", 50.0, 0.0, 1.0),
    ]
    edges = [
        Edge("JA", "JB", 10.0, 2.4, 2, "", "corridor"),
        Edge("JB", "JC", 30.0, 1.2, 1, "q_JB_JC", "corridor"),
        Edge("JC", "JD", 10.0, 2.4, 2, "", "corridor"),
    ]
    corridors = [Corridor("q_JB_JC", (("JB", "JC"),), 30.0)]
    return RoadmapModel(
        instance_id="chain_test", nodes=nodes, edges=edges, corridors=corridors,
        endpoints=["JA", "JD"], min_separation_m=1.0, wall_inflation_m=0.0,
    )


@pytest.mark.parametrize("regime", ["exclusive", "following"])
def test_mid_join_actually_fires_and_keeps_invariants(regime):
    """동→서→동이면 셋째가 앞선 동쪽 배치에 **중간 삽입**된다 — 켰는데 0회 발화 방지.

    손 계산: π_c 방향 열이 말미 추가만이면 [+1, -1, +1](배치 셋),
    중간 삽입이 발화하면 [+1, +1, -1](배치 둘)이어야 한다.
    """
    st = SpikeState(_chain_model(), regime, 2.0, 1.0, allow_mid=True)
    st.insert_robot("r0", "JA", "JD")
    st.insert_robot("r1", "JD", "JA")
    info = st.insert_robot("r2", "JA", "JD")
    assert info["mid_join_candidates"] == 1
    assert info["mid_join_applied"] == 1
    assert info["fell_back"] == 0
    assert [t.direction for t in st.corr_seq["q_JB_JC"]] == [+1, +1, -1]
    assert st.check_acyclic()
    assert st.check_hcons() == []


def test_tail_only_mode_makes_new_batch_instead():
    """중간 삽입을 끄면 같은 시나리오가 말미 새 배치가 된다 — 옵션이 실제로 갈린다."""
    st = SpikeState(_chain_model(), "exclusive", 2.0, 1.0, allow_mid=False)
    st.insert_robot("r0", "JA", "JD")
    st.insert_robot("r1", "JD", "JA")
    info = st.insert_robot("r2", "JA", "JD")
    assert info["mid_join_candidates"] == 0
    assert [t.direction for t in st.corr_seq["q_JB_JC"]] == [+1, -1, +1]
    assert st.check_acyclic()
    assert st.check_hcons() == []


def test_hcons_restriction_equal_on_both_ends():
    """H-cons 검사의 내용 확인 — 양끝 제한 열이 통과 단위 대응으로 같다(손 대조)."""
    st = SpikeState(_chain_model(), "exclusive", 2.0, 1.0)
    st.insert_robot("r0", "JA", "JD")
    st.insert_robot("r1", "JD", "JA")
    st.insert_robot("r2", "JA", "JD")
    sa = st._restricted("q_JB_JC", "JB")
    sb = st._restricted("q_JB_JC", "JC")
    assert sa == sb == [("r0", 1), ("r2", 1), ("r1", 1)]


def test_acyclicity_checker_actually_fires_on_a_cycle():
    """검증기가 실제로 순환을 잡는다 — 손으로 만든 순환 순서(265a §4-3 반례 꼴)."""
    st = SpikeState(_chain_model(), "exclusive", 2.0, 1.0)
    st.steps = {"r": ["JB", "JC"], "z": ["JC", "JB"]}
    st.omega["JB"] = [("z", 1), ("r", 0)]
    st.omega["JC"] = [("r", 1), ("z", 0)]
    assert st.check_acyclic() is False


def test_penalty_regime_switch_changes_headway():
    """체제 스위치가 h를 실제로 바꾼다 — 배타 h=D=30 s, 추종 h=δ_follow=2 s."""
    ex = SpikeState(_chain_model(), "exclusive", 2.0, 1.0)
    fo = SpikeState(_chain_model(), "following", 2.0, 1.0)
    assert ex.headway_s("q_JB_JC") == 30.0
    assert fo.headway_s("q_JB_JC") == 2.0
    ex.insert_robot("r0", "JA", "JD")
    fo.insert_robot("r0", "JA", "JD")
    # 같은 방향 말미 합류 비용 = 배치 크기 x h — 체제에 따라 다르다.
    assert ex.entry_penalty("q_JB_JC", +1) == 30.0
    assert fo.entry_penalty("q_JB_JC", +1) == 2.0


def test_generated_map_sweep_keeps_invariants_and_counts():
    """생성 지도에서 로봇 12대 순차 삽입 — 항목 수 회계와 불변식."""
    model = generate_corridor_roadmap(CFG, CorridorGenParams(seed=0))
    for regime in ("exclusive", "following"):
        st = SpikeState(model, regime, 2.0, CFG.nominal_speed_mps)
        import random

        rng = random.Random(7)
        nodes = sorted(st.omega)
        for k in range(12):
            s, g = rng.sample(nodes, 2)
            st.insert_robot(f"r{k:03d}", s, g)
            assert st.check_acyclic()
            assert st.check_hcons() == []
        assert st.total_entries() == sum(len(s) for s in st.steps.values())
        assert sum(len(lst) for lst in st.omega.values()) == st.total_entries()


def test_remove_and_reinsert_keeps_invariants():
    """수리 탐침의 두 연산(제거·재삽입)이 불변식을 지킨다."""
    model = generate_corridor_roadmap(CFG, CorridorGenParams(seed=1))
    st = SpikeState(model, "exclusive", 2.0, CFG.nominal_speed_mps)
    import random

    rng = random.Random(11)
    nodes = sorted(st.omega)
    origins = {}
    for k in range(10):
        s, g = rng.sample(nodes, 2)
        origins[f"r{k:03d}"] = (s, g)
        st.insert_robot(f"r{k:03d}", s, g)
    st.remove_robot("r003")
    assert "r003" not in st.steps
    assert all(e[0] != "r003" for lst in st.omega.values() for e in lst)
    assert st.check_acyclic()
    assert st.check_hcons() == []
    st.insert_robot("r003", *origins["r003"])
    assert st.check_acyclic()
    assert st.check_hcons() == []
    assert set(st.steps) == set(origins)


def test_sweep_is_deterministic():
    """같은 인자의 스윕 두 번 = 같은 구조 지문(벽시계 제외)."""
    base = CorridorGenParams()
    kw = dict(delta_follow_s=2.0, allow_mid=True, map_params_base=base, repair_ks=[1])
    _, _, d1 = run_sweep([10], ["exclusive"], [0, 1], **kw)
    _, _, d2 = run_sweep([10], ["exclusive"], [0, 1], **kw)
    assert d1 == d2
    assert len(d1) == 2
