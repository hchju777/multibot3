"""S4 — T0 틱의 **릴리스 판정 루프** (탐색용 시제).

🔴 **경계 스키마를 모른다.** `sadg_core`와 같은 규율이며, 경계 JSON 키·닫힌 어휘가 이 파일에
문자열로 나오지 않는다. 재발행 **산출**은 어댑터의 몫이고 여기서는 **발화(플래그)만** 한다.

여기 있는 것 (`11c_req_sadg_p2.md` §5-2·§5-5 · `_r2` §R1-2 · `12c_arch_sadg_p2.md` §A-3-1):

1. **릴리스 판정** — `pend(s) = #{커밋된 선행 중 미완료}` · `rel(s) ⟺ pend(s)=0`. **증분 갱신**이
   정본이고 전체 재계산은 **교차검증**이다(§5-2: *"전체 순회는 폴백이 아니라 교차검증"*).
2. **커밋 적용 지점 E1** — 틱 안의 **단 한 자리**. 게이트 ①②③④ 뒤 all-or-nothing.
3. **원자성 감시** — 🔴 *"적용과 재확인 사이에 양보 지점이 없다"*를 **「같은 틱」이 아니라
   「그 사이에 외부에 보이는 산출이 하나도 계산되지 않는다」**로 구현한다. 에포크가 열려 있는
   동안 외부 가시 산출을 만지면 `AtomicityViolation`을 던진다.
4. **재발행 발화/산출 분리 E2** — 틱은 `O(1)` 플래그만 세우고 아티팩트를 만들지 않는다.
5. **진행 결손** $D_r$ — `12c` §A-3-1의 `D_r <- now - tau_nom[frontier[r]]` 그대로.

🔴 **여기 없는 것**: 재선택(스위칭) 선택기 · 승급 결정 규칙 rho · 사다리 · 계측 링버퍼 ·
분위수 · 예산 측정. **`theta_esc`는 값 부재이므로 트리거가 존재하지 않는다.**
게이트 ②③④는 **T1 제안이 있을 때만** 발동하는데 **이 시제에는 제안을 만드는 선택기가 없다** —
시험에서 손으로 만든 제안으로만 발동시킨다(§57 참조).
"""

from __future__ import annotations

from dataclasses import dataclass, field

from sadg_core import ExecGraph, Precedence, SegKey, derive_head_robot, is_acyclic

# 🔴 [원문] `11c_req_sadg_p2.md` §5-2: "Delta t_T0 = 50 ms 하드".
# **이 시제가 그 값을 동결하지 않는다.** 사전등록 상수가 아니며 인자로 바꿀 수 있다.
DELTA_T_T0_S = 0.050

# 재발행 사유 — 내부 어휘. `11c_r2` §R1-4의 네 시점.
CAUSE_INITIAL = "initial_compile"
CAUSE_T1 = "t1_commit"
CAUSE_T2 = "t2_commit"
CAUSE_UNDERIVABLE = "underivable_commit"  # D7 gap 커밋. 반드시 즉시 발화한다


class AtomicityViolation(RuntimeError):
    """🔴 커밋 에포크가 열린 동안 외부 가시 산출을 계산하려 했다 (E1 위반)."""


class ContractViolation(RuntimeError):
    """계약 위반. 틱 **밖**에서만 던진다 — 틱 안 예외 금지는 `src/CLAUDE.md` CN-16."""


# ---------------------------------------------------------------------------
# 사건
# ---------------------------------------------------------------------------


@dataclass(frozen=True)
class EntryEvent:
    """정점 점유 영역 진입 사건. **절대 시각을 싣지 않는다**(§1-3(3))."""

    robot: str
    segment: SegKey  # entered=True이면 그 정점으로 **들어오는** 세그먼트 (§17-2-2)
    seq: int  # 로봇별 단조 증가. 건너뜀 = 유실 판정 -> D7
    entered: bool = True


@dataclass(frozen=True)
class CompletionEvent:
    segment: SegKey


@dataclass
class Proposal:
    """T1이 낼 «제안». 🔴 **이 시제에는 이것을 만드는 선택기가 없다** — 시험 전용이다."""

    choices: dict[str, int]  # 위치 -> 그 그룹에서 고른 대안 인덱스
    basis_groups: tuple[str, ...]  # 결정변수로 삼은 미정 그룹 (chi^val_1의 첫 성분)
    basis_blocked: frozenset  # 요청 시점의 B (chi^val_1의 둘째 성분)
    rz_seq: int = 0


