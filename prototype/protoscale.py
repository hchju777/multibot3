"""물리 규모 설정 로더.

이 프로토타입에서 길이·폭·속도·차체 치수가 나오는 **유일한 곳**이다.
아이작 심 에셋 치수가 정해지면 config/scale.yaml 하나만 바꾼다.

🔴 탐색용 임시값이며 사전등록 상수가 아니다.
"""

from __future__ import annotations

import math
import os
from dataclasses import dataclass

import yaml

DEFAULT_SCALE_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "config", "scale.yaml")


@dataclass(frozen=True)
class ScaleConfig:
    """config/scale.yaml 한 장의 값. 코드 어디에도 이 값들의 사본이 없다."""

    corridor_segment_length_m: float
    endpoint_stub_length_m: float
    aisle_width_wide_m: float
    aisle_width_narrow_m: float
    stub_width_m: float
    wall_inflation_m: float
    nominal_speed_mps: float
    body_width_m: float
    body_length_m: float
    min_separation_m: float
    lateral_gap_m: float
    max_capacity_robots: int
    source_path: str

    def capacity_rule_a(self, width_m: float) -> int:
        """폭에서 동시 통과 로봇 수 상한을 낸다(규칙 rule_a).

        cap = clamp( floor(width / (body_width + lateral_gap)), 1, max )
        """
        lane = self.body_width_m + self.lateral_gap_m
        raw = int(math.floor(width_m / lane)) if lane > 0 else 1
        return max(1, min(self.max_capacity_robots, raw))


def load_scale(path: str | None = None) -> ScaleConfig:
    """scale.yaml을 읽는다. 값 검증은 여기서 한 번만 한다."""
    path = path or DEFAULT_SCALE_PATH
    with open(path, "r", encoding="utf-8") as handle:
        raw = yaml.safe_load(handle)

    geo = raw["geometry"]
    rob = raw["robot"]
    cap = raw["capacity_rule_a"]
    cfg = ScaleConfig(
        corridor_segment_length_m=float(geo["corridor_segment_length_m"]),
        endpoint_stub_length_m=float(geo["endpoint_stub_length_m"]),
        aisle_width_wide_m=float(geo["aisle_width_wide_m"]),
        aisle_width_narrow_m=float(geo["aisle_width_narrow_m"]),
        stub_width_m=float(geo["stub_width_m"]),
        wall_inflation_m=float(geo["wall_inflation_m"]),
        nominal_speed_mps=float(rob["nominal_speed_mps"]),
        body_width_m=float(rob["body_width_m"]),
        body_length_m=float(rob["body_length_m"]),
        min_separation_m=float(rob["min_separation_m"]),
        lateral_gap_m=float(cap["lateral_gap_m"]),
        max_capacity_robots=int(cap["max_capacity_robots"]),
        source_path=os.path.abspath(path),
    )
    _check(cfg)
    return cfg


def _check(cfg: ScaleConfig) -> None:
    positives = {
        "corridor_segment_length_m": cfg.corridor_segment_length_m,
        "endpoint_stub_length_m": cfg.endpoint_stub_length_m,
        "aisle_width_wide_m": cfg.aisle_width_wide_m,
        "aisle_width_narrow_m": cfg.aisle_width_narrow_m,
        "stub_width_m": cfg.stub_width_m,
        "nominal_speed_mps": cfg.nominal_speed_mps,
        "body_width_m": cfg.body_width_m,
        "body_length_m": cfg.body_length_m,
        "min_separation_m": cfg.min_separation_m,
    }
    for name, value in sorted(positives.items()):
        if not value > 0.0:
            raise ValueError(f"scale.yaml: {name}는 0보다 커야 한다 (지금 {value})")
    if cfg.wall_inflation_m < 0.0:
        raise ValueError("scale.yaml: wall_inflation_m는 0 이상이어야 한다")
    if cfg.aisle_width_narrow_m > cfg.aisle_width_wide_m:
        raise ValueError("scale.yaml: 교행 불가 통로 폭이 교행 가능 통로 폭보다 넓다")
