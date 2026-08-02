"""로드맵 생성 코어 — 내부 표현만 다룬다.

이 파일은 경계 스키마(mrs.roadmap)를 **모른다.** JSON 키 문자열이 하나도 없다.
경계로 나가는 변환은 boundary_adapter.py 한 곳에만 있다.

🔴 탐색용 시제다. 여기서 나온 어떤 수치도 논문 증거가 아니다.
"""

from __future__ import annotations

import heapq
import math
import random
from dataclasses import dataclass, field

from protoscale import ScaleConfig

GENERATOR_VERSION = "prototype-gen_roadmap-0.1.0"
CAPACITY_RULE_ID = "rule_a"

# 기하 양자화 자릿수(밀리미터보다 잘다). **코어에서** 양자화한다 —
# 어댑터가 반올림하면 내부 값과 경계 값이 달라져 왕복이 항등이 아니게 되고,
# 그 차이는 나중에 «어느 쪽이 진짜인가»를 묻게 만든다.
COORD_DECIMALS = 6


def _q(value: float) -> float:
    return round(float(value), COORD_DECIMALS)


# ---------------------------------------------------------------- 내부 자료구조


@dataclass(frozen=True)
class Node:
    """정점 하나. key는 내부 식별자이며 경계 id와 같은 문자열을 쓴다."""

    key: str
    x_m: float
    y_m: float
    clearance_m: float


TRAVERSALS = ("bidirectional", "forward_only", "backward_only")


@dataclass(frozen=True)
class Edge:
    """간선 하나. corridor_key가 빈 문자열이면 어느 통로에도 속하지 않는다.

    traversal은 **정적 통행 제약**이며 교행 가부가 아니다(계약 §4-2) —
    교행 불가는 capacity_robots=1 + corridor_key로 표현한다. 이 시제의 생성기는
    bidirectional만 만들지만 계획기는 세 값을 모두 다룬다.
    """

    u: str
    v: str
    length_m: float
    width_m: float
    capacity_robots: int
    corridor_key: str
    kind: str  # "aisle" | "cross" | "stub" — 내부 분류. 경계로 나가지 않는다.
    traversal: str = "bidirectional"

    @property
    def traversal_forward(self) -> bool:
        """u -> v 방향으로 통행할 수 있는가."""
        return self.traversal in ("bidirectional", "forward_only")

    @property
    def traversal_backward(self) -> bool:
        """v -> u 방향으로 통행할 수 있는가."""
        return self.traversal in ("bidirectional", "backward_only")


@dataclass(frozen=True)
class Corridor:
    """함께 방향이 뒤집히는 간선 묶음."""

    key: str
    edge_keys: tuple[tuple[str, str], ...]
    length_m: float


@dataclass
class RoadmapModel:
    instance_id: str
    nodes: list[Node]
    edges: list[Edge]
    corridors: list[Corridor]
    endpoints: list[str]
    min_separation_m: float
    wall_inflation_m: float
    capacity_rule_id: str = CAPACITY_RULE_ID
    generator_version: str = GENERATOR_VERSION

    def node_map(self) -> dict[str, Node]:
        return {n.key: n for n in self.nodes}


@dataclass(frozen=True)
class GenParams:
    """생성 인자. 물리 규모는 여기 없다 — ScaleConfig에만 있다."""

    aisles: int = 4  # 세로 통로(격자 열) 개수
    cross: int = 3  # 가로 교차통로(격자 행) 개수
    endpoints: int = 8  # 엔드포인트 지선 개수
    narrow_ratio: float = 0.5  # 교행 불가로 만들 내부 통로의 비율
    seed: int = 0


# ---------------------------------------------------------------- 생성 알고리즘


def _junction_key(col: int, row: int) -> str:
    return f"J{col:02d}{row:02d}"


def _stub_key(col: int, row: int, side: str) -> str:
    return f"P{col:02d}{row:02d}{side}"