@dataclass
class TickInput:
    tick_seq: int
    now_s: float
    entry_events: list[EntryEvent] = field(default_factory=list)
    completions: list[CompletionEvent] = field(default_factory=list)
    blocked_edges: frozenset = frozenset()
    proposal: Proposal | None = None
    rz_seq: int = 0


@dataclass
class GateVerdict:
    """게이트 ①②③④의 판정. **사유별로 나눠 센다**(합치면 A-N8 발동 여부를 말할 수 없다)."""

    acyclic: bool
    fresh: bool
    c6: bool
    rz_fresh: bool

    @property
    def ok(self) -> bool:
        return self.acyclic and self.fresh and self.c6 and self.rz_fresh

    @property
    def first_failed(self) -> str | None:
        for name, value in (
            ("gate1", self.acyclic), ("gate2", self.fresh),
            ("gate3", self.c6), ("gate4", self.rz_fresh),
        ):
            if not value:
                return name
        return None


@dataclass
class TickOutput:
    tick_seq: int
    released: list[SegKey]
    blocked_reason: dict[SegKey, SegKey]  # 릴리스되지 않은 프런티어 -> 기다리는 선행 세그먼트
    deficits: dict[str, float]
    republish_pending: bool
    republish_cause: str | None
    gate: GateVerdict | None


# ---------------------------------------------------------------------------
# 상태
# ---------------------------------------------------------------------------


@dataclass
class ExecutionState:
    """T0가 소유하는 유일한 상태. **쓰기 경로가 `run_tick` 하나뿐이다.**"""

    graph: ExecGraph
    pend: dict[SegKey, int] = field(default_factory=dict)
    succs: dict[SegKey, list[SegKey]] = field(default_factory=dict)
    done: set = field(default_factory=set)
    frontier: dict[str, SegKey | None] = field(default_factory=dict)
    committed_choice: dict[str, int] = field(default_factory=dict)  # 위치 -> 대안 인덱스
    entered_at_location: dict[str, str] = field(default_factory=dict)  # 위치 -> 첫 진입 로봇
    last_entry_seq: dict[str, int] = field(default_factory=dict)
    blocked_edges: frozenset = frozenset()
    tau_nom: dict[SegKey, float] = field(default_factory=dict)
    commit_seq: int = 0
    commit_epoch: int = 0
    rz_seq: int = 0
    republish_pending: bool = False
    republish_cause: str | None = None
    counters: dict[str, int] = field(default_factory=dict)
    _epoch_open: bool = False

    # ---- 외부 가시 산출. 🔴 에포크가 열려 있으면 만지지 못한다 -------------

    def release_view(self) -> list[SegKey]:
        """릴리스 비트의 뷰. **경계 출력이 아니다**(§R3-3: 소비자가 없다)."""
        self._require_epoch_closed("release_view")
        return sorted(s for s in self.frontier.values() if s is not None and self.pend[s] == 0
                      and s not in self.done)

    def republish_snapshot(self) -> ExecGraph:
        """재발행 **산출**의 입력. 🔴 틱 밖에서만 부른다(E2)."""
        self._require_epoch_closed("republish_snapshot")
        return self.graph

    def _require_epoch_closed(self, what: str) -> None:
        if self._epoch_open:
            raise AtomicityViolation(
                f"커밋 에포크가 열린 동안 외부 가시 산출 '{what}'을 계산하려 했다 — "
                "E1은 「같은 틱」이 아니라 「그 사이에 외부에 보이는 산출이 하나도 계산되지 "
                "않는다」이다"
            )

    def bump(self, key: str, n: int = 1) -> None:
        self.counters[key] = self.counters.get(key, 0) + n


# ---------------------------------------------------------------------------
# 초기화
# ---------------------------------------------------------------------------


def committed_edges(state: ExecutionState) -> list[Precedence]:
    """**커밋된 의존만** 본다 — 미커밋 그룹의 대안 간선은 릴리스를 막지 않는다(§5-2).

    막으면 두 대안이 서로를 막아 즉시 교착이다.
    """
    edges = list(state.graph.fixed)
    for g in state.graph.groups:
        idx = state.committed_choice.get(g.location)
        if idx is not None:
            edges.extend(g.alternatives[idx].edges)
    return edges


