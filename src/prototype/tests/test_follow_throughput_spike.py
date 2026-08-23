"""추종 체제 처리량 스파이크(265d C부) 시험 — 진입 간격 대체 · 손 계산 · 안전 불변식.

🔴 결과의 좋고 나쁨을 판정하지 않는다 — 측정 장치가 재려는 것을 재는지 고정한다.
기존 corridor_rotation_core는 수정하지 않았다 — 이 시험은 파생 스펙이 코어의
의미론(배수-후-전환·전환-선점 P-tie)을 그대로 물려받는지 본다.
"""

from corridor_rotation_core import CorridorSpec, DemandSpec, PolicySpec, RunSpec, simulate
from follow_throughput_spike import FollowCorridorSpec, build_variants, window_entry_counts
from report_rotation import green_entries

F30 = FollowCorridorSpec("L30_follow2", 1, 30.0, 1.0, follow_headway_s=2.0)
X30 = CorridorSpec("L30_excl", 1, 30.0, 1.0)


def _run(corr, policy, horizon, warmup, fleet=80, skew=0.5):
    dem = DemandSpec(fleet=fleet, skew=skew, think_mean_s=0.0)
    trace = {}
    spec = RunSpec(corridor=corr, demand=dem, policy=policy, seed=0,
                   horizon_s=horizon, warmup_s=warmup)
    return simulate(spec, trace=trace), trace


def test_follow_spec_overrides_headway_only():
    assert F30.headway_s == 2.0
    assert F30.transit_s == 30.0
    assert X30.headway_s == 30.0  # 배타 무노드: h = D
    assert X30.transit_s == 30.0


def test_follow_hand_computation_matches():
    """추종 L30·δ=2·T=15·β=1 포화: n=⌈15/2⌉=8, 반주기 44 s, 처리량 8/44x3600.

    관측창을 반주기(44 s)의 정수배로 맞춰 경계 잘림 없이 손 값과 대조한다.
    """
    res, trace = _run(F30, PolicySpec("T15_b1", 15.0, 1.0), horizon=3960.0, warmup=440.0)
    assert abs(res.throughput_per_h - 8 / 44 * 3600.0) < 1e-6
    counts = window_entry_counts(trace, 440.0)
    assert counts and set(counts) == {8}
    assert green_entries(15.0, 2.0) == 8


def test_follow_concurrency_exceeds_one_but_same_direction():
    """추종이 실제로 발화한다(동시 재실 > 1) + 겹치는 시간은 전부 같은 방향."""
    _res, trace = _run(F30, PolicySpec("T15_b1", 15.0, 1.0), horizon=1800.0, warmup=300.0)
    entries = trace["entries"]
    events = []
    for t_in, _side, t_out in entries:
        events.append((t_in, 1))
        events.append((t_out, -1))
    events.sort()
    live = peak = 0
    for _t, delta in events:
        live += delta
        peak = max(peak, live)
    assert peak == 8  # 창당 8대가 전부 안에 있는 순간이 실재한다
    for i, (t_in, side, t_out) in enumerate(entries):
        for t_in2, side2, _ in entries[i + 1:]:
            if t_in2 >= t_out:
                break
            assert side2 == side, "겹치는 시간에 반대 방향 — 정면 교행"


def test_ptie_integer_point_entries_equal_ceil():
    """βT/h가 정수인 자리(βT=30, δ=2 ⇒ 15) — P-tie로 창당 15대(16 아님)."""
    res, trace = _run(F30, PolicySpec("T15_b2", 15.0, 2.0), horizon=3480.0, warmup=580.0)
    counts = window_entry_counts(trace, 580.0)
    assert counts and set(counts) == {15}
    assert green_entries(30.0, 2.0) == 15
    assert abs(res.throughput_per_h - 15 / 58 * 3600.0) < 1e-6


def test_exclusive_hand_computation_and_capacity_one():
    """배타 L30·T=60·β=1 포화: n=2(진입 0·30 s), 반주기 60 s, 처리량 120/h, 동시 1대."""
    res, trace = _run(X30, PolicySpec("T60_b1", 60.0, 1.0), horizon=3600.0, warmup=600.0)
    assert abs(res.throughput_per_h - 120.0) < 1e-6
    counts = window_entry_counts(trace, 600.0)
    assert counts and set(counts) == {2}
    events = []
    for t_in, _side, t_out in trace["entries"]:
        events.append((t_in, 1))
        events.append((t_out, -1))
    events.sort()
    live = 0
    for _t, delta in events:
        live += delta
        assert live <= 1


def test_build_variants_shapes():
    """변형 목록 — 배타 1 + 추종 δ별 + ref 사슬(30 m이면 5 m x 6구간)."""
    vs = build_variants(30.0, (2.0, 4.0), 1.0, 5.0)
    names = [n for n, _ in vs]
    assert names == ["excl", "follow2", "follow4", "ref_seg5"]
    ref = vs[-1][1]
    assert ref.segments == 6 and ref.headway_s == 5.0 and ref.transit_s == 30.0
