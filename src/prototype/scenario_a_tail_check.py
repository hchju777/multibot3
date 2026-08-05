#!/usr/bin/env python3
"""§65-8 M2′ 3번 — 시나리오 S-A 4변형에서 꼬리 재컴파일을 상태 몇 개 시점에서 잰다 (탐색용 시제).

`episode_timeline.py`의 `run_episode(..., view=...)`를 그대로 재사용한다(코어 미수정, 새 코드
없음 — 이 파일은 S-A 아티팩트 로딩 + 체크포인트 선정만 한다). `report_scenario_a.py`와 같은
방식으로 로드맵+이산 계획을 읽고(어댑터 함수만 재사용), `compile_graph`+`prune_cyclic_alternatives`로
정본과 같은 그래프를 만든 뒤 릴리스 재생을 돌린다.

체크포인트 셋(고정, 인자화하지 않았다 — 시나리오 자체가 고정 토폴로지라 의미가 있는 지점도
고정이다):
  1. 첫 완료 사건
  2. **A가 통로를 다 지난 직후** — A의 세그먼트 중 도착 정점이 `R`인 것이 완료되는 사건
  3. 마지막 완료 사건(전원 완료)

출력은 `stats/`에 둔다. 🔴 탐색용 시제이며 증거가 아니다.
"""

from __future__ import annotations

import argparse
import json
import os

from boundary_adapter import plan_from_boundary, roadmap_from_boundary
from episode_timeline import run_episode
from protoscale import load_scale
from sadg_core import compile_graph, plan_view_of, prune_cyclic_alternatives

HERE = os.path.dirname(os.path.abspath(__file__))
VARIANTS = [("single", 2), ("single", 1), ("chain", 2), ("chain", 1)]


def load_bundle(out_dir: str):
    with open(os.path.join(out_dir, "roadmap.json"), "r", encoding="utf-8") as handle:
        model = roadmap_from_boundary(json.load(handle))
    with open(os.path.join(out_dir, "discrete_plan.json"), "r", encoding="utf-8") as handle:
        result = plan_from_boundary(json.load(handle))
    return model, result


def pick_checkpoints(graph, rows):
    """(이름, 행) 목록. `rows`는 `run_episode`가 낸 `EpisodeResult.rows`."""
    if not rows:
        return []
    seg_by_key = {s.key: s for s in graph.segments}
    checkpoints = [("첫 완료 사건", rows[0])]

    a_cross_row = None
    for r in rows:
        robot, number = r.completed_segment.split("#")
        if robot != "A":
            continue
        seg = seg_by_key.get((robot, int(number)))
        if seg is not None and seg.dst_node == "R":
            a_cross_row = r
            break
    if a_cross_row is not None:
        checkpoints.append(("A가 통로를 다 지난 직후(dst=R)", a_cross_row))

    checkpoints.append(("마지막 완료 사건(전원 완료)", rows[-1]))
    return checkpoints


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="S-A 4변형 꼬리 재컴파일 체크포인트 (탐색용 시제)")
    ap.add_argument("--out-dir", default=os.path.join(HERE, "out"))
    ap.add_argument("--stats-out", default=os.path.join(HERE, "stats", "scenario_a_tail_check.json"))
    args = ap.parse_args(argv)

    cfg = load_scale()
    report = []
    print("# S-A 4변형 — 꼬리 재컴파일 체크포인트 (탐색용 시제, 증거 아님)\n")
    header = "토폴로지/용량 | 체크포인트 | 완료세그먼트 | 고정잔여방 | 재컴파일방 | t=0값 | 재컴파일>고정 | 재컴파일>t0"
    print(header)
    print("-" * len(header))

    for topology, capacity in VARIANTS:
        out_dir = os.path.join(args.out_dir, f"scenA_{topology}_cap{capacity}")
        model, result = load_bundle(out_dir)
        view = plan_view_of(result)
        graph = compile_graph(view, alt_generator="lift")
        prune_cyclic_alternatives(graph)  # out/ 정본과 같은 처리

        ep = run_episode(graph, model, cfg.nominal_speed_mps, blockage=None,
                          max_ticks=5000, view=view, record_entries=False)
        checkpoints = pick_checkpoints(graph, ep.rows)

        variant_rows = []
        for name, row in checkpoints:
            exceeds_fixed = (
                row.recompiled_acyclic_combinations is not None
                and row.recompiled_acyclic_combinations >= 0
                and row.acyclic_combinations >= 0
                and row.recompiled_acyclic_combinations > row.acyclic_combinations
            )
            exceeds_t0 = (
                row.recompiled_acyclic_combinations is not None
                and row.recompiled_acyclic_combinations >= 0
                and row.recompiled_acyclic_combinations > ep.baseline_acyclic_combinations
            )
            variant_rows.append(
                {
                    "checkpoint": name,
                    "completed_segment": row.completed_segment,
                    "fixed_acyclic_combinations": row.acyclic_combinations,
                    "recompiled_acyclic_combinations": row.recompiled_acyclic_combinations,
                    "baseline_acyclic_combinations": ep.baseline_acyclic_combinations,
                    "recompile_exceeds_fixed": exceeds_fixed,
                    "recompile_exceeds_baseline": exceeds_t0,
                }
            )
            print(
                f"{topology}/cap{capacity} | {name} | {row.completed_segment} | "
                f"{row.acyclic_combinations} | {row.recompiled_acyclic_combinations} | "
                f"{ep.baseline_acyclic_combinations} | {exceeds_fixed} | {exceeds_t0}"
            )
        report.append({
            "instance_tag": model.instance_id,
            "baseline_acyclic_combinations": ep.baseline_acyclic_combinations,
            "problems": ep.problems,
            "checkpoints": variant_rows,
        })

    os.makedirs(os.path.dirname(os.path.abspath(args.stats_out)), exist_ok=True)
    with open(args.stats_out, "w", encoding="utf-8") as handle:
        json.dump(
            {
                "note": "내부 회계다. 경계 아티팩트가 아니며 논문 증거가 아니다. 탐색용 시제.",
                "variants": report,
            },
            handle,
            indent=2,
            ensure_ascii=False,
        )
        handle.write("\n")
    print(f"\nwrote: {os.path.abspath(args.stats_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