def _rebuild_pend(state: ExecutionState) -> dict[SegKey, int]:
    """**전체 재계산.** 정본이 아니라 교차검증용이다(§5-2)."""
    pend = {s.key: 0 for s in state.graph.segments}
    for e in committed_edges(state):
        if e.src not in state.done:
            pend[e.dst] += 1
    return pend


def _rebuild_succs(state: ExecutionState) -> dict[SegKey, list[SegKey]]:
    succs: dict[SegKey, list[SegKey]] = {s.key: [] for s in state.graph.segments}
    for e in committed_edges(state):
        succs[e.src].append(e.dst)
    return succs


def _recompute_frontier(state: ExecutionState) -> None:
    """`frontier[r]` — 아직 완료되지 않은 **가장 이른** 세그먼트. 없으면 `None` 센티넬(§R4-1)."""
    by_robot: dict[str, list[SegKey]] = {}
    for s in state.graph.segments:
        by_robot.setdefault(s.robot, []).append(s.key)
    for robot, keys in by_robot.items():
        remaining = [k for k in sorted(keys, key=lambda k: k[1]) if k not in state.done]
        state.frontier[robot] = remaining[0] if remaining else None


def make_state(graph: ExecGraph, tau_nom: dict[SegKey, float] | None = None) -> ExecutionState:
    """기동 시 1회. **고정 크기 사전 할당**의 자리이며 틱 안에서 다시 만들지 않는다."""
    state = ExecutionState(graph=graph, tau_nom=dict(tau_nom or {}))
    state.pend = _rebuild_pend(state)
    state.succs = _rebuild_succs(state)
    _recompute_frontier(state)
    state.republish_pending = True
    state.republish_cause = CAUSE_INITIAL
    return state


# ---------------------------------------------------------------------------
# 게이트 ①②③④
# ---------------------------------------------------------------------------


def evaluate_gates(state: ExecutionState, proposal: Proposal, rz_seq: int) -> GateVerdict:
    """네 게이트를 **판정만** 한다. 🔴 어느 대안이 좋은지는 판정하지 않는다 — 선택기가 아니다.

    ① 선택 조합의 비순환 / ② chi^val_1 신선도 / ③ C6 재확인 / ④ rz_seq 최신성.
    **게이트는 C5가 아니다** — 현재 조합만 본다(§5-5).
    """
    nodes = [s.key for s in state.graph.segments]
    by_location = {g.location: g for g in state.graph.groups}

    edges = list(state.graph.fixed)
    for location, group in by_location.items():
        idx = proposal.choices.get(location, state.committed_choice.get(location))
        if idx is not None:
            edges.extend(group.alternatives[idx].edges)
    acyclic = is_acyclic(nodes, edges)

    # ② chi^val_1 = (결정변수로 삼은 미정 그룹 중 그 사이 커밋된 것이 있는가, B)
    fresh = all(g not in state.committed_choice for g in proposal.basis_groups)
    fresh = fresh and proposal.basis_blocked == state.blocked_edges

    # ③ C6 재확인 — 이미 진입 커밋된 그룹의 순서를 뒤집으려 하는가
    c6 = all(
        state.committed_choice.get(loc) in (None, idx) for loc, idx in proposal.choices.items()
    )

    return GateVerdict(acyclic, fresh, c6, proposal.rz_seq == rz_seq)


# ---------------------------------------------------------------------------
# 틱
# ---------------------------------------------------------------------------


