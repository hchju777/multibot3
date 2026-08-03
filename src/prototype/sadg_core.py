"""S3 — 실행 의존 그래프(SADG) 알고리즘 코어 (탐색용 시제).

🔴 **이 파일은 경계 스키마를 모른다.** `roadmap_core`·`assignment_core`·`plan_core`와 같은
규율이다 — 경계 JSON 키(`schema`·`segments`·`dependencies`·`switch_groups`·`alternatives`·
`commit_seq`)와 경계의 닫힌 어휘(`"sequential"`·`"ordering"`)가 이 파일에 **문자열로 나오지
않는다.** 변환은 `boundary_adapter.py`에만 산다.

여기서 만드는 것 넷:

1. **세그먼트** — 로봇별 `steps`의 연속 쌍 하나가 세그먼트 하나. 개수 = 단계 수 − 1.
2. **로봇 내부 순차 의존**(Type-1) — 같은 로봇의 연속 세그먼트 사이. 스위칭 대상이 아니다.
3. **로봇 간 통행 순서 의존**(Type-2) — **근거는 `visit_order`뿐이다.**
4. **스위치 그룹** — 공유 위치마다 하나. 대안 생성 규칙은 **인자로 갈아 끼운다**(확장점 E3).

대안 생성 규칙 두 가지(`11c_req_sadg_r3.md` §T-1-2 · `12c_arch_sadg_p2.md` §A-5-2):

- `lift` — **선두 올리기. 정본이다.** `A_g = { lift_i(pi_v) : i in FirstOcc(pi_v) }`.
- `swap` — 인접 쌍 교환. **구판이며 지우지 않는다** — 두 생성기의 도달 가능 순서 집합이
  포함 관계가 아니고, N6-R 경로로 되돌릴 때 필요하기 때문이다.

🔴 **여기서 나오는 어떤 수치도 증거가 아니다.**
"""

from __future__ import annotations

import random
from dataclasses import dataclass, field
from itertools import product

from plan_core import PlanResult, fold_timeline

# ---------------------------------------------------------------------------
# 내부 어휘. 경계의 닫힌 어휘로 옮기는 것은 어댑터의 몫이다.
# ---------------------------------------------------------------------------
INTRA_ROBOT = "intra_robot"  # 로봇 내부 순차 (Type-1)
INTER_ROBOT = "inter_robot"  # 로봇 간 통행 순서 (Type-2)

Item = tuple[str, int]  # 통행 순서의 한 항목 = (로봇, 그 로봇의 접은 뒤 순번)
SegKey = tuple[str, int]  # 세그먼트 식별 = (로봇, 세그먼트 번호). 문자열 인코딩은 경계의 것이다


# ---------------------------------------------------------------------------
# 자료구조
# ---------------------------------------------------------------------------


@dataclass(frozen=True)
class Segment:
    """한 세그먼트 = 간선 하나의 이동. `number`는 출발 단계의 순번과 같다."""

    robot: str
    number: int
    src_node: str
    dst_node: str

    @property
    def key(self) -> SegKey:
        return (self.robot, self.number)


@dataclass(frozen=True)
class Precedence:
    """의존 하나. `kind`는 **유래**로 정한다(INTRA_ROBOT / INTER_ROBOT)."""

    src: SegKey
    dst: SegKey
    kind: str


@dataclass(frozen=True)
class Alternative:
    """한 위치의 통행 순서 대안 하나. `order`가 «내용»이고 `edges`가 그 내용의 의존이다."""

    order: tuple[Item, ...]
    edges: tuple[Precedence, ...]
    is_original: bool
    source_index: int  # lift: 선두로 올린 위치 / swap: 교환한 앞쪽 위치. 내부 진단용


@dataclass
class SwitchGroup:
    location: str
    original: tuple[Item, ...]
    alternatives: list[Alternative]


@dataclass
class CompileStats:
    """컴파일 부산물. 🔴 **경계로 나가지 않는다** — 전부 내부 회계다."""

    alt_generator: str = "lift"
    shared_locations: int = 0
    groups_emitted: int = 0
    groups_folded_single: int = 0  # |A_g| = 1 이라 고정 의존으로 접힌 위치 수
    dropped_unrepresentable: int = 0  # 표현 불가로 버린 대안 수
    pinned_start_items: int = 0  # 순번 0 항목(로봇이 그 정점에서 출발) 수
    pinned_terminal_items: int = 0  # 마지막 단계 항목(그 정점을 영구 점유) 수
    same_robot_ordering_edges: int = 0  # 🔴 언제나 0이어야 한다 (§55-7-1 (다))
    critical_path_segments: int = 0  # 공칭 그래프의 최장경로 위 세그먼트 수
    total_combinations: int = 1


