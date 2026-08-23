#!/usr/bin/env python3
"""추종 체제의 통로 처리량 스파이크 (265d C부) — 배타(h=D) 대 추종(h=δ_follow).

    python3 follow_throughput_spike.py --out-dir <디렉터리>
    python3 follow_throughput_spike.py --determinism-check

🔴 **탐색용 시제다.** 수치는 논문 증거가 아니고 어떤 값도 동결이 아니다.
🔴 **판정하지 않는다** — 원자료(CSV/JSON)와 수만 낸다.

무엇을 재나: 무노드 통로(30 m·60 m)에서 점유 체제 셋의 처리량·대기 —
  excl   : 배타 점유 A-occ — 통로에 한 번에 한 대, h = D (`265a` §265a-1-2)
  follow : 같은 방향 연속 진입 허용, h = δ_follow (스파이크 입력)
  ref    : 구간 5 m 사슬(261의 다노드 대조군과 같은 구성) — h = 5 s
그리고 n = ⌈βT/h⌉(P-tie — `265a` §265a-1-1 통일 문언) 예측과
포화 근사식 처리량 = n/((n−1)h + D)를 실측·실현 창 진입 수와 대조한다.

재사용: simulate/CorridorSpec/…(corridor_rotation_core — **수정 0줄**),
green_entries·saturation_rate_per_h(report_rotation — ε-내림 구현이 ⌈βT/h⌉와 동치·
측정된 P-tie 관례와 같은 쪽), write_csv·_rows_equal(sweep_rotation).

추종 체제의 구현: CorridorSpec의 진입 간격(headway_s)만 δ_follow로 바꾼 파생
클래스 — simulate()는 진입 간격·통과 시간 둘만 읽으므로 코어 수정이 필요 없다.
배수(drain)는 마지막 진입 + D 그대로다(배치 전체가 빠져야 전환 — 배수-후-전환 유지).
동시 재실 대수는 최대 ⌈D/δ_follow⌉가 되며(추종), 그 물리 성립 여부(차간 거리
δ_follow×v)는 이 스파이크가 판정하지 않는다.

수요를 체제 간 동일하게: 「일하는 시간」을 **ref 통로 기준 부하**로 유도해 세 체제에
같은 값을 쓴다 — 로봇별 난수 스트림이 (대수·치우침·씨앗)만의 함수라(공통 난수)
세 체제가 같은 도착 흐름을 받는다. 🔴 261의 부하 정의(각 통로 자신의 관 처리율
기준)와 다르다 — 체제 간 대조가 목적이라 기준을 하나로 고정했다. 기록만 한다.
"""

from __future__ import annotations

import argparse
import json
import math
import os
import sys
import time
from dataclasses import dataclass

from corridor_rotation_core import (
    CorridorSpec,
    DemandSpec,
    PolicySpec,
    RunSpec,
    simulate,
    think_mean_for_load,
)
from protoscale import load_scale
from report_rotation import green_entries, saturation_rate_per_h
from sweep_rotation import _rows_equal, write_csv

SPIKE_VERSION = "prototype-follow_throughput_spike-0.1.0"
HERE = os.path.dirname(os.path.abspath(__file__))


@dataclass(frozen=True)
class FollowCorridorSpec(CorridorSpec):
    """추종 체제 통로 — 진입 간격만 δ_follow로 바꾼다(통과 시간·기하 그대로).

    segments=1(무노드)로 만들고 follow_headway_s를 준다. capacity_robots 속성은
    구간 수(=1)를 돌려주지만 simulate()는 그 속성을 읽지 않는다 — 추종의 동시
    재실은 진입 간격이 결정한다(최대 ⌈D/δ⌉).
    """

    follow_headway_s: float = 0.0

    @property
    def headway_s(self) -> float:  # type: ignore[override]
        return self.follow_headway_s


def build_variants(length_m: float, deltas: tuple[float, ...], speed: float,
                   ref_seg_m: float) -> list[tuple[str, CorridorSpec]]:
    """한 통로 길이에 대한 점유 체제 변형들. (이름, 스펙) 목록."""
    out: list[tuple[str, CorridorSpec]] = [
        ("excl", CorridorSpec(f"L{length_m:g}_excl", 1, length_m, speed)),
    ]
    for d in deltas:
        out.append((f"follow{d:g}",
                    FollowCorridorSpec(f"L{length_m:g}_follow{d:g}", 1, length_m, speed,
                                       follow_headway_s=d)))
    n_seg = int(round(length_m / ref_seg_m))
    assert abs(n_seg * ref_seg_m - length_m) < 1e-9, "ref 구간이 통로 길이를 나누지 않는다"
    out.append(("ref_seg5", CorridorSpec(f"L{length_m:g}_ref_seg{ref_seg_m:g}",
                                         n_seg, ref_seg_m, speed)))
    return out


