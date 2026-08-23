#!/usr/bin/env python3
"""새 방향 통로 로드맵 생성기 — 교차로에만 정점, 통로는 정점 없는 간선 하나 (265d 스파이크).

    python3 gen_corridor_roadmap.py --seed 0 --out <파일>.json
    python3 gen_corridor_roadmap.py --seed 0 --wait-spurs --out <파일>.json

🔴 **탐색용 시제다.** 여기서 나오는 어떤 지도도 논문 증거가 아니고 동결 대상이 아니다.
🔴 이 파일은 경계 스키마를 **직접 알지 못한다** — 경계 변환은 기존 `boundary_adapter.
roadmap_to_boundary` 하나를 그대로 쓴다(JSON 키 문자열 0개 규율 유지).

구 생성기(`gen_roadmap.py`·`roadmap_core.generate_roadmap`)와의 차이:
  - 구 생성기: 통로 = 구간 5 m 간선 **여러 개**의 묶음(내부 정점 있음).
  - 이 생성기: **통로 = 간선 하나**(내부 정점 0), 길이 수십 m(스파이크 입력 집합에서 추첨).
    `U31-2`(통로 안 무노드)의 지도 실물이 0개라는 `265b` §265b-1의 자리를 채우는 표본이다.
  - 교행 불가 표시는 계약 관례 그대로: capacity_robots=1 + corridor id (traversal은
    bidirectional 유지 — roadmap.schema.json §4-2).
  - `R-264-1` 후보 검증용 **통로 입구 대기 지선** 켜기/끄기 옵션(`wait_spurs`).

기존 것 재사용: `roadmap_core`의 Node/Edge/Corridor/RoadmapModel/validate_model,
`protoscale.ScaleConfig`(폭·용량 규칙·최소 이격), `boundary_adapter.roadmap_to_boundary`.
"""

from __future__ import annotations

import argparse
import json
import math
import os
import random
from dataclasses import asdict, dataclass

from protoscale import ScaleConfig, load_scale
from roadmap_core import Corridor, Edge, Node, RoadmapModel, validate_model

SPIKE_GENERATOR_VERSION = "prototype-gen_corridor_roadmap-0.1.0"

# 기하 양자화 — roadmap_core.COORD_DECIMALS와 같은 규율(코어에서 양자화).
_DECIMALS = 6


def _q(value: float) -> float:
    return round(float(value), _DECIMALS)


@dataclass(frozen=True)
class CorridorGenParams:
    """생성 인자. 물리 폭·용량 규칙은 ScaleConfig에서 온다(값 사본 없음).

    corridor_lengths_m가 이 생성기의 핵심 스파이크 입력이다 — 격자 한 칸(=통로 후보
    간선 하나)의 길이를 이 집합에서 시드 결정론으로 추첨한다. 동결 아님.
    """

    cols: int = 8
    rows: int = 6
    endpoints: int = 16
    narrow_ratio: float = 0.5
    corridor_lengths_m: tuple[float, ...] = (15.0, 30.0, 60.0)
    wait_spurs: bool = False
    spur_length_m: float = 2.0
    seed: int = 0


def _jkey(col: int, row: int) -> str:
    return f"J{col:02d}{row:02d}"