@dataclass
class ExecGraph:
    instance_id: str
    revision: int  # 판본 번호. 경계에서 무엇으로 불리는지는 어댑터가 안다
    segments: list[Segment]
    fixed: list[Precedence]
    groups: list[SwitchGroup]
    stats: CompileStats = field(default_factory=CompileStats)

    def segment_keys(self) -> set[SegKey]:
        return {s.key for s in self.segments}


@dataclass
class PlanView:
    """알고리즘 코어가 보는 이산 계획. **경계 문서가 아니다.**"""

    instance_id: str
    revision: int
    steps: dict[str, list[str]]  # 로봇 -> 접은 뒤 정점 열
    visit_order: list[tuple[str, list[Item]]]


def plan_view_of(result: PlanResult) -> PlanView:
    """내부 `PlanResult`를 계획 뷰로 좁힌다. 목표·우선순위 같은 상류 상태는 버린다."""
    steps: dict[str, list[str]] = {}
    for path in result.paths:
        folded, _ = fold_timeline(path.timeline)
        steps[path.robot] = folded
    return PlanView(
        instance_id=result.instance_id,
        revision=int(result.plan_revision),
        steps=steps,
        visit_order=[(loc, [(r, i) for r, i in seq]) for loc, seq in result.visit_order],
    )


# ---------------------------------------------------------------------------
# 세그먼트와 로봇 내부 의존
# ---------------------------------------------------------------------------


def build_segments(view: PlanView) -> list[Segment]:
    """로봇별 단계 열에서 세그먼트를 만든다. **개수 = 단계 수 − 1**(계약 §1-2(6))."""
    out: list[Segment] = []
    for robot in sorted(view.steps):
        nodes = view.steps[robot]
        for i in range(len(nodes) - 1):
            out.append(Segment(robot, i, nodes[i], nodes[i + 1]))
    return out


def build_intra_deps(view: PlanView) -> list[Precedence]:
    """같은 로봇의 연속 세그먼트를 잇는다. 이것이 뒤집히면 상류 계획과의 대응이 깨진다."""
    out: list[Precedence] = []
    for robot in sorted(view.steps):
        for i in range(len(view.steps[robot]) - 2):
            out.append(Precedence((robot, i), (robot, i + 1), INTRA_ROBOT))
    return out


def departing_segment(view: PlanView, item: Item) -> SegKey | None:
    """항목이 가리키는 정점을 **떠나는** 세그먼트. 마지막 단계면 존재하지 않는다.

    계약 §1-2(7)이 *"어느 terminal 값이든 마지막 정점은 해제되지 않는다"*이므로, 마지막
    단계 항목은 그 정점을 영원히 붙잡는다 — 떠나는 세그먼트가 **원리적으로** 없다.
    """
    robot, index = item
    return (robot, index) if index < len(view.steps[robot]) - 1 else None


def arriving_segment(view: PlanView, item: Item) -> SegKey | None:
    """항목이 가리키는 정점에 **들어오는** 세그먼트. 순번 0(출발 정점)이면 존재하지 않는다."""
    robot, index = item
    return (robot, index - 1) if index >= 1 else None


