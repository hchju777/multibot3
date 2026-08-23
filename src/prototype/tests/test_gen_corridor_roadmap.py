"""통로 로드맵 생성기(265d A부) 시험 — 구조 자기 검사 · 스키마 정합 · 결정론.

🔴 결과의 좋고 나쁨을 판정하지 않는다 — 생성기가 만들기로 한 구조를 실제로
만드는지, 검사기가 실제로 발화하는지(0건 검사 방지)만 고정한다.
"""

import dataclasses
import json
import os

import pytest
from jsonschema import Draft202012Validator

from boundary_adapter import roadmap_to_boundary
from gen_corridor_roadmap import (
    CorridorGenParams,
    generate_corridor_roadmap,
    validate_corridor_spike,
)
from protoscale import load_scale
from roadmap_core import validate_model

HERE = os.path.dirname(os.path.abspath(__file__))
SCHEMA_DIR = os.path.normpath(os.path.join(HERE, "..", "..", "mrs_msgs", "schema"))
CFG = load_scale()


def _schema(name):
    with open(os.path.join(SCHEMA_DIR, name), "r", encoding="utf-8") as handle:
        return json.load(handle)


def _errors(schema, doc):
    return [
        f"{list(e.path)}: {e.message}" for e in Draft202012Validator(schema).iter_errors(doc)
    ]


@pytest.mark.parametrize("seed", [0, 1, 2])
@pytest.mark.parametrize("spurs", [False, True])
def test_structure_self_check_passes(seed, spurs):
    params = CorridorGenParams(seed=seed, wait_spurs=spurs)
    model = generate_corridor_roadmap(CFG, params)
    assert validate_model(model) == []
    assert validate_corridor_spike(model, params) == []


@pytest.mark.parametrize("seed", [0, 1, 2])
def test_corridors_are_single_long_edges(seed):
    """새 방향의 핵심 — 통로마다 간선 하나, 내부 정점 0, 길이는 입력 집합 안."""
    params = CorridorGenParams(seed=seed)
    model = generate_corridor_roadmap(CFG, params)
    assert model.corridors, "교행 불가 통로가 0개면 스파이크 목적을 잃는다"
    edge_by_pair = {(e.u, e.v): e for e in model.edges}
    for c in model.corridors:
        assert len(c.edge_keys) == 1
        e = edge_by_pair[c.edge_keys[0]]
        assert e.capacity_robots == 1
        assert c.length_m in params.corridor_lengths_m


def test_lengths_are_tens_of_meters_mixed():
    """길이 집합 {15,30,60}이 실제로 섞여 나온다(씨앗 몇 개 합쳐서)."""
    seen = set()
    for seed in range(4):
        model = generate_corridor_roadmap(CFG, CorridorGenParams(seed=seed))
        seen |= {c.length_m for c in model.corridors}
    assert seen == {15.0, 30.0, 60.0}


def test_wait_spur_option_changes_structure():
    """대기 지선 켬/끔이 실제 구조 차이로 나타난다 — 켰는데 0회 발화를 막는다."""
    off = generate_corridor_roadmap(CFG, CorridorGenParams(seed=0, wait_spurs=False))
    on = generate_corridor_roadmap(CFG, CorridorGenParams(seed=0, wait_spurs=True))
    spur_nodes_on = [n.key for n in on.nodes if n.key.startswith("W")]
    spur_nodes_off = [n.key for n in off.nodes if n.key.startswith("W")]
    assert spur_nodes_off == []
    assert len(spur_nodes_on) == 2 * len(on.corridors)  # 통로 양끝마다 하나
    assert set(spur_nodes_on) <= set(on.endpoints)  # 대기 정점은 V^ep에 실린다


def test_validator_actually_fires():
    """자기 검사가 실제로 걸린다 — 통로 간선 용량을 2로 바꾸면 위반이 나와야 한다."""
    params = CorridorGenParams(seed=0)
    model = generate_corridor_roadmap(CFG, params)
    pair = model.corridors[0].edge_keys[0]
    for i, e in enumerate(model.edges):
        if (e.u, e.v) == pair:
            model.edges[i] = dataclasses.replace(e, capacity_robots=2)
            break
    assert any("용량" in p for p in validate_corridor_spike(model, params))


@pytest.mark.parametrize("spurs", [False, True])
def test_boundary_doc_passes_schema(spurs):
    """mrs.roadmap 3.0.0 구조 검증 — 기존 시험과 같은 수단(확장 키워드는 못 본다)."""
    model = generate_corridor_roadmap(CFG, CorridorGenParams(seed=0, wait_spurs=spurs))
    doc = roadmap_to_boundary(model)
    assert _errors(_schema("roadmap.schema.json"), doc) == []
    assert doc["schema_version"] == "3.0.0"


def test_deterministic_same_seed_same_doc():
    """같은 인자 두 번 = 같은 경계 문서(바이트 동일 직렬화)."""
    a = roadmap_to_boundary(generate_corridor_roadmap(CFG, CorridorGenParams(seed=3)))
    b = roadmap_to_boundary(generate_corridor_roadmap(CFG, CorridorGenParams(seed=3)))
    assert json.dumps(a, sort_keys=True) == json.dumps(b, sort_keys=True)


def test_different_seed_differs():
    """씨앗이 다르면 지도가 다르다 — 씨앗이 실제로 쓰인다."""
    a = roadmap_to_boundary(generate_corridor_roadmap(CFG, CorridorGenParams(seed=0)))
    b = roadmap_to_boundary(generate_corridor_roadmap(CFG, CorridorGenParams(seed=1)))
    assert json.dumps(a, sort_keys=True) != json.dumps(b, sort_keys=True)
