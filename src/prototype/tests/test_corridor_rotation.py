"""통로 방향 전환 측정 장치 시험 — 불변식 · 결정론 · 공통 난수 · 정책 의미론 · 기존 코드 대조.

🔴 이 시험들은 **측정 장치가 재려는 것을 재고 있는가**를 고정한다. 결과의 좋고 나쁨을 판정하지 않는다.
"""

import math

import pytest

from corridor_rotation_core import (
    CorridorSpec,
    DemandSpec,
    PolicySpec,
    RunSpec,
    measure_corridors,
    simulate,
    think_mean_for_load,
)
from plan_core import _Graph
from protoscale import load_scale
from roadmap_core import GenParams, generate_roadmap

CFG = load_scale()
INF = float("inf")

C10 = CorridorSpec("L10_n2_seg5", 2, 5.0, 1.0)
C60 = CorridorSpec("L60_n2_seg30", 2, 30.0, 1.0)

POLICIES = [
    PolicySpec("T15_b1", 15.0, 1.0),
    PolicySpec("T15_b4", 15.0, 4.0),
    PolicySpec("T15_binf", 15.0, INF),
    PolicySpec("T60_b1", 60.0, 1.0),
    PolicySpec("reactive", 0.0, INF),
]


def _spec(policy, corridor=C10, fleet=40, skew=0.7, load=1.0, seed=0, horizon=1800.0):
    think = think_mean_for_load(corridor, fleet, load)
    dem = DemandSpec(fleet=fleet, skew=skew, think_mean_s=think, load_target=load)
    return RunSpec(
        corridor=corridor, demand=dem, policy=policy, seed=seed, horizon_s=horizon, warmup_s=300.0
    )


# ------------------------------------------------------------------ 안전 불변식


@pytest.mark.parametrize("policy", POLICIES)
@pytest.mark.parametrize("corridor", [C10, C60])
def test_no_two_directions_inside_at_once(policy, corridor):
    """🔴 이 측정의 안전 불변식 — 겹치는 시간에 통로 안에 있는 로봇은 전부 같은 방향이다."""
    trace = {}
    simulate(_spec(policy, corridor=corridor), trace=trace)
    entries = trace["entries"]
    assert entries, "진입이 하나도 없으면 불변식 시험이 0건 검사가 된다"
    for i, (t_in, side, t_out) in enumerate(entries):
        for t_in2, side2, _t_out2 in entries[i + 1 :]:
            if t_in2 >= t_out:
                break  # 진입 시각 오름차순이라 이후는 전부 겹치지 않는다
            assert side2 == side, f"{t_in2}에 반대 방향 진입 — 정면 교행"


@pytest.mark.parametrize("policy", POLICIES)
def test_entry_headway_never_violated(policy):
    """연속 진입 간격이 구간 통과 시간(진입 간격) 미만이면 용량 1 구간이 겹친다."""
    trace = {}
    simulate(_spec(policy), trace=trace)
    entries = trace["entries"]
    for (t0, _, _), (t1, _, _) in zip(entries, entries[1:]):
        assert t1 - t0 >= C10.headway_s - 1e-9


@pytest.mark.parametrize("policy", POLICIES)
def test_concurrent_occupancy_never_exceeds_segments(policy):
    """동시에 통로 안에 있는 로봇 수가 구간 수를 넘지 않는다."""
    trace = {}
    simulate(_spec(policy), trace=trace)
    events = []
    for t_in, _side, t_out in trace["entries"]:
        events.append((t_in, 1))
        events.append((t_out, -1))
    events.sort()
    live = 0
    for _t, delta in events:
        live += delta
        assert live <= C10.capacity_robots


# ------------------------------------------------------------------ 정책 의미론


@pytest.mark.parametrize("floor_s,beta", [(15.0, 1.0), (15.0, 2.0), (30.0, 4.0), (5.0, 8.0)])
def test_hold_between_floor_and_cap(floor_s, beta):
    """유지 시간이 바닥 이상 상한 이하다 — 바닥과 편향 상한이 실제로 발화한다."""
    policy = PolicySpec(f"T{floor_s:g}_b{beta:g}", floor_s, beta)
    trace = {}
    simulate(_spec(policy, load=1.2), trace=trace)
    switches = trace["switches"]
    assert len(switches) >= 3, "전환이 세 번 미만이면 유지 시간 시험이 표본 부족이다"
    drain = C10.transit_s
    for a, b in zip(switches, switches[1:]):
        hold = b - a  # 배수 시간이 붙어 있으므로 바닥 <= 유지 + 배수 <= 상한 + 배수
        assert hold >= floor_s - 1e-9
        assert hold <= beta * floor_s + drain + 1e-9


