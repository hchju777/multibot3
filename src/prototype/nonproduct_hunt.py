#!/usr/bin/env python3
"""비곱(non-product) 인스턴스 헌팅 — $\\mathcal{Z}^{\\mathrm{acy}}$가 곱꼴이 아닌 자리를 찾는다.

🔴 **탐색용 시제이며 증거가 아니다. 어떤 값도 사전등록 상수가 아니다. 사전등록을 발동시키지 않는다.**

## 무엇을 재는가

그룹별 대안 집합 $A_g$의 곱집합 $\\prod_g A_g$ 안에서 **비순환인 조합들의 집합**을
$\\mathcal{Z}^{\\mathrm{acy}}$라 하고, 각 그룹으로의 사영을 $A_g^{\\mathrm{proj}}$라 한다.

$$\\mathcal{Z}^{\\mathrm{acy}}\\ \\subseteq\\ \\prod_g A_g^{\\mathrm{proj}}\\quad(\\text{항상}),\\qquad
  \\text{**비곱**}\\ :\\Leftrightarrow\\ \\mathcal{Z}^{\\mathrm{acy}}\\ \\subsetneq\\ \\prod_g A_g^{\\mathrm{proj}}$$

곱꼴이면 «각 그룹에서 살아남은 대안을 아무렇게나 조합해도 비순환»이므로 집합 소속형(M)이
지정형(D)과 **같은 값을 더 적은 장치로** 낸다(`66_p2` §66-9-4). 비곱이면 그렇지 않다.

🔴 **가지치기 전에 재야 한다.** `prune_cyclic_alternatives`는 순환 조합에 참여한 대안을
**통째로 지우므로** 그 결과는 정의상 곱꼴이다. 가지치기 뒤에 재면 «곱꼴만 보인다»가
측정 절차의 산물이 된다. 이 스크립트는 **가지치기하지 않은 그래프**에서 잰다.

## 왜 곱꼴만 나왔는가 — 진단 지표

2개 이상의 그룹이 함께여야 순환이 생기려면 **같은 로봇 쌍이 두 위치를 서로 반대 순서로
지나야** 한다(반평행 쌍). 그래서 각 인스턴스에서 다음을 함께 센다:

- `antiparallel_group_pairs` — 로봇 $r,s$와 그룹 위치 $v,w$가 $r: v\\prec w$, $s: w\\prec v$인 조합 수.
- `solo_cyclic_alternatives` — 혼자서 순환을 만드는 대안 수(어떤 조합에 넣어도 순환).
- `min_forcing_support` — 순환을 강제하는 **최소 그룹 수**. 1이면 곱꼴이 구조적으로 따라온다.
"""

from __future__ import annotations

import argparse
import json
import math
import os
from itertools import combinations, product

from assignment_core import AssignParams, AssignmentModel, GoalItem, RobotPlan
from assignment_core import build_assignment as build_warehouse_assignment
from plan_core import PlanParams, plan_instance
from protoscale import load_scale
from roadmap_core import Edge, GenParams, Node, RoadmapModel, generate_roadmap
from sadg_core import (
    PlanView,
    apply_lift_cut,
    compile_graph,
    is_acyclic,
    plan_view_of,
    prune_cyclic_alternatives,
)

HERE = os.path.dirname(os.path.abspath(__file__))
MAX_COMBINATIONS = 1 << 16


# ---------------------------------------------------------------------------
# 곱꼴 판별
# ---------------------------------------------------------------------------