def window_entry_counts(trace: dict, warm: float) -> list[int]:
    """전환 사이 초록 창별 진입 대수 — 창 시작·끝이 관측창(warm 이후) 안인 창만."""
    switches = trace.get("switches", [])
    entries = [t for (t, _side, _exit) in trace.get("entries", [])]
    counts: list[int] = []
    for a, b in zip(switches, switches[1:]):
        if a < warm:
            continue
        counts.append(sum(1 for t in entries if a <= t < b))
    return counts


def build_rows(grid: dict) -> list[dict]:
    """격자를 펼쳐 전 run을 돌리고 행을 만든다. 순서·내용 결정론적."""
    rows: list[dict] = []
    speed = grid["speed_mps"]
    for length in grid["lengths_m"]:
        variants = build_variants(length, tuple(grid["deltas_s"]), speed, grid["ref_seg_m"])
        ref_spec = variants[-1][1]
        for load in grid["loads_ref"]:
            think = think_mean_for_load(ref_spec, grid["fleet"], load)
            for skew in grid["skews"]:
                dem = DemandSpec(fleet=grid["fleet"], skew=skew,
                                 think_mean_s=think, load_target=load)
                for vname, corr in variants:
                    for floor in grid["floors_s"]:
                        for beta in grid["betas"]:
                            pol = PolicySpec(f"T{floor:g}_b{beta:g}", floor, beta)
                            for seed in grid["seeds"]:
                                spec = RunSpec(corridor=corr, demand=dem, policy=pol,
                                               seed=seed, horizon_s=grid["horizon_s"],
                                               warmup_s=grid["warmup_s"])
                                trace: dict = {}
                                res = simulate(spec, trace=trace)
                                counts = window_entry_counts(trace, grid["warmup_s"])
                                h = corr.headway_s
                                d_tr = corr.transit_s
                                n_pred = green_entries(beta * floor, h)
                                row = {
                                    "length_m": length, "variant": vname,
                                    "headway_s": h, "transit_s": d_tr,
                                    "fleet": grid["fleet"], "skew": skew,
                                    "load_ref": load, "think_mean_s": round(think, 6),
                                    "floor_s": floor, "beta": beta, "seed": seed,
                                    "n_pred": n_pred,
                                    "thr_pred_sat_per_h": round(
                                        saturation_rate_per_h(n_pred, h, d_tr), 3),
                                    "windows": len(counts),
                                    "win_entries_min": min(counts) if counts else 0,
                                    "win_entries_med": (sorted(counts)[len(counts) // 2]
                                                        if counts else 0),
                                    "win_entries_max": max(counts) if counts else 0,
                                }
                                for key, val in res.as_row().items():
                                    row[key] = val
                                rows.append(row)
    return rows


CELL_KEYS = ("length_m", "variant", "skew", "load_ref", "floor_s", "beta")
MEAN_FIELDS = ("throughput_per_h", "wait_mean_s", "wait_p95_s", "wait_max_s",
               "switches", "drain_frac", "floor_block_frac", "offered_per_h",
               "win_entries_med")


def aggregate(rows: list[dict]) -> list[dict]:
    """씨앗 평균 + 최대 대기 씨앗 최댓값 + 검열 합계 — 원자료를 대체하지 않는다."""
    cells: dict[tuple, list[dict]] = {}
    for row in rows:
        cells.setdefault(tuple(row[k] for k in CELL_KEYS), []).append(row)
    out: list[dict] = []
    for key, group in sorted(cells.items(), key=lambda kv: [str(x) for x in kv[0]]):
        agg = dict(zip(CELL_KEYS, key))
        agg["seeds"] = len(group)
        for field in MEAN_FIELDS:
            vals = [r[field] for r in group
                    if not (isinstance(r[field], float) and math.isnan(r[field]))]
            agg[f"{field}_mean"] = round(sum(vals) / len(vals), 4) if vals else float("nan")
        # 🔴 nan 필터 — 과부하 셀은 관측창 안 대기 표본이 0인 씨앗이 있다(전부 예열 전
        # 요청·검열). sweep_rotation.aggregate와 같은 관례: 있는 표본의 최댓값만.
        wmax = [r["wait_max_s"] for r in group
                if not (isinstance(r["wait_max_s"], float) and math.isnan(r["wait_max_s"]))]
        agg["wait_max_s_max"] = max(wmax) if wmax else float("nan")
        agg["censored_count_sum"] = sum(r["censored_count"] for r in group)
        agg["censored_max_s_max"] = max(r["censored_max_s"] for r in group)
        for field in ("headway_s", "transit_s", "n_pred", "thr_pred_sat_per_h"):
            agg[field] = group[0][field]
        agg["thr_meas_over_pred"] = round(
            agg["throughput_per_h_mean"] / agg["thr_pred_sat_per_h"], 4)
        out.append(agg)
    return out


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="추종 체제 처리량 스파이크 (증거 아님·판정 없음)")
    ap.add_argument("--lengths", default="30,60")
    ap.add_argument("--deltas", default="2,4")
    ap.add_argument("--loads", default="0.4,0.8,1.2")
    ap.add_argument("--skews", default="0.5,0.9")
    ap.add_argument("--floors", default="15,60")
    ap.add_argument("--betas", default="1,4")
    ap.add_argument("--fleet", type=int, default=80)
    ap.add_argument("--seeds", type=int, default=5)
    ap.add_argument("--horizon", type=float, default=3600.0)
    ap.add_argument("--warmup", type=float, default=600.0)
    ap.add_argument("--out-dir", default=os.path.join(HERE, "out", "follow_spike"))
    ap.add_argument("--determinism-check", action="store_true")
    args = ap.parse_args(argv)

    cfg = load_scale()
    grid = {
        "lengths_m": [float(x) for x in args.lengths.split(",")],
        "deltas_s": [float(x) for x in args.deltas.split(",")],
        "loads_ref": [float(x) for x in args.loads.split(",")],
        "skews": [float(x) for x in args.skews.split(",")],
        "floors_s": [float(x) for x in args.floors.split(",")],
        "betas": [float(x) for x in args.betas.split(",")],
        "fleet": args.fleet,
        "seeds": list(range(args.seeds)),
        "horizon_s": args.horizon,
        "warmup_s": args.warmup,
        "speed_mps": cfg.nominal_speed_mps,
        "ref_seg_m": 5.0,
    }
    t0 = time.time()
    rows = build_rows(grid)
    elapsed = time.time() - t0
    print(f"# run {len(rows)}건 · {elapsed:.1f} s", file=sys.stderr)

    if args.determinism_check:
        again = build_rows(grid)
        same = len(rows) == len(again) and all(
            _rows_equal(a, b) for a, b in zip(rows, again))
        print(f"# determinism_identical={same} rows={len(rows)}")
        if not same:
            return 1

    os.makedirs(args.out_dir, exist_ok=True)
    runs_path = os.path.join(args.out_dir, "follow_runs.csv")
    cells_path = os.path.join(args.out_dir, "follow_cells.csv")
    meta_path = os.path.join(args.out_dir, "follow_meta.json")
    write_csv(runs_path, rows)
    write_csv(cells_path, aggregate(rows))
    meta = {
        "spike_version": SPIKE_VERSION,
        "not_evidence": "탐색용 시제. 논문 증거·동결 상수 아님.",
        "grid": {k: v for k, v in grid.items()},
        "n_formula": "n = ceil(beta*T/h) — P-tie(상한 시각엔 전환이 진입을 선점), "
                     "265a §265a-1-1 통일 문언. green_entries의 ε-내림 구현과 동치.",
        "load_semantics": "loads_ref는 ref(구간 5 m 사슬) 관 처리율 기준 — 세 체제에 "
                          "같은 「일하는 시간」을 써서 도착 흐름을 동일하게 한다. "
                          "261의 통로별 부하 정의와 다르다.",
        "elapsed_s": round(elapsed, 2),
        "runs": len(rows),
    }
    with open(meta_path, "w", encoding="utf-8") as handle:
        json.dump(meta, handle, ensure_ascii=False, indent=2)
    print(f"# wrote {runs_path}\n# wrote {cells_path}\n# wrote {meta_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