def ordering_edges(view: PlanView, order: tuple[Item, ...]) -> tuple[Precedence, ...] | None:
    """통행 순서 하나를 의존 열로 옮긴다. **표현 불가면 None.**

    한 쌍 `(a, b)`의 뜻은 *"a가 그 정점을 떠난 뒤에야 b가 들어온다"*이므로 필요한 것은
    **a의 떠나는 세그먼트**와 **b의 들어오는 세그먼트**다. 둘 중 하나가 없으면 그 순서는
    이 계약 위에서 표현할 수 없다(§2-2(5): 릴리스 조건은 그래프가 함축한다).

    🔴 **같은 로봇 쌍은 간선을 만들지 않는다**(`55_contract_holes.md` §55-7-1 읽기 (다)).
    계약 §2-1의 술어가 *"`ordering` ⟺ 두 세그먼트의 `robot`이 다르다"*이므로 같은 로봇에
    `ordering`을 붙이면 정의 위반이고, `sequential`을 붙이면 Type-1과 완전 중복이다.
    **아무것도 넣지 않아도 도달 관계가 바뀌지 않는다** — 같은 로봇 $i$가 순번 $p<q$에서 $v$를
    지날 때 필요한 간선은 `i#p -> i#(q-1)`인데 Type-1이 `i#p -> ... -> i#(q-1)`을 이미 준다.
    정리 H1이 모든 대안에서 로봇 내부 항목 순서를 보존하므로 $p<q$는 어느 대안에서도 유지된다.

    같은 로봇 쌍은 **표현 가능성 판정에서도 건너뛴다.** 건너뛰어도 판정이 바뀌지 않는다 —
    `a`가 종점 항목이면 같은 로봇의 더 뒤 방문 `b`가 존재할 수 없고, `b`가 출발 항목이면
    같은 로봇의 더 앞 방문 `a`가 존재할 수 없기 때문이다. [유도]
    """
    edges: list[Precedence] = []
    for a, b in zip(order, order[1:]):
        if a[0] == b[0]:
            continue
        src = departing_segment(view, a)
        dst = arriving_segment(view, b)
        if src is None or dst is None:
            return None
        edges.append(Precedence(src, dst, INTER_ROBOT))
    return tuple(edges)


# ---------------------------------------------------------------------------
# 대안 생성기 — 확장점 E3. 인자 한 줄로 갈린다
# ---------------------------------------------------------------------------


def first_occurrence_indices(items: tuple[Item, ...]) -> list[int]:
    """각 로봇의 **첫 출현 항목** 인덱스 집합 FirstOcc(pi_v). `11c_r3` §T-1-2."""
    seen: set[str] = set()
    out: list[int] = []
    for i, (robot, _) in enumerate(items):
        if robot not in seen:
            seen.add(robot)
            out.append(i)
    return out


def gen_head_lift(items: tuple[Item, ...]) -> list[tuple[int, tuple[Item, ...]]]:
    """**정본.** 첫 출현 항목을 하나씩 선두로 올린다. `lift_0`가 원 순서다.

    `lift_i(pi_v) = (x_i, x_0, ..., x_{i-1}, x_{i+1}, ..., x_k)` — 나머지 상대 순서를 유지한다.
    """
    out: list[tuple[int, tuple[Item, ...]]] = []
    for i in first_occurrence_indices(items):
        out.append((i, (items[i],) + items[:i] + items[i + 1 :]))
    return out


def gen_adjacent_swap(items: tuple[Item, ...]) -> list[tuple[int, tuple[Item, ...]]]:
    """**구판.** 원 순서 + 로봇이 다른 인접 쌍의 교환. `11c_r2` §R2-1의 같은 로봇 쌍 필터 포함."""
    out: list[tuple[int, tuple[Item, ...]]] = [(-1, tuple(items))]
    for j in range(len(items) - 1):
        if items[j][0] == items[j + 1][0]:
            continue  # 같은 로봇 쌍은 뒤집지 않는다 — Type-1 고정과 정면 충돌한다
        swapped = list(items)
        swapped[j], swapped[j + 1] = swapped[j + 1], swapped[j]
        out.append((j, tuple(swapped)))
    return out


ALT_GENERATORS = {"lift": gen_head_lift, "swap": gen_adjacent_swap}


def preserves_intra_robot_order(original: tuple[Item, ...], order: tuple[Item, ...]) -> bool:
    """불변식 I-9 / 정리 H1 — 모든 로봇의 항목 상대 순서가 보존되는가."""
    for robot in {r for r, _ in original}:
        before = [it for it in original if it[0] == robot]
        after = [it for it in order if it[0] == robot]
        if before != after:
            return False
    return True


# ---------------------------------------------------------------------------
# 컴파일
# ---------------------------------------------------------------------------


