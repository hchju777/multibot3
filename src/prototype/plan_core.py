"""S2 — 우선순위 기반 시공간 A* 경로 계획 코어. **경계 스키마를 모른다.**

내부 타임스텝 sigma는 여기에만 있다. 경계(`mrs.discrete_plan`)로 나가는 것은 **순서 인덱스**뿐이며
변환은 boundary_adapter.py가 한다(계약 §1-2(1): *"index는 타임스텝이 아니다"*).

충돌 모델은 `11b_req_mapf.md` §4(D8) 그대로다 — 정점·간선(교차)·following·회전 **전부 금지**.
로드맵이 주는 정적 제약도 지킨다 — `traversal` 방향 · 간선 용량 · 교행 불가 통로 묶음.

🔴 우선순위 기반은 **완전하지 않다.** 못 푸는 인스턴스가 나오는 것이 정상이며,
이 파일은 그 실패를 감추지 않고 사유와 함께 돌려준다.
"""

from __future__ import annotations

import heapq
import random
from collections import defaultdict, deque
from dataclasses import dataclass, field

from assignment_core import AssignmentModel
from roadmap_core import RoadmapModel

PRIORITY_RULES = ("most_goals", "fewest_goals", "robot_id", "longest_route", "random")
TERMINALS = ("goal_reached", "plan_truncated", "no_progress")


@dataclass(frozen=True)
class PlanParams:
    """계획 인자. **하나도 경계로 나가지 않는다**(전부 알고리즘 파라미터다)."""

    priority: str = "most_goals"
    horizon: int = 96  # 내부 타임스텝 상한
    seed: int = 0
    plan_revision: int = 0


@dataclass
class RobotPath:
    robot: str
    timeline: list[str]  # 타임스텝별 위치(대기 포함). timeline[0]이 시작 정점
    terminal: str
    goals_total: int
    goals_covered: int
    goal_at_step: dict[str, int] = field(default_factory=dict)  # goal_id -> 접은 뒤 순번


@dataclass
class PlanResult:
    instance_id: str
    plan_revision: int
    paths: list[RobotPath]
    visit_order: list[tuple[str, list[tuple[str, int]]]]
    priority_order: list[str]
    ok: bool
    failures: list[tuple[str, str]]  # (robot, 사유)
    unplanned: list[str] = field(default_factory=list)  # 계획 자체를 못 준 로봇

    @property
    def roster_complete(self) -> bool:
        """🔴 인스턴스 로봇 **전부**에게 계획을 주었는가.

        거짓이면 계약(§17-3-1(8) · x-set-equal)을 만족하는 아티팩트를 만들 수 없다 —
        **우선순위 기반이 이 인스턴스를 푼 것이 아니다.** 억지로 채우지 않는다.
        """
        return not self.unplanned

    def path_of(self, robot: str) -> RobotPath:
        return next(p for p in self.paths if p.robot == robot)


# ---------------------------------------------------------------- 로드맵 색인


class _Graph:
    """계획용 로드맵 색인. 방향·용량·통로를 여기서 한 번만 푼다."""

    def __init__(self, model: RoadmapModel):
        self.moves: dict[str, list[tuple[str, tuple[str, str]]]] = defaultdict(list)
        self.capacity: dict[tuple[str, str], int] = {}
        self.corridor: dict[tuple[str, str], str] = {}
        self.orient: dict[tuple[str, str], tuple[str, str]] = {}
        for e in model.edges:
            key = (e.u, e.v)
            self.capacity[key] = e.capacity_robots
            self.corridor[key] = e.corridor_key
            if e.traversal_forward:
                self.moves[e.u].append((e.v, key))
            if e.traversal_backward:
                self.moves[e.v].append((e.u, key))
        for node in self.moves:
            self.moves[node].sort()
        self.nodes = [n.key for n in model.nodes]

    def edge_key(self, u: str, v: str) -> tuple[str, str]:
        return (u, v) if (u, v) in self.capacity else (v, u)

    def hops(self, target: str) -> dict[str, int]:
        """target까지의 **간선 수** 거리(역방향 BFS). 길이를 순서 산출에 쓰지 않는다(11b §1)."""
        dist = {target: 0}
        queue = deque([target])
        while queue:
            cur = queue.popleft()
            for nxt, _ in self.moves[cur]:
                # 역방향 탐색: nxt -> cur 이동이 가능하므로 nxt의 거리를 잰다
                if nxt not in dist:
                    dist[nxt] = dist[cur] + 1
                    queue.append(nxt)
        return dist