def test_beta_one_is_fixed_period_when_both_sides_are_saturated():
    """β=1이면 전환 간격이 「바닥 + 배수」로 일정하다 — 수요와 무관한 균등 주기.

    🔴 양쪽이 다 포화일 때만 일정하다. 한쪽 줄이 비면 마지막 진입이 앞당겨져 배수가 짧아지고
    간격이 「바닥」까지 줄어든다 — 그 자리는 위 `test_hold_between_floor_and_cap`가 잡는다.
    """
    policy = PolicySpec("T15_b1", 15.0, 1.0)
    trace = {}
    simulate(_spec(policy, fleet=80, skew=0.5, load=3.0), trace=trace)
    steady = [s for s in trace["switches"] if s >= 300.0]  # 예열 구간의 채워지는 과도는 뺀다
    gaps = [round(b - a, 6) for a, b in zip(steady, steady[1:])]
    assert len(set(gaps)) == 1, f"균등 주기인데 간격이 여럿이다: {sorted(set(gaps))[:5]}"


def test_floor_zero_with_finite_beta_is_rejected():
    """🔴 바닥 0 + 유한 편향 상한은 최대 유지 시간 0이라 정의되지 않는다 — 만들 수 없어야 한다."""
    with pytest.raises(ValueError):
        PolicySpec("bad", 0.0, 4.0)


def test_reactive_switches_only_when_current_side_empties():
    """반응형(바닥 0·상한 무한)은 바닥 손실이 0이다 — 빈 쪽을 붙잡고 있는 시간이 없다."""
    res = simulate(_spec(PolicySpec("reactive", 0.0, INF), skew=0.9, load=0.8))
    assert res.floor_block_frac == 0.0


def test_large_floor_produces_floor_block_time():
    """바닥이 크고 수요가 치우치면 「빈 쪽을 붙잡고 있는 시간」이 실제로 발화한다."""
    res = simulate(_spec(PolicySpec("T60_b1", 60.0, 1.0), skew=0.98, load=0.4))
    assert res.floor_block_frac > 0.0


# ------------------------------------------------------------------ 시간 회계


@pytest.mark.parametrize("policy", POLICIES)
def test_state_fractions_partition_the_window(policy):
    """관측창 시간이 네 상태로 남김없이 나뉜다 — 잃은 시간을 셋 중 어디에도 안 넣고 흘리지 않는다."""
    res = simulate(_spec(policy))
    total = res.busy_frac + res.drain_frac + res.floor_block_frac + res.idle_frac
    assert abs(total - 1.0) < 1e-9


def test_saturated_uniform_matches_hand_computation():
    """포화에서 균등 주기의 처리량이 손으로 센 값과 같다 — 시뮬레이터가 딴 것을 재고 있지 않다.

    바닥 T=15 s · 진입 간격 h=5 s · 통과 60/... 이 아니라 통과 10 s(L10)일 때
    한 초록 구간에 진입 3대(0·5·10 s), 마지막 진입이 10 s이므로 배수는 20 s에 끝난다.
    ⇒ 주기 20 s에 3대 ⇒ 540 대/시.

    🔴 **양쪽이 다 포화(치우침 0.5)일 때의 값이다.** 수요가 치우치면 한쪽 초록이 빈 채로 흘러
    총량이 이보다 낮아진다 — 그것은 결함이 아니라 균등 주기의 성질이며 쓸어보기가 재는 대상이다.
    """
    res = simulate(
        _spec(PolicySpec("T15_b1", 15.0, 1.0), fleet=80, skew=0.5, load=3.0, horizon=3600.0)
    )
    assert abs(res.throughput_per_h - 540.0) < 1e-6


