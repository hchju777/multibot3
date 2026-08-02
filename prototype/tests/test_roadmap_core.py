"""로드맵 생성 코어 시험 — 결정론 · 구조 불변식 · 물리 규모가 설정에서만 나오는가."""

import copy
import os

import pytest
import yaml

from protoscale import DEFAULT_SCALE_PATH, load_scale
from roadmap_core import GenParams, edge_length_stats, generate_roadmap, validate_model

CFG = load_scale()


def _gen(seed=0, **kw):
    params = GenParams(seed=seed, **kw)
    return generate_roadmap(CFG, params)


def test_same_seed_same_roadmap():
    a, b = _gen(seed=3), _gen(seed=3)
    assert a == b


def test_different_seed_differs():
    seeds = {tuple(sorted(e.corridor_key for e in _gen(seed=s).edges)) for s in range(6)}
    assert len(seeds) > 1, "시드를 바꿔도 통로 배치가 하나뿐이면 생성기가 시드를 안 쓴다"


def test_structural_invariants_hold():
    for seed in range(8):
        model = _gen(seed=seed)
        assert validate_model(model) == []


def test_mixed_capacity_present():
    """교행 가능(용량>=2)과 교행 불가(용량 1) 간선이 섞여 있어야 한다."""
    model = _gen(seed=0)
    caps = {e.capacity_robots for e in model.edges}
    assert any(c >= 2 for c in caps)
    assert 1 in caps
    assert model.corridors, "교행 불가 통로 묶음이 하나도 없다"


def test_narrow_edges_are_bundled_into_corridors():
    """통로에 속한 간선은 전부 용량 1이고, 통로 길이는 구성 간선 길이의 합이다."""
    model = _gen(seed=1)
    for c in model.corridors:
        members = [e for e in model.edges if e.corridor_key == c.key]
        assert members
        assert all(e.capacity_robots == 1 for e in members)
        assert c.length_m == pytest.approx(sum(e.length_m for e in members))
        assert set(c.edge_keys) == {(e.u, e.v) for e in members}


def test_min_edge_length_ge_min_separation():
    model = _gen(seed=2)
    stats = edge_length_stats(model)
    assert stats["min_m"] >= CFG.min_separation_m


def test_geometry_comes_from_config_only(tmp_path):
    """scale.yaml만 바꾸면 기하가 통째로 바뀐다 — 코드에 치수가 박혀 있지 않다."""
    with open(DEFAULT_SCALE_PATH, "r", encoding="utf-8") as handle:
        raw = yaml.safe_load(handle)
    scaled = copy.deepcopy(raw)
    scaled["geometry"]["corridor_segment_length_m"] = 8.0
    scaled["geometry"]["endpoint_stub_length_m"] = 3.0
    path = tmp_path / "scale_alt.yaml"
    with open(path, "w", encoding="utf-8") as handle:
        yaml.safe_dump(scaled, handle)

    base = generate_roadmap(CFG, GenParams(seed=0))
    alt = generate_roadmap(load_scale(str(path)), GenParams(seed=0))
    assert edge_length_stats(base)["median_m"] == 5.0
    assert edge_length_stats(alt)["median_m"] == 8.0
    assert {e.length_m for e in alt.edges} == {8.0, 3.0}
    # 위상은 같고 기하만 바뀐다
    assert [n.key for n in base.nodes] == [n.key for n in alt.nodes]


def test_capacity_rule_a_is_monotone_in_width():
    assert CFG.capacity_rule_a(CFG.aisle_width_narrow_m) == 1
    assert CFG.capacity_rule_a(CFG.aisle_width_wide_m) >= 2
    assert CFG.capacity_rule_a(0.1) == 1  # 하한 1로 잘린다
    assert CFG.capacity_rule_a(1000.0) == CFG.max_capacity_robots


def test_endpoints_are_nodes_and_unique():
    model = _gen(seed=4, endpoints=6)
    keys = {n.key for n in model.nodes}
    assert len(model.endpoints) == 6
    assert set(model.endpoints) <= keys
    assert len(set(model.endpoints)) == len(model.endpoints)


def test_broken_model_is_detected():
    """자체 점검이 실제로 위반을 잡는가 — 참조 무결성을 일부러 깬다."""
    from roadmap_core import Edge

    model = _gen(seed=0)
    model.edges = list(model.edges) + [Edge("NO_SUCH_A", "NO_SUCH_B", 5.0, 2.4, 2, "", "aisle")]
    problems = validate_model(model)
    assert any("nodes에 없다" in p for p in problems)


def test_too_small_grid_rejected():
    with pytest.raises(ValueError):
        generate_roadmap(CFG, GenParams(aisles=1, cross=3))