def generate_roadmap(cfg: ScaleConfig, params: GenParams) -> RoadmapModel:
    """창고형 로드맵을 시드 결정론적으로 생성한다.

    구조: 세로 통로 x 가로 교차통로 격자 + 엔드포인트 지선.
    교행 가능/불가는 traversal이 아니라 **폭 -> 용량 + 통로 묶음**으로 표현한다
    (계약 §4-2: 교행 불가는 capacity_robots:1 + corridor이고 traversal은
    여전히 bidirectional이다. forward_only는 영구 단방향 램프를 뜻하며 다른 사실이다).
    """
    if params.aisles < 2 or params.cross < 2:
        raise ValueError("aisles와 cross는 각각 2 이상이어야 한다")
    rng = random.Random(params.seed)

    seg = cfg.corridor_segment_length_m
    stub_len = cfg.endpoint_stub_length_m

    # 1) 어느 통로를 교행 불가로 둘지 — 결정론적. 바깥 둘레는 항상 교행 가능으로
    #    남겨 넓은 간선만으로도 전 구역이 연결되게 한다.
    narrow_cols = {
        c for c in range(1, params.aisles - 1) if rng.random() < params.narrow_ratio
    }
    narrow_rows = {
        r for r in range(1, params.cross - 1) if rng.random() < params.narrow_ratio
    }
    if not narrow_cols and params.aisles >= 3:
        narrow_cols = {1}  # 교행 불가가 하나도 없으면 시제의 목적을 잃는다

    # 2) 교차점
    junctions: list[tuple[int, int]] = [
        (c, r) for c in range(params.aisles) for r in range(params.cross)
    ]
    coords: dict[str, tuple[float, float]] = {}
    for col, row in junctions:
        coords[_junction_key(col, row)] = (_q(col * seg), _q(row * seg))

    # 3) 간선 — 세로 통로, 가로 교차통로
    edges: list[Edge] = []
    corridor_members: dict[str, list[tuple[str, str]]] = {}

    def width_of(is_narrow: bool) -> float:
        return cfg.aisle_width_narrow_m if is_narrow else cfg.aisle_width_wide_m

    for col in range(params.aisles):
        is_narrow = col in narrow_cols
        ckey = f"q_v{col:02d}" if is_narrow else ""
        for row in range(params.cross - 1):
            u = _junction_key(col, row)
            v = _junction_key(col, row + 1)
            w = width_of(is_narrow)
            edges.append(
                Edge(u, v, _q(seg), _q(w), cfg.capacity_rule_a(w), ckey, "aisle")
            )
            if ckey:
                corridor_members.setdefault(ckey, []).append((u, v))

    for row in range(params.cross):
        is_narrow = row in narrow_rows
        ckey = f"q_h{row:02d}" if is_narrow else ""
        for col in range(params.aisles - 1):
            u = _junction_key(col, row)
            v = _junction_key(col + 1, row)
            w = width_of(is_narrow)
            edges.append(
                Edge(u, v, _q(seg), _q(w), cfg.capacity_rule_a(w), ckey, "cross")
            )
            if ckey:
                corridor_members.setdefault(ckey, []).append((u, v))

    # 4) 엔드포인트 지선 — 교차점에서 대각으로 뻗어 격자 간선과 겹치지 않게 둔다
    n_ep = min(params.endpoints, len(junctions))
    chosen = sorted(rng.sample(sorted(junctions), n_ep))
    sides = {"NE": (1, 1), "NW": (-1, 1), "SE": (1, -1), "SW": (-1, -1)}
    side_names = sorted(sides)
    endpoints: list[str] = []
    diag = stub_len / math.sqrt(2.0)
    for col, row in chosen:
        side = side_names[rng.randrange(len(side_names))]
        dx, dy = sides[side]
        jk = _junction_key(col, row)
        pk = _stub_key(col, row, side)
        jx, jy = coords[jk]
        coords[pk] = (_q(jx + dx * diag), _q(jy + dy * diag))
        w = cfg.stub_width_m
        edges.append(
            Edge(jk, pk, _q(stub_len), _q(w), cfg.capacity_rule_a(w), "", "stub")
        )
        endpoints.append(pk)

    # 5) 여유 반경 — 이 시제에서는 「인접 간선 폭의 절반」이라는 기하 대용값이다.
    #    본 생성기는 정적 장애물 거리변환의 노드 표본이어야 한다(11b §5-3). 한계로 보고한다.
    widest: dict[str, float] = {}
    for e in edges:
        widest[e.u] = max(widest.get(e.u, 0.0), e.width_m)
        widest[e.v] = max(widest.get(e.v, 0.0), e.width_m)

    nodes = [
        Node(key, coords[key][0], coords[key][1], _q(widest[key] / 2.0))
        for key in sorted(coords)
    ]

    corridors = [
        Corridor(
            key=ckey,
            edge_keys=tuple(sorted(members)),
            length_m=_q(sum(e.length_m for e in edges if e.corridor_key == ckey)),
        )
        for ckey, members in sorted(corridor_members.items())
    ]

    edges.sort(key=lambda e: (e.kind, e.u, e.v))
    instance_id = (
        f"proto_w{params.aisles}x{params.cross}"
        f"_ep{n_ep}_s{params.seed}"
    )
    return RoadmapModel(
        instance_id=instance_id,
        nodes=nodes,
        edges=edges,
        corridors=corridors,
        endpoints=sorted(endpoints),
        min_separation_m=cfg.min_separation_m,
        wall_inflation_m=cfg.wall_inflation_m,
    )


# ---------------------------------------------------------------- 내부 자체 점검


