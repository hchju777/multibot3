#!/usr/bin/env python3
"""S1 — 최소 로드맵 생성기 (탐색용 시제).

창고형 로드맵을 시드 결정론적으로 만들어 mrs.roadmap 아티팩트로 쓴다.
물리 규모는 config/scale.yaml 하나에서만 나온다.

사용 예:
    python3 gen_roadmap.py --seed 0 --aisles 4 --cross 3 --endpoints 8 \
        --out out/roadmap.json --schema-check

🔴 탐색용 시제이며 증거가 아니다. 어떤 수치도 사전등록 상수가 아니다.
"""

from __future__ import annotations

import argparse
import json
import os
import sys

from boundary_adapter import roadmap_to_boundary
from protoscale import DEFAULT_SCALE_PATH, load_scale
from roadmap_core import GenParams, edge_length_stats, generate_roadmap, validate_model

HERE = os.path.dirname(os.path.abspath(__file__))
ROADMAP_SCHEMA_PATH = os.path.normpath(
    os.path.join(HERE, "..", "mrs_msgs", "schema", "roadmap.schema.json")
)


def structural_schema_check(doc: dict, schema_path: str) -> int:
    """표준 JSON Schema 검증만 돌린다.

    ⚠️ 확장 키워드(x-ref 참조 무결성 · x-unique-key 유일성 · x-equal-to 등)는
    표준 검증기가 전부 무시한다. 여기서 오류 0이어도 «계약 만족»이 아니다.
    """
    from jsonschema import Draft202012Validator

    with open(schema_path, "r", encoding="utf-8") as handle:
        schema = json.load(handle)
    errors = list(Draft202012Validator(schema).iter_errors(doc))
    for err in errors[:10]:
        print(f"  [schema] {list(err.path)}: {err.message[:160]}", file=sys.stderr)
    return len(errors)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="최소 로드맵 생성기 (탐색용 시제)")
    ap.add_argument("--seed", type=int, default=0, help="난수 시드. 같은 시드면 같은 로드맵")
    ap.add_argument("--aisles", type=int, default=4, help="세로 통로(격자 열) 개수")
    ap.add_argument("--cross", type=int, default=3, help="가로 교차통로(격자 행) 개수")
    ap.add_argument("--endpoints", type=int, default=8, help="엔드포인트 지선 개수")
    ap.add_argument(
        "--narrow-ratio", type=float, default=0.5, help="내부 통로를 교행 불가로 둘 비율"
    )
    ap.add_argument("--scale", default=DEFAULT_SCALE_PATH, help="물리 규모 설정 파일")
    ap.add_argument("--out", default=os.path.join(HERE, "out", "roadmap.json"))
    ap.add_argument("--schema-check", action="store_true", help="표준 스키마 구조 검증도 돌린다")
    args = ap.parse_args(argv)

    cfg = load_scale(args.scale)
    params = GenParams(
        aisles=args.aisles,
        cross=args.cross,
        endpoints=args.endpoints,
        narrow_ratio=args.narrow_ratio,
        seed=args.seed,
    )
    model = generate_roadmap(cfg, params)

    problems = validate_model(model)
    for p in problems:
        print(f"  [self-check] {p}", file=sys.stderr)

    doc = roadmap_to_boundary(model)
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    with open(args.out, "w", encoding="utf-8") as handle:
        json.dump(doc, handle, indent=2, ensure_ascii=False, sort_keys=False)
        handle.write("\n")

    stats = edge_length_stats(model)
    narrow = sum(1 for e in model.edges if e.capacity_robots == 1)
    print(f"instance_id      : {model.instance_id}")
    print(f"scale.yaml       : {cfg.source_path}")
    print(f"nodes / edges    : {len(model.nodes)} / {len(model.edges)}")
    print(f"corridors        : {len(model.corridors)} (교행 불가 간선 {narrow}개)")
    print(f"endpoints        : {len(model.endpoints)}")
    print(
        "edge length (m)  : min {min_m:.3f} / median {median_m:.3f} / max {max_m:.3f}".format(
            **stats
        )
    )
    print(f"self-check 위반  : {len(problems)}")
    print(f"wrote            : {os.path.abspath(args.out)}")

    schema_errors = 0
    if args.schema_check:
        schema_errors = structural_schema_check(doc, ROADMAP_SCHEMA_PATH)
        print(f"schema errors    : {schema_errors} (구조만. 확장 규칙 미검사)")

    return 1 if (problems or schema_errors) else 0


if __name__ == "__main__":
    raise SystemExit(main())
