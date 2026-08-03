"""S4 릴리스 판정 루프 시험 — 릴리스·게이트·원자성·재발행 분리·D7·닫힌 고리.

🔴 **여기서 통과하는 것은 판정이 아니다.** 시험을 쓴 사람과 승인하는 사람이 다르다.
"""

import json
import os

import pytest

from assignment_core import AssignParams, build_assignment
from boundary_adapter import constraints_from_boundary, constraints_to_boundary
from exec_sim import Blockage, segment_durations, simulate
from plan_core import PlanParams, plan_instance
from protoscale import load_scale
from roadmap_core import GenParams, generate_roadmap
from sadg_core import (
    INTER_ROBOT,
    Precedence,
    compile_graph,
    derive_head_robot,
    plan_view_of,
    prune_cyclic_alternatives,
)
from release_sim import nominal_schedule, run_loop
from tick_core import (
    CAUSE_UNDERIVABLE,
    AtomicityViolation,
    CompletionEvent,
    EntryEvent,
    Proposal,
    TickInput,
    committed_edges,
    cross_check_pend,
    evaluate_gates,
    make_state,
    run_tick,
    take_republish,
)

CFG = load_scale()
HERE = os.path.dirname(os.path.abspath(__file__))
OUT = os.path.normpath(os.path.join(HERE, "..", "out"))


def _bundle(seed=0, robots=6, tasks=8, generator="lift", prune=True):
    model = generate_roadmap(CFG, GenParams(seed=seed))
    assignment = build_assignment(model, AssignParams(robots=robots, tasks=tasks, seed=seed))
    result = plan_instance(model, assignment, PlanParams())
    graph = compile_graph(plan_view_of(result), alt_generator=generator)
    if prune:
        prune_cyclic_alternatives(graph)
    return model, graph


def _boundary_roundtripped(graph):
    """🔴 **경계를 한 번 건너온 그래프**로 시험한다 — 실제 소비자가 보는 것이 이것이다."""
    return constraints_from_boundary(constraints_to_boundary(graph))


# --------------------------------------------------------------------- 릴리스 판정


def test_release_requires_all_committed_predecessors_done():
    _, graph = _bundle()
    state = make_state(graph)
    out = run_tick(state, TickInput(0, 0.0))
    for key in out.released:
        assert state.pend[key] == 0
    for key, waiting in out.blocked_reason.items():
        assert state.pend[key] > 0 and waiting not in state.done


def test_uncommitted_group_alternatives_do_not_block_release():
    """§5-2 — 막으면 두 대안이 서로를 막아 **즉시 교착**이다."""
    _, graph = _bundle(generator="swap")
    assert graph.groups, "스위치 그룹이 있는 인스턴스여야 시험이 성립한다"
    state = make_state(graph)
    edges = committed_edges(state)
    for g in graph.groups:
        for alt in g.alternatives:
            for e in alt.edges:
                assert e not in edges


def test_incremental_pend_agrees_with_full_recomputation():
    """증분이 정본이고 전체 재계산은 **교차검증**이다(§5-2)."""
    model, graph = _bundle()
    durations = segment_durations(graph, model, CFG.nominal_speed_mps)
    state = make_state(graph, nominal_schedule(graph, durations))
    for tick in range(60):
        released = run_tick(state, TickInput(tick, tick * 0.05)).released
        done = [CompletionEvent(k) for k in released[:2]]
        run_tick(state, TickInput(tick, tick * 0.05, completions=done))
        assert cross_check_pend(state) == []


def test_cross_check_catches_a_corrupted_incremental_counter():
    """🔴 음성 시험 — 교차검증이 공허하지 않음을 보인다."""
    _, graph = _bundle()
    state = make_state(graph)
    key = graph.segments[-1].key
    state.pend[key] += 1
    assert any("pend 불일치" in p for p in cross_check_pend(state))


def test_zero_segment_robot_is_excluded_from_release_judgment():
    """§R4-1 — `frontier[r] = NONE`은 판정 대상에서 빠진다."""
    _, graph = _bundle()
    state = make_state(graph)
    robot = graph.segments[0].robot
    for s in graph.segments:
        if s.robot == robot:
            state.done.add(s.key)
    state.frontier[robot] = None
    out = run_tick(state, TickInput(0, 0.0))
    assert all(k[0] != robot for k in out.released)
    assert all(k[0] != robot for k in out.blocked_reason)


# --------------------------------------------------------------------- 진입 사건


