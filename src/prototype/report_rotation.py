#!/usr/bin/env python3
"""쓸어보기 원자료에서 보고서가 인용하는 표를 만든다 — **집계만 한다. 판정하지 않는다.**

    python3 report_rotation.py --cells <디렉터리>/rotation_cells.csv --out <디렉터리>/rotation_tables.txt

🔴 탐색용 시제의 수치이며 논문 증거가 아니다.
"""

from __future__ import annotations

import argparse
import csv
import math
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
BETAS = ["1", "1.5", "2", "4", "8", "inf"]
FLOORS = [5, 15, 30, 60]
# 통로 이름 -> (진입 간격 s, 통과 시간 s). 공칭 속도 1.00 m/s에서 유도된 값이며
# rotation_cells.csv의 headway_s·transit_s와 같아야 한다(아래에서 대조한다).
GEO = {
    "L10_n2_seg5": (5.0, 10.0),
    "L15_n3_seg5": (5.0, 15.0),
    "L30_n2_seg15": (15.0, 30.0),
    "L60_n2_seg30": (30.0, 60.0),
    "L30_n6_seg5": (5.0, 30.0),
}


def load_cells(path: str) -> dict:
    """셀 CSV를 (통로·대수·치우침·부하) -> {정책: 행}으로 읽는다."""
    out: dict = {}
    with open(path, encoding="utf-8") as handle:
        for row in csv.DictReader(handle):
            for key, val in list(row.items()):
                if key not in ("corridor", "policy"):
                    try:
                        row[key] = float(val)
                    except ValueError:
                        row[key] = float("nan")
            cell = (row["corridor"], row["fleet"], row["skew"], row["load_target"])
            out.setdefault(cell, {})[row["policy"]] = row
    return out


def green_entries(max_hold_s: float, headway_s: float) -> int:
    """초록 창 하나에 진입할 수 있는 대수 = k*진입간격 < 최대유지 를 만족하는 k의 개수."""
    return math.floor((max_hold_s - 1e-12) / headway_s) + 1


def saturation_rate_per_h(n: int, headway_s: float, transit_s: float) -> float:
    """양쪽 다 포화일 때의 근사 처리량 [대/시]. 한 방향 주기 = (n-1)*진입간격 + 통과시간."""
    return n / ((n - 1) * headway_s + transit_s) * 3600.0