# ---------------------------------------------------------------- 예약표


class _Reservations:
    """이미 계획된(= 우선순위가 높은) 로봇들이 차지한 시공간."""

    def __init__(self, graph: _Graph, horizon: int):
        self.graph = graph
        self.horizon = horizon
        self.vertex: dict[tuple[int, str], str] = {}
        self.edge: dict[tuple[int, tuple[str, str]], list[tuple[str, str, str]]] = defaultdict(list)
        self.corr_dir: dict[tuple[int, str], int] = {}
        self.parked: dict[str, tuple[str, int]] = {}

    def vertex_free(self, t: int, node: str) -> bool:
        if (t, node) in self.vertex:
            return False
        owner = self.parked.get(node)
        return not (owner and t >= owner[1])

    def can_wait(self, t: int, node: str) -> bool:
        return self.vertex_free(t + 1, node)

    def can_move(self, t: int, u: str, v: str) -> bool:
        # following·회전 금지: 목적 정점이 **지금도** 비어 있어야 한다 (11b D8)
        if not self.vertex_free(t, v) or not self.vertex_free(t + 1, v):
            return False
        key = self.graph.edge_key(u, v)
        users = self.edge.get((t, key), [])
        if any(uu == v and vv == u for _, uu, vv in users):  # swap
            return False
        if len(users) >= self.graph.capacity[key]:  # 간선 용량
            return False
        corridor = self.graph.corridor[key]
        if corridor:
            direction = 1 if (u, v) == key else -1
            fixed = self.corr_dir.get((t, corridor))
            if fixed is not None and fixed != direction:
                return False  # 교행 불가 통로: 같은 시각에 역방향 통행 금지
        return True

    def can_park(self, t: int, node: str) -> bool:
        if node in self.parked:
            return False
        return all(self.vertex_free(tt, node) for tt in range(t, self.horizon + 2))

    def commit(self, robot: str, timeline: list[str]) -> None:
        for t, node in enumerate(timeline):
            self.vertex[(t, node)] = robot
        for t in range(len(timeline) - 1):
            u, v = timeline[t], timeline[t + 1]
            if u == v:
                continue
            key = self.graph.edge_key(u, v)
            self.edge[(t, key)].append((robot, u, v))
            corridor = self.graph.corridor[key]
            if corridor:
                self.corr_dir[(t, corridor)] = 1 if (u, v) == key else -1
        self.parked[timeline[-1]] = (robot, len(timeline) - 1)


# ---------------------------------------------------------------- 저수준 탐색


def _search(
    graph: _Graph,
    res: _Reservations,
    start: str,
    goals: list[str],
    horizon: int,
) -> tuple[list[str] | None, int]:
    """한 로봇의 multi-label 시공간 A*. (타임라인, 덮은 목표 수)를 돌려준다.

    목표를 다 덮지 못하면 **가장 많이 덮고 안전하게 설 수 있는 상태**에서 자른다 —
    계약 §1-2(5)가 절단을 허용한다. 억지로 성공으로 만들지 않는다.
    """
    hops = [graph.hops(g) for g in goals]

    def heuristic(node: str, gi: int) -> int:
        total, cur = 0, node
        for k in range(gi, len(goals)):
            step = hops[k].get(cur)
            if step is None:
                return 10**6
            total += step
            cur = goals[k]
        return total

    start_gi = 0
    while start_gi < len(goals) and goals[start_gi] == start:
        start_gi += 1

    parent: dict[tuple[str, int, int], tuple[str, int, int] | None] = {(start, 0, start_gi): None}
    best: tuple[int, int, tuple[str, int, int]] | None = None
    counter = 0
    open_heap: list[tuple[int, int, int, tuple[str, int, int]]] = []
    heapq.heappush(open_heap, (heuristic(start, start_gi), 0, counter, (start, 0, start_gi)))
    seen: set[tuple[str, int, int]] = set()

    while open_heap:
        _, _, _, state = heapq.heappop(open_heap)
        if state in seen:
            continue
        seen.add(state)
        node, t, gi = state
        if res.can_park(t, node):
            score = (gi, -t)
            if best is None or score > (best[0], -best[1]):
                best = (gi, t, state)
            if gi == len(goals):
                break
        if t >= horizon:
            continue
        moves = [(node, True)] + [(nxt, False) for nxt, _ in graph.moves[node]]
        for nxt, is_wait in moves:
            ok = res.can_wait(t, node) if is_wait else res.can_move(t, node, nxt)
            if not ok:
                continue
            ngi = gi
            while ngi < len(goals) and goals[ngi] == nxt:
                ngi += 1
            nxt_state = (nxt, t + 1, ngi)
            if nxt_state in parent:
                continue
            parent[nxt_state] = state
            counter += 1
            heapq.heappush(
                open_heap,
                (t + 1 + heuristic(nxt, ngi), heuristic(nxt, ngi), counter, nxt_state),
            )

    if best is None:
        return None, 0
    timeline: list[str] = []
    cursor: tuple[str, int, int] | None = best[2]
    while cursor is not None:
        timeline.append(cursor[0])
        cursor = parent[cursor]
    timeline.reverse()
    return timeline, best[0]