def joint_analysis(graph, max_combinations: int = MAX_COMBINATIONS) -> dict:
    """$\\mathcal{Z}^{\\mathrm{acy}}$를 전수 열거하고 곱꼴 여부를 낸다.

    상한을 넘으면 **표본으로 강등하지 않고 `exhaustive=False`로 판정을 비운다** — 곱꼴 여부는
    전수로만 말할 수 있는 성질이고, 표본으로 «곱꼴이다»라고 적으면 그것이 허위 보고다.
    """
    nodes = [s.key for s in graph.segments]
    base = list(graph.fixed)
    per_group = [[a.edges for a in g.alternatives] for g in graph.groups]
    counts = [len(a) for a in per_group]
    total = 1
    for c in counts:
        total *= c

    out: dict = {
        "groups": len(graph.groups),
        "alternatives_per_group": counts,
        "total_combinations": total,
        "exhaustive": total <= max_combinations,
        "fixed_only_acyclic": is_acyclic(nodes, base),
    }
    if not out["exhaustive"] or not out["fixed_only_acyclic"]:
        return out

    acyclic: list[tuple[int, ...]] = []
    iterator = product(*[range(c) for c in counts]) if counts else iter([()])
    for combo in iterator:
        edges = list(base)
        for gi, ai in enumerate(combo):
            edges.extend(per_group[gi][ai])
        if is_acyclic(nodes, edges):
            acyclic.append(combo)

    projections = [
        sorted({combo[gi] for combo in acyclic}) for gi in range(len(graph.groups))
    ]
    product_size = 1
    for p in projections:
        product_size *= len(p)
    out.update(
        {
            "acyclic_combinations": len(acyclic),
            "projection_sizes": [len(p) for p in projections],
            "projection_product_size": product_size,
            "non_product": len(acyclic) < product_size,
            "solo_cyclic_alternatives": sum(
                c - len(p) for c, p in zip(counts, projections)
            ),
        }
    )

    # 🔴 최소 지지 크기는 **싸게** 낸다. 크기 1의 강제 = «혼자 순환하는 대안»이고, 그것은
    # 사영에서 이미 빠지므로 `solo_cyclic_alternatives > 0`과 동치다. 부분집합 전수 열거는
    # 조합 폭발이라 돌리지 않는다 — 대신 목격자 조합에 대해서만, 상한 안에서만 정확히 낸다.
    has_cycle = len(acyclic) < total
    out["has_cycle"] = has_cycle
    if not has_cycle:
        out["min_forcing_support"] = None
    elif out["solo_cyclic_alternatives"] > 0:
        out["min_forcing_support"] = 1
    else:
        out["min_forcing_support"] = 2  # 2 이상이라는 뜻. 정확한 값은 목격자에서만 낸다

    if out["non_product"]:
        witness = _first_product_hole(acyclic, projections)
        out["non_product_witness"] = {
            "combination": list(witness),
            "labels": [
                f"{graph.groups[gi].location}#{ai}:"
                + "->".join(f"{r}@{i}" for r, i in graph.groups[gi].alternatives[ai].order)
                for gi, ai in enumerate(witness)
            ],
            "minimal_forcing_support": _minimal_forcing_support(
                graph, witness, counts, per_group, nodes, base, max_size=3, budget=total
            ),
        }
    return out


def _first_product_hole(acyclic, projections) -> tuple[int, ...]:
    """사영의 곱에는 있는데 $\\mathcal{Z}^{\\mathrm{acy}}$에는 없는 조합 하나. 사전식 첫 번째."""
    seen = set(acyclic)
    for combo in product(*projections):
        if combo not in seen:
            return combo
    raise ValueError("곱꼴인데 구멍을 찾으라고 불렸다")


def _is_forced_cyclic(graph, fixed_map, counts, per_group, nodes, base) -> bool:
    """부분 지정 `fixed_map`(그룹 인덱스 -> 대안)의 **모든 확장**이 순환인가."""
    free = [gi for gi in range(len(counts)) if gi not in fixed_map]
    for tail in product(*[range(counts[gi]) for gi in free]):
        edges = list(base)
        combo = dict(fixed_map)
        combo.update(dict(zip(free, tail)))
        for gi, ai in combo.items():
            edges.extend(per_group[gi][ai])
        if is_acyclic(nodes, edges):
            return False
    return True


