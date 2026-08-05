#!/usr/bin/env python3
"""S-A 4변형 보고 — **전수** 비순환 조합 수와 순환을 만드는 순서쌍을 센다 (탐색용 시제).

`gen_constraints.py --on-cycle report`의 화면 출력(`비순환 검사 N/M`)은 **첫 실패에서
멈춘 조합 수**다 — `_workspace/60_session9_decisions.md` §60-1이 지적한 바로 그 함정이다.
이 스크립트는 **끝까지 전수로** 돌려 비순환 조합이 몇 개인지, 그리고 순환하는 각 조합이
어느 스위치 그룹의 어느 순서(대안)들의 조합인지, 대표 사례에서 실제 순환 고리 하나를
segment 단위로 뽑아 보인다.

`sadg_core.py`·`gen_constraints.py`를 고치지 않는다 — 같은 입력(로드맵·이산 계획)에서
`compile_graph`를 **다시** 부를 뿐이다(생성자의 자체 점검과 같은 종류이며 판정이 아니다).

사용:
    python3 report_scenario_a.py --out-dir out --stats-out stats/scenario_a_report.json

🔴 탐색용 시제이며 증거가 아니다.
"""

from __future__ import annotations

import argparse
import json
import os
from itertools import product

from boundary_adapter import plan_from_boundary, roadmap_from_boundary
from sadg_core import compile_graph, is_acyclic, plan_view_of

HERE = os.path.dirname(os.path.abspath(__file__))
VARIANTS = [
    ("single", 2),
    ("single", 1),
    ("chain", 2),
    ("chain", 1),
]


def _label(group, alt_index) -> str:
    alt = group.alternatives[alt_index]
    order = "->".join(r for r, _ in alt.order) if alt.order else "?"
    return f"{group.location}#{alt_index}:{order}"


def find_one_cycle(nodes, edges):
    """Kahn으로 비순환 부분을 걷어내고 남은 부분 그래프에서 DFS로 순환 고리 하나를 뽑는다.

    돌려주는 것: 순환에 참여하는 SegKey 목록(닫힌 고리, 마지막 원소가 처음과 같다). 없으면 None.
    """
    adj: dict = {n: [] for n in nodes}
    indeg = {n: 0 for n in nodes}
    for e in edges:
        adj[e.src].append(e.dst)
        indeg[e.dst] += 1
    queue = [n for n in nodes if indeg[n] == 0]
    indeg2 = dict(indeg)
    removed = set()
    while queue:
        u = queue.pop()
        removed.add(u)
        for v in adj[u]:
            indeg2[v] -= 1
            if indeg2[v] == 0:
                queue.append(v)
    remaining = [n for n in nodes if n not in removed]
    if not remaining:
        return None
    remaining_set = set(remaining)
    sub_adj = {n: [v for v in adj[n] if v in remaining_set] for n in remaining}
    visited: set = set()
    onstack: set = set()
    stack: list = []

    def dfs(u):
        visited.add(u)
        stack.append(u)
        onstack.add(u)
        for v in sub_adj[u]:
            if v not in visited:
                result = dfs(v)
                if result is not None:
                    return result
            elif v in onstack:
                idx = stack.index(v)
                return stack[idx:] + [v]
        stack.pop()
        onstack.discard(u)
        return None

    for n in remaining:
        if n not in visited:
            cyc = dfs(n)
            if cyc is not None:
                return cyc
    return None  # 걸러낸 뒤에도 남았는데 DFS가 못 찾으면 논리 결함 — 지어내지 않는다


def exhaustive_combo_scan(graph):
    """**끝까지** 돈다 — 첫 실패에서 멈추지 않는다. (전수 비순환 수, 전체 조합 수, 순환 조합 라벨 목록)."""
    nodes = [s.key for s in graph.segments]
    base = list(graph.fixed)
    per_group = [[alt.edges for alt in g.alternatives] for g in graph.groups]
    counts = [len(a) for a in per_group]
    total = 1
    for c in counts:
        total *= c
    acyclic_n = 0
    cyclic_labels: list[list[str]] = []
    iterator = product(*[range(c) for c in counts]) if counts else iter([()])
    for combo in iterator:
        edges = list(base)
        for gi, ai in enumerate(combo):
            edges.extend(per_group[gi][ai])
        if is_acyclic(nodes, edges):
            acyclic_n += 1
        else:
            cyclic_labels.append([_label(graph.groups[gi], ai) for gi, ai in enumerate(combo)])
    return acyclic_n, total, cyclic_labels