# ---------------------------------------------------------------- 상위 루프


def _order_robots(plan: AssignmentModel, params: PlanParams, graph: _Graph) -> list[str]:
    robots = sorted(plan.robots, key=lambda p: p.robot)
    rule = params.priority
    if rule == "robot_id":
        return [p.robot for p in robots]
    if rule == "most_goals":
        return [p.robot for p in sorted(robots, key=lambda p: (-len(p.goals), p.robot))]
    if rule == "fewest_goals":
        return [p.robot for p in sorted(robots, key=lambda p: (len(p.goals), p.robot))]
    if rule == "longest_route":
        def route(p):
            total, cur = 0, p.start
            for g in p.goals:
                total += graph.hops(g.location).get(cur, 10**6)
                cur = g.location
            return total
        return [p.robot for p in sorted(robots, key=lambda p: (-route(p), p.robot))]
    if rule == "random":
        keys = [p.robot for p in robots]
        random.Random(params.seed).shuffle(keys)
        return keys
    raise ValueError(f"알 수 없는 우선순위 규칙: {rule} (가능: {PRIORITY_RULES})")


def plan_instance(model: RoadmapModel, plan: AssignmentModel, params: PlanParams) -> PlanResult:
    """전 로봇을 우선순위 순으로 하나씩 계획한다(prioritized planning)."""
    if params.priority not in PRIORITY_RULES:
        raise ValueError(f"알 수 없는 우선순위 규칙: {params.priority}")
    graph = _Graph(model)
    res = _Reservations(graph, params.horizon)
    order = _order_robots(plan, params, graph)
    by_robot = {p.robot: p for p in plan.robots}

    paths: list[RobotPath] = []
    failures: list[tuple[str, str]] = []
    unplanned: list[str] = []
    for robot in order:
        rp = by_robot[robot]
        goals = [g.location for g in rp.goals]
        timeline, covered = _search(graph, res, rp.start, goals, params.horizon)
        if timeline is None:
            failures.append((robot, "park_impossible — 어디에도 안전하게 설 수 없다"))
            unplanned.append(robot)
            continue
        res.commit(robot, timeline)
        steps, fold_of = fold_timeline(timeline)
        if covered == len(goals):
            terminal = "goal_reached"
        elif len(steps) == 1:
            terminal = "no_progress"
        else:
            terminal = "plan_truncated"
        goal_at: dict[str, int] = {}
        cursor = 0
        for g in rp.goals[:covered]:
            for t in range(cursor, len(timeline)):
                if timeline[t] == g.location:
                    goal_at[g.goal_id] = fold_of[t]
                    cursor = t
                    break
        paths.append(
            RobotPath(robot, timeline, terminal, len(goals), covered, goal_at)
        )
        if terminal != "goal_reached":
            failures.append(
                (robot, f"{terminal} — 목표 {covered}/{len(goals)} 덮음 (지평 {params.horizon})")
            )

    paths.sort(key=lambda p: p.robot)
    return PlanResult(
        instance_id=plan.instance_id,
        plan_revision=params.plan_revision,
        paths=paths,
        visit_order=build_visit_order(paths),
        priority_order=order,
        ok=not unplanned and all(p.terminal == "goal_reached" for p in paths),
        failures=failures,
        unplanned=unplanned,
    )


# ---------------------------------------------------------------- 접기·통행 순서