def validate_model(model: RoadmapModel) -> list[str]:
    """내부 모델의 구조 불변식을 점검하고 위반 문자열 목록을 돌려준다.

    확장 키워드(x-ref 참조 무결성 · x-unique-key 유일성)가 요구하는 것을
    **생성 측에서 미리** 본다. 이것은 층 1 검증기의 대체물이 아니다.
    """
    problems: list[str] = []
    keys = [n.key for n in model.nodes]
    if len(set(keys)) != len(keys):
        problems.append("node id 중복")
    node_keys = set(keys)

    for n in model.nodes:
        if not n.clearance_m > 0.0:
            problems.append(f"clearance_m <= 0: {n.key}")

    corridor_keys = {c.key for c in model.corridors}
    if len(corridor_keys) != len(model.corridors):
        problems.append("corridor id 중복")
    if "" in corridor_keys:
        problems.append("corridor id가 빈 문자열이다 (센티널과 충돌)")

    pairs = set()
    for e in model.edges:
        if e.u not in node_keys or e.v not in node_keys:
            problems.append(f"간선 끝점이 nodes에 없다: {e.u}->{e.v}")
        if e.u == e.v:
            problems.append(f"자기 순환 간선: {e.u}")
        if (e.u, e.v) in pairs or (e.v, e.u) in pairs:
            problems.append(f"중복 간선: {e.u}->{e.v}")
        pairs.add((e.u, e.v))
        if not e.length_m > 0.0 or not e.width_m > 0.0:
            problems.append(f"length_m/width_m가 0 이하: {e.u}->{e.v}")
        if e.capacity_robots < 1:
            problems.append(f"capacity_robots < 1: {e.u}->{e.v}")
        if e.corridor_key and e.corridor_key not in corridor_keys:
            problems.append(f"간선이 없는 통로를 참조: {e.corridor_key}")
        if e.length_m < model.min_separation_m:
            problems.append(
                f"간선 길이 < min_separation_m: {e.u}->{e.v} "
                f"({e.length_m} < {model.min_separation_m})"
            )

    for c in model.corridors:
        for pair in c.edge_keys:
            if pair not in pairs:
                problems.append(f"통로 {c.key}가 없는 간선을 참조: {pair}")
        if not c.length_m > 0.0:
            problems.append(f"통로 length_m <= 0: {c.key}")

    if len(set(model.endpoints)) != len(model.endpoints):
        problems.append("endpoints 중복")
    for ep in model.endpoints:
        if ep not in node_keys:
            problems.append(f"endpoint가 nodes에 없다: {ep}")

    comps = _components(model)
    if len(comps) > 1:
        problems.append(f"그래프가 {len(comps)}조각으로 끊겼다")
    if model.endpoints:
        seen = {frozenset(c) for c in comps if set(model.endpoints) & c}
        if len(seen) > 1:
            problems.append("엔드포인트가 하나의 연결 성분에 있지 않다 (W1 위반)")
    return problems


def _components(model: RoadmapModel) -> list[set[str]]:
    adj = adjacency(model)
    unseen = {n.key for n in model.nodes}
    out: list[set[str]] = []
    while unseen:
        root = min(unseen)
        stack, comp = [root], set()
        while stack:
            cur = stack.pop()
            if cur in comp:
                continue
            comp.add(cur)
            unseen.discard(cur)
            stack.extend(v for v, _ in adj.get(cur, []) if v not in comp)
        out.append(comp)
    return out


# ---------------------------------------------------------------- 그래프 조회


def adjacency(model: RoadmapModel) -> dict[str, list[tuple[str, float]]]:
    """인접 리스트(무방향). 이 시제는 bidirectional 간선만 생성한다.

    forward_only/backward_only를 생성하게 되면 이 함수가 방향을 봐야 한다 —
    그때까지 방향 분기를 미리 만들지 않는다.
    """
    adj: dict[str, list[tuple[str, float]]] = {n.key: [] for n in model.nodes}
    for e in model.edges:
        # 끝점이 nodes에 없어도 죽지 않는다 — 그 위반은 validate_model이 보고한다.
        adj.setdefault(e.u, []).append((e.v, e.length_m))
        adj.setdefault(e.v, []).append((e.u, e.length_m))
    for key in adj:
        adj[key].sort()
    return adj


def shortest_path_len(model: RoadmapModel, src: str, dst: str) -> float:
    """간선 길이 합 최소. 도달 불가면 math.inf."""
    if src == dst:
        return 0.0
    adj = adjacency(model)
    dist = {src: 0.0}
    pq = [(0.0, src)]
    while pq:
        d, cur = heapq.heappop(pq)
        if cur == dst:
            return d
        if d > dist.get(cur, math.inf):
            continue
        for nxt, w in adj.get(cur, []):
            nd = d + w
            if nd < dist.get(nxt, math.inf):
                dist[nxt] = nd
                heapq.heappush(pq, (nd, nxt))
    return math.inf


def edge_length_stats(model: RoadmapModel) -> dict[str, float]:
    lengths = sorted(e.length_m for e in model.edges)
    n = len(lengths)
    median = 0.0 if n == 0 else (
        lengths[n // 2] if n % 2 else (lengths[n // 2 - 1] + lengths[n // 2]) / 2.0
    )
    return {
        "count": float(n),
        "min_m": lengths[0] if n else 0.0,
        "median_m": median,
        "max_m": lengths[-1] if n else 0.0,
    }