def compile_graph(view: PlanView, alt_generator: str = "lift", revision: int = 0) -> ExecGraph:
    """계획 뷰 -> 실행 의존 그래프.

    `|A_g| = 1`이면 **그룹을 발행하지 않고** 원 순서를 고정 의존으로 접는다
    (`11c_r2` §R2-2 · `12c_arch_sadg.md` `ConstraintCompiler` 행).
    """
    if alt_generator not in ALT_GENERATORS:
        raise ValueError(f"알 수 없는 대안 생성기: {alt_generator}")
    generate = ALT_GENERATORS[alt_generator]

    segments = build_segments(view)
    fixed = build_intra_deps(view)
    stats = CompileStats(alt_generator=alt_generator, shared_locations=len(view.visit_order))
    groups: list[SwitchGroup] = []

    for location, raw_items in view.visit_order:
        items = tuple(raw_items)
        for it in items:
            if it[1] == 0:
                stats.pinned_start_items += 1
            if departing_segment(view, it) is None:
                stats.pinned_terminal_items += 1

        seen_orders: set[tuple[Item, ...]] = set()
        alternatives: list[Alternative] = []
        for source_index, order in generate(items):
            if order in seen_orders:
                continue
            edges = ordering_edges(view, order)
            if edges is None:
                stats.dropped_unrepresentable += 1
                continue
            seen_orders.add(order)
            alternatives.append(
                Alternative(order, edges, order == items, source_index)
            )

        if not alternatives:
            # 원 순서조차 표현 불가 — 상류 계획과 계약이 어긋난 것이다. 조용히 넘기지 않는다.
            raise ValueError(
                f"위치 {location}: 원 통행 순서를 의존으로 표현할 수 없다 (항목 {items})"
            )
        if len(alternatives) == 1:
            fixed.extend(alternatives[0].edges)
            stats.groups_folded_single += 1
            continue
        groups.append(SwitchGroup(location, items, alternatives))

    stats.groups_emitted = len(groups)
    total = 1
    for g in groups:
        total *= len(g.alternatives)
    stats.total_combinations = total
    graph = ExecGraph(view.instance_id, revision, segments, fixed, groups, stats)
    stats.same_robot_ordering_edges = _count_duplicate_direction_edges(graph)
    graph.stats.critical_path_segments = critical_path_segments(graph)
    return graph


# ---------------------------------------------------------------------------
# 그래프 술어
# ---------------------------------------------------------------------------


def _count_duplicate_direction_edges(graph: ExecGraph) -> int:
    """같은 로봇을 잇는 `INTER_ROBOT` 간선 수. 🔴 **(다) 아래에서 언제나 0**이며 `self_check`가 센다."""
    count = sum(
        1 for e in graph.fixed if e.kind == INTER_ROBOT and e.src[0] == e.dst[0]
    )
    for g in graph.groups:
        for alt in g.alternatives:
            count += sum(1 for e in alt.edges if e.src[0] == e.dst[0])
    return count


def nominal_edges(graph: ExecGraph) -> list[Precedence]:
    """공칭 그래프 = 고정 의존 + 각 그룹의 **원 순서** 대안. 상류 계획이 고른 순서다."""
    edges = list(graph.fixed)
    for g in graph.groups:
        for alt in g.alternatives:
            if alt.is_original:
                edges.extend(alt.edges)
                break
    return edges


def _kahn(nodes: list[SegKey], edges) -> tuple[bool, list[SegKey]]:
    """Kahn 위상 정렬. (비순환인가, 위상 순서)."""
    index = {n: i for i, n in enumerate(nodes)}
    adjacency: list[list[int]] = [[] for _ in nodes]
    indegree = [0] * len(nodes)
    for e in edges:
        u, v = index[e.src], index[e.dst]
        adjacency[u].append(v)
        indegree[v] += 1
    queue = [i for i in range(len(nodes)) if indegree[i] == 0]
    order: list[int] = []
    while queue:
        u = queue.pop()
        order.append(u)
        for v in adjacency[u]:
            indegree[v] -= 1
            if indegree[v] == 0:
                queue.append(v)
    return len(order) == len(nodes), [nodes[i] for i in order]


def is_acyclic(nodes: list[SegKey], edges) -> bool:
    """중복 간선을 그대로 견딘다 — 같은 방향의 간선이 둘이어도 진입 차수 회계가 맞다."""
    return _kahn(nodes, edges)[0]


def critical_path_segments(graph: ExecGraph) -> int:
    """공칭 그래프의 **최장경로 위 세그먼트 수**(`NOTATION.md`의 lambda*).

    🔴 **경계가 아니다.** 스키마의 금지 목록에 올라 있으며 `result.json`의 대상이다.
    위상 순회 **1회**로 낸다(`12c` `ConstraintCompiler` 행).
    """
    nodes = [s.key for s in graph.segments]
    if not nodes:
        return 0
    edges = nominal_edges(graph)
    ok, order = _kahn(nodes, edges)
    if not ok:
        return -1  # 순환이면 최장경로가 정의되지 않는다. 값을 지어내지 않는다
    preds: dict[SegKey, list[SegKey]] = {n: [] for n in nodes}
    for e in edges:
        preds[e.dst].append(e.src)
    depth: dict[SegKey, int] = {}
    for node in order:
        depth[node] = 1 + max((depth[p] for p in preds[node]), default=0)
    return max(depth.values())