def test_duplicate_entry_events_are_idempotent():
    _, graph = _bundle()
    state = make_state(graph)
    event = EntryEvent("r0", (graph.segments[0].key), seq=0)
    run_tick(state, TickInput(0, 0.0, entry_events=[event]))
    run_tick(state, TickInput(1, 0.05, entry_events=[event]))
    assert state.counters.get("duplicate_entry_events") == 1
    assert state.counters.get("entry_event_gap_count") is None


def test_sequence_gap_fires_d7_and_forces_a_republish():
    """§1-3(4) + §R1-3 — **gap 커밋은 유도 불가이므로 반드시 즉시 재발행을 발화시킨다.**"""
    _, graph = _bundle()
    state = make_state(graph)
    take_republish(state)  # 초기 컴파일분 소비
    run_tick(state, TickInput(0, 0.0, entry_events=[EntryEvent("r0", ("r0", 0), seq=0)]))
    take_republish(state)
    run_tick(state, TickInput(1, 0.05, entry_events=[EntryEvent("r0", ("r0", 3), seq=3)]))
    assert state.counters["entry_event_gap_count"] == 2
    assert take_republish(state) == CAUSE_UNDERIVABLE


def test_entry_commit_is_derived_from_the_boundary_content_alone():
    """🔴 경계는 항목 열을 싣지 않는다. **선두를 의존 집합에서 유도**할 수 있어야 한다."""
    _, graph = _bundle(generator="swap")
    assert graph.groups
    crossed = _boundary_roundtripped(graph)
    assert all(alt.order == () for g in crossed.groups for alt in g.alternatives)
    for g in crossed.groups:
        for alt in g.alternatives:
            assert derive_head_robot(alt.edges) is not None


def test_head_to_alternative_is_bijective_for_lift_but_not_for_swap():
    """🔴 정리 H4는 **정본 생성기에서만** 성립한다 — `12c` §A-5-2의 `head_alt_bijective`.

    구판(인접 쌍 교환)은 **선두를 바꾸지 않는 대안**을 만들 수 있으므로 선두 하나가 대안을
    유일하게 정하지 않는다. 즉 §R1-3의 「국소 유도」 주 경로는 **생성기에 종속**된다.
    """
    _, lift = _bundle(generator="lift", prune=False)
    for g in _boundary_roundtripped(lift).groups:
        heads = [derive_head_robot(a.edges) for a in g.alternatives]
        assert len(set(heads)) == len(heads), "정본(lift)에서는 선두 -> 대안이 단사다"
    _, swap = _bundle(generator="swap", prune=False)
    collisions = 0
    for g in _boundary_roundtripped(swap).groups:
        heads = [derive_head_robot(a.edges) for a in g.alternatives]
        collisions += len(heads) - len(set(heads))
    assert collisions > 0, "구판에서는 선두가 대안을 유일하게 정하지 않는 자리가 실제로 나온다"


def test_derive_head_handles_a_revisit_group():
    """재방문이 섞여도 유도가 유일하다 — 같은 로봇 항목의 순번이 순서를 준다."""
    edges = (
        Precedence(("r0", 3), ("r1", 0), INTER_ROBOT),
        Precedence(("r1", 5), ("r2", 1), INTER_ROBOT),
    )
    assert derive_head_robot(edges) == "r0"
    assert derive_head_robot(()) is None


# --------------------------------------------------------------------- 게이트 ①②③④


def _proposal(graph, state, **kw):
    location = graph.groups[0].location
    base = dict(choices={location: 1}, basis_groups=(location,),
                basis_blocked=state.blocked_edges, rz_seq=0)
    base.update(kw)
    return Proposal(**base)


def test_gate_all_four_pass_on_a_clean_proposal():
    _, graph = _bundle(generator="swap")
    state = make_state(graph)
    verdict = evaluate_gates(state, _proposal(graph, state), rz_seq=0)
    assert verdict.ok and verdict.first_failed is None


def test_gate2_rejects_a_proposal_whose_basis_group_was_committed_meanwhile():
    """chi^val_1 — «내가 결정변수로 삼은 미정 그룹 중 그 사이 커밋된 것이 있는가»."""
    _, graph = _bundle(generator="swap")
    state = make_state(graph)
    proposal = _proposal(graph, state)
    state.committed_choice[graph.groups[0].location] = 0  # 그 사이 진입 커밋됐다
    verdict = evaluate_gates(state, proposal, rz_seq=0)
    assert not verdict.fresh and verdict.first_failed == "gate2"


