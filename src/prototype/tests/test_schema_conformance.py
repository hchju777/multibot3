"""경계 산출물 시험 — 계약 스키마의 **구조** 검증.

⚠️ 표준 JSON Schema 검증기는 확장 키워드(x-ref · x-unique-key · x-equal-to ·
x-sorted · x-exclusive-with · x-set-equal · x-derived-from)를 **전부 무시한다**
(48_contract_amendment.md §18-7: 52자리 중 0건 검사). 여기서 오류 0이라는 사실은
«구조가 맞다»는 뜻이지 «계약을 만족한다»는 뜻이 아니다. 확장 규칙은 별도 담당의
층 1 검증기가 본다.
"""

import json
import os

import pytest
from jsonschema import Draft202012Validator

from assignment_core import AssignParams, build_assignment
from boundary_adapter import assignment_to_boundary, plan_to_boundary, roadmap_to_boundary
from plan_core import PlanParams, plan_instance
from protoscale import load_scale
from roadmap_core import GenParams, generate_roadmap

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


@pytest.mark.parametrize("seed", [0, 1, 2, 3, 4])
def test_roadmap_passes_structural_validation(seed):
    doc = roadmap_to_boundary(generate_roadmap(CFG, GenParams(seed=seed)))
    assert _errors(_schema("roadmap.schema.json"), doc) == []


@pytest.mark.parametrize("seed", [0, 1, 2, 3, 4])
def test_assignment_passes_structural_validation(seed):
    model = generate_roadmap(CFG, GenParams(seed=seed))
    doc = assignment_to_boundary(
        build_assignment(model, AssignParams(robots=6, tasks=14, seed=seed))
    )
    assert _errors(_schema("assignment.schema.json"), doc) == []


def test_scaled_roadmap_still_validates():
    """규모를 키워도 구조 검증을 통과한다."""
    doc = roadmap_to_boundary(
        generate_roadmap(CFG, GenParams(aisles=8, cross=6, endpoints=20, seed=9))
    )
    assert _errors(_schema("roadmap.schema.json"), doc) == []


def test_validator_actually_rejects_a_contract_violation():
    """검증이 실제로 걸리는가 — §18-3이 금지한 null corridor를 일부러 넣는다."""
    doc = roadmap_to_boundary(generate_roadmap(CFG, GenParams(seed=0)))
    doc["edges"][0]["corridor"] = None
    assert _errors(_schema("roadmap.schema.json"), doc) != []


def test_forbidden_algorithm_parameter_is_rejected_by_closure():
    """additionalProperties:false가 새 파라미터의 유입을 막는다."""
    doc = roadmap_to_boundary(generate_roadmap(CFG, GenParams(seed=0)))
    doc["seed"] = 7
    assert _errors(_schema("roadmap.schema.json"), doc) != []


@pytest.mark.parametrize("seed", [0, 1, 2, 3, 4])
def test_discrete_plan_passes_structural_validation(seed):
    model = generate_roadmap(CFG, GenParams(seed=seed))
    assignment = build_assignment(model, AssignParams(robots=6, tasks=8, seed=seed))
    doc = plan_to_boundary(plan_instance(model, assignment, PlanParams()))
    assert _errors(_schema("discrete_plan.schema.json"), doc) == []


def test_discrete_plan_rejects_unknown_terminal():
    """terminal은 닫힌 어휘 3값이다 — 넷째 값을 넣으면 걸린다."""
    model = generate_roadmap(CFG, GenParams(seed=0))
    assignment = build_assignment(model, AssignParams(robots=6, tasks=8, seed=0))
    doc = plan_to_boundary(plan_instance(model, assignment, PlanParams()))
    doc["plans"][0]["terminal"] = "fallback_used"
    assert _errors(_schema("discrete_plan.schema.json"), doc) != []


def test_discrete_plan_rejects_single_item_visit_order():
    """visit_order[].sequence는 항목 2개 이상이어야 한다(minItems: 2)."""
    model = generate_roadmap(CFG, GenParams(seed=0))
    assignment = build_assignment(model, AssignParams(robots=6, tasks=8, seed=0))
    doc = plan_to_boundary(plan_instance(model, assignment, PlanParams()))
    assert doc["visit_order"], "공유 위치가 있어야 시험이 성립한다"
    doc["visit_order"][0]["sequence"] = doc["visit_order"][0]["sequence"][:1]
    assert _errors(_schema("discrete_plan.schema.json"), doc) != []
