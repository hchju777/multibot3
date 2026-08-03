#!/usr/bin/env python3
"""S3 — 실행 의존 그래프(SADG) 컴파일 CLI (탐색용 시제).

`mrs.roadmap` + `mrs.discrete_plan`을 읽어 `mrs.execution_constraints` 1.1.0을 만든다.

    python3 gen_constraints.py --alt-gen lift --schema-check

🔴 **출력 파일 이름은 반드시 `execution_constraints.json`이다.** 층 1 검증기가
`{스키마이름}.json`으로 스키마를 찾으므로 이름을 꾸미면 «대응 스키마 없음»으로
**종료 코드 2(판정 불가)**가 된다. 변형은 이름이 아니라 **디렉터리**로 가른다.

🔴 **이 CLI가 자기 산출물에 돌리는 검사는 판정이 아니다.** 통과 판정은 `boundary-verifier`의 몫이다.
"""

from __future__ import annotations

import argparse
import json
import os
import sys

from boundary_adapter import (
    constraints_from_boundary,
    constraints_to_boundary,
    plan_from_boundary,
    roadmap_from_boundary,
)
from gen_roadmap import structural_schema_check
from sadg_core import (
    ALT_GENERATORS,
    check_all_switch_combinations,
    compile_graph,
    heads_of,
    inject_cycle,
    plan_view_of,
    prune_cyclic_alternatives,
    self_check,
)

HERE = os.path.dirname(os.path.abspath(__file__))
SCHEMA_PATH = os.path.normpath(
    os.path.join(HERE, "..", "mrs_msgs", "schema", "execution_constraints.schema.json")
)
ARTIFACT_NAME = "execution_constraints.json"