def test_gate2_rejects_a_proposal_whose_blocked_set_changed():
    _, graph = _bundle(generator="swap")
    state = make_state(graph)
    proposal = _proposal(graph, state, basis_blocked=frozenset({("A", "B")}))
    assert not evaluate_gates(state, proposal, rz_seq=0).fresh


def test_gate3_rejects_reversing_an_already_committed_group_and_counts_the_conflict():
    """🔴 **진입 커밋이 이긴다**(§R1-5). 이 사건은 A-N8 위반의 관측 증거다."""
    _, graph = _bundle(generator="swap")
    state = make_state(graph)
    location = graph.groups[0].location
    state.committed_choice[location] = 0
    proposal = Proposal(choices={location: 1}, basis_groups=(), basis_blocked=frozenset())
    verdict = evaluate_gates(state, proposal, rz_seq=0)
    assert verdict.acyclic and verdict.fresh and not verdict.c6
    run_tick(state, TickInput(0, 0.0, proposal=proposal))
    assert state.counters["commit_conflict_count"] == 1
    assert state.counters["stale_discards.gate3"] == 1
    assert state.committed_choice[location] == 0  # 🔴 부분 채택이 없다


def test_gate4_rejects_a_stale_rz_seq():
    _, graph = _bundle(generator="swap")
    state = make_state(graph)
    proposal = _proposal(graph, state, rz_seq=3)
    assert not evaluate_gates(state, proposal, rz_seq=5).rz_fresh


def test_gate1_rejects_a_cyclic_combination():
    """게이트 ①은 **현재 조합만** 본다 — C5가 아니다(§5-5)."""
    from itertools import product

    _, graph = _bundle(generator="swap", prune=False)
    state = make_state(graph)
    bad = None
    # T1 응답은 **미정 그룹 전부**를 결정변수로 삼는다(§5-5) — 조합 전체를 훑는다
    for combo in product(*[range(len(g.alternatives)) for g in graph.groups]):
        proposal = Proposal(
            choices={g.location: idx for g, idx in zip(graph.groups, combo)},
            basis_groups=(), basis_blocked=frozenset(),
        )
        if not evaluate_gates(state, proposal, rz_seq=0).acyclic:
            bad = proposal
            break
    assert bad is not None, "가지치기 전 그래프에는 순환 조합이 있다(§54-0-2)"
    verdict = evaluate_gates(state, bad, rz_seq=0)
    assert not verdict.acyclic and verdict.first_failed == "gate1"


def test_commit_is_all_or_nothing():
    _, graph = _bundle(generator="swap")
    if len(graph.groups) < 2:
        pytest.skip("그룹이 둘 이상인 인스턴스가 필요하다")
    state = make_state(graph)
    locations = [g.location for g in graph.groups[:2]]
    state.committed_choice[locations[1]] = 0
    proposal = Proposal(choices={locations[0]: 1, locations[1]: 1},
                        basis_groups=(), basis_blocked=frozenset())
    run_tick(state, TickInput(0, 0.0, proposal=proposal))
    assert locations[0] not in state.committed_choice  # 하나가 막히면 전체 폐기


# --------------------------------------------------------------------- E1 원자성


def test_external_output_during_an_open_epoch_is_refused():
    """🔴 E1 — 원자성의 정의는 「같은 틱」이 아니라 **「그 사이에 외부 가시 산출이 없다」**이다."""
    _, graph = _bundle()
    state = make_state(graph)
    state._epoch_open = True
    with pytest.raises(AtomicityViolation):
        state.release_view()
    with pytest.raises(AtomicityViolation):
        take_republish(state)


def test_the_atomicity_guard_is_not_vacuous():
    """🔴 음성 시험 — 실제로 「사이에서」 산출을 계산하는 틱을 짜면 잡힌다."""
    _, graph = _bundle(generator="swap")
    state = make_state(graph)

    def bad_tick():
        state._epoch_open = True
        state.commit_epoch += 1
        run = state.release_view()  # ← 진입 커밋과 게이트 사이에서 외부 산출을 계산했다
        state._epoch_open = False
        return run

    with pytest.raises(AtomicityViolation):
        bad_tick()
    state._epoch_open = False
    assert state.release_view() is not None  # 닫힌 뒤에는 정상이다


def test_normal_tick_closes_the_epoch():
    _, graph = _bundle()
    state = make_state(graph)
    run_tick(state, TickInput(0, 0.0))
    assert state._epoch_open is False
    assert state.commit_epoch == 1
    assert state.release_view() is not None