def generate_corridor_roadmap(cfg: ScaleConfig, params: CorridorGenParams) -> RoadmapModel:
    """교차로 격자 + 정점 없는 긴 통로 간선의 로드맵을 시드 결정론적으로 생성한다.

    구조: cols x rows 교차점 격자. 이웃 교차점 사이가 **간선 하나**이며 길이는
    corridor_lengths_m에서 추첨한다(같은 열 간격·행 간격은 같은 길이 — 평면 좌표 정합).
    둘레 간선은 교행 가능으로 남기고(연결성 관례 — 구 생성기와 같은 이유),
    내부 간선을 narrow_ratio 확률로 교행 불가(용량 1 + 단일 간선 통로)로 만든다.

    @param cfg 물리 규모(폭·용량 규칙·지선 길이)
    @param params 생성 인자(격자 크기·길이 집합·대기 지선 여부·시드)
    @return 내부 RoadmapModel (경계 변환은 boundary_adapter가 한다)
    @throws ValueError 격자가 2x2 미만이거나 길이 집합이 비었을 때
    """
    if params.cols < 2 or params.rows < 2:
        raise ValueError("cols와 rows는 각각 2 이상이어야 한다")
    if not params.corridor_lengths_m:
        raise ValueError("corridor_lengths_m가 비어 있다")
    rng = random.Random(params.seed)

    # 1) 격자 간격 — 열 사이·행 사이 길이를 집합에서 추첨(시드 결정론).
    col_gap = [rng.choice(params.corridor_lengths_m) for _ in range(params.cols - 1)]
    row_gap = [rng.choice(params.corridor_lengths_m) for _ in range(params.rows - 1)]
    xs = [0.0]
    for g in col_gap:
        xs.append(xs[-1] + g)
    ys = [0.0]
    for g in row_gap:
        ys.append(ys[-1] + g)

    coords: dict[str, tuple[float, float]] = {}
    for c in range(params.cols):
        for r in range(params.rows):
            coords[_jkey(c, r)] = (_q(xs[c]), _q(ys[r]))

    # 2) 격자 간선 — 전부 「교차점—교차점 한 구간」. 내부 간선만 교행 불가 후보.
    def interior_h(c: int, r: int) -> bool:  # 가로 간선 (c,r)-(c+1,r)
        return 0 < r < params.rows - 1

    def interior_v(c: int, r: int) -> bool:  # 세로 간선 (c,r)-(c,r+1)
        return 0 < c < params.cols - 1

    raw_edges: list[tuple[str, str, float, bool]] = []  # (u, v, length, interior)
    for r in range(params.rows):
        for c in range(params.cols - 1):
            raw_edges.append((_jkey(c, r), _jkey(c + 1, r), col_gap[c], interior_h(c, r)))
    for c in range(params.cols):
        for r in range(params.rows - 1):
            raw_edges.append((_jkey(c, r), _jkey(c, r + 1), row_gap[r], interior_v(c, r)))

    narrow_flags = [interior and rng.random() < params.narrow_ratio for (_, _, _, interior) in raw_edges]
    if not any(narrow_flags):
        # 교행 불가가 하나도 없으면 스파이크의 목적을 잃는다 — 첫 내부 간선을 강제.
        for i, (_, _, _, interior) in enumerate(raw_edges):
            if interior:
                narrow_flags[i] = True
                break

    edges: list[Edge] = []
    corridors: list[Corridor] = []
    narrow_pairs: list[tuple[str, str]] = []
    for (u, v, length, _interior), is_narrow in zip(raw_edges, narrow_flags):
        width = cfg.aisle_width_narrow_m if is_narrow else cfg.aisle_width_wide_m
        ckey = f"q_{u}_{v}" if is_narrow else ""
        edges.append(
            Edge(u, v, _q(length), _q(width), cfg.capacity_rule_a(width), ckey, "corridor")
        )
        if is_narrow:
            corridors.append(Corridor(key=ckey, edge_keys=((u, v),), length_m=_q(length)))
            narrow_pairs.append((u, v))

    # 3) 엔드포인트 지선 — 구 생성기와 같은 대각 지선 관례.
    junctions = [(c, r) for c in range(params.cols) for r in range(params.rows)]
    n_ep = min(params.endpoints, len(junctions))
    chosen = sorted(rng.sample(sorted(junctions), n_ep))
    sides = {"NE": (1, 1), "NW": (-1, 1), "SE": (1, -1), "SW": (-1, -1)}
    side_names = sorted(sides)
    endpoints: list[str] = []
    diag = cfg.endpoint_stub_length_m / math.sqrt(2.0)
    for c, r in chosen:
        side = side_names[rng.randrange(len(side_names))]
        dx, dy = sides[side]
        jk = _jkey(c, r)
        pk = f"P{c:02d}{r:02d}{side}"
        jx, jy = coords[jk]
        coords[pk] = (_q(jx + dx * diag), _q(jy + dy * diag))
        edges.append(
            Edge(jk, pk, _q(cfg.endpoint_stub_length_m), _q(cfg.stub_width_m),
                 cfg.capacity_rule_a(cfg.stub_width_m), "", "stub")
        )
        endpoints.append(pk)

    # 4) 대기 지선(R-264-1 후보) — 교행 불가 통로의 **양끝** 교차로마다 지선 하나.
    #    지선 정점은 「목표·대기 지점이 놓일 수 있는 정점」이므로 endpoints에 넣는다
    #    (roadmap.schema.json endpoints 설명: V^ep = 목표·대기 지점).
    if params.wait_spurs:
        spur_count: dict[str, int] = {}
        spur_dirs = [(1, 1), (-1, 1), (1, -1), (-1, -1), (1, 0), (-1, 0), (0, 1), (0, -1)]
        for u, v in narrow_pairs:
            for jk in (u, v):
                k = spur_count.get(jk, 0)
                spur_count[jk] = k + 1
                wk = f"W{jk[1:]}_{k:02d}"
                dx, dy = spur_dirs[k % len(spur_dirs)]
                norm = math.hypot(dx, dy)
                jx, jy = coords[jk]
                coords[wk] = (
                    _q(jx + dx / norm * params.spur_length_m),
                    _q(jy + dy / norm * params.spur_length_m),
                )
                edges.append(
                    Edge(jk, wk, _q(params.spur_length_m), _q(cfg.stub_width_m),
                         cfg.capacity_rule_a(cfg.stub_width_m), "", "spur")
                )
                endpoints.append(wk)

    # 5) 여유 반경 — 구 생성기와 같은 「인접 간선 폭 절반」 대용값(한계 승계).
    widest: dict[str, float] = {}
    for e in edges:
        widest[e.u] = max(widest.get(e.u, 0.0), e.width_m)
        widest[e.v] = max(widest.get(e.v, 0.0), e.width_m)
    nodes = [
        Node(key, coords[key][0], coords[key][1], _q(widest[key] / 2.0))
        for key in sorted(coords)
    ]
    edges.sort(key=lambda e: (e.kind, e.u, e.v))
    corridors.sort(key=lambda c: c.key)

    tag = "_spur" if params.wait_spurs else ""
    return RoadmapModel(
        instance_id=f"spike_corr_{params.cols}x{params.rows}_ep{n_ep}_s{params.seed}{tag}",
        nodes=nodes,
        edges=edges,
        corridors=corridors,
        endpoints=sorted(endpoints),
        min_separation_m=cfg.min_separation_m,
        wall_inflation_m=cfg.wall_inflation_m,
        generator_version=SPIKE_GENERATOR_VERSION,
    )