def table_ratio_by_load(cells: dict, emit) -> None:
    emit("## T1. β=1 대비 β=∞ 처리량 비 — 부하·통로별 (모든 치우침·대수·바닥에 대한 분포)")
    emit("통로            부하 |  n |   최소   중앙   최대")
    buckets: dict = {}
    for (corr, _fleet, _skew, load), pols in cells.items():
        for floor in FLOORS:
            a = pols[f"T{floor}_b1"]["throughput_per_h_mean"]
            b = pols[f"T{floor}_binf"]["throughput_per_h_mean"]
            buckets.setdefault((corr, load), []).append(b / a)
    for (corr, load), vals in sorted(buckets.items()):
        vals.sort()
        mid = vals[len(vals) // 2]
        emit(f"{corr:14s} {load:4.1f} | {len(vals):2d} | {vals[0]:6.2f} {mid:6.2f} {vals[-1]:6.2f}")
    emit("")


def table_cell_grid(cells: dict, picks, emit) -> None:
    emit("## T2. 대표 셀의 β x 바닥 격자 — 처리량[대/시] / 최대대기[s] / 전환[회] / 배수손실[비율]")
    for pick in picks:
        pols = cells[pick]
        ref = pols["unlocked_ref"]
        rea = pols["reactive_T0"]
        emit(
            f"### {pick[0]} · 대수 {pick[1]:.0f} · 치우침 {pick[2]} · 부하 {pick[3]}"
            f"  | 참조선(방향제약 없음) {ref['throughput_per_h_mean']:.1f}/h"
            f" | 반응형(바닥0·상한무한) {rea['throughput_per_h_mean']:.1f}/h,"
            f" 최대대기 {rea['wait_max_s_mean']:.0f} s, 전환 {rea['switches_mean']:.1f}회"
        )
        emit("바닥T |" + "".join(f"        β={b:<4s}      " for b in BETAS))
        for floor in FLOORS:
            line = f"{floor:5d} |"
            for beta in BETAS:
                r = pols[f"T{floor}_b{beta}"]
                line += (
                    f" {r['throughput_per_h_mean']:6.1f}/{r['wait_max_s_mean']:6.0f}"
                    f"/{r['switches_mean']:5.1f}/{r['drain_frac_mean']:.2f}"
                )
            emit(line)
        emit("")


def table_saturation_formula(cells: dict, emit) -> None:
    emit("## T3. 포화 근사식 대조 — 처리량 = n / ((n-1)*진입간격 + 통과시간),  n = floor(β*T/진입간격)+1")
    emit("(대수 80 · 치우침 0.5 · 부하 1.2 = 양쪽 다 포화)")
    emit("통로            T  β    n   예측/시   실측/시    실측/예측")
    worst = 0.0
    lines = []
    for (corr, fleet, skew, load), pols in sorted(cells.items()):
        if (fleet, skew, load) != (80.0, 0.5, 1.2):
            continue
        headway, transit = GEO[corr]
        for floor in FLOORS:
            for beta in BETAS:
                if beta == "inf":
                    continue
                r = pols[f"T{floor}_b{beta}"]
                assert abs(r["headway_s"] - headway) < 1e-9, "GEO가 CSV와 어긋난다"
                assert abs(r["transit_s"] - transit) < 1e-9, "GEO가 CSV와 어긋난다"
                n = green_entries(float(beta) * floor, headway)
                pred = saturation_rate_per_h(n, headway, transit)
                meas = r["throughput_per_h_mean"]
                worst = max(worst, abs(meas / pred - 1.0))
                if floor in (5, 60):
                    lines.append(
                        f"{corr:14s} {floor:2d} {beta:>4s} {n:4d} {pred:9.1f} {meas:9.1f}"
                        f"    {meas / pred:6.3f}"
                    )
    for line in lines:
        emit(line)
    emit(f"# 유한 β 전 조합(통로 5 x 바닥 4 x β 5 = 100)에서 |실측/예측 - 1| 최대 = {worst:.3f}")
    emit("")


def table_shapes(cells: dict, emit) -> None:
    emit("## T4. 모양 세기 — (셀 x 바닥) 480 조합 기준")
    for tol in (0.02, 0.10):
        nonmono = 0
        interior = 0
        for pols in cells.values():
            for floor in FLOORS:
                thr = [pols[f"T{floor}_b{b}"]["throughput_per_h_mean"] for b in BETAS]
                if any(thr[i] > thr[i + 1] * (1 + tol) for i in range(len(thr) - 1)):
                    nonmono += 1
                wmax = [pols[f"T{floor}_b{b}"]["wait_max_s_mean"] for b in BETAS]
                low = min(range(len(wmax)), key=lambda i: wmax[i])
                if (
                    0 < low < len(wmax) - 1
                    and wmax[low] < wmax[0] * (1 - tol)
                    and wmax[low] < wmax[-1] * (1 - tol)
                ):
                    interior += 1
        emit(
            f"허용오차 {tol:.0%} | 처리량이 β에 대해 감소하는 조합 {nonmono}/480"
            f" | 최대대기가 안쪽 β에서 최소인 조합 {interior}/480"
        )
    worse = [
        (cell, floor)
        for cell, pols in cells.items()
        for floor in FLOORS
        if pols[f"T{floor}_binf"]["wait_max_s_mean"] > pols[f"T{floor}_b1"]["wait_max_s_mean"]
    ]
    emit(f"β=∞의 최대대기가 β=1보다 큰 조합 {len(worse)}/480")
    gt = [
        (cell, floor)
        for cell, pols in cells.items()
        for floor in FLOORS
        if pols[f"T{floor}_b1"]["throughput_per_h_mean"]
        > pols[f"T{floor}_binf"]["throughput_per_h_mean"] + 1e-9
    ]
    emit(f"β=1의 처리량이 β=∞보다 큰 조합 {len(gt)}/480 (전부 부하 0.4의 수요 제한 셀)")
    emit("")


def table_censoring(cells: dict, emit) -> None:
    emit("## T5. 검열 — 관측창 끝까지 진입 못 한 요청 (최대대기를 아래로 편향시킨다)")
    by_load: dict = {}
    for (_corr, _fleet, _skew, load), pols in cells.items():
        for row in pols.values():
            acc = by_load.setdefault(load, [0, 0])
            acc[1] += 1
            if row["censored_max_s_max"] > row["wait_max_s_max"]:
                acc[0] += 1
    for load, (bad, total) in sorted(by_load.items()):
        emit(f"부하 {load:4.1f} | 검열 최대치가 관측 최대대기를 넘는 (셀 x 정책) {bad}/{total}")
    emit("⇒ 최대대기는 검열 건수·검열 최대치와 함께가 아니면 읽을 수 없다(PRECOMMIT PRE-4-2).")
    emit("")


def table_floor_vs_beta(cells: dict, emit) -> None:
    emit("## T6. 바닥(T)과 편향 상한(β) 중 무엇이 처리량을 더 움직이는가 — 셀별 변동 폭 비교")
    emit("(β 고정하고 T를 5->60으로 옮길 때의 폭  대  T 고정하고 β를 1->무한으로 옮길 때의 폭)")
    emit("통로            부하 | T폭 중앙값[대/시]  β폭 중앙값[대/시]")
    buckets: dict = {}
    for (corr, _fleet, _skew, load), pols in cells.items():
        t_span = [
            max(pols[f"T{f}_b{b}"]["throughput_per_h_mean"] for f in FLOORS)
            - min(pols[f"T{f}_b{b}"]["throughput_per_h_mean"] for f in FLOORS)
            for b in BETAS
        ]
        b_span = [
            max(pols[f"T{f}_b{b}"]["throughput_per_h_mean"] for b in BETAS)
            - min(pols[f"T{f}_b{b}"]["throughput_per_h_mean"] for b in BETAS)
            for f in FLOORS
        ]
        acc = buckets.setdefault((corr, load), ([], []))
        acc[0].extend(t_span)
        acc[1].extend(b_span)
    for (corr, load), (tspan, bspan) in sorted(buckets.items()):
        tspan.sort()
        bspan.sort()
        emit(
            f"{corr:14s} {load:4.1f} | {tspan[len(tspan) // 2]:16.1f}  "
            f"{bspan[len(bspan) // 2]:16.1f}"
        )
    emit("")


def table_fleet(cells: dict, emit) -> None:
    emit("## T7. 부대 규모 20 대 80 — 같은 통로·치우침·부하에서")
    emit("통로            치우침 부하 | 대수 | β=1 처리량/최대대기 | β=∞ 처리량/최대대기 (바닥 15 s)")
    for corr in ("L10_n2_seg5", "L60_n2_seg30"):
        for skew in (0.5, 0.9):
            for load in (0.8, 1.2):
                for fleet in (20.0, 80.0):
                    pols = cells[(corr, fleet, skew, load)]
                    a, b = pols["T15_b1"], pols["T15_binf"]
                    emit(
                        f"{corr:14s} {skew:5.2f} {load:4.1f} | {fleet:4.0f} |"
                        f" {a['throughput_per_h_mean']:8.1f}/{a['wait_max_s_mean']:7.0f} |"
                        f" {b['throughput_per_h_mean']:8.1f}/{b['wait_max_s_mean']:7.0f}"
                    )
    emit("")


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="쓸어보기 집계표 (탐색용 시제 — 증거가 아니다)")
    ap.add_argument("--cells", default=os.path.join(HERE, "out", "rotation", "rotation_cells.csv"))
    ap.add_argument("--out", default="")
    args = ap.parse_args(argv)

    cells = load_cells(args.cells)
    lines: list[str] = []

    def emit(text: str = "") -> None:
        lines.append(text)

    emit("# 통로 방향 전환 쓸어보기 집계 — 🔴 탐색용 시제이며 증거가 아니다")
    emit(f"# 원자료: {os.path.abspath(args.cells)} · 셀 {len(cells)}개")
    emit("")
    table_ratio_by_load(cells, emit)
    table_cell_grid(
        cells,
        [
            ("L10_n2_seg5", 80.0, 0.7, 0.8),
            ("L10_n2_seg5", 80.0, 0.9, 1.2),
            ("L60_n2_seg30", 80.0, 0.7, 0.8),
            ("L30_n6_seg5", 80.0, 0.9, 0.8),
        ],
        emit,
    )
    table_saturation_formula(cells, emit)
    table_shapes(cells, emit)
    table_censoring(cells, emit)
    table_floor_vs_beta(cells, emit)
    table_fleet(cells, emit)

    text = "\n".join(lines)
    print(text)
    if args.out:
        with open(args.out, "w", encoding="utf-8") as handle:
            handle.write(text + "\n")
        print(f"# wrote {args.out}", file=sys.stderr)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