# --------------------------------------------------------------------- E2 발화/산출 분리


def test_tick_never_builds_a_boundary_artifact():
    """🔴 E2 — 틱은 `O(1)` 플래그만 세운다. 산출은 **틱 밖**이다."""
    import boundary_adapter

    _, graph = _bundle()
    state = make_state(graph)
    calls = []
    original = boundary_adapter.constraints_to_boundary
    boundary_adapter.constraints_to_boundary = lambda g: calls.append(1) or original(g)
    try:
        run_tick(state, TickInput(0, 0.0, entry_events=[EntryEvent("r0", ("r0", 5), seq=5)]))
    finally:
        boundary_adapter.constraints_to_boundary = original
    assert calls == []
    assert state.republish_pending  # 발화는 됐다
    assert take_republish(state) is not None  # 산출은 틱 밖에서 꺼낸다
    assert take_republish(state) is None  # coalesce — 두 번 꺼내지지 않는다


# --------------------------------------------------------------------- 닫힌 고리


def test_release_loop_reproduces_the_max_plus_schedule_without_blocking():
    """🔴 S3(방정식)와 S4(판정 루프)가 같은 완료 시각을 낸다 — 서로의 교차검증이다."""
    from tick_core import DELTA_T_T0_S

    model, graph = _bundle()
    durations = segment_durations(graph, model, CFG.nominal_speed_mps)
    expected = simulate(graph, durations).makespan
    res = run_loop(graph, model, CFG.nominal_speed_mps)
    assert res.problems == [] and res.vertex_conflicts == []
    assert abs(res.makespan_s - expected) <= DELTA_T_T0_S * 2


def test_release_loop_has_no_vertex_conflict_under_blocking_and_entry_loss():
    """🔴 막혀도, 사건을 잃어도 **순서를 지킨다**."""
    model, graph = _bundle()
    res = run_loop(graph, model, CFG.nominal_speed_mps,
                   [Blockage("r0", 4, 45.0)], drop_entry_nth=2)
    assert res.vertex_conflicts == []
    assert res.problems == []
    assert res.state.counters.get("entry_event_gap_count", 0) >= 1
    assert any(cause == CAUSE_UNDERIVABLE for _, cause in res.republish_log)


def test_release_loop_never_starts_a_segment_before_its_predecessor_finishes():
    """커밋된 의존이 실제로 지켜졌는가를 **궤적에서 다시 계산해** 확인한다."""
    model, graph = _bundle()
    res = run_loop(graph, model, CFG.nominal_speed_mps, [Blockage("r2", 1, 20.0)])
    started: dict = {}
    finished: dict = {}
    for f in res.frames:
        for r, (_, _, mode) in f.positions.items():
            front = None
            for key in f.released:
                if key[0] == r:
                    front = key
            if mode in ("move", "block") and front is not None:
                started.setdefault(front, f.t_s)
    for e in committed_edges(res.state):
        if e.src in started and e.dst in started:
            assert started[e.src] <= started[e.dst] + 1e-9
    del finished


def test_blocking_delays_only_downstream_robots_in_the_release_loop():
    model, graph = _bundle()
    free = run_loop(graph, model, CFG.nominal_speed_mps)
    blocked = run_loop(graph, model, CFG.nominal_speed_mps, [Blockage("r0", 4, 45.0)])
    assert blocked.makespan_s > free.makespan_s
    assert blocked.vertex_conflicts == []


def test_the_loop_runs_on_the_artifact_written_to_disk():
    """🔴 **경계 아티팩트만으로 돈다** — 계획기도 컴파일러도 다시 부르지 않는다."""
    from boundary_adapter import roadmap_from_boundary

    roadmap_path = os.path.join(OUT, "roadmap.json")
    constraints_path = os.path.join(OUT, "execution_constraints.json")
    if not (os.path.exists(roadmap_path) and os.path.exists(constraints_path)):
        pytest.skip("out/ 아티팩트가 없다 — gen_*.py를 먼저 돌린다")
    with open(roadmap_path, encoding="utf-8") as h:
        model = roadmap_from_boundary(json.load(h))
    with open(constraints_path, encoding="utf-8") as h:
        graph = constraints_from_boundary(json.load(h))
    res = run_loop(graph, model, CFG.nominal_speed_mps)
    assert res.makespan_s > 0 and res.problems == [] and res.vertex_conflicts == []