def test_unlocked_reference_reaches_pipe_rate_when_saturated():
    """방향 제약을 끈 참조선은 포화에서 관 처리율(3600/진입간격)에 붙는다."""
    res = simulate(_spec(PolicySpec("unlocked", 0.0, INF, unlocked=True), fleet=80, load=3.0))
    assert res.throughput_per_h > 0.99 * 3600.0 / C10.headway_s


# ------------------------------------------------------------------ 결정론·공통 난수


@pytest.mark.parametrize("policy", POLICIES)
def test_determinism(policy):
    """같은 인자를 두 번 돌리면 모든 지표가 같다."""
    a = simulate(_spec(policy, seed=3))
    b = simulate(_spec(policy, seed=3))
    assert a == b


def test_common_random_numbers_across_policies():
    """정책이 달라도 같은 (규모·치우침·부하·시드)면 첫 요청 흐름이 같다.

    부하를 아주 낮춰 관측 구간에 로봇마다 요청이 한 번 이하가 되게 하면,
    요청 수가 정책과 무관하게 같아야 한다 — 아니면 정책 비교가 난수 차이로 오염된다.
    """
    offered = {
        p.label: simulate(_spec(p, fleet=20, load=0.02, horizon=1200.0)).offered_per_h
        for p in POLICIES
    }
    assert len(set(offered.values())) == 1, offered


# ------------------------------------------------------------------ 기존 코드 대조


def test_measured_corridor_geometry_matches_grid_inputs():
    """격자에 쓴 「오늘 값」이 생성기에서 그대로 나온다 — 값을 지어내지 않았음을 고정한다."""
    found = measure_corridors(CFG, lambda s: GenParams(seed=s), range(12))
    assert found
    assert {m.edges for m in found} == {2, 3}
    assert {m.length_m for m in found} == {10.0, 15.0}
    assert {c for m in found for c in m.capacities} == {1}


def test_plan_core_corridor_lock_is_per_timestep_not_per_epoch():
    """🔴 기존 통로 방향 잠금(`plan_core`)의 의미론을 고정한다.

    `plan_core._Reservations.corr_dir`는 **(타임스텝, 통로)** 로 걸린다 — 같은 타임스텝의
    역방향 «이동»만 막는다. 방향 에폭(뒤집을 때까지 한 방향 유지)을 강제하는 장치가 아니다.
    그래서 이 측정은 그 잠금을 그대로 쓰지 못하고, **연속 시간의 배타**를 따로 세운다
    (위 `test_no_two_directions_inside_at_once`가 그 강한 쪽이다).

    이 시험은 그 사실을 코드로 붙잡아 둔다 — 나중에 의미론이 바뀌면 여기서 깨진다.
    """
    from plan_core import _Reservations

    model = generate_roadmap(CFG, GenParams(seed=0))
    graph = _Graph(model)
    corridor_edges = [e for e in model.edges if e.corridor_key]
    assert corridor_edges, "통로 소속 간선이 없으면 이 시험은 0건 검사다"
    edge = corridor_edges[0]
    res = _Reservations(graph, horizon=16)

    # 같은 타임스텝의 역방향 이동은 막힌다
    res.corr_dir[(3, edge.corridor_key)] = 1
    assert res.can_move(3, edge.v, edge.u) is False

    # 그러나 다른 타임스텝이면 같은 통로에서 역방향 이동이 허용된다 — 에폭이 아니다
    fresh = _Reservations(graph, horizon=16)
    fresh.corr_dir[(3, edge.corridor_key)] = 1
    assert fresh.can_move(4, edge.v, edge.u) is True


# ------------------------------------------------------------------ 유도식


@pytest.mark.parametrize("load", [0.4, 0.8, 1.2])
def test_think_mean_for_load_round_trip(load):
    """부하 목표에서 유도한 「일하는 시간」이 그 부하를 되돌려 준다."""
    think = think_mean_for_load(C10, 40, load)
    rate = 40 / (think + C10.transit_s)  # 요청률 [대/s]
    assert abs(rate * C10.headway_s - load) < 1e-9


def test_think_mean_clamped_at_zero_is_reported_not_hidden():
    """부하 목표가 너무 크면 「일하는 시간」이 0으로 잘린다 — 음수를 만들지 않는다."""
    assert think_mean_for_load(C60, 10, 10.0) == 0.0
    assert not math.isnan(think_mean_for_load(C60, 10, 10.0))
