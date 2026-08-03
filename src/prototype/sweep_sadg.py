#!/usr/bin/env python3
"""S3 스윕 — 시드 × 대안 생성기에서 **A4(전 조합 비순환)가 실제로 성립하는가**를 센다.

    python3 sweep_sadg.py --seeds 12 --robots 6 --tasks 8

🔴 **이 숫자는 증거가 아니다.** 시제 생성기의 인스턴스 분포이고, 사전등록 대상도 논문 표도 아니다.
여기서 나오는 «lift가 swap보다 좁다» 같은 관찰은 **어블레이션 결과가 아니라 다음에 무엇을 물어야
하는지의 힌트**다.
"""

from __future__ import annotations

import argparse

from assignment_core import AssignParams, build_assignment
from plan_core import PlanParams, plan_instance
from protoscale import load_scale
from roadmap_core import GenParams, generate_roadmap
from sadg_core import (
    check_all_switch_combinations,
    compile_graph,
    is_acyclic,
    nominal_edges,
    plan_view_of,
    prune_cyclic_alternatives,
)


def solo_cyclic_alternatives(graph) -> tuple[int, int]:
    """**한 그룹만** 원 순서에서 벗어났을 때 순환하는 대안 수 / 전체 비원본 대안 수.

    이것이 (i) «혼자서도 안 되는 대안»과 (ii) «혼자서는 되지만 조합에서 안 되는 대안»을 가른다.
    """
    nodes = [s.key for s in graph.segments]
    base = list(graph.fixed)
    originals = [next(a for a in g.alternatives if a.is_original) for g in graph.groups]
    bad = total = 0
    for gi, group in enumerate(graph.groups):
        for alt in group.alternatives:
            if alt.is_original:
                continue
            total += 1
            edges = list(base)
            for gj, orig in enumerate(originals):
                edges.extend(alt.edges if gj == gi else orig.edges)
            if not is_acyclic(nodes, edges):
                bad += 1
    return bad, total


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="S3 스윕 (탐색용 시제 — 증거가 아니다)")
    ap.add_argument("--seeds", type=int, default=12)
    ap.add_argument("--robots", type=int, default=6)
    ap.add_argument("--tasks", type=int, default=8)
    args = ap.parse_args(argv)

    cfg = load_scale()
    print(f"# 인스턴스: 로봇 {args.robots} · 태스크 {args.tasks} · 시드 0~{args.seeds - 1}")
    print("# 🔴 탐색용 시제의 분포이며 증거가 아니다\n")
    header = ("생성기 | 계획된 시드 | 공유위치 | 그룹 | 접힘 | 표현불가 | "
              "A4 성립 | 단독순환대안 | 가지치기 후 그룹 | lambda*")
    print(header)
    print("-" * len(header))

    for generator in ("lift", "swap"):
        rows = []
        for seed in range(args.seeds):
            model = generate_roadmap(cfg, GenParams(seed=seed))
            assignment = build_assignment(
                model, AssignParams(robots=args.robots, tasks=args.tasks, seed=seed)
            )
            result = plan_instance(model, assignment, PlanParams())
            if result.unplanned:
                continue  # 계획을 못 준 인스턴스는 실행 계층이 다루는 종류가 아니다
            graph = compile_graph(plan_view_of(result), alt_generator=generator)
            report = check_all_switch_combinations(graph)
            bad, total = solo_cyclic_alternatives(graph)
            before = graph.stats.groups_emitted
            prune_cyclic_alternatives(graph)
            rows.append(
                (graph.stats.shared_locations, before, graph.stats.groups_folded_single,
                 graph.stats.dropped_unrepresentable, report.ok, bad, total,
                 graph.stats.groups_emitted, graph.stats.critical_path_segments)
            )
        n = len(rows)
        if not n:
            print(f"{generator:6s} | 계획된 시드 0개")
            continue
        print(
            f"{generator:6s} | {n:11d} | {sum(r[0] for r in rows) / n:8.1f} | "
            f"{sum(r[1] for r in rows) / n:4.1f} | {sum(r[2] for r in rows) / n:4.1f} | "
            f"{sum(r[3] for r in rows) / n:8.1f} | {sum(r[4] for r in rows):2d}/{n:<4d} | "
            f"{sum(r[5] for r in rows):3d}/{sum(r[6] for r in rows):<8d} | "
            f"{sum(r[7] for r in rows) / n:16.2f} | {sum(r[8] for r in rows) / n:6.1f}"
        )
    print("\n🔴 「A4 성립」은 **가지치기 전** 원본 대안 집합이 전 조합 비순환인 시드 수다.")
    print("🔴 「가지치기 후 그룹」이 0에 가까우면 그 인스턴스 계열에서 **재순서화 여지가 없다**는 뜻이다.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
