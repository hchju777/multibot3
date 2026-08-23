#!/usr/bin/env python3
"""교행 불가 통로 하나의 방향 전환 정책 쓸어보기(sweep) — 원자료 생성기.

    python3 sweep_rotation.py --measure-corridors      # 오늘 로드맵의 통로 기하를 읽어만 온다
    python3 sweep_rotation.py --grid quick --jobs 8    # 작은 격자
    python3 sweep_rotation.py --grid full  --jobs 16 --out-dir <디렉터리>
    python3 sweep_rotation.py --grid quick --determinism-check

🔴 **탐색용 시제다.** 여기서 나오는 어떤 수치도 논문 증거가 아니고 사전등록 상수가 아니다.
🔴 이 스크립트는 **판정하지 않는다.** CSV와 JSON 원자료만 낸다. 「낫다/충분하다」를 쓰지 않는다.

쓸어보는 축과 **왜 그 범위인가**는 GRID_RATIONALE에 적혀 있고, 그대로 meta.json에 실린다.
"""

from __future__ import annotations

import argparse
import csv
import json
import math
import os
import sys
import time
from concurrent.futures import ProcessPoolExecutor
from dataclasses import asdict

from corridor_rotation_core import (
    MEASURE_VERSION,
    CorridorSpec,
    DemandSpec,
    PolicySpec,
    RunSpec,
    measure_corridors,
    simulate,
    think_mean_for_load,
)
from protoscale import DEFAULT_SCALE_PATH, load_scale

HERE = os.path.dirname(os.path.abspath(__file__))
INF = float("inf")

# --------------------------------------------------------------- 격자와 그 사유

# (이름, 구간 수, 구간 길이 m) — 통로 전체 길이 = 구간 수 x 구간 길이
CORRIDORS_FULL = [
    ("L10_n2_seg5", 2, 5.0),
    ("L15_n3_seg5", 3, 5.0),
    ("L30_n2_seg15", 2, 15.0),
    ("L60_n2_seg30", 2, 30.0),
    ("L30_n6_seg5", 6, 5.0),
]
SKEWS_FULL = [0.5, 0.7, 0.9, 0.98]
LOADS_FULL = [0.4, 0.8, 1.2]
FLEETS_FULL = [20, 80]
FLOORS_FULL = [5.0, 15.0, 30.0, 60.0]
BETAS_FULL = [1.0, 1.5, 2.0, 4.0, 8.0, INF]
SEEDS_FULL = list(range(12))

CORRIDORS_QUICK = [("L10_n2_seg5", 2, 5.0), ("L60_n2_seg30", 2, 30.0)]
SKEWS_QUICK = [0.5, 0.9]
LOADS_QUICK = [0.8]
FLEETS_QUICK = [20]
FLOORS_QUICK = [5.0, 30.0]
BETAS_QUICK = [1.0, 4.0, INF]
SEEDS_QUICK = list(range(3))

GRID_RATIONALE = {
    "corridor_geometry": (
        "L10_n2_seg5·L15_n3_seg5는 오늘 생성기 실측값이다(--measure-corridors로 재현). "
        "L30_n2_seg15·L60_n2_seg30은 사용자 요구 「노드는 교차로에만, 간선 수십 m」를 담는다. "
        "L30_n6_seg5는 대조군이다 — 길이는 L30_n2_seg15와 같은데 노드가 촘촘하다. "
        "길이(=배수 시간)와 구간 길이(=진입 간격·동시 수용 대수) 중 무엇이 지배하는지를 가른다."
    ),
    "skew": (
        "0.5(균등)·0.7·0.9·0.98. 0.98은 「거의 한 방향인데 반대 수요가 드물게 있다」는 자리이고 "
        "균등 주기의 낭비와 기아 위험이 동시에 가장 커지는 곳이라 넣었다. "
        "1.0(완전 한쪽)은 반대쪽 수요가 0이라 그쪽 최대 대기가 정의되지 않아 뺐다."
    ),
    "load": (
        "0.4(저부하)·0.8(방향 잠금 용량 근처)·1.2(과부하). "
        "부하 1.0의 정의는 «방향 제약이 없을 때의 관 처리율»이며 방향을 잠그면 그보다 낮으므로, "
        "0.8이 이미 포화 근처이고 1.2가 확실한 과부하다."
    ),
    "fleet": "20·80. 목표 규모 10~80대의 양 끝 근처. 유한 대수이므로 줄이 무한히 자라지 않는다.",
    "floor_s": (
        "5·15·30·60 s. 5와 15는 이 레포에 이미 있는 시간 상수다"
        "(사전등록 27의 Δ_min=5.000 s와 되돌림값 15.000 s). "
        "30·60은 통로 배수 시간(10~60 s)에 대해 바닥/배수 비를 0.08~12배로 벌리려고 내가 골랐다."
    ),
    "beta": "1(균등 주기)·1.5·2·4·8·무한(반응형). 한 축 위의 여섯 점.",
    "extra_policies": (
        "reactive_T0(바닥 0 + 상한 무한 = 순수 반응형)와 "
        "unlocked_ref(방향 제약을 아예 끈 도달 불가 참조선) 둘을 더 넣었다."
    ),
    "seeds": "시드 0~11. 정책이 달라도 같은 (규모·치우침·부하·시드)면 로봇별 난수열이 같다(공통 난수).",
    "horizon": "3600 s 중 앞 600 s는 예열로 버린다(PRECOMMIT.md PRE-4).",
    "chosen_by": "이 값들은 module-developer가 골랐다. 사전등록된 격자가 아니다.",
}