def _minimal_forcing_support(
    graph, witness, counts, per_group, nodes, base, max_size: int = 3, budget: int = 1 << 13
) -> list[int] | None:
    """구멍 조합에서 **순환을 강제하는 최소 그룹 부분집합**. 크기 오름차순 탐색.

    `max_size`·`budget`을 넘으면 **`None`을 돌려준다** — 못 찾은 것이 아니라 **안 찾은 것**이며
    그 둘을 값으로 구별한다.
    """
    if budget > (1 << 13):
        return None
    indices = list(range(len(counts)))
    for size in range(1, min(max_size, len(indices)) + 1):
        for subset in combinations(indices, size):
            fixed_map = {gi: witness[gi] for gi in subset}
            if _is_forced_cyclic(graph, fixed_map, counts, per_group, nodes, base):
                return list(subset)
    return None


# ---------------------------------------------------------------------------
# 진단 — 왜 곱꼴만 나오는가
# ---------------------------------------------------------------------------


def antiparallel_diagnosis(view: PlanView, graph) -> dict:
    """반평행 쌍을 센다 — **2그룹 순환의 구조적 필요조건**이다.

    로봇 $r,s$가 그룹 위치 $v,w$를 **서로 반대 순서로** 지나면(그리고 두 위치 모두 그룹이면)
    한쪽 그룹의 순서를 뒤집는 조합이 순환을 만들 수 있다. 셋 이상 로봇이 얽힌 순환은 세지
    않는다 — 이 지표는 **하한**이며 0이라고 해서 2그룹 순환이 없다는 뜻은 아니다.
    """
    group_locations = [g.location for g in graph.groups]
    index_of: dict[str, dict[str, int]] = {}
    for robot, nodes in view.steps.items():
        pos: dict[str, int] = {}
        for i, node in enumerate(nodes):
            pos.setdefault(node, i)
        index_of[robot] = pos

    antiparallel = 0
    examples: list[str] = []
    robots = sorted(view.steps)
    for r, s in combinations(robots, 2):
        for v, w in combinations(group_locations, 2):
            pr, ps = index_of[r], index_of[s]
            if v in pr and w in pr and v in ps and w in ps:
                if (pr[v] < pr[w]) != (ps[v] < ps[w]):
                    antiparallel += 1
                    if len(examples) < 5:
                        examples.append(f"{r},{s} @ {v},{w}")
    return {
        "group_locations": len(group_locations),
        "antiparallel_group_pairs": antiparallel,
        "antiparallel_examples": examples,
    }


def pruned_product_size(view: PlanView) -> dict:
    """가지치기(`prune_cyclic_alternatives`)가 남기는 **곱꼴** 조합 수.

    🔴 이것이 «M(집합 소속형)이 실제로 인증할 수 있는 크기»의 한 실현이다 — 가지치기는
    순환에 참여한 비원본 대안을 하나씩 지우는 **탐욕 규칙**이며, 최대 곱꼴 부분집합을 내는
    최적 절차가 아니다. 그래서 이 값은 **M의 하한**이지 M의 최적값이 아니다.
    """
    graph = compile_graph(view, alt_generator="lift")
    removed, _log = prune_cyclic_alternatives(graph)
    return {
        "removed_alternatives": removed,
        "groups": len(graph.groups),
        "product_size": graph.stats.total_combinations,
    }


def analyse_view(view: PlanView, lift_cut: bool = False) -> dict:
    """계획 뷰 하나를 컴파일해 곱꼴 여부와 진단을 낸다. **가지치기하지 않는다.**"""
    graph = compile_graph(view, alt_generator="lift")
    lc_stats = None
    if lift_cut:
        stats = apply_lift_cut(graph, view)
        lc_stats = {
            "promoted_edges": stats.promoted_edges,
            "groups_folded_degenerate": stats.groups_folded_degenerate,
            "terminal_items_blocked": stats.terminal_items_blocked,
            "groups_with_terminal_item": stats.groups_with_terminal_item,
        }
    out = joint_analysis(graph)
    out.update(antiparallel_diagnosis(view, graph))
    out["dropped_unrepresentable"] = graph.stats.dropped_unrepresentable
    out["groups_folded_single"] = graph.stats.groups_folded_single
    if lc_stats is not None:
        out["lift_cut"] = lc_stats
    return out