def fold_timeline(timeline: list[str]) -> tuple[list[str], list[int]]:
    """연속한 동일 정점(대기)을 접는다. (접은 열, 타임스텝->접은 순번)."""
    steps: list[str] = []
    fold_of: list[int] = []
    for node in timeline:
        if not steps or steps[-1] != node:
            steps.append(node)
        fold_of.append(len(steps) - 1)
    return steps, fold_of


def build_visit_order(paths: list[RobotPath]) -> list[tuple[str, list[tuple[str, int]]]]:
    """계약 §1-2(2)(3): **접기 전 원본**에서 각 정점 진입 순으로 뽑고 index를 접은 뒤로 재매핑.

    싣는 기준은 «항목 수 >= 2»이며 «둘 이상의 로봇»이 아니다 — 한 로봇의 재방문도 항목 둘이다.
    """
    entries: dict[str, list[tuple[int, str, int]]] = defaultdict(list)
    for path in paths:
        _, fold_of = fold_timeline(path.timeline)
        for t, node in enumerate(path.timeline):
            if t == 0 or path.timeline[t - 1] != node:  # 진입 시각
                entries[node].append((t, path.robot, fold_of[t]))
    out: list[tuple[str, list[tuple[str, int]]]] = []
    for node in sorted(entries):
        items = sorted(entries[node])
        if len(items) >= 2:
            out.append((node, [(robot, idx) for _, robot, idx in items]))
    return out


# ---------------------------------------------------------------- 자체 점검


def validate_plan(model: RoadmapModel, assignment: AssignmentModel, result: PlanResult) -> list[str]:
    """산출 계획을 **다시** 검사한다. 탐색이 지킨 것을 독립적으로 확인하는 자리다."""
    problems: list[str] = []
    graph = _Graph(model)
    if {p.robot for p in result.paths} != {p.robot for p in assignment.robots}:
        problems.append("계획의 로봇 집합이 배정의 로봇 집합과 다르다 (x-set-equal)")

    horizon = max((len(p.timeline) for p in result.paths), default=1)
    occupancy: dict[tuple[int, str], str] = {}
    for path in result.paths:
        for t in range(horizon + 2):
            node = path.timeline[min(t, len(path.timeline) - 1)]
            owner = occupancy.get((t, node))
            if owner is not None:
                problems.append(f"정점 충돌 t={t} @{node}: {owner} vs {path.robot}")
            occupancy[(t, node)] = path.robot

    edge_use: dict[tuple[int, tuple[str, str]], list[tuple[str, str, str]]] = defaultdict(list)
    corr_dir: dict[tuple[int, str], tuple[int, str]] = {}
    for path in result.paths:
        steps, _ = fold_timeline(path.timeline)
        for a, b in zip(steps, steps[1:]):
            if graph.edge_key(a, b) not in graph.capacity:
                problems.append(f"인접하지 않은 두 정점이 이어졌다: {a}->{b} ({path.robot})")
        for t in range(len(path.timeline) - 1):
            u, v = path.timeline[t], path.timeline[t + 1]
            if u == v:
                continue
            key = graph.edge_key(u, v)
            if key not in graph.capacity:
                continue
            if not any(nxt == v for nxt, _ in graph.moves[u]):
                problems.append(f"traversal 위반: {u}->{v} ({path.robot})")
            edge_use[(t, key)].append((path.robot, u, v))
            corridor = graph.corridor[key]
            if corridor:
                direction = 1 if (u, v) == key else -1
                prev = corr_dir.get((t, corridor))
                if prev is not None and prev[0] != direction:
                    problems.append(
                        f"교행 불가 통로 역행 t={t} @{corridor}: {prev[1]} vs {path.robot}"
                    )
                corr_dir[(t, corridor)] = (direction, path.robot)
    for (t, key), users in sorted(edge_use.items()):
        if len(users) > graph.capacity[key]:
            problems.append(f"간선 용량 초과 t={t} @{key}: {len(users)} > {graph.capacity[key]}")
        orientations = {(u, v) for _, u, v in users}
        if len(orientations) > 1:
            problems.append(f"간선 교차(swap) 충돌 t={t} @{key}")

    for location, seq in result.visit_order:
        if len(seq) < 2:
            problems.append(f"visit_order 항목 수 < 2: {location}")
        for robot, index in seq:
            steps, _ = fold_timeline(result.path_of(robot).timeline)
            if index >= len(steps) or steps[index] != location:
                problems.append(f"visit_order index가 실제 step과 다르다: {location} {robot}#{index}")
    return problems