def policy_list(floors, betas) -> list[PolicySpec]:
    """정책 목록을 만든다. 바닥 0 + 유한 편향 상한은 정의되지 않아 만들지 않는다."""
    out = [
        PolicySpec(label="reactive_T0", floor_s=0.0, beta=INF),
        PolicySpec(label="unlocked_ref", floor_s=0.0, beta=INF, unlocked=True),
    ]
    for floor in floors:
        for beta in betas:
            tag = "inf" if math.isinf(beta) else f"{beta:g}"
            out.append(PolicySpec(label=f"T{floor:g}_b{tag}", floor_s=floor, beta=beta))
    return out


def build_specs(grid: str, horizon: float, warmup: float, speed: float) -> list[RunSpec]:
    """격자를 펼쳐 RunSpec 목록을 만든다. 순서는 결정론적이다."""
    if grid == "quick":
        corridors, skews, loads = CORRIDORS_QUICK, SKEWS_QUICK, LOADS_QUICK
        fleets, floors, betas, seeds = FLEETS_QUICK, FLOORS_QUICK, BETAS_QUICK, SEEDS_QUICK
    else:
        corridors, skews, loads = CORRIDORS_FULL, SKEWS_FULL, LOADS_FULL
        fleets, floors, betas, seeds = FLEETS_FULL, FLOORS_FULL, BETAS_FULL, SEEDS_FULL

    specs: list[RunSpec] = []
    policies = policy_list(floors, betas)
    for name, segments, seg_len in corridors:
        corr = CorridorSpec(
            label=name, segments=segments, segment_length_m=seg_len, speed_mps=speed
        )
        for fleet in fleets:
            for load in loads:
                think = think_mean_for_load(corr, fleet, load)
                for skew in skews:
                    dem = DemandSpec(
                        fleet=fleet, skew=skew, think_mean_s=think, load_target=load
                    )
                    for pol in policies:
                        for seed in seeds:
                            specs.append(
                                RunSpec(
                                    corridor=corr,
                                    demand=dem,
                                    policy=pol,
                                    seed=seed,
                                    horizon_s=horizon,
                                    warmup_s=warmup,
                                )
                            )
    return specs


def _row(spec: RunSpec) -> dict:
    """한 실행을 돌리고 인자 + 지표를 한 줄로 만든다."""
    res = simulate(spec)
    row = {
        "corridor": spec.corridor.label,
        "segments": spec.corridor.segments,
        "segment_length_m": spec.corridor.segment_length_m,
        "corridor_length_m": spec.corridor.length_m,
        "headway_s": spec.corridor.headway_s,
        "transit_s": spec.corridor.transit_s,
        "speed_mps": spec.corridor.speed_mps,
        "fleet": spec.demand.fleet,
        "skew": spec.demand.skew,
        "load_target": spec.demand.load_target,
        "think_mean_s": spec.demand.think_mean_s,
        "policy": spec.policy.label,
        "floor_s": spec.policy.floor_s,
        "beta": "inf" if math.isinf(spec.policy.beta) else spec.policy.beta,
        "max_hold_s": "inf" if math.isinf(spec.policy.max_hold_s) else spec.policy.max_hold_s,
        "unlocked": int(spec.policy.unlocked),
        "seed": spec.seed,
        "horizon_s": spec.horizon_s,
        "warmup_s": spec.warmup_s,
    }
    row.update(asdict(res))
    # 파생값 둘 — 방향 제약이 없을 때의 관 처리율 대비 얼마를 썼는가.
    pipe = 3600.0 / spec.corridor.headway_s
    row["pipe_rate_per_h"] = pipe
    row["slot_utilization"] = res.throughput_per_h / pipe
    return row