def validate_corridor_spike(model: RoadmapModel, params: CorridorGenParams) -> list[str]:
    """이 생성기 고유의 구조 자기 검사 — 위반 문자열 목록을 돌려준다(빈 목록 = 위반 0건).

    `roadmap_core.validate_model`(공통 구조)에 **더해** 새 방향의 요구를 본다:
    통로 = 간선 하나 · 통로 간선 용량 1 · 통로 양끝이 교차로 정점 ·
    통로 길이가 입력 집합 안 · 대기 지선 옵션의 켬/끔이 실제 구조로 나타남.
    """
    problems = list(validate_model(model))
    edge_by_pair = {(e.u, e.v): e for e in model.edges}
    for c in model.corridors:
        if len(c.edge_keys) != 1:
            problems.append(f"통로 {c.key}가 간선 {len(c.edge_keys)}개다 (한 간선이어야 한다)")
            continue
        pair = c.edge_keys[0]
        e = edge_by_pair.get(pair)
        if e is None:
            continue  # validate_model이 이미 보고한다
        if e.capacity_robots != 1:
            problems.append(f"통로 간선 {pair} 용량 {e.capacity_robots} (1이어야 한다)")
        if not (e.u.startswith("J") and e.v.startswith("J")):
            problems.append(f"통로 {c.key} 끝점이 교차로가 아니다: {pair}")
        if not any(abs(c.length_m - L) < 1e-9 for L in params.corridor_lengths_m):
            problems.append(f"통로 {c.key} 길이 {c.length_m}가 입력 집합 밖이다")
    spur_nodes = [n.key for n in model.nodes if n.key.startswith("W")]
    if params.wait_spurs:
        adj_spur: set[str] = set()
        for e in model.edges:
            if e.kind == "spur":
                adj_spur.add(e.u)
        for c in model.corridors:
            for end in c.edge_keys[0]:
                if end not in adj_spur:
                    problems.append(f"통로 {c.key} 입구 {end}에 대기 지선이 없다")
    elif spur_nodes:
        problems.append(f"wait_spurs=False인데 대기 지선 정점 {len(spur_nodes)}개")
    return problems