def load_inputs(roadmap_path: str, plan_path: str):
    with open(roadmap_path, "r", encoding="utf-8") as handle:
        model = roadmap_from_boundary(json.load(handle))
    with open(plan_path, "r", encoding="utf-8") as handle:
        plan = plan_from_boundary(json.load(handle))
    if model.instance_id != plan.instance_id:
        raise ValueError(
            f"instance_id가 다르다: 로드맵 {model.instance_id} vs 계획 {plan.instance_id}"
        )
    return model, plan


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="실행 의존 그래프 컴파일러 (탐색용 시제)")
    ap.add_argument("--roadmap", default=os.path.join(HERE, "out", "roadmap.json"))
    ap.add_argument("--plan", default=os.path.join(HERE, "out", "discrete_plan.json"))
    ap.add_argument(
        "--alt-gen", default="lift", choices=sorted(ALT_GENERATORS),
        help="확장점 E3 — lift(선두 올리기, 정본) / swap(인접 쌍 교환, 구판)",
    )
    ap.add_argument("--commit-seq", type=int, default=0, help="판본 번호. 시각이 아니다")
    ap.add_argument("--out-dir", default=os.path.join(HERE, "out"),
                    help=f"산출 디렉터리. 파일 이름은 언제나 {ARTIFACT_NAME}이다")
    ap.add_argument("--max-groups", type=int, default=20,
                    help="전수 비순환 검사할 스위치 그룹 수 상한. 넘으면 표본으로 강등한다")
    ap.add_argument("--max-combinations", type=int, default=1 << 16,
                    help="전수 검사할 조합 수 상한. 넘으면 표본으로 강등한다")
    ap.add_argument("--samples", type=int, default=2000, help="표본 강등 시 검사할 조합 수")
    ap.add_argument("--sample-seed", type=int, default=0, help="표본 추출 시드")
    ap.add_argument("--schema-check", action="store_true", help="표준 JSON Schema 구조 검증")
    ap.add_argument("--stats-out", default=None,
                    help="내부 회계(경계 아님) 저장 경로. 기본은 stats/{out-dir 태그}.json. "
                         "🔴 경계 아티팩트 디렉터리 밖이다 — 층 1 검증기가 비경계 JSON을 "
                         "아티팩트로 오인해 종료 코드 2를 내기 때문이다(실측)")
    ap.add_argument("--inject-cycle", action="store_true",
                    help="🔴 음성 시험 — 일부러 순환을 만든다. 정상 실행에서 쓰지 마라")
    ap.add_argument(
        "--on-cycle", default="report", choices=["report", "prune"],
        help="어떤 조합이 순환할 때: report=그대로 내고 종료 코드 4 / "
             "prune=순환을 만드는 대안을 지운다(요구사항 문서에 없는 내 처리다)",
    )
    args = ap.parse_args(argv)

    model, plan = load_inputs(args.roadmap, args.plan)
    view = plan_view_of(plan)
    graph = compile_graph(view, alt_generator=args.alt_gen, revision=args.commit_seq)

    pruned, prune_log = 0, []
    if args.on_cycle == "prune":
        pruned, prune_log = prune_cyclic_alternatives(
            graph, max_groups=args.max_groups, max_combinations=args.max_combinations
        )

    # 음성 시험은 **가지치기 뒤에** 주입한다 — 그래야 «다른 순환이 없는 그래프에 하나만
    # 심었고 검사기가 그것을 잡았다»가 되고, 이미 있던 순환에 묻히지 않는다.
    injected = None
    if args.inject_cycle:
        injected = inject_cycle(graph)

    adjacency = set()
    for e in model.edges:
        adjacency.add((e.u, e.v))
        if e.traversal == "bidirectional":
            adjacency.add((e.v, e.u))
    problems = self_check(graph, view, adjacency)
    for p in problems:
        print(f"  [self-check] {p}", file=sys.stderr)

    report = check_all_switch_combinations(
        graph,
        max_groups=args.max_groups,
        max_combinations=args.max_combinations,
        samples=args.samples,
        seed=args.sample_seed,
    )

    doc = constraints_to_boundary(graph)
    out_path = os.path.join(args.out_dir, ARTIFACT_NAME)
    os.makedirs(os.path.abspath(args.out_dir), exist_ok=True)
    with open(out_path, "w", encoding="utf-8") as handle:
        json.dump(doc, handle, indent=2, ensure_ascii=False)
        handle.write("\n")

    st = graph.stats
    alt_sizes = [len(g.alternatives) for g in graph.groups]
    print(f"instance_id        : {graph.instance_id}")
    print(f"대안 생성기        : {args.alt_gen} (확장점 E3 — 인자 한 줄로 갈린다)")
    print(f"세그먼트           : {len(graph.segments)}개 (= 로봇별 단계 수 − 1의 합)")
    print(f"고정 의존          : {len(graph.fixed)}개")
    print(f"공유 위치          : {st.shared_locations}개")
    print(f"스위치 그룹        : {st.groups_emitted}개  |A_g| = {alt_sizes}")
    print(f"|A_g|=1 로 접힌 위치: {st.groups_folded_single}개 (그룹으로 발행하지 않았다)")
    print(f"표현 불가로 버린 대안: {st.dropped_unrepresentable}개 "
          f"(출발 항목 {st.pinned_start_items} · 종점 항목 {st.pinned_terminal_items})")
    print(f"중복 방향 간선     : {st.same_robot_ordering_edges}개 (Type-1과 같은 방향)")
    print(f"임계 경로 세그먼트 수: {st.critical_path_segments}  🔴 경계가 아니다")
    print(f"self-check 위반    : {len(problems)}")
    if pruned:
        print(f"🔴 순환 대안 제거  : {pruned}개 — A4를 구성으로 강제했다 (요구사항 문서 밖 처리)")
        for line in prune_log:
            print(f"     - {line}")
    if injected:
        print(f"🔴 순환 주입       : {injected}")

    coverage = "전수" if report.exhaustive else "부분 표본 — 증명이 아니다"
    print(f"비순환 검사        : {report.checked}/{report.total} 조합 ({coverage})")
    if not report.ok:
        print(f"🔴 순환하는 조합   : {report.failing_labels}")
    print(f"Heads(A_g)         : {[sorted(heads_of(g)) for g in graph.groups]}  🔴 경계가 아니다")
    print(f"wrote              : {os.path.abspath(out_path)}")

    tag = os.path.relpath(os.path.abspath(args.out_dir), HERE).replace(os.sep, "_")
    stats_path = args.stats_out or os.path.join(HERE, "stats", f"{tag}.json")
    os.makedirs(os.path.dirname(os.path.abspath(stats_path)), exist_ok=True)
    with open(stats_path, "w", encoding="utf-8") as handle:
        json.dump(
            {
                "note": "내부 회계다. 경계 아티팩트가 아니며 논문의 증거가 아니다.",
                "instance_id": graph.instance_id,
                "alt_generator_key": st.alt_generator,
                # 🔴 키 이름은 NOTATION.md 91행이 정한 `result.json:critical_path_len`이다.
                # `lambda_star`로 쓰면 check_schema_purity.py의 /lambda/ 패턴에 걸린다(실측).
                "critical_path_len": st.critical_path_segments,
                "switch_group_count": st.groups_emitted,
                "alternatives_per_group": alt_sizes,
                "folded_single_alternative_locations": st.groups_folded_single,
                "dropped_unrepresentable_alternatives": st.dropped_unrepresentable,
                "pinned_start_items": st.pinned_start_items,
                "pinned_terminal_items": st.pinned_terminal_items,
                "duplicate_direction_edges": st.same_robot_ordering_edges,
                "pruned_cyclic_alternatives": pruned,
                "c5_mode": "exhaustive" if report.exhaustive else "sampled",
                "c5_checked": report.checked,
                "c5_total": report.total,
            },
            handle,
            indent=2,
            ensure_ascii=False,
        )
        handle.write("\n")
    print(f"wrote              : {os.path.abspath(stats_path)} "
          "(내부 회계 — 경계 아님. 아티팩트 디렉터리 밖에 둔다)")

    schema_errors = 0
    if args.schema_check:
        schema_errors = structural_schema_check(doc, SCHEMA_PATH)
        print(f"schema errors      : {schema_errors} (구조만. 확장 규칙 미검사)")

    roundtrip_ok = constraints_to_boundary(constraints_from_boundary(doc)) == doc
    print(f"어댑터 왕복 항등   : {roundtrip_ok} (경계 -> 내부 -> 경계)")

    if problems or schema_errors or not roundtrip_ok:
        return 2
    if not report.ok:
        return 4
    if not report.exhaustive:
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