def run_tick(state: ExecutionState, tick: TickInput) -> TickOutput:
    """한 틱. **호출 순서가 곧 규칙이다**(`12c` §A-3-1을 그대로 옮겼다).

    🔴 **에포크가 열린 구간(2~4)에서 외부 가시 산출을 하나도 계산하지 않는다.**
    `_epoch_open` 감시가 그것을 강제하고, 음성 시험이 감시가 공허하지 않음을 보인다.
    """
    state._epoch_open = True
    state.commit_epoch += 1
    commit_seq_bumped = False

    # 2)-3) 관측 반영 + 진입 커밋 --------------------------------------------
    for event in sorted(tick.entry_events, key=lambda e: (e.seq, e.robot)):
        _ingest_entry_event(state, event)
    if _apply_entry_commits(state):
        commit_seq_bumped = True

    # 4) E1 — 선언 커밋 적용 지점 (프로세스 전체에서 유일) -------------------
    verdict = None
    if tick.proposal is not None:
        verdict = evaluate_gates(state, tick.proposal, tick.rz_seq)
        if verdict.ok:
            for location, idx in tick.proposal.choices.items():
                state.committed_choice[location] = idx  # all-or-nothing
            _reindex(state)
            if not commit_seq_bumped:
                state.commit_seq += 1
                commit_seq_bumped = True
            _mark_republish(state, CAUSE_T1)
        else:
            state.bump(f"stale_discards.{verdict.first_failed}")
            if verdict.acyclic and verdict.fresh and not verdict.c6:
                state.bump("commit_conflict_count")  # A-N8 위반의 관측 증거
    # ---- 여기까지가 「양보 지점 없음」 구간 --------------------------------
    state._epoch_open = False

    # 5) 완료 반영 — pend[] **증분** 감소 -----------------------------------
    for c in tick.completions:
        _apply_completion(state, c)

    # 6) 차단 반영. reason은 저장만 하고 어떤 분기에도 넣지 않는다 (I-6)
    state.blocked_edges = tick.blocked_edges

    # 7) 릴리스 판정 + 진행 결손
    released, blocked_reason = _release_check(state)
    deficits = _progress_deficit(state, tick.now_s)

    # 8) rho — 🔴 **없다.** theta_esc가 값 부재이므로 트리거가 존재하지 않는다
    # 11) E2 발화 — O(1) 플래그만. 산출은 틱 밖이다
    return TickOutput(
        tick_seq=tick.tick_seq,
        released=released,
        blocked_reason=blocked_reason,
        deficits=deficits,
        republish_pending=state.republish_pending,
        republish_cause=state.republish_cause,
        gate=verdict,
    )


def _ingest_entry_event(state: ExecutionState, event: EntryEvent) -> None:
    """단조 seq · 멱등 중복 · **건너뜀은 유실 판정 -> D7**(§1-3(4))."""
    last = state.last_entry_seq.get(event.robot, -1)
    if event.seq <= last:
        state.bump("duplicate_entry_events")  # 멱등: 중복 수신은 무해해야 한다
        return
    if event.seq > last + 1:
        state.bump("entry_event_gap_count", event.seq - last - 1)
        # D7 — 「이미 진입한 것으로 간주」. 되돌리지 않는다(되돌림 자체가 C6 위반).
        # 유도 불가 커밋이므로 **반드시 즉시 재발행을 발화**시킨다(§R1-3).
        _mark_republish(state, CAUSE_UNDERIVABLE)
    state.last_entry_seq[event.robot] = event.seq
    location = _segment_of(state, event.segment)
    if location is not None and location not in state.entered_at_location:
        state.entered_at_location[location] = event.robot


def _segment_of(state: ExecutionState, key: SegKey) -> str | None:
    for s in state.graph.segments:
        if s.key == key:
            return s.dst_node
    return None


def _apply_entry_commits(state: ExecutionState) -> bool:
    """진입 커밋 — **첫 진입 로봇이 그 위치의 순서를 고정한다**(§2-3).

    🔴 **재선택이 아니다.** 최적화도 선호도 없고, 물리적으로 먼저 들어간 사실을 기록할 뿐이다.
    이 시제의 정본 산출물은 `switch_groups`가 비어 있어 이 함수가 **아무 일도 하지 않는다.**
    """
    changed = False
    for group in state.graph.groups:
        if group.location in state.committed_choice:
            continue
        first = state.entered_at_location.get(group.location)
        if first is None:
            continue
        # 🔴 선두는 **대안의 내용(의존 집합)에서 유도한다** — 경계가 항목 열을 싣지 않으므로
        # 이 유도가 없으면 진입 커밋이 경계 아티팩트만으로는 성립하지 않는다(정리 H4).
        for idx, alt in enumerate(group.alternatives):
            head = alt.order[0][0] if alt.order else derive_head_robot(alt.edges)
            if head is None:
                state.bump("head_underivable")
                continue
            if head == first:
                state.committed_choice[group.location] = idx
                changed = True
                break
        else:
            # 실현된 선두를 갖는 대안이 없다 — CE-HEAD. 커밋하지 않고 계수만 한다
            state.bump("head_not_in_alternatives")
    if changed:
        _reindex(state)
        state.commit_seq += 1
    return changed