@dataclass
class AcyclicReport:
    """🔴 «표본 통과»와 «전수 통과»를 한 칸에 넣지 않는다."""

    checked: int
    total: int
    exhaustive: bool
    failing_combo: tuple[int, ...] | None = None
    failing_labels: list[str] = field(default_factory=list)

    @property
    def ok(self) -> bool:
        return self.failing_combo is None


def check_all_switch_combinations(
    graph: ExecGraph,
    max_groups: int = 20,
    max_combinations: int = 1 << 16,
    samples: int = 2000,
    seed: int = 0,
) -> AcyclicReport:
    """**임의의 스위칭 조합에서 비순환인가** — 이 프로젝트의 핵심 보장.

    조합 수 `prod_g |A_g|`가 상한 안이면 **전수**, 넘으면 **표본으로 강등**하고
    `exhaustive=False`를 돌려준다. 표본 통과는 증명이 아니며 호출자가 그것을 표기해야 한다
    (`11c` §4-4의 `c5_mode=sampled` — 그 run은 교착 자유 증거에서 제외된다).
    """
    nodes = [s.key for s in graph.segments]
    base = list(graph.fixed)
    per_group = [[alt.edges for alt in g.alternatives] for g in graph.groups]
    counts = [len(alts) for alts in per_group]
    total = 1
    for c in counts:
        total *= c

    def _labels(combo):
        return [
            f"{g.location}#{idx}:{'->'.join(r for r, _ in g.alternatives[idx].order)}"
            for g, idx in zip(graph.groups, combo)
        ]

    if not is_acyclic(nodes, base):
        return AcyclicReport(0, total, False, (), ["고정 의존만으로 이미 순환한다"])

    exhaustive = len(graph.groups) <= max_groups and total <= max_combinations
    checked = 0
    if exhaustive:
        iterator = product(*[range(c) for c in counts]) if counts else iter([()])
        for combo in iterator:
            checked += 1
            edges = list(base)
            for g, idx in enumerate(combo):
                edges.extend(per_group[g][idx])
            if not is_acyclic(nodes, edges):
                return AcyclicReport(checked, total, True, combo, _labels(combo))
        return AcyclicReport(checked, total, True)

    rng = random.Random(seed)
    seen: set[tuple[int, ...]] = set()
    attempts = 0
    while len(seen) < samples and attempts < samples * 4 + 16:
        attempts += 1
        combo = tuple(rng.randrange(c) for c in counts)
        if combo in seen:
            continue
        seen.add(combo)
        checked += 1
        edges = list(base)
        for g, idx in enumerate(combo):
            edges.extend(per_group[g][idx])
        if not is_acyclic(nodes, edges):
            return AcyclicReport(checked, total, False, combo, _labels(combo))
    return AcyclicReport(checked, total, False)


# ---------------------------------------------------------------------------
# 자체 점검 — 🔴 **생성자의 자체 점검은 판정이 아니다**
# ---------------------------------------------------------------------------


