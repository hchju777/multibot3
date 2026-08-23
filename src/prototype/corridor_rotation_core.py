"""교행 불가 통로 **하나**의 방향 전환 정책 측정 코어 — 대기행렬 사건 구동 모형.

    「바닥(최소 유지 시간 T) + 편향 상한(beta)」 한 축으로 세 정책을 표현한다.
        beta = 1   -> 균등 주기 (바닥이 곧 전부. 반대쪽이 비어 있어도 T마다 뒤집는다)
        1 < beta   -> 편향      (T 이상 beta*T 이하로 유지. 그 사이에는 수요를 본다)
        beta = inf -> 반응형    (상한 없음. 현재 방향 줄이 빌 때만 뒤집는다)

🔴 **탐색용 시제다. 여기서 나오는 어떤 수치도 논문 증거가 아니고 사전등록 상수가 아니다.**
🔴 **통로 하나만 떼어낸 모형이다.** 로드맵 전체·MAPF·실행 계층·재계획이 하나도 들어 있지 않다.

이 파일은 경계 스키마(mrs.*)를 **모른다.** JSON 키 문자열이 하나도 없고, 아티팩트를 내지 않는다.
물리 규모(길이·속도)는 `protoscale.ScaleConfig` 또는 호출자가 준 값에서만 온다.

기존 알고리즘 코드를 **고치지 않는다.** `roadmap_core`는 통로 기하를 **읽어 오는 데만** 쓴다.
"""

from __future__ import annotations

import heapq
import math
import random
from dataclasses import asdict, dataclass, field

MEASURE_VERSION = "prototype-corridor_rotation-0.1.0"

# 상태 이름 — 관측창 시간을 이 넷으로 남김없이 나눈다.
STATE_BUSY = "busy"  # 통로에 로봇이 있고 새 진입도 열려 있다
# 🔴 "drain"은 «통로가 비어 있는 시간»이 아니다. 뒤집기로 정한 순간부터 마지막 로봇이 나갈 때까지의
#    구간이며, 그 동안 통로에는 여전히 그 마지막 로봇이 들어 있다. 여기서 잃는 것은
#    «진입 슬롯»이지 «통과»가 아니다. 배수 손실 비율을 읽을 때 이 뜻으로 읽어야 한다.
STATE_DRAIN = "drain"  # 뒤집기로 정했고 마지막 로봇이 나가기를 기다린다(새 진입 금지)
STATE_FLOOR_BLOCK = "floor_block"  # 통로가 비었고 현재 방향 줄도 비었는데 바닥 때문에 못 뒤집는다
STATE_IDLE = "idle_no_demand"  # 통로가 비었고 양쪽 다 수요가 없다
STATES = (STATE_BUSY, STATE_DRAIN, STATE_FLOOR_BLOCK, STATE_IDLE)


# ---------------------------------------------------------------- 입력 자료구조


@dataclass(frozen=True)
class CorridorSpec:
    """교행 불가 통로 하나의 기하.

    통로는 **간선 하나가 아니라 묶음**이다(`256`§256-2 `K3`). 오늘 로드맵 실측에서
    구성 간선은 전부 용량 1이므로, 모형은 **용량 1 구간 segments개가 사슬로 이어진 관**이다.

    - 진입 간격(headway) = 구간 길이 / 속도 : 앞 로봇이 첫 구간을 비우는 데 걸리는 시간
    - 통과 시간(transit) = 전체 길이 / 속도
    - 동시 수용 대수 = segments (구간마다 1대)

    속도가 균일하므로 사슬 안에서 막힘이 생기지 않는다 —
    진입 간격만 지키면 앞뒤가 서로 밀지 않는다. (K5 로봇별 속도 차이는 담지 않는다.)
    """

    label: str
    segments: int
    segment_length_m: float
    speed_mps: float

    @property
    def length_m(self) -> float:
        return self.segments * self.segment_length_m

    @property
    def headway_s(self) -> float:
        return self.segment_length_m / self.speed_mps

    @property
    def transit_s(self) -> float:
        return self.length_m / self.speed_mps

    @property
    def capacity_robots(self) -> int:
        return self.segments


@dataclass(frozen=True)
class DemandSpec:
    """수요. 유한 대수의 로봇이 「일하다가 통로를 쓰겠다고 줄에 선다」를 반복한다.

    치우침(skew)은 「줄에 설 때 어느 쪽 끝에 서는가」의 확률이다 — skew=0.5가 균등,
    skew=1.0이면 전부 한 방향.

    🔴 로봇이 통로 입구까지 **오는 시간이 없다**(줄에 순간이동한다). 한계로 보고한다.
    """

    fleet: int
    skew: float
    think_mean_s: float
    load_target: float = float("nan")  # think_mean_s를 유도한 목표 부하. 기록용