def _reindex(state: ExecutionState) -> None:
    """커밋으로 커밋된 간선 집합이 바뀌면 `pend`·`succs`를 다시 만든다."""
    state.pend = _rebuild_pend(state)
    state.succs = _rebuild_succs(state)


def _apply_completion(state: ExecutionState, event: CompletionEvent) -> None:
    """**증분 갱신** — 후속 간선만 감소한다. 그래프 크기에 비례하지 않는다(§5-2 D6)."""
    if event.segment in state.done:
        state.bump("duplicate_completions")
        return
    state.done.add(event.segment)
    for nxt in state.succs.get(event.segment, ()):  # 중복 간선도 그대로 센다
        state.pend[nxt] -= 1
    robot = event.segment[0]
    remaining = sorted(
        (s.key for s in state.graph.segments if s.robot == robot and s.key not in state.done),
        key=lambda k: k[1],
    )
    state.frontier[robot] = remaining[0] if remaining else None


def _release_check(state: ExecutionState):
    """`rel(s) <=> pend(s) = 0`. `frontier[r] = None`인 로봇은 **판정 대상에서 제외**(§R4-1)."""
    released: list[SegKey] = []
    reason: dict[SegKey, SegKey] = {}
    for robot in sorted(state.frontier):
        s = state.frontier[robot]
        if s is None:
            continue
        if state.pend[s] == 0:
            released.append(s)
        else:
            for e in committed_edges(state):
                if e.dst == s and e.src not in state.done:
                    reason[s] = e.src  # 무엇을 기다리는가 — 화면에 적기 위한 값
                    break
    return released, reason


def _progress_deficit(state: ExecutionState, now_s: float) -> dict[str, float]:
    """`D_r <- now - tau_nom[frontier[r]]` (`12c` §A-3-1). **양수가 「늦음」**(N-S).

    🔴 `theta_esc`는 **값 부재**이므로 이 값은 **보고만 되고 어떤 분기에도 들어가지 않는다.**
    """
    out: dict[str, float] = {}
    for robot, s in state.frontier.items():
        if s is None:
            out[robot] = 0.0  # 세그먼트 0개 — (가) 목표 도달 정지로 간주. 🔴 U-h 미결
        else:
            out[robot] = now_s - state.tau_nom.get(s, now_s)
    return out


def _mark_republish(state: ExecutionState, cause: str) -> None:
    """E2 발화 — **플래그만.** 한 틱에 여러 번 불려도 재발행은 1회로 묶인다(coalesce)."""
    state.republish_pending = True
    state.republish_cause = cause
    state.bump(f"republish_cause.{cause}")


def take_republish(state: ExecutionState) -> str | None:
    """🔴 **틱 밖**에서 부르는 재발행 워커. 플래그를 내리고 사유를 돌려준다(E2의 「산출」 쪽)."""
    if not state.republish_pending:
        return None
    state._require_epoch_closed("take_republish")
    cause = state.republish_cause
    state.republish_pending = False
    state.republish_cause = None
    state.bump("republish_count")
    return cause


# ---------------------------------------------------------------------------
# 교차검증 — 🔴 증분이 정본이고 이것은 «다시 계산해 대조하는» 자리다
# ---------------------------------------------------------------------------


def cross_check_pend(state: ExecutionState) -> list[str]:
    """전체 재계산과 증분 결과를 대조한다. **불일치 1건이면 결함**(§5-2)."""
    fresh = _rebuild_pend(state)
    return [
        f"pend 불일치 {k}: 증분 {state.pend[k]} vs 재계산 {fresh[k]}"
        for k in sorted(fresh)
        if state.pend[k] != fresh[k]
    ]


def invariant_check(state: ExecutionState) -> list[str]:
    """검사 가능 불변식 — I-5(C6) 계열과 세그먼트 0개 로봇 처리."""
    problems: list[str] = []
    for robot, s in state.frontier.items():
        if s is not None and s in state.done:
            problems.append(f"{robot}: 프런티어가 이미 완료된 세그먼트다 {s}")
    for key, value in state.pend.items():
        if value < 0:
            problems.append(f"pend가 음수다 {key}: {value}")
    for location, idx in state.committed_choice.items():
        group = next((g for g in state.graph.groups if g.location == location), None)
        if group is None or not (0 <= idx < len(group.alternatives)):
            problems.append(f"커밋된 대안 인덱스가 범위 밖이다: {location} -> {idx}")
    return problems