def self_check(
    graph: ExecGraph, view: PlanView, adjacency: set[tuple[str, str]] | None = None
) -> list[str]:
    """산출 그래프를 **다시** 검사한다. 통과 판정은 `boundary-verifier`의 몫이다."""
    problems: list[str] = []
    keys = graph.segment_keys()

    for robot, nodes in view.steps.items():
        got = sum(1 for s in graph.segments if s.robot == robot)
        if got != len(nodes) - 1:
            problems.append(f"{robot}: 세그먼트 {got}개 != 단계 {len(nodes)}개 − 1 (계약 §1-2(6))")
    if len(keys) != len(graph.segments):
        problems.append("세그먼트 식별자가 중복된다")

    if adjacency is not None:
        for s in graph.segments:
            if (s.src_node, s.dst_node) not in adjacency:
                problems.append(f"세그먼트 {s.key}가 로드맵에 없는 간선을 쓴다: {s.src_node}->{s.dst_node}")

    def _check_edges(edges, where: str) -> None:
        for e in edges:
            if e.src not in keys:
                problems.append(f"{where}: 출발 세그먼트가 선언되지 않았다: {e.src}")
            if e.dst not in keys:
                problems.append(f"{where}: 도착 세그먼트가 선언되지 않았다: {e.dst}")

    _check_edges(graph.fixed, "고정 의존")

    # 🔴 계약 §2-1의 검사 가능한 술어 (55_contract_holes.md §55-6 S7):
    #   type == sequential  <=>  from·to 세그먼트의 robot이 같다
    #   type == ordering    <=>  다르다
    def _check_type_predicate(edges, where: str) -> None:
        for e in edges:
            same = e.src[0] == e.dst[0]
            if e.kind == INTRA_ROBOT and not same:
                problems.append(f"{where}: 로봇 내부 의존이 서로 다른 로봇을 잇는다: {e.src} -> {e.dst}")
            if e.kind == INTER_ROBOT and same:
                problems.append(f"{where}: 로봇 간 의존이 같은 로봇 안에 있다: {e.src} -> {e.dst} (§2-1 술어 위반)")

    _check_type_predicate(graph.fixed, "고정 의존")
    locations = {loc for loc, _ in view.visit_order}
    for g in graph.groups:
        if g.location not in locations:
            problems.append(f"그룹 {g.location}의 위치가 상류 통행 순서에 없다")
        if len(g.alternatives) < 2:
            problems.append(f"그룹 {g.location}의 대안이 {len(g.alternatives)}개다 — 발행하면 안 된다")
        if not any(a.is_original for a in g.alternatives):
            problems.append(f"그룹 {g.location}: 원 순서가 대안 집합에 없다 (C4 충족 불가)")
        if len({a.order for a in g.alternatives}) != len(g.alternatives):
            problems.append(f"그룹 {g.location}: 대안이 중복된다 (상호 배타 위반)")
        members = {r for r, _ in g.original}
        for alt in g.alternatives:
            _check_edges(alt.edges, f"대안 {g.location}/{alt.source_index}")
            _check_type_predicate(alt.edges, f"대안 {g.location}/{alt.source_index}")
            if sorted(alt.order) != sorted(g.original):
                problems.append(f"그룹 {g.location}: 대안이 원 항목 집합과 다르다")
            if {r for r, _ in alt.order} != members:
                problems.append(f"그룹 {g.location}: 대안의 구성원 로봇 집합이 다르다")
            if not preserves_intra_robot_order(g.original, alt.order):
                problems.append(f"그룹 {g.location}: 불변식 I-9 위반 (로봇 내부 순서가 뒤집혔다)")
    return problems


def prune_cyclic_alternatives(
    graph: ExecGraph, max_groups: int = 20, max_combinations: int = 1 << 16
) -> tuple[int, list[str]]:
    """🔴 **A4를 「가정」에서 「구성」으로 내린다** — 순환을 만드는 대안을 지운다.

    `11c` §4-1의 A4는 *"초기 그래프가 도달 가능한 모든 z에서 비순환"*이라는 **인스턴스 조건**이며,
    `11c_r3` §T-1-4 B4가 *"컴파일 시 전수 검사가 그대로 판정한다"*까지만 적었다.
    **판정이 「거짓」일 때 무엇을 하는지는 요구사항 문서에 없다.** 이 함수는 내가 정한 처리이며
    (원 순서는 절대 지우지 않고, 순환 조합에 참여한 비원본 대안을 하나씩 지운다) 그 사실을
    산출물에 적는다. 지운 수만큼 **표현력이 줄고 대안 집합이 생성기의 정의와 달라진다** —
    정리 H3(Heads 완비)이 이 지점에서 깨질 수 있다.

    돌려주는 것: (지운 대안 수, 지운 내역).
    """
    removed = 0
    log: list[str] = []
    for _ in range(1024):
        report = check_all_switch_combinations(
            graph, max_groups=max_groups, max_combinations=max_combinations
        )
        if report.ok:
            break
        note = "전수 검사 불가 — 표본에서 걸린 조합만 지운다 (완전성 없음)"
        if not report.exhaustive and note not in log:
            log.append(note)
        if report.failing_combo == ():
            log.append("고정 의존만으로 순환한다 — 지울 대안이 없다")
            break
        victim = None
        for gi, ai in enumerate(report.failing_combo):
            if not graph.groups[gi].alternatives[ai].is_original:
                victim = (gi, ai)
                break
        if victim is None:
            log.append("순환 조합이 원 순서로만 이뤄졌다 — 지울 수 없다")
            break
        gi, ai = victim
        group = graph.groups[gi]
        order = "->".join(f"{r}@{i}" for r, i in group.alternatives[ai].order)
        log.append(f"{group.location}: 대안 {order} 제거")
        del group.alternatives[ai]
        removed += 1
        if len(group.alternatives) == 1:
            # 선택할 것이 없는 그룹은 발행하지 않는다 — |A_g|=1 규칙과 같은 처리다
            graph.fixed.extend(group.alternatives[0].edges)
            graph.stats.groups_folded_single += 1
            graph.groups.remove(group)
    graph.stats.groups_emitted = len(graph.groups)
    total = 1
    for g in graph.groups:
        total *= len(g.alternatives)
    graph.stats.total_combinations = total
    graph.stats.same_robot_ordering_edges = _count_duplicate_direction_edges(graph)
    graph.stats.critical_path_segments = critical_path_segments(graph)
    return removed, log