def _chunk(specs: list[RunSpec]) -> list[dict]:
    return [_row(s) for s in specs]


def run_all(specs: list[RunSpec], jobs: int) -> list[dict]:
    """전부 돌린다. 결과 순서는 specs 순서와 같다(작업 스케줄과 무관)."""
    if jobs <= 1:
        return [_row(s) for s in specs]
    size = max(1, len(specs) // (jobs * 4) + 1)
    chunks = [specs[i : i + size] for i in range(0, len(specs), size)]
    rows: list[dict] = []
    with ProcessPoolExecutor(max_workers=jobs) as pool:
        for part in pool.map(_chunk, chunks):
            rows.extend(part)
    return rows


CELL_KEYS = ("corridor", "fleet", "skew", "load_target", "policy")
MEAN_FIELDS = (
    "throughput_per_h",
    "throughput_side0_per_h",
    "throughput_side1_per_h",
    "wait_mean_s",
    "wait_p95_s",
    "wait_max_s",
    "wait_max_side0_s",
    "wait_max_side1_s",
    "switches",
    "switch_period_mean_s",
    "drain_frac",
    "floor_block_frac",
    "busy_frac",
    "idle_frac",
    "offered_per_h",
    "queue_len_max",
    "slot_utilization",
)


def aggregate(rows: list[dict]) -> list[dict]:
    """시드 평균 + 최대 대기의 시드 최댓값 + 검열 합계. 원자료를 대체하지 않는다."""
    cells: dict[tuple, list[dict]] = {}
    for row in rows:
        cells.setdefault(tuple(row[k] for k in CELL_KEYS), []).append(row)
    out = []
    for key, group in sorted(cells.items(), key=lambda kv: [str(x) for x in kv[0]]):
        agg = dict(zip(CELL_KEYS, key))
        agg["seeds"] = len(group)
        for field in MEAN_FIELDS:
            vals = [r[field] for r in group if not _isnan(r[field])]
            agg[f"{field}_mean"] = (sum(vals) / len(vals)) if vals else float("nan")
        wmax = [r["wait_max_s"] for r in group if not _isnan(r["wait_max_s"])]
        agg["wait_max_s_max"] = max(wmax) if wmax else float("nan")
        agg["censored_count_sum"] = sum(r["censored_count"] for r in group)
        agg["censored_max_s_max"] = max(r["censored_max_s"] for r in group)
        for field in ("headway_s", "transit_s", "corridor_length_m", "floor_s", "max_hold_s"):
            agg[field] = group[0][field]
        out.append(agg)
    return out


def _isnan(value) -> bool:
    return isinstance(value, float) and math.isnan(value)


def _rows_equal(a: dict, b: dict) -> bool:
    """두 줄이 같은가. 🔴 nan != nan 이므로 그냥 == 로 비교하면 결정론 검사가 항상 거짓을 낸다.

    (표본 부족으로 nan이 나오는 열이 실제로 있다 — 전환 0회인 참조선의 전환 주기 등.)
    """
    if a.keys() != b.keys():
        return False
    for key in a:
        va, vb = a[key], b[key]
        if _isnan(va) and _isnan(vb):
            continue
        if va != vb:
            return False
    return True


def write_csv(path: str, rows: list[dict]) -> None:
    if not rows:
        return
    fields = list(rows[0].keys())
    with open(path, "w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=fields)
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def cmd_measure_corridors(argv_seeds: int) -> int:
    """오늘 생성기의 통로 기하를 읽어만 온다 — 격자에 쓴 수의 출처 확인."""
    from roadmap_core import GenParams

    cfg = load_scale()
    found = measure_corridors(cfg, lambda s: GenParams(seed=s), range(argv_seeds))
    print(f"# scale.yaml = {cfg.source_path}")
    print(f"# 공칭 속도 {cfg.nominal_speed_mps} m/s · 구간 길이 {cfg.corridor_segment_length_m} m")
    print(f"# 좁은 통로 폭 {cfg.aisle_width_narrow_m} m -> 용량 {cfg.capacity_rule_a(cfg.aisle_width_narrow_m)}")
    print("seed,corridor,edges,length_m,capacities")
    for m in found:
        print(f"{m.seed},{m.key},{m.edges},{m.length_m},{'|'.join(str(c) for c in m.capacities)}")
    edges = sorted(m.edges for m in found)
    lens = sorted(m.length_m for m in found)
    mid = len(edges) // 2
    median_e = edges[mid] if len(edges) % 2 else (edges[mid - 1] + edges[mid]) / 2
    median_l = lens[mid] if len(lens) % 2 else (lens[mid - 1] + lens[mid]) / 2
    print(f"# 통로 {len(found)}개 · 간선수 집합 {sorted(set(edges))} · 중앙값 {median_e}")
    print(f"# 길이 집합 {sorted(set(lens))} · 중앙값 {median_l}")
    print(f"# 구성 간선 용량 집합 {sorted({c for m in found for c in m.capacities})}")
    return 0


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="통로 방향 전환 쓸어보기 (탐색용 시제 — 증거가 아니다)")
    ap.add_argument("--grid", choices=("quick", "full"), default="full")
    ap.add_argument("--jobs", type=int, default=1)
    ap.add_argument("--horizon", type=float, default=3600.0)
    ap.add_argument("--warmup", type=float, default=600.0)
    ap.add_argument("--scale", default=DEFAULT_SCALE_PATH)
    ap.add_argument("--out-dir", default=os.path.join(HERE, "out", "rotation"))
    ap.add_argument("--measure-corridors", action="store_true", help="통로 기하만 읽고 끝낸다")
    ap.add_argument("--roadmap-seeds", type=int, default=12)
    ap.add_argument("--determinism-check", action="store_true", help="같은 격자를 두 번 돌려 대조한다")
    args = ap.parse_args(argv)

    if args.measure_corridors:
        return cmd_measure_corridors(args.roadmap_seeds)

    cfg = load_scale(args.scale)
    specs = build_specs(args.grid, args.horizon, args.warmup, cfg.nominal_speed_mps)
    print(f"# 실행 {len(specs)}건 · 격자 {args.grid} · 병렬 {args.jobs}", file=sys.stderr)

    t0 = time.time()
    rows = run_all(specs, args.jobs)
    elapsed = time.time() - t0
    print(f"# {elapsed:.1f} s", file=sys.stderr)

    if args.determinism_check:
        again = run_all(specs, args.jobs)
        same = len(rows) == len(again) and all(
            _rows_equal(a, b) for a, b in zip(rows, again)
        )
        print(f"# determinism_identical={same} rows={len(rows)}")
        if not same:
            for a, b in zip(rows, again):
                if not _rows_equal(a, b):
                    print(f"# first_diff a={a}\n#           b={b}", file=sys.stderr)
                    break
            return 1

    os.makedirs(args.out_dir, exist_ok=True)
    runs_path = os.path.join(args.out_dir, "rotation_runs.csv")
    cells_path = os.path.join(args.out_dir, "rotation_cells.csv")
    meta_path = os.path.join(args.out_dir, "rotation_meta.json")
    write_csv(runs_path, rows)
    write_csv(cells_path, aggregate(rows))
    meta = {
        "measure_version": MEASURE_VERSION,
        "grid": args.grid,
        "runs": len(rows),
        "elapsed_s": round(elapsed, 2),
        "horizon_s": args.horizon,
        "warmup_s": args.warmup,
        "scale_path": cfg.source_path,
        "speed_mps": cfg.nominal_speed_mps,
        "grid_rationale": GRID_RATIONALE,
        "not_evidence": "탐색용 시제의 수치이며 논문 증거가 아니다. 사전등록 상수도 아니다.",
    }
    with open(meta_path, "w", encoding="utf-8") as handle:
        json.dump(meta, handle, ensure_ascii=False, indent=2)
    print(f"# wrote {runs_path}\n# wrote {cells_path}\n# wrote {meta_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