# ---------------------------------------------------------------------------
# 인스턴스 족 — 무엇을 얼마나 훑었는지가 이 스크립트 산출물의 절반이다
# ---------------------------------------------------------------------------


def build_loop_roadmap(
    cfg, loop_nodes: int, one_way: bool, capacity: int, robot_specs: list[tuple[str, int, int]]
) -> RoadmapModel:
    """**환상 통로(roundabout)** 로드맵. 반평행 쌍을 구조적으로 만들려고 손으로 짰다.

    일방통행(`one_way=True`)이면 $J_i\\to J_{i+1}$만 통행 가능하므로, 서로 반대편에서
    출발해 서로의 출발점으로 가는 두 로봇은 **같은 회전 방향으로 서로 다른 호**를 돌게 되고
    두 접점을 **반대 순서로** 지난다 — 이것이 2그룹 순환의 구조적 조건이다.
    """
    seg = cfg.corridor_segment_length_m
    stub = cfg.endpoint_stub_length_m
    wide = cfg.aisle_width_wide_m
    narrow = cfg.aisle_width_narrow_m
    width = wide if capacity >= 2 else narrow
    stub_cap = cfg.capacity_rule_a(cfg.stub_width_m)

    radius = seg * loop_nodes / (2.0 * math.pi)
    coords: dict[str, tuple[float, float]] = {}
    for i in range(loop_nodes):
        angle = 2.0 * math.pi * i / loop_nodes
        coords[f"J{i}"] = (radius * math.cos(angle), radius * math.sin(angle))

    edges: list[Edge] = []
    traversal = "forward_only" if one_way else "bidirectional"
    for i in range(loop_nodes):
        j = (i + 1) % loop_nodes
        edges.append(Edge(f"J{i}", f"J{j}", seg, width, capacity, "", "aisle", traversal))

    endpoints: list[str] = []
    for name, start_at, goal_at in robot_specs:
        for tag, junction in (("S", start_at), ("G", goal_at)):
            key = f"{tag}{name}"
            jx, jy = coords[f"J{junction}"]
            scale = 1.0 + stub / max(radius, 1e-6)
            coords[key] = (jx * scale, jy * scale)
            edges.append(
                Edge(f"J{junction}", key, stub, cfg.stub_width_m, stub_cap, "", "stub")
            )
        endpoints.append(f"G{name}")

    widest: dict[str, float] = {}
    for e in edges:
        widest[e.u] = max(widest.get(e.u, 0.0), e.width_m)
        widest[e.v] = max(widest.get(e.v, 0.0), e.width_m)
    nodes = [Node(k, coords[k][0], coords[k][1], widest[k] / 2.0) for k in sorted(coords)]

    tag = "ow" if one_way else "bi"
    return RoadmapModel(
        instance_id=f"loop{loop_nodes}_{tag}_cap{capacity}_r{len(robot_specs)}",
        nodes=nodes,
        edges=edges,
        corridors=[],
        endpoints=sorted(endpoints),
        min_separation_m=cfg.min_separation_m,
        wall_inflation_m=cfg.wall_inflation_m,
        generator_version="prototype-nonproduct_hunt-0.1.0",
    )