def derive_head_robot(edges) -> str | None:
    """🔴 **대안의 「내용」(의존 집합)만으로 선두 로봇을 유도한다.** 배열 순서를 쓰지 않는다.

    `11c_r3` 정리 H4가 *"선두 로봇 하나가 A_g의 대안 하나를 유일하게 지정한다"*라고 했고
    `11c_r2` §R1-3이 *"저수준이 실현된 대안을 국소 유도한다"*를 신선도의 **주 경로**로 삼았다.
    그러려면 **경계에 실린 것만으로** 「이 대안의 선두는 누구인가」가 나와야 한다.

    유도 방법: 간선 `(dep(x_i) -> arr(x_{i+1}))`에서 항목 `(로봇, 순번)`을 복원하고,
    **로봇 간 간선 + 같은 로봇 항목의 순번 순서**를 선행 관계로 삼아 **선행자가 없는 항목**을
    찾는다. 같은 로봇 쌍의 간선을 싣지 않아도(§55-7-1 (다)) 유도가 성립한다 —
    같은 로봇 항목의 순서는 순번이 주기 때문이다. [유도]

    돌려주는 것: 선두 로봇. 유일하지 않거나 간선이 없으면 **None**(지어내지 않는다).
    """
    items: set[Item] = set()
    cross: list[tuple[Item, Item]] = []
    for e in edges:
        before = (e.src[0], e.src[1])
        after = (e.dst[0], e.dst[1] + 1)
        items.add(before)
        items.add(after)
        cross.append((before, after))
    if not items:
        return None
    has_pred = {it: False for it in items}
    for _, after in cross:
        has_pred[after] = True
    for robot in {r for r, _ in items}:
        own = sorted((i for i in items if i[0] == robot), key=lambda i: i[1])
        for later in own[1:]:
            has_pred[later] = True
    heads = [it for it, flag in has_pred.items() if not flag]
    if len(heads) != 1:
        return None
    return heads[0][0]


def heads_of(group: SwitchGroup) -> set[str]:
    """Heads(A_g) — 대안들의 선두 로봇 집합. 🔴 **경계로 나가지 않는다**(내부 진단용)."""
    return {alt.order[0][0] for alt in group.alternatives}


def inject_cycle(graph: ExecGraph) -> str:
    """🔴 **일부러 순환을 만든다.** 음성 시험 전용이며 정상 경로에서 부르지 않는다.

    첫 그룹의 마지막 대안에 **역방향 간선 하나**를 더한다. 그 대안이 선택되는 모든 조합에서
    2-순환이 생기므로, 검사기가 «어떤 조합에서든 비순환»을 실제로 보는지가 드러난다.
    잡지 못하는 검사기는 통과를 뜻하지 않는다.
    """
    for g in graph.groups:
        alt = g.alternatives[-1]
        if not alt.edges:
            continue
        e = alt.edges[0]
        g.alternatives[-1] = Alternative(
            alt.order,
            alt.edges + (Precedence(e.dst, e.src, INTER_ROBOT),),
            alt.is_original,
            alt.source_index,
        )
        return f"{g.location} 대안 {len(g.alternatives) - 1}에 역방향 간선 {e.dst}->{e.src} 주입"
    raise ValueError("주입할 스위치 그룹이 없다")