@dataclass(frozen=True)
class PolicySpec:
    """방향 전환 정책. **값 둘이 세 모드를 전부 표현한다**(`260`§260-3의 주장).

    - floor_s   : 바닥. 방향이 켜진 뒤 **최소한** 이만큼은 유지한다
    - beta      : 편향 상한. 최대 유지 시간 = beta * floor_s (beta=inf면 상한 없음)
    - unlocked  : 참조선. 방향 제약을 **아예 끄고** 양쪽이 같은 관을 쓴다.
                  🔴 물리적으로 불가능하며(용량 1 사슬에서 정면 교행) **도달 불가 상한**이다

    🔴 floor_s = 0 이면서 beta가 유한하면 최대 유지 시간이 0이 되어 정책이 정의되지 않는다
    (매 순간 전환). 그 조합은 만들 수 없게 막는다 — `PRECOMMIT.md` P1이 못 박은 자리다.
    """

    label: str
    floor_s: float
    beta: float
    unlocked: bool = False

    def __post_init__(self) -> None:
        if self.unlocked:
            return
        if self.beta < 1.0:
            raise ValueError("beta는 1 이상이어야 한다(1이 균등 주기다)")
        if self.floor_s <= 0.0 and math.isfinite(self.beta):
            raise ValueError(
                "floor_s=0과 유한 beta는 함께 쓸 수 없다 — 최대 유지 시간이 0이 되어 정의되지 않는다"
            )
        if self.floor_s < 0.0:
            raise ValueError("floor_s는 음수일 수 없다")

    @property
    def max_hold_s(self) -> float:
        """방향을 유지할 수 있는 상한. beta=inf면 무한."""
        if math.isinf(self.beta):
            return math.inf
        return self.beta * self.floor_s


@dataclass(frozen=True)
class RunSpec:
    """한 번의 실행. 같은 RunSpec이면 언제 어디서 돌려도 같은 결과가 나온다."""

    corridor: CorridorSpec
    demand: DemandSpec
    policy: PolicySpec
    seed: int
    horizon_s: float = 3600.0
    warmup_s: float = 600.0
    initial_direction: int = 0


# ---------------------------------------------------------------- 출력 자료구조


@dataclass
class RunResult:
    """한 번의 실행이 낸 원자료. **판정 어휘를 담지 않는다 — 수만 담는다.**"""

    throughput_per_h: float
    throughput_side0_per_h: float
    throughput_side1_per_h: float
    wait_mean_s: float
    wait_p95_s: float
    wait_max_s: float
    wait_max_side0_s: float
    wait_max_side1_s: float
    wait_samples: int
    censored_count: int
    censored_max_s: float
    switches: int
    switch_period_mean_s: float
    drain_frac: float
    floor_block_frac: float
    busy_frac: float
    idle_frac: float
    served_total: int
    queue_len_max: int
    offered_per_h: float

    def as_row(self) -> dict:
        return asdict(self)


# ---------------------------------------------------------------- 유도 계산


def think_mean_for_load(corridor: CorridorSpec, fleet: int, load_target: float) -> float:
    """목표 부하에서 「일하는 시간」 평균을 유도한다.

    부하 정의: 방향 제약이 **없을 때**의 관 처리율(1/진입간격)에 대한 요청률의 비.
        요청률 = fleet / (think_mean + transit)  이므로
        think_mean = fleet * headway / load_target - transit
    음수가 나오면 0으로 자른다(그 셀은 목표 부하보다 더 센 부하가 된다 — 결과에 기록한다).

    @param corridor 통로 기하
    @param fleet 로봇 대수
    @param load_target 목표 부하(1.0이 방향 제약 없는 관의 처리율과 같은 요청률)
    @return 「일하는 시간」 지수분포의 평균 [s]
    """
    if load_target <= 0.0:
        raise ValueError("load_target은 양수여야 한다")
    raw = fleet * corridor.headway_s / load_target - corridor.transit_s
    return max(0.0, raw)


def _percentile(values: list[float], q: float) -> float:
    """정렬 표본의 백분위(선형 보간 없음 — 가장 가까운 위쪽 순위). 표본이 없으면 nan."""
    if not values:
        return float("nan")
    ordered = sorted(values)
    idx = min(len(ordered) - 1, max(0, int(math.ceil(q * len(ordered))) - 1))
    return ordered[idx]


# ---------------------------------------------------------------- 시뮬레이터