def solo_cyclic(graph):
    """**한 그룹만** 원 순서에서 벗어났을 때 순환하는 대안 목록 (다른 그룹은 원 순서 고정)."""
    nodes = [s.key for s in graph.segments]
    base = list(graph.fixed)
    originals = [next(a for a in g.alternatives if a.is_original) for g in graph.groups]
    bad: list[str] = []
    for gi, group in enumerate(graph.groups):
        for ai, alt in enumerate(group.alternatives):
            if alt.is_original:
                continue
            edges = list(base)
            for gj, orig in enumerate(originals):
                edges.extend(alt.edges if gj == gi else orig.edges)
            if not is_acyclic(nodes, edges):
                bad.append(_label(group, ai))
    return bad


def analyze_variant(out_dir: str) -> dict:
    with open(os.path.join(out_dir, "roadmap.json"), "r", encoding="utf-8") as handle:
        model = roadmap_from_boundary(json.load(handle))
    with open(os.path.join(out_dir, "discrete_plan.json"), "r", encoding="utf-8") as handle:
        result = plan_from_boundary(json.load(handle))
    graph = compile_graph(plan_view_of(result), alt_generator="lift", revision=0)

    acyclic_n, total, cyclic_labels = exhaustive_combo_scan(graph)
    solo_bad = solo_cyclic(graph)

    nodes = [s.key for s in graph.segments]
    witness = None
    if cyclic_labels:
        # 첫 순환 조합을 다시 만들어 실제 고리를 뽑는다(표시용 — 앞의 전수 스캔과 같은 계산의 재현).
        first_combo_labels = cyclic_labels[0]
        edges = list(graph.fixed)
        for gi, group in enumerate(graph.groups):
            label = first_combo_labels[gi]
            ai = int(label.split("#", 1)[1].split(":", 1)[0])
            edges.extend(group.alternatives[ai].edges)
        cyc = find_one_cycle(nodes, edges)
        if cyc:
            witness = [f"{r}#{i}" for r, i in cyc]

    return {
        "instance_tag": model.instance_id,
        "switch_group_count": len(graph.groups),
        "alternatives_per_group": [len(g.alternatives) for g in graph.groups],
        "total_combinations": total,
        "acyclic_combinations_exhaustive": acyclic_n,
        "cyclic_combinations_exhaustive": total - acyclic_n,
        "solo_cyclic_alternatives": solo_bad,
        "first_cyclic_combo_labels": cyclic_labels[0] if cyclic_labels else [],
        "witness_cycle_segments": witness,
    }


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="S-A 4변형 전수 조합 보고 (탐색용 시제)")
    ap.add_argument("--out-dir", default=os.path.join(HERE, "out"))
    ap.add_argument("--stats-out", default=os.path.join(HERE, "stats", "scenario_a_report.json"))
    args = ap.parse_args(argv)

    rows = []
    for topology, capacity in VARIANTS:
        d = os.path.join(args.out_dir, f"scenA_{topology}_cap{capacity}")
        rows.append((topology, capacity, analyze_variant(d)))

    print("# S-A 4변형 — 전수 비순환 조합 (탐색용 시제, 증거 아님)\n")
    header = (
        "토폴로지 | 용량 | 스위치그룹 | |A_g| | 전체조합 | 전수비순환 | 전수순환 | "
        "단독순환대안수"
    )
    print(header)
    print("-" * len(header))
    for topology, capacity, r in rows:
        print(
            f"{topology:8s} | {capacity:4d} | {r['switch_group_count']:10d} | "
            f"{r['alternatives_per_group']!s:12s} | {r['total_combinations']:8d} | "
            f"{r['acyclic_combinations_exhaustive']:10d} | "
            f"{r['cyclic_combinations_exhaustive']:8d} | {len(r['solo_cyclic_alternatives']):14d}"
        )
    print()
    for topology, capacity, r in rows:
        print(f"## {topology} / capacity={capacity} ({r['instance_tag']})")
        print(f"- 단독순환대안: {r['solo_cyclic_alternatives']}")
        print(f"- 첫 순환 조합 라벨: {r['first_cyclic_combo_labels']}")
        print(f"- 그 조합의 실제 순환 고리(segment): {r['witness_cycle_segments']}")
        print()

    os.makedirs(os.path.dirname(os.path.abspath(args.stats_out)), exist_ok=True)
    with open(args.stats_out, "w", encoding="utf-8") as handle:
        json.dump(
            {
                "note": "내부 회계다. 경계 아티팩트가 아니며 논문 증거가 아니다.",
                "variants": [r for _, _, r in rows],
            },
            handle,
            indent=2,
            ensure_ascii=False,
        )
        handle.write("\n")
    print(f"wrote stats: {os.path.abspath(args.stats_out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
