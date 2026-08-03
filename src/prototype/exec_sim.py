"""S3 — **실행 의존 그래프가 허락할 때만** 세그먼트가 진행하는 재생 시뮬레이터.

🔴 이것은 **릴리스 판정의 완전한 형태가 아니다.** 여기 없는 것: 재선택(스위칭)·커밋 게이트·
진입 사건·신선도·계측·승급 사다리. 그것은 S4다. 여기 있는 것은 **고정 순서 실행** 하나뿐이다.

그럼에도 S2와 결정적으로 다른 점 하나: **경계 아티팩트만으로 재생한다.**
`53_prototype_s2.md` §53-5-1이 *"경계 아티팩트만으로는 충돌 없이 재생할 수 없다 — 대기가 접혀
사라졌기 때문이다"*라고 적었는데, **잃어버린 것은 대기가 아니라 순서였고 그 순서를 실행 의존
그래프가 되돌려 준다.** 그래서 `anim.py`처럼 계획기를 다시 돌리지 않는다.

시각은 **우리가 지어낸 것**이다(공칭 속도 × 간선 길이). 사전등록 상수가 아니다.
"""

from __future__ import annotations

from dataclasses import dataclass, field

from sadg_core import ExecGraph, SegKey, nominal_edges


@dataclass
class Blockage:
    """차단 주입 — 로봇 하나를 한 세그먼트 위에서 세운다. 외생 지연 delta의 조악한 대용이다."""

    robot: str
    segment_number: int
    seconds: float


@dataclass
class SimResult:
    start: dict[SegKey, float]
    end: dict[SegKey, float]
    duration: dict[SegKey, float]
    blocked_extra: dict[SegKey, float]
    preds: dict[SegKey, list[SegKey]]
    makespan: float
    order: list[SegKey]
    waiting_for: dict[SegKey, SegKey | None] = field(default_factory=dict)


def segment_durations(graph: ExecGraph, model, nominal_speed_mps: float) -> dict[SegKey, float]:
    """세그먼트 하나의 공칭 소요 = 간선 길이 / 공칭 속도. **값은 `config/scale.yaml`에서만 온다.**"""
    length: dict[tuple[str, str], float] = {}
    for e in model.edges:
        length[(e.u, e.v)] = e.length_m
        length[(e.v, e.u)] = e.length_m
    return {s.key: length[(s.src_node, s.dst_node)] / nominal_speed_mps for s in graph.segments}


def simulate(
    graph: ExecGraph, durations: dict[SegKey, float], blockages: list[Blockage] | None = None
) -> SimResult:
    """max-plus 전파 — 세그먼트는 **들어오는 모든 의존의 출발 세그먼트가 끝나야** 시작한다.

    `sadg.md`의 재귀 그대로다: tau_i = max_{j in pred(i)} tau_j + d_i.
    비순환이므로 위상 순서 한 번으로 끝난다.
    """
    from sadg_core import _kahn

    nodes = [s.key for s in graph.segments]
    edges = nominal_edges(graph)
    ok, order = _kahn(nodes, edges)
    if not ok:
        raise ValueError("공칭 그래프가 순환한다 — 재생할 수 없다 (그것이 데드락의 정의다)")

    preds: dict[SegKey, list[SegKey]] = {n: [] for n in nodes}
    for e in edges:
        preds[e.dst].append(e.src)

    extra: dict[SegKey, float] = {n: 0.0 for n in nodes}
    for b in blockages or []:
        key = (b.robot, b.segment_number)
        if key in extra:
            extra[key] += b.seconds

    start: dict[SegKey, float] = {}
    end: dict[SegKey, float] = {}
    waiting_for: dict[SegKey, SegKey | None] = {}
    for node in order:
        best, source = 0.0, None
        for p in preds[node]:
            if end[p] > best:
                best, source = end[p], p
        start[node] = best
        waiting_for[node] = source
        end[node] = best + durations[node] + extra[node]
    return SimResult(
        start=start,
        end=end,
        duration=durations,
        blocked_extra=extra,
        preds=preds,
        makespan=max(end.values(), default=0.0),
        order=order,
        waiting_for=waiting_for,
    )


def robot_state(graph: ExecGraph, sim: SimResult, robot: str, t: float):
    """시각 t에서 로봇의 (상태, 세그먼트, 진행비) — 상태는 move / wait / block / done.

    차단된 세그먼트는 **구간 전체에 걸쳐 느리게** 지나간다(진행비 = 경과/전체 소요).
    「간선 중간에 멈췄다가 달린다」로 그려도 되지만 둘 다 **우리가 지어낸 타이밍**이므로
    더 단순한 쪽을 골랐고 그 사실을 여기 적는다.
    """
    segments = sorted((s for s in graph.segments if s.robot == robot), key=lambda s: s.number)
    if not segments:
        return "done", None, 0.0
    for s in segments:
        if t < sim.start[s.key]:
            return "wait", s, 0.0
        if t < sim.end[s.key]:
            span = sim.end[s.key] - sim.start[s.key]
            ratio = (t - sim.start[s.key]) / span if span > 0 else 1.0
            state = "block" if sim.blocked_extra[s.key] > 0 else "move"
            return state, s, min(1.0, ratio)
    return "done", segments[-1], 1.0


def robot_position(graph: ExecGraph, sim: SimResult, model, robot: str, t: float):
    """시각 t에서 로봇의 (x, y, 상태). 대기 중이면 정점 위에 서 있다."""
    nodes = model.node_map()
    state, segment, ratio = robot_state(graph, sim, robot, t)
    if segment is None:
        return None
    if state == "done":
        n = nodes[segment.dst_node]
        return n.x_m, n.y_m, state
    a, b = nodes[segment.src_node], nodes[segment.dst_node]
    if state == "wait":
        return a.x_m, a.y_m, state
    return a.x_m + (b.x_m - a.x_m) * ratio, a.y_m + (b.y_m - a.y_m) * ratio, state


def blocking_witness(graph: ExecGraph, sim: SimResult, robot: str, t: float):
    """지금 이 로봇이 **누구를 기다리는가** — 순서를 지키고 있음을 화면에 적기 위한 값."""
    state, segment, _ = robot_state(graph, sim, robot, t)
    if state != "wait" or segment is None:
        return None
    source = sim.waiting_for.get(segment.key)
    if source is None or source[0] == robot:
        return None
    return source


def vertex_occupancy_conflicts(graph: ExecGraph, sim: SimResult, samples: int = 4000):
    """🔴 재생이 실제로 충돌 없는가를 **다시 계산해** 확인한다. 자체 점검이지 판정이 아니다.

    같은 시각에 두 로봇이 같은 정점을 차지하면 위반이다. 정점 점유 구간은
    «들어오는 세그먼트가 끝난 시각 ~ 나가는 세그먼트가 시작한 시각»이며, 세그먼트 위에 있는
    동안에는 간선을 점유한다(그 간선의 양 끝 정점은 비어 있는 것으로 본다).
    """
    if sim.makespan <= 0:
        return []
    robots = sorted({s.robot for s in graph.segments})
    conflicts = []
    for i in range(samples + 1):
        t = sim.makespan * i / samples
        here: dict[str, str] = {}
        for r in robots:
            state, segment, ratio = robot_state(graph, sim, r, t)
            if segment is None:
                continue
            if state in ("move", "block"):
                continue  # 간선 위에 있다 — 정점을 차지하지 않는다
            node = segment.dst_node if state == "done" else segment.src_node
            if node in here:
                conflicts.append((round(t, 3), node, here[node], r))
            else:
                here[node] = r
    return conflicts