def simulate(spec: RunSpec, trace: dict | None = None) -> RunResult:
    """통로 하나를 사건 구동으로 돌려 원자료 지표를 낸다.

    난수는 **로봇마다 독립 스트림**에서 나온다 — 그래서 (규모·치우침·부하·시드)가 같으면
    정책이 달라도 각 로봇의 k번째 「일하는 시간」과 k번째 방향 추첨이 같다(공통 난수).

    @param spec 실행 인자 전부
    @param trace None이 아니면 "entries"(진입 시각·쪽)와 "switches"(전환 시각)를 여기에 담는다.
                 시험이 불변식을 직접 확인하는 통로이며, 지표 계산에는 영향을 주지 않는다
    @return 관측창 안의 지표
    @throws ValueError 인자가 모순일 때
    """
    corr, dem, pol = spec.corridor, spec.demand, spec.policy
    if not 0.0 <= dem.skew <= 1.0:
        raise ValueError("skew는 0과 1 사이여야 한다")
    if spec.warmup_s >= spec.horizon_s:
        raise ValueError("warmup_s는 horizon_s보다 작아야 한다")

    headway = corr.headway_s
    transit = corr.transit_s
    horizon = spec.horizon_s
    warm = spec.warmup_s
    window = horizon - warm

    rngs = [random.Random(spec.seed * 100_003 + i) for i in range(dem.fleet)]

    def draw_think(i: int) -> float:
        mean = dem.think_mean_s
        if mean <= 0.0:
            return 0.0
        return rngs[i].expovariate(1.0 / mean)

    def draw_side(i: int) -> int:
        return 0 if rngs[i].random() < dem.skew else 1

    wakes: list[tuple[float, int]] = []
    for i in range(dem.fleet):
        heapq.heappush(wakes, (draw_think(i), i))

    queues: tuple[list[tuple[float, int]], list[tuple[float, int]]] = ([], [])
    head = [0, 0]  # 각 줄에서 이미 빠져나간 앞부분 색인(리스트를 popleft 하지 않는다)

    def qlen(side: int) -> int:
        return len(queues[side]) - head[side]

    direction = spec.initial_direction
    draining = False
    pending = 0
    drain_end = 0.0
    green_start = 0.0
    last_entry = -math.inf

    acc = dict.fromkeys(STATES, 0.0)
    waits: list[float] = []
    waits_side: tuple[list[float], list[float]] = ([], [])
    served = [0, 0]
    served_window = [0, 0]
    switch_times: list[float] = []
    offered_window = 0
    queue_len_max = 0

    t = 0.0
    while t < horizon:
        moved = True
        while moved:
            moved = False

            while wakes and wakes[0][0] <= t:
                wake_t, robot = heapq.heappop(wakes)
                side = draw_side(robot)
                queues[side].append((wake_t, robot))
                if wake_t >= warm:
                    offered_window += 1
                moved = True

            if draining and t >= drain_end:
                draining = False
                direction = pending
                green_start = t
                switch_times.append(t)
                moved = True

            if not draining and not pol.unlocked:
                held = t - green_start
                want_switch = False
                if held >= pol.max_hold_s:
                    want_switch = True  # 상한에 걸렸다 — 수요와 무관하게 뒤집는다
                elif held >= pol.floor_s and qlen(direction) == 0 and qlen(1 - direction) > 0:
                    want_switch = True  # 바닥을 지났고, 이쪽은 비었고, 저쪽은 기다린다
                if want_switch:
                    draining = True
                    pending = 1 - direction
                    drain_end = max(t, last_entry + transit)
                    moved = True

            if not draining and t >= last_entry + headway:
                side = _pick_side(queues, head, direction, pol.unlocked)
                if side is not None:
                    req_t, _robot_id = queues[side][head[side]]
                    head[side] += 1
                    wait = t - req_t
                    if req_t >= warm:
                        waits.append(wait)
                        waits_side[side].append(wait)
                    last_entry = t
                    served[side] += 1
                    if t >= warm:
                        served_window[side] += 1
                    exit_t = t + transit
                    robot = _robot_id
                    heapq.heappush(wakes, (exit_t + draw_think(robot), robot))
                    if trace is not None:
                        trace.setdefault("entries", []).append((t, side, exit_t))
                    moved = True

        queue_len_max = max(queue_len_max, qlen(0) + qlen(1))

        # --- 지금 상태를 한 이름으로 분류한다(관측창 시간을 남김없이 나눈다)
        corridor_busy = t < last_entry + transit
        if draining:
            state = STATE_DRAIN
        elif corridor_busy:
            state = STATE_BUSY
        elif pol.unlocked:
            state = STATE_IDLE if qlen(0) + qlen(1) == 0 else STATE_BUSY
        elif qlen(direction) == 0 and qlen(1 - direction) > 0:
            state = STATE_FLOOR_BLOCK
        else:
            state = STATE_IDLE

        # --- 다음 사건 시각
        cands = [horizon]
        if wakes:
            cands.append(wakes[0][0])
        if draining:
            cands.append(drain_end)
        else:
            if not pol.unlocked:
                if t < green_start + pol.floor_s:
                    cands.append(green_start + pol.floor_s)
                if math.isfinite(pol.max_hold_s) and t < green_start + pol.max_hold_s:
                    cands.append(green_start + pol.max_hold_s)
            if _pick_side(queues, head, direction, pol.unlocked) is not None:
                cands.append(last_entry + headway)
            if corridor_busy:
                cands.append(last_entry + transit)
        t_next = min(c for c in cands if c > t)
        t_next = min(t_next, horizon)

        lo, hi = max(t, warm), min(t_next, horizon)
        if hi > lo:
            acc[state] += hi - lo
        t = t_next

    censored: list[float] = []
    for side in (0, 1):
        for req_t, _ in queues[side][head[side] :]:
            if req_t >= warm:
                censored.append(horizon - req_t)

    if trace is not None:
        trace["switches"] = list(switch_times)
        trace.setdefault("entries", [])

    period = (
        (switch_times[-1] - switch_times[0]) / (len(switch_times) - 1)
        if len(switch_times) >= 2
        else float("nan")
    )
    switches_window = sum(1 for s in switch_times if s >= warm)

    return RunResult(
        throughput_per_h=sum(served_window) * 3600.0 / window,
        throughput_side0_per_h=served_window[0] * 3600.0 / window,
        throughput_side1_per_h=served_window[1] * 3600.0 / window,
        wait_mean_s=(sum(waits) / len(waits)) if waits else float("nan"),
        wait_p95_s=_percentile(waits, 0.95),
        wait_max_s=max(waits) if waits else float("nan"),
        wait_max_side0_s=max(waits_side[0]) if waits_side[0] else float("nan"),
        wait_max_side1_s=max(waits_side[1]) if waits_side[1] else float("nan"),
        wait_samples=len(waits),
        censored_count=len(censored),
        censored_max_s=max(censored) if censored else 0.0,
        switches=switches_window,
        switch_period_mean_s=period,
        drain_frac=acc[STATE_DRAIN] / window,
        floor_block_frac=acc[STATE_FLOOR_BLOCK] / window,
        busy_frac=acc[STATE_BUSY] / window,
        idle_frac=acc[STATE_IDLE] / window,
        served_total=sum(served),
        queue_len_max=queue_len_max,
        offered_per_h=offered_window * 3600.0 / window,
    )