def boundary_doc(model: RoadmapModel) -> dict:
    """경계 아티팩트 — 기존 어댑터를 그대로 쓴다(이 파일에 JSON 키 문자열 없음)."""
    from boundary_adapter import roadmap_to_boundary

    return roadmap_to_boundary(model)


def schema_errors(doc: dict) -> list[str]:
    """mrs.roadmap 3.0.0 구조 검증 — 기존 시험(test_schema_conformance)과 같은 수단.

    ⚠️ 확장 키워드(x-ref 등)는 표준 검증기가 무시한다 — 오류 0 = «구조가 맞다»까지다.
    """
    from jsonschema import Draft202012Validator

    here = os.path.dirname(os.path.abspath(__file__))
    schema_path = os.path.normpath(
        os.path.join(here, "..", "mrs_msgs", "schema", "roadmap.schema.json")
    )
    with open(schema_path, "r", encoding="utf-8") as handle:
        schema = json.load(handle)
    return [
        f"{list(e.path)}: {e.message}"
        for e in Draft202012Validator(schema).iter_errors(doc)
    ]


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="통로 로드맵 생성기 (탐색용 시제 — 증거 아님)")
    ap.add_argument("--cols", type=int, default=8)
    ap.add_argument("--rows", type=int, default=6)
    ap.add_argument("--endpoints", type=int, default=16)
    ap.add_argument("--narrow-ratio", type=float, default=0.5)
    ap.add_argument("--lengths", default="15,30,60", help="통로 길이 집합 [m], 콤마 구분")
    ap.add_argument("--wait-spurs", action="store_true")
    ap.add_argument("--seed", type=int, default=0)
    ap.add_argument("--out", default="", help="경계 JSON을 쓸 경로 (비우면 안 씀)")
    args = ap.parse_args(argv)

    cfg = load_scale()
    params = CorridorGenParams(
        cols=args.cols, rows=args.rows, endpoints=args.endpoints,
        narrow_ratio=args.narrow_ratio,
        corridor_lengths_m=tuple(float(x) for x in args.lengths.split(",")),
        wait_spurs=args.wait_spurs, seed=args.seed,
    )
    model = generate_corridor_roadmap(cfg, params)
    problems = validate_corridor_spike(model, params)
    doc = boundary_doc(model)
    errs = schema_errors(doc)
    lens = sorted({c.length_m for c in model.corridors})
    print(f"# instance_id={model.instance_id}")
    print(f"# nodes={len(model.nodes)} edges={len(model.edges)} corridors={len(model.corridors)}"
          f" endpoints={len(model.endpoints)}")
    print(f"# corridor_lengths_present={lens}")
    print(f"# structure_problems={len(problems)} schema_errors={len(errs)}")
    for p in problems:
        print(f"#   구조: {p}")
    for e in errs:
        print(f"#   스키마: {e}")
    if args.out:
        os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
        with open(args.out, "w", encoding="utf-8") as handle:
            json.dump(doc, handle, ensure_ascii=False, indent=2)
        print(f"# wrote {args.out}")
    meta = {"params": asdict(params), "generator_version": SPIKE_GENERATOR_VERSION}
    print(f"# params={json.dumps(meta, ensure_ascii=False)}")
    return 0 if not problems and not errs else 1


if __name__ == "__main__":
    raise SystemExit(main())