def loop_robot_specs(loop_nodes: int, robots: int) -> list[tuple[str, int, int]]:
    """로봇 $i$는 $J_{o_i}$에서 출발해 **정반대편** 접점으로 간다. 어긋난 시작점으로 겹침을 만든다."""
    half = loop_nodes // 2
    specs: list[tuple[str, int, int]] = []
    for i in range(robots):
        start = (i * max(1, loop_nodes // max(robots, 1))) % loop_nodes
        specs.append((chr(ord("A") + i), start, (start + half) % loop_nodes))
    return specs


def loop_family(cfg, loop_nodes_list, robots_list, one_way_list, capacities) -> list[dict]:
    """환상 통로 족을 훑는다. 훑은 조합을 **전부** 기록한다 — «안 찾았다»와 «없다»를 가른다."""
    rows: list[dict] = []
    for loop_nodes in loop_nodes_list:
        for robots in robots_list:
            for one_way in one_way_list:
                for capacity in capacities:
                    specs = loop_robot_specs(loop_nodes, robots)
                    model = build_loop_roadmap(cfg, loop_nodes, one_way, capacity, specs)
                    assignment = AssignmentModel(
                        instance_id=model.instance_id,
                        revision=0,
                        robots=[
                            RobotPlan(name, f"S{name}", [GoalItem(f"g-{name}", f"t{name}", f"G{name}")])
                            for name, _, _ in specs
                        ],
                        unassigned_tasks=[],
                    )
                    rows.append(
                        _analyse_instance(
                            model,
                            assignment,
                            {
                                "family": "loop",
                                "loop_nodes": loop_nodes,
                                "robots": robots,
                                "one_way": one_way,
                                "capacity": capacity,
                            },
                        )
                    )
    return rows


def warehouse_family(cfg, robots_list, indices, aisles_list, cross_list, narrow_ratios) -> list[dict]:
    """창고형 격자 족. `65`·`66`이 본 것과 같은 생성기이며 규모·형상 인자를 넓혔다."""
    rows: list[dict] = []
    for aisles in aisles_list:
        for cross in cross_list:
            for narrow in narrow_ratios:
                for robots in robots_list:
                    for index in indices:
                        model = generate_roadmap(
                            cfg,
                            GenParams(
                                aisles=aisles, cross=cross, narrow_ratio=narrow, seed=index
                            ),
                        )
                        assignment = build_warehouse_assignment(
                            model, AssignParams(robots=robots, tasks=robots + 2, seed=index)
                        )
                        rows.append(
                            _analyse_instance(
                                model,
                                assignment,
                                {
                                    "family": "warehouse",
                                    "aisles": aisles,
                                    "cross": cross,
                                    "narrow_ratio": narrow,
                                    "robots": robots,
                                    "generation_index": index,
                                },
                            )
                        )
    return rows


def scenario_a_family(cfg) -> list[dict]:
    """S-A 4변형 — `66_p2` §66-9-1이 «곱꼴»이라 적은 자리를 같은 절차로 다시 잰다."""
    import gen_scenario_a

    rows: list[dict] = []
    for topology in ("single", "chain"):
        for capacity in (1, 2):
            model = gen_scenario_a.build_roadmap(cfg, topology, capacity, 2)
            assignment = gen_scenario_a.build_assignment(topology, capacity)
            rows.append(
                _analyse_instance(
                    model,
                    assignment,
                    {"family": "scenario_a", "topology": topology, "capacity": capacity},
                )
            )
    return rows


def _analyse_instance(model, assignment, config: dict) -> dict:
    """계획 → 컴파일 → 곱꼴 판별. 계획 실패는 **감추지 않고 사유와 함께** 기록한다."""
    plan = plan_instance(model, assignment, PlanParams(priority="most_goals", horizon=96))
    row = {"config": config, "instance_tag": model.instance_id}
    if plan.unplanned:
        row["skipped"] = f"unplanned: {sorted(plan.unplanned)}"
        return row
    view = plan_view_of(plan)
    try:
        row["baseline"] = analyse_view(view, lift_cut=False)
        row["lift_cut"] = analyse_view(view, lift_cut=True)
        row["pruned"] = pruned_product_size(view)
    except ValueError as exc:  # 원 통행 순서 표현 불가 — 지어내지 않는다
        row["skipped"] = f"compile_error: {exc}"
    return row


# ---------------------------------------------------------------------------
# CLI
# ---------------------------------------------------------------------------


def summarise(rows: list[dict]) -> dict:
    """족별 집계. 분모(시행 수)를 언제나 함께 낸다."""
    by_family: dict[str, dict] = {}
    for row in rows:
        fam = row["config"]["family"]
        agg = by_family.setdefault(
            fam,
            {
                "attempted": 0,
                "skipped": 0,
                "analysed": 0,
                "non_exhaustive": 0,
                "non_product_baseline": 0,
                "non_product_lift_cut": 0,
                "min_support_ge2": 0,
                "antiparallel_present": 0,
                "instances_with_cycle": 0,
            },
        )
        agg["attempted"] += 1
        if "skipped" in row:
            agg["skipped"] += 1
            continue
        base = row["baseline"]
        if not base.get("exhaustive") or not base.get("fixed_only_acyclic"):
            agg["non_exhaustive"] += 1
            continue
        agg["analysed"] += 1
        if base.get("non_product"):
            agg["non_product_baseline"] += 1
        if row["lift_cut"].get("non_product"):
            agg["non_product_lift_cut"] += 1
        if base.get("has_cycle"):
            agg["instances_with_cycle"] += 1
            if base.get("min_forcing_support", 1) >= 2:
                agg["min_support_ge2"] += 1
        if base.get("antiparallel_group_pairs", 0) > 0:
            agg["antiparallel_present"] += 1
    return by_family


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="비곱 인스턴스 헌팅 (탐색용 시제)")
    ap.add_argument("--families", default="loop,warehouse,scenario_a")
    ap.add_argument("--warehouse-robots", default="4,6,8")
    ap.add_argument("--warehouse-indices", type=int, default=20)
    ap.add_argument("--warehouse-aisles", default="3,4,5")
    ap.add_argument("--warehouse-cross", default="2,3,4")
    ap.add_argument("--warehouse-narrow", default="0.0,0.5,1.0")
    ap.add_argument("--loop-nodes", default="4,6,8")
    ap.add_argument("--loop-robots", default="2,3,4")
    ap.add_argument("--out", default=os.path.join(HERE, "stats", "nonproduct_hunt.json"))
    args = ap.parse_args(argv)

    cfg = load_scale()
    families = [f.strip() for f in args.families.split(",") if f.strip()]
    rows: list[dict] = []
    scope: dict = {}

    if "loop" in families:
        loop_nodes = [int(v) for v in args.loop_nodes.split(",")]
        loop_robots = [int(v) for v in args.loop_robots.split(",")]
        scope["loop"] = {
            "loop_nodes": loop_nodes,
            "robots": loop_robots,
            "one_way": [True, False],
            "capacity": [1, 2],
        }
        rows += loop_family(cfg, loop_nodes, loop_robots, [True, False], [1, 2])

    if "warehouse" in families:
        wr = [int(v) for v in args.warehouse_robots.split(",")]
        aisles = [int(v) for v in args.warehouse_aisles.split(",")]
        cross = [int(v) for v in args.warehouse_cross.split(",")]
        narrow = [float(v) for v in args.warehouse_narrow.split(",")]
        indices = list(range(args.warehouse_indices))
        scope["warehouse"] = {
            "robots": wr,
            "aisles": aisles,
            "cross": cross,
            "narrow_ratio": narrow,
            "generation_indices": len(indices),
        }
        rows += warehouse_family(cfg, wr, indices, aisles, cross, narrow)

    if "scenario_a" in families:
        scope["scenario_a"] = {"topology": ["single", "chain"], "capacity": [1, 2]}
        rows += scenario_a_family(cfg)

    summary = summarise(rows)
    payload = {
        "note": "탐색용 시제. 증거가 아니며 사전등록을 발동시키지 않는다.",
        "measured_before_pruning": True,
        "search_scope": scope,
        "summary": summary,
        "rows": rows,
    }
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    with open(args.out, "w", encoding="utf-8") as handle:
        json.dump(payload, handle, indent=2, ensure_ascii=False)
        handle.write("\n")

    for fam, agg in sorted(summary.items()):
        print(
            f"[{fam}] 시도 {agg['attempted']} / 건너뜀 {agg['skipped']} / 분석 {agg['analysed']}"
            f" / 비곱(기본) {agg['non_product_baseline']} / 비곱(LIFT-CUT) {agg['non_product_lift_cut']}"
            f" / 최소지지>=2 {agg['min_support_ge2']} / 반평행 있음 {agg['antiparallel_present']}"
        )
    print(f"wrote: {os.path.abspath(args.out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