def _pick_side(
    queues: tuple[list, list], head: list[int], direction: int, unlocked: bool
) -> int | None:
    """지금 진입시킬 로봇이 있는 쪽을 고른다.

    방향이 잠겨 있으면 현재 방향만, 참조선(unlocked)이면 **먼저 줄 선 쪽**을 고른다.

    @param queues 양쪽 줄
    @param head 각 줄에서 이미 빠져나간 앞부분 색인
    @param direction 현재 방향
    @param unlocked 방향 제약을 끈 참조선인가
    @return 진입시킬 쪽(0 또는 1), 없으면 None
    """
    if not unlocked:
        side = direction
        return side if len(queues[side]) - head[side] > 0 else None
    live = [s for s in (0, 1) if len(queues[s]) - head[s] > 0]
    if not live:
        return None
    return min(live, key=lambda s: queues[s][head[s]][0])


# ---------------------------------------------------------------- 통로 기하 실측


@dataclass
class MeasuredCorridor:
    """생성된 로드맵에서 읽어낸 통로 하나. **값을 지어내지 않기 위한 경로다.**"""

    seed: int
    key: str
    edges: int
    length_m: float
    capacities: tuple[int, ...] = field(default_factory=tuple)


def measure_corridors(cfg, gen_params_factory, seeds) -> list[MeasuredCorridor]:
    """`roadmap_core`가 만든 로드맵에서 통로 기하를 **읽어만 온다**(고치지 않는다).

    @param cfg protoscale.ScaleConfig
    @param gen_params_factory seed를 받아 roadmap_core.GenParams를 돌려주는 호출체
    @param seeds 훑을 시드들
    @return 통로별 실측값
    """
    from roadmap_core import generate_roadmap  # 지연 임포트 — 코어 자체는 로드맵에 안 매인다

    out: list[MeasuredCorridor] = []
    for seed in seeds:
        model = generate_roadmap(cfg, gen_params_factory(seed))
        for corridor in model.corridors:
            caps = tuple(
                sorted({e.capacity_robots for e in model.edges if e.corridor_key == corridor.key})
            )
            out.append(
                MeasuredCorridor(
                    seed=seed,
                    key=corridor.key,
                    edges=len(corridor.edge_keys),
                    length_m=corridor.length_m,
                    capacities=caps,
                )
            )
    return out
