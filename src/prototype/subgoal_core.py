"""S5 — 서브골 층 **알고리즘 코어** (탐색용 시제).

🔴 **이 파일은 경계 스키마를 모른다.** `roadmap_core`·`sadg_core`·`tick_core`와 같은 규율이다 —
경계 JSON 키와 닫힌 어휘가 이 파일에 문자열로 나오지 않는다. 변환은 어댑터의 몫이다.

여기 있는 것 (`204b`§3-4c 정본 · §3-4f 라운드 구조 · `205` 유계 후진 · `206` 뒤쪽 보호):

1. **부채꼴** — 도달 가능 집합. 반경 폭은 가감속이, 각도 폭은 곡률 상한이 정한다.
2. **넷 빼기** — 벽 팽창 · 금지 영역(미릴리스 노드) · 남의 **약속** 튜브 · 비공표 최악 모델.
3. **비용 최소 선택** — 목적지·이탈·속도 변화. 🔴 **편도 여기서 정해진다**(`U14-8`):
   목적지까지 싼 쪽이 이기고, 동점이면 진행 방향 우측.
4. **백트래킹** — 같은 깊이 다음 후보 → 이전 깊이 반경 축소(=감속) → 실패.
5. **라운드 사슬**(`U14-9`) — 한 주기 안에서 우선순위 순으로 여러 번 돈다. 위가 정하면 아래가
   받고, 아래가 막히면 **불가가 위로 올라가** 위가 다시 고른다.
6. **유계 후진**(`205`) — 속도 0에서만 뒤 부채꼴이 열리고, **출발 노드 영역 재진입 금지**.
7. **비대칭 상호 양보**(`CAND-1`) — 1위도 조금 비킨다. **기저 + 덧붙임**(`208-T1`)이며
   공표를 못 받아도 기저는 선다.

🔴 **여기 없는 것**: 이차계획 · 추종 오차 · 통신 지연·유실 모델 · 계측 링버퍼.
궤적은 서브골 열의 **직선 보간**으로 근사한다 — 그래서 「공표 튜브 ⊆ 검사 튜브」 불변식은
이 시제가 **검증하지 않는다**.

🔴 **값의 출처**: 아래 `SimParams`의 기본값은 전부 **시제 전용 임시값**이며 사전등록 상수가
아니다. 어느 것도 계약·정식화에서 온 것이 아니다 — 그 사실을 `provenance()`가 출력한다.
"""

from __future__ import annotations

import math
from dataclasses import dataclass, field, replace

TAU = 2.0 * math.pi


# --------------------------------------------------------------------------- 값

@dataclass(frozen=True)
class SimParams:
    """🔴 전부 **시제 전용 임시값**. 사전등록 상수가 아니고 계약에서 오지도 않았다."""

    dt_s: float = 0.5           # 서브골 사이 시간칸
    horizon: int = 4            # 서브골 열 길이 (개수)
    n_angle: int = 9            # 부채꼴 각도 표본
    n_radius: int = 3           # 부채꼴 반경 표본
    v_max: float = 1.0          # m/s
    a_max: float = 1.0          # m/s^2
    kappa_max: float = 1.0      # 1/m  (최소 회전 반경 1 m)
    body_r: float = 0.35        # 차체 외접 반경
    d_sep: float = 1.0          # 최소 이격 (중심 간)
    budget: int = 400           # 백트래킹 방문 상한
    rounds: int = 6             # 한 주기 안의 라운드 상한 (U14-9)
    w_goal: float = 1.0         # 비용 — 목적지 접근
    w_dev: float = 0.35         # 비용 — 참조선 이탈
    w_dv: float = 0.25          # 비용 — 속도 변화
    w_side: float = 0.02        # 비용 — 우측 편향 (동점 해소용. 작아야 한다)
    yield_base: float = 0.30    # 기저 양보 비율 (208-T1: 공표를 못 받아도 서는 몫)
    reverse_ok: bool = False    # 유계 후진 허용
    v_rev_max: float = 0.3      # 후진 속력 상한
    rear_sense_m: float = 1.5   # 후방 관측 거리 (RG-2). 0이면 후진 불가


def provenance() -> str:
    return (
        "🔴 SimParams 의 모든 값은 **시제 전용 임시값**이다. 사전등록(27) 상수가 아니고 "
        "계약·정식화에서 온 것도 아니다. 결론을 값에 의존시키지 마라."
    )


# --------------------------------------------------------------------------- 자료

@dataclass(frozen=True)
class State:
    x: float
    y: float
    yaw: float
    v: float


@dataclass
class Robot:
    key: str
    st: State
    goal: tuple[float, float]          # 목표 유도 정점 (다음 노드)
    prio: int                          # 작을수록 높다
    ref_y: float | None = None         # 참조선 (통로 중심). None 이면 이탈 항 0
    start_node: tuple[float, float, float] | None = None  # 유계 후진 금지 원 (x,y,r)
    yield_cost: float = 1.0            # 공표하는 양보 비용 (CAND-1a)
    v_cap: float | None = None         # 이 로봇의 속력 상한. None 이면 SimParams.v_max


@dataclass(frozen=True)
class Disk:
    x: float
    y: float
    r: float


@dataclass
class Tube:
    """공표물 — 서브골 열을 차체 폭으로 쓴 것 + 제동 꼬리. (위치, 시각) 열이다(R-F18)."""

    owner: str
    pts: list[tuple[float, float, float]]   # (x, y, t)
    radius: float
    kind: str = "commit"                    # "commit" | "intent"
    age_s: float = 0.0                      # 공표된 뒤 흐른 시간. 0 이면 갓 받은 것

    def at(self, t: float) -> tuple[float, float] | None:
        """시각 t 에서의 위치. 열 밖이면 None."""
        if not self.pts:
            return None
        if t <= self.pts[0][2]:
            return (self.pts[0][0], self.pts[0][1])
        if t >= self.pts[-1][2]:
            return (self.pts[-1][0], self.pts[-1][1])
        for k in range(len(self.pts) - 1):
            t0, t1 = self.pts[k][2], self.pts[k + 1][2]
            if t0 <= t <= t1:
                s = 0.0 if t1 == t0 else (t - t0) / (t1 - t0)
                return (
                    self.pts[k][0] + s * (self.pts[k + 1][0] - self.pts[k][0]),
                    self.pts[k][1] + s * (self.pts[k + 1][1] - self.pts[k][1]),
                )
        return None


@dataclass
class World:
    """벽은 y 상하한으로만 둔다 — 이 시제는 축 정렬 통로만 본다."""

    y_lo: float
    y_hi: float
    keepouts: list[Disk] = field(default_factory=list)   # 미릴리스 노드 영역
    blobs: list[Disk] = field(default_factory=list)      # 비공표 장애물(사람 등)


@dataclass
class Result:
    kind: str                 # "OK" | "FAIL"
    seq: list[tuple[float, float, float]]
    cause: str = ""
    visited: int = 0
    reversed_used: bool = False


# --------------------------------------------------------------------------- 기하

def reach_envelope(tau: float, prm: SimParams) -> float:
    """🔴 **「남의 모델」 — 포함 봉투.** `sector_samples`(「내 후보」)와 **다른 함수다.**

    부채꼴은 §211-31 에서 **타이트**하게 고쳤다(`asin(κ·rr/2)`). 타이트한 것은 **내 후보에는
    옳고 남의 모델로는 위험 방향**이다 — 남이 갈 수 있는 곳을 **적게** 잡으면 안 덮는다.
    부채꼴 하나를 두 용도로 쓰고 있었다(`214`§214-2 지적).

    나이 `tau` 인 공표는 「그때 갈라진 뒤 둘 다 자유롭게 움직였다」로 봐야 한다 — 내 튜브의
    예측과 상대의 실제가 각각 분기점에서 최대 `v_max·tau` 떨어질 수 있으므로 **차이는
    `2·v_max·tau` 이하**다. 🟢 **속도 상한만 쓰므로 상대 상태를 몰라도 성립한다.**

    🔴 **보수적이다. 그것이 요점이다** — `214` 가 (가)를 «매우 보수적»이라 예고했고,
    얼마나 보수적인지는 **재야 아는 것**이다. 🔴 회전·곡률은 안 썼다. 병진 상한만이며
    **이것이 완전한 봉투라고 주장하지 않는다.**
    """
    return 2.0 * prm.v_max * max(0.0, tau)


def check_margin(prm: SimParams) -> float:
    """🔴 **검사 여유 ρ — 「공표 튜브 ⊆ 검사 튜브」(204b§3-4c 증명 대상 6호)의 기하 성분.**

    검사는 서브골 사이를 **직선**으로 보는데 비홀로노믹 로봇은 그 직선을 못 그린다. 한 칸이
    길이 `s` 이고 곡률 상한이 `κ` 면, 끝점의 현 반각은 `α = asin(κs/2)` 이고 그때 원호의
    반지름은 **`R = s/(2 sin α) = 1/κ` 로 고정**이므로 현 밖 이탈은 `R(1−cos α)` 다.

    🔴 **값을 지어낸 것이 아니라 선언된 한계에서 유도된 것이다.** 그리고 이 값은 **원호 계열의
    것**이다 — 계열이 바뀌면 달라진다(실측: 같은 끝점·헤딩에서 에르미트는 이것의 약 0.58배).
    ⇒ 정본은 **어떤 허용 궤적이든 지켜야 하는 상계**여야 하며, 그것은 이 시제가 정할 일이
    아니다. 여기서는 **관측된 계열 중 가장 큰 것**(원호)을 쓴다.

    🔴 **여기 없는 것**: 추종 오차 · QP 의 폴리라인 이탈. 그 둘이 더해져야 완전한 ρ 다.
    """
    s_max = prm.v_max * prm.dt_s
    alpha = math.asin(min(1.0, prm.kappa_max * s_max / 2.0))
    return (1.0 - math.cos(alpha)) / max(prm.kappa_max, 1e-9)


def _seg_disk_hit(p, q, c: Disk, pad: float) -> bool:
    """선분 p-q 가 원 c 를 pad 만큼 부풀린 것과 만나는가."""
    r = c.r + pad
    px, py = p
    qx, qy = q
    dx, dy = qx - px, qy - py
    L2 = dx * dx + dy * dy
    if L2 <= 1e-12:
        return math.hypot(px - c.x, py - c.y) <= r
    s = ((c.x - px) * dx + (c.y - py) * dy) / L2
    s = max(0.0, min(1.0, s))
    return math.hypot(px + s * dx - c.x, py + s * dy - c.y) <= r


def _moving_min_dist(p0, p1, q0, q1) -> float:
    """두 점이 같은 시간칸을 등속 직선으로 갈 때의 **최소 거리**. 해석해다.

    🔴 남을 «끝점 스냅숏»으로만 보면 서로 **뚫고 지나가는 교환**을 못 잡는다 — 내 선분 대 남의
    점은 남이 서 있다고 가정한 것이다. 시제 c6 이 그 자리를 0.97 m(요구 1.00)로 드러냈다.
    """
    dx0, dy0 = p0[0] - q0[0], p0[1] - q0[1]
    dvx = (p1[0] - p0[0]) - (q1[0] - q0[0])
    dvy = (p1[1] - p0[1]) - (q1[1] - q0[1])
    a = dvx * dvx + dvy * dvy
    if a <= 1e-15:
        return math.hypot(dx0, dy0)
    s = max(0.0, min(1.0, -(dx0 * dvx + dy0 * dvy) / a))
    return math.hypot(dx0 + s * dvx, dy0 + s * dvy)


def _wall_hit(p, q, w: World, pad: float) -> bool:
    return (min(p[1], q[1]) - pad < w.y_lo) or (max(p[1], q[1]) + pad > w.y_hi)


def _inside(p, c: Disk) -> bool:
    return math.hypot(p[0] - c.x, p[1] - c.y) <= c.r


# --------------------------------------------------------------------------- 부채꼴

def sector_samples(st: State, prm: SimParams, backward: bool = False,
                   v_cap: float | None = None):
    """도달 가능 집합 표본. 🔴 모양을 고른 것이 아니라 운동학이 준 것이다.

    반경 = v*dt ± a*dt^2/2, 각도 = ± kappa_max * v * dt.
    **후진은 속도 0 에서만 열린다** — 205 판정: 지령 후진은 정지 뒤의 다른 입력이다.
    """
    out = []
    dt = prm.dt_s
    v = prm.v_rev_max if backward else st.v
    base = v * dt
    span = 0.5 * prm.a_max * dt * dt
    lo, hi = max(0.0, base - span), base + span
    # 🔴 **속도 상한을 부채꼴에 건다.** 안 걸면 「이동거리/dt」로 갱신되는 속도가 매 틱 스스로를
    #    키운다(1.0 → 1.25 → 1.56 …). 시제에서 v_max=1.0 인데 1.75 까지 갔고, 부채꼴이 멀어져
    #    좁은 곳에서 후보가 전부 죽어 판이 굳었다. 도상으로도 한 번 계획으로도 안 보였다 —
    #    **틱을 굴려야 드러나는 종류**다.
    hi = min(hi, (prm.v_max if v_cap is None else min(prm.v_max, v_cap)) * dt)
    lo = min(lo, hi)
    if backward:
        if st.v > 1e-6:
            return out                      # 정지 선행 (205 필요조건)
        lo, hi = 0.05, prm.v_rev_max * dt
    # 🔴 **각도 폭은 「한 칸 길이」마다 다르다.** 원호의 관계는 «호 길이 s 동안 헤딩이 κs 돌고,
    #    **끝점은 시작 헤딩에서 그 절반**(κs/2)만큼 벌어진다»다. 예전 코드는 (가) 절반을 안 나눴고
    #    (나) 저속에 `max(base, 0.2)` 바닥을 깔았다 ⇒ 여는 각이 **1.9배(v=1.0) ~ 6.4배(v=0.125)**
    #    과대했다. 그러면 부채꼴이 «도달 가능 집합»이 아니라 **갈 수 없는 곳을 포함한 집합**이 되고,
    #    204b§3-4c 가 부채꼴을 정본으로 삼은 근거("설계 선택이 아니라 실제로 갈 수 있는 곳")가
    #    무너진다. 짧은 칸은 좁게, 긴 칸은 넓게 — 반경 안쪽에서 각을 연다.
    hd = st.yaw + (math.pi if backward else 0.0)
    for j in range(prm.n_radius):
        rr = lo + (hi - lo) * (j / max(1, prm.n_radius - 1)) if prm.n_radius > 1 else hi
        if rr <= 1e-6:
            continue
        hmax = math.asin(min(1.0, prm.kappa_max * rr / 2.0))
        for i in range(prm.n_angle):
            a = hd - hmax + (2 * hmax) * (i / max(1, prm.n_angle - 1))
            out.append((st.x + rr * math.cos(a), st.y + rr * math.sin(a), a, rr))
    return out


# --------------------------------------------------------------------------- 비용

def _cost(r: Robot, prm: SimParams, frm, to, ang: float, rad: float, depth: int) -> float:
    """목적지 · 이탈 · 속도 변화 · 우측 편향.

    🔴 **편은 여기서 정해진다**(U14-8). 목적지 항이 대부분을 가르고, 남으면 우측 항이 가른다.
    우측 가중은 작아야 한다 — 크면 목적지를 이기고 그것은 규칙으로 되돌아가는 것이다.
    """
    gx, gy = r.goal
    d_now = math.hypot(frm[0] - gx, frm[1] - gy)
    d_new = math.hypot(to[0] - gx, to[1] - gy)
    c = prm.w_goal * (d_new - d_now)
    if r.ref_y is not None:
        c += prm.w_dev * abs(to[1] - r.ref_y)
    c += prm.w_dv * abs(rad / prm.dt_s - r.st.v)
    # 진행 방향 기준 우측 = yaw - 90도 쪽. 화면 y 아래가 양이면 부호가 뒤집힌다.
    side = math.sin(ang) * (to[1] - frm[1]) + math.cos(ang) * 0.0
    c += prm.w_side * (-(to[1] - frm[1]) if math.cos(r.st.yaw) >= 0 else (to[1] - frm[1]))
    return c


# --------------------------------------------------------------------------- 탐색

def plan(r: Robot, w: World, others: list[Tube], prm: SimParams,
         yield_share: float = 0.0) -> Result:
    """서브골 열을 만든다. 실패하면 OK 가 아니라 FAIL 이다 — 부분해를 내지 않는다.

    `yield_share` 는 CAND-1 의 양보 몫이며 **부호 있는 미터**다 — 참조선에서 그만큼 밀린
    위치를 선호하게 만든다. 🔴 **부호가 없으면 못 쓴다.** 늘 같은 방향으로 밀면 ① 마주 오는
    둘이 같은 벽으로 몰리고 ② 같은 방향으로 가는 둘은 «각자의 우측»이 같은 쪽이라 나란히
    같은 벽에 붙는다. 편을 정하는 것은 `round_solve` 다 — 거기서만 **쌍**이 보이기 때문이다.
    """
    pad = prm.body_r + check_margin(prm)      # 🔴 벽·금지영역·장애물에도 여유를 얹는다
    visited = [0]
    rev_used = [False]

    def blocked(frm, to, t_from, t_to) -> bool:
        if _wall_hit(frm, to, w, pad):
            return True
        for k in w.keepouts:
            # 🔴 **이미 안에 있으면 「나가는 것」은 막지 않는다.**
            #    구간 교차만 보면 출발점이 안에 있는 순간 모든 후보가 죽어, 목구멍 이탈이
            #    후진 여부와 무관하게 **구조적으로 불가능**해진다(시제 c10 이 이것을 잡았다).
            #    205 판정의 문언 그대로다 — "이탈은 위반이 아니다. 판정하는 것은 진입뿐이다."
            if _inside(frm, Disk(k.x, k.y, k.r + pad)):
                if math.hypot(to[0] - k.x, to[1] - k.y) <= math.hypot(frm[0] - k.x, frm[1] - k.y):
                    return True          # 안에 있는데 더 깊어진다 → 막는다
                continue                 # 멀어진다 → 허용 (이탈)
            if _seg_disk_hit(frm, to, k, pad):
                return True
        for b in w.blobs:
            if _seg_disk_hit(frm, to, b, pad + prm.d_sep - prm.body_r):
                return True
        for tb in others:
            if tb.kind != "commit":
                continue
            p0, p1 = tb.at(t_from), tb.at(t_to)
            if p0 is None or p1 is None:
                continue
            # 🔴 나이만큼 **포함 봉투**로 부풀린다(`INV-3`). 0 이면 오늘과 같다.
            infl = reach_envelope(tb.age_s, prm)
            # 🔴 **둘 다 움직인다.** 남을 끝점으로만 보면 서로 뚫고 지나가는 교환이 통과한다.
            #    제약은 `d ≥ d_sep` 이므로 위반은 `d < d_sep` 이다. `<=` 로 쓰면 **한계에 정확히
            #    놓인 배치가 첫 틱부터 불가**가 된다 — 시제 h3 이 A×C = B×D = 1.00 인 초기
            #    배치에서 틱0에 통째로 굳었다.
            # 🔴 **둘 다 부푼다** — 여유를 두 번 얹는다.
            if _moving_min_dist(frm, to, p0, p1) < prm.d_sep + 2.0 * check_margin(prm) + infl - 1e-9:
                return True
        return False

    def rear_ok(frm, to) -> bool:
        """유계 후진 — 출발 노드 영역 **재진입** 금지. 이탈은 허용(205 목구멍 이탈).

        🔴 **경계가 없으면 후진을 막는다.** 예전에는 `start_node is None` 이면 무제한 후진을
        허용했는데, 그러면 **경계가 기본이 아니라 옵션**이 되어 «안 주면 위반»이 된다.
        시제 h4 에서 앞으로 가려던 호송 둘이 **무제한으로 뒤로 물러나** 판이 풀렸다 —
        설계가 좋아서가 아니라 제약이 안 걸려서였다. 205 의 유계 후진은 **진입한 세그먼트
        위에서** 물러났다 **같은 세그먼트를 완주**하는 것이고, 그 경계 없이는 정의되지 않는다.
        """
        if r.start_node is None:
            return False
        c = Disk(*r.start_node)
        return not (not _inside(frm, c) and _inside(to, c))

    def brake_ok(st: State, t0: float) -> bool:
        """🔴 **지평 끝에서 「실제로 멈출 수 있는가」를 검사한다.**

        지금까지는 열 끝에 「그 자리에 선다」를 **붙이기만** 하고 검사하지 않았다. 그런데
        끝에서 속도가 남아 있으면 **그 자리에 못 선다** — 속도 1.0 이면 멈추는 데 `v²/2a` =
        0.5 m 가 더 든다. 그 거리를 안 보면 **「직전 공표는 항상 유효하다」(재귀 실현가능성)가
        서지 않는다.** 그것이 백트래킹의 바닥이고 예산 절단의 안전 근거다.
        🔴 여기서 서는 것은 **정적 장애물에 대한 절반**이다 — 움직이는 상대에 대해서는
        상대 튜브가 그 시각까지 안 뻗으면 끝점으로 물리므로 **보장이 아니라 근사**다.
        """
        d = st.v * st.v / (2.0 * max(prm.a_max, 1e-9))
        if d < 1e-6:
            return True
        to = (st.x + d * math.cos(st.yaw), st.y + d * math.sin(st.yaw))
        return not blocked((st.x, st.y), to, t0, t0 + 2.0 * d / max(st.v, 1e-6))

    def rec(st: State, depth: int, t0: float, acc):
        if visited[0] >= prm.budget:
            return None
        if depth >= prm.horizon:
            return list(acc) if brake_ok(st, t0) else None
        cands = [(s, False) for s in sector_samples(st, prm, v_cap=r.v_cap)]
        if prm.reverse_ok and prm.rear_sense_m > 0.0:
            cands += [(s, True) for s in sector_samples(st, prm, backward=True, v_cap=r.v_cap)]
        scored = []
        for (x, y, a, rr), back in cands:
            if back and rr > prm.rear_sense_m:
                continue
            eff = replace(r, ref_y=(None if r.ref_y is None else r.ref_y + yield_share))
            scored.append((_cost(eff, prm, (st.x, st.y), (x, y), a, rr, depth), x, y, a, rr, back))
        scored.sort(key=lambda z: z[0])
        for _, x, y, a, rr, back in scored:
            visited[0] += 1
            if visited[0] > prm.budget:
                return None
            t1 = t0 + prm.dt_s
            if blocked((st.x, st.y), (x, y), t0, t1):
                continue
            if back and not rear_ok((st.x, st.y), (x, y)):
                continue
            nxt = State(x, y, a, 0.0 if back else rr / prm.dt_s)
            acc.append((x, y, t1))
            if back:
                rev_used[0] = True
            got = rec(nxt, depth + 1, t1, acc)
            if got is not None:
                return got
            acc.pop()
        return None

    seq = rec(r.st, 0, 0.0, [])
    if seq is None:
        cause = "BUDGET_EXHAUSTED" if visited[0] >= prm.budget else "EMPTY_CANDIDATES"
        return Result("FAIL", [], cause, visited[0], rev_used[0])
    return Result("OK", seq, "", visited[0], rev_used[0])


# --------------------------------------------------------------------------- 사유를 믿을 만하게

def width_verdict(r: Robot, w: World, others: list[Tube], prm: SimParams) -> str | None:
    """🔴 **기하 사전 판정** — 탐색을 한 번도 안 돌리고 「폭이 애초에 모자란가」를 본다.

    중심이 갈 수 있는 폭(벽에서 차체 반경만큼 물러난 띠)에서 상대 튜브가 요구하는 이격을
    빼고 남는 것이 없으면, **비켜도 안 되는 것**이라 사슬을 돌릴 이유가 없다.
    싸고(탐색 0회) 확실하지만 **축 정렬 통로의 정면 대면에만** 답을 준다 — 그 밖에는 None.
    """
    free = (w.y_hi - prm.body_r) - (w.y_lo + prm.body_r)
    if free < 0:
        return "GEOM_NO_WIDTH"
    if not others:
        return None
    # 상대 중심이 이 띠 안에 있으면, 나와 상대가 이격을 두고 나란히 설 수 있어야 한다.
    for tb in others:
        if tb.kind != "commit" or not tb.pts:
            continue
        ys = [p[1] for p in tb.pts]
        if min(ys) < w.y_lo + prm.body_r - 1e-9 or max(ys) > w.y_hi - prm.body_r + 1e-9:
            continue
        if free < prm.d_sep - 1e-9:
            return "GEOM_NO_WIDTH"
    return None


def plan_robust(r: Robot, w: World, others: list[Tube], prm: SimParams,
                yield_share: float = 0.0) -> Result:
    """정밀 → (예산 소진 시) 성긴 재시도. **사유를 믿을 만하게 만드는 것이 목적이다.**

    🔴 예산 소진은 「없다」가 아니라 「다 못 봤다」다. 그것을 그대로 사슬에 넘기면
    ① 막힘으로 오인해 헛되이 양보하거나 ② 막힘 아님으로 오인해 진짜 막힘을 놓친다 —
    시제에서 둘 다 실제로 났다. 그래서 **성기게라도 전수에 가깝게 훑어 본 뒤** 보고한다.
    성긴 격자가 놓치는 좁은 틈은 남지만 그 오류는 **보수 쪽**이다(갈 수 있는데 안 간다).
    """
    g = width_verdict(r, w, others, prm)
    if g is not None:
        return Result("FAIL", [], g, 0, False)
    fine = plan(r, w, others, prm, yield_share)
    if fine.kind == "OK" or fine.cause != "BUDGET_EXHAUSTED":
        return fine
    coarse_prm = replace(prm, n_angle=3, n_radius=1)
    coarse = plan(r, w, others, coarse_prm, yield_share)
    if coarse.kind == "OK":
        return replace(coarse, cause="COARSE_OK")
    return replace(coarse, cause="BLOCKED_COARSE", visited=fine.visited + coarse.visited)


def to_tube(r: Robot, res: Result, prm: SimParams, kind: str = "commit") -> Tube:
    """열을 튜브로. 끝에 **제동 꼬리**를 붙인다 — 갱신이 실패해도 유효한 약속이다."""
    pts = [(r.st.x, r.st.y, 0.0)] + list(res.seq)
    if pts:
        lx, ly, lt = pts[-1]
        pts.append((lx, ly, lt + prm.dt_s * 2.0))     # 정지 연장
    return Tube(r.key, pts, prm.body_r, kind)


# --------------------------------------------------------------------------- 라운드 사슬

def static_tube(key: str, x: float, y: float, prm: SimParams) -> Tube:
    """🔴 **공표하지 않는 것을 「없는 것」으로 두지 않는다** — 제자리 최악 모형(204b `INV-3`).

    점 하나짜리 튜브는 `Tube.at` 이 열 밖을 끝점으로 물리므로 **모든 시각에 그 자리**가 된다.
    실패해서 멈춘 로봇 · 도착해 계획에서 빠진 로봇이 여기 든다.
    """
    return Tube(key, [(x, y, 0.0)], prm.body_r, "commit")


def default_side(r: Robot, w: World) -> float:
    """U14-8 의 편 선택 — **이미 고른 편이 있으면 그쪽, 대칭이 안 깨지면 진행 방향 우측.**

    무조건 우측으로 박으면 이미 위·아래로 갈라져 있던 둘이 **함께 중앙으로 모여** 정면으로
    만난다 — 시제 h3 이 그것으로 4/4 → 2/4 가 됐다.
    """
    mid = 0.5 * (w.y_lo + w.y_hi)
    if r.ref_y is not None and abs(r.ref_y - mid) > 1e-6:
        return 1.0 if r.ref_y > mid else -1.0
    return -1.0 if math.cos(r.st.yaw) >= 0 else 1.0


def preposition_shares(order: list[Robot], w: World, prm: SimParams,
                       share: dict[str, float]) -> None:
    """🔴 **양보는 실패한 뒤에 시작하면 늦다** — 만날 시점의 기하로 **미리** 세운다.

    사슬은 «막혔다»를 받고서야 몫을 키운다. 그런데 횡 이동 속도는 곡률 상한이 묶고
    (틱당 약 `kappa·v·dt²`), 마주 오면 접근 속도는 그 몇 배다 — 시제 h1 에서 중심띠 1.16 m 에
    필요 이격 1.0 을 두고 **벌릴 시간이 없어** 둘이 코앞에서 굳었다. **반응만으로는 못 푼다.**

    그래서 최근접 시점 `t*` 를 풀어 그때의 횡 간격을 보고, 모자란 만큼을 **비용 비율로 나눠
    (CAND-1a) 기저 위에 얹는다**(208-T1 — 얹기만 하고 내리지 않는다). 몫을 «누가 얼마나»로
    가르는 것은 비용이지 규칙이 아니다.
    """
    cap = max(0.0, (w.y_hi - w.y_lo) / 2.0 - prm.body_r)
    look = prm.dt_s * prm.horizon * 2.0            # 이만큼 앞까지만 본다
    for i, a in enumerate(order):
        for b in order[i + 1:]:
            rx, ry = b.st.x - a.st.x, b.st.y - a.st.y
            vx = b.st.v * math.cos(b.st.yaw) - a.st.v * math.cos(a.st.yaw)
            vy = b.st.v * math.sin(b.st.yaw) - a.st.v * math.sin(a.st.yaw)
            opposed = -(math.cos(a.st.yaw) * math.cos(b.st.yaw)
                        + math.sin(a.st.yaw) * math.sin(b.st.yaw))
            vv = vx * vx + vy * vy
            if vv <= 1e-9:
                # 🔴 **전원이 멈추면 상대 속도가 0 이라 최근접 시각이 정의되지 않는다.**
                #    그대로 건너뛰면 예비 양보가 **가장 필요한 순간에 침묵**하고, 몫이 기저에
                #    묶여 판이 영영 굳는다 — 시제 h1(비용 5:1)이 틱46에 여섯 대 전원 정지로
                #    그렇게 굳었다(C2 는 -0.58 까지 갈 수 있는데 -0.30 에 서 있었다).
                #    멈춰 있으면 **지금이 최근접**이며, 서로 닿을 수 있는 거리 안일 때만 본다.
                if opposed <= 0.0 or math.hypot(rx, ry) > prm.d_sep + prm.v_max * prm.dt_s * prm.horizon:
                    continue
                t = 0.0
            else:
                t = -(rx * vx + ry * vy) / vv      # 최근접 시각
                if t <= 0.0 or t > look:
                    continue                       # 멀어지는 중이거나 아직 멀다
            # 🔬 **같은 방향 쌍도 포함한다** — 실험으로 갈랐다. 편이 «로봇별»이면 같은 방향
            #    무리는 **다 같은 쪽**으로 가므로 호송이 통째로 「우측 통행」이 되고, 마주 오는
            #    쪽은 제 우측으로 간다. 빼면 h1 이 43틱 6/6 → 120틱 3/6 으로 나빠졌다.
            dy_meet = ry + vy * t                  # 만날 때의 횡 간격 (b - a)
            need = prm.d_sep - abs(dy_meet)
            if need <= 1e-6:
                continue
            # 🔴 **편은 로봇별 규칙(U14-8)으로 정한다 — 쌍별로 정하지 않는다.**
            #    「둘은 반드시 반대 편으로」를 쌍마다 배정해 봤더니 **여러 만남에 걸쳐 합쳐지지
            #    않았다**: h3 에서 (B,C)가 정한 C 의 편과 (C,D)가 정한 편이 어긋나고, 큰 쪽을
            #    취하니 C 와 D 가 0.2 m 로 남아 **4/4 → 0/4** 가 됐다. 전역 배정 문제를 쌍별
            #    탐욕으로 푼 것이다. 측정이 이전 규칙이 낫다고 말한다.
            ca = max(a.yield_cost, 1e-6)
            cb = max(b.yield_cost, 1e-6)
            # 🔴 **몫은 «절대 목표»가 아니라 «덧붙임»이다**(208-T1 의 「기저 + 덧붙임」).
            #    부족분을 절대 목표로 주면, 이미 나가 있는 로봇에게 **되돌아오라**는 뜻이 되고
            #    단조 폐포가 그것을 막아 **아무도 안 움직인다** — 시제 h1(비용 5:1) 틱46 에서
            #    B 가 +0.58, C2 가 -0.31 로 간격 0.89(요구 1.0, 부족 0.11)인데 몫은 0.055 로
            #    계산돼 기저 0.30 에 묶였다. 지금 나가 있는 만큼 위에 부족분을 얹는다.
            off_a = max(0.0, default_side(a, w) * (a.st.y - (a.ref_y or 0.0)))
            off_b = max(0.0, default_side(b, w) * (b.st.y - (b.ref_y or 0.0)))
            ta = min(cap, off_a + need * cb / (ca + cb))
            tb = min(cap, off_b + need * ca / (ca + cb))
            # 🔴 **상한에 잘린 몫은 상대가 떠안는다.** 안 그러면 둘의 합이 요구 이격을 못 채우고
            #    그 부족이 **조용히** 난다 — 비용비 5:1 인 h1 에서 0.167+0.580 = 0.747 로 0.253 m
            #    모자랐고 판이 6/6 → 2/6 으로 무너졌다. 배분비는 «누가 얼마나»만 정할 뿐
            #    **피복을 면제하지 않는다**(208§208-4 #2 의 짝 정합). 둘 다 상한이면 그때는
            #    정말로 못 지나가는 것이고, 그것은 사슬이 아니라 상류가 풀 자리다.
            short = (off_a + off_b + need) - (ta + tb)
            if short > 1e-9:
                grow = min(cap - ta, short)
                ta += grow
                tb = min(cap, tb + short - grow)
            if ta >= abs(share[a.key]):
                share[a.key] = default_side(a, w) * ta
            if tb >= abs(share[b.key]):
                share[b.key] = default_side(b, w) * tb


def round_solve(robots: list[Robot], w: World, prm: SimParams, verbose: bool = False,
                statics: list[Tube] | None = None,
                stale_rounds: int = 0, age_per_round: float = 0.0,
                seed_tubes: dict | None = None):
    """한 제어 주기 — 우선순위 순으로 여러 라운드를 돌린다(U14-9).

    위가 정하면 아래가 그 튜브를 받고, 아래가 실패하면 **불가가 위로 올라가** 위가 양보 몫을
    키워 다시 고른다. 🔴 종료성은 **증명되지 않았다** — 라운드 상한이 그것을 대신한다.

    `statics` 는 계획 집합 **밖**의 점유물(도착해 빠진 로봇 등). 이것을 안 넘기면 그 로봇은
    남에게 **투명**해진다.

    🔴 `stale_rounds=1` 은 `214`§214-2-2 의 반례 **`214-W1`** 을 **실행으로** 재현한다 —
    「나는 직전 라운드의 공표만 받았는데 상대는 이번 라운드의 약속을 실행한다」.
    `age_per_round` 는 그 낡음에 매길 나이다. 🔴 **0 이면 낡은 정보를 «갓 받은 것»처럼 쓴다** —
    그것이 오늘 `INV-3` 가 함수를 안 정한 상태이며, 반례가 노리는 자리다.
    """
    base_static = list(statics or [])
    order = sorted(robots, key=lambda z: z.prio)
    # 🔴 **기저에서 출발한다**(CAND-1 · 208-T1). 1위의 양보는 아래의 실패에 대한 **반응이 아니라
    #    기본 동작**이다. 0 에서 출발하면 1위가 중앙선을 차지하고, 빠듯한 통로에서 아래가
    #    지나갈 자리가 남지 않는다 — 시제 c2 가 그것을 잡았다.
    #    맨 아래(가장 낮은 우선순위)는 원래 다 지므로 기저를 얹지 않는다.
    lowest = max(r.prio for r in order) if order else 0
    share = {r.key: (prm.yield_base * default_side(r, w) if r.prio < lowest else 0.0)
             for r in order}
    preposition_shares(order, w, prm, share)      # 기저 위에 «만날 기하»를 얹는다
    log = []
    # 🔴 **실패해서 멈춘 로봇은 「막힌 것」으로 놓고 위쪽이 다시 고른다** — 이것이 되돌림의
    #    본체다. 안 넣으면 아래가 공표를 못 한 사이 위가 **서 있는 그 로봇을 밟고 지나간다**
    #    (시제 c6·h1·h3 이 셋 다 같은 모양으로 잡혔다: 중심 간 0.19·0.14·0.27 m).
    #    🔴 **라운드를 첫 실패에서 끊지 않는다.** 끊으면 그 아래는 계획조차 안 되어 `stuck`
    #    에도 `tubes` 에도 없고, 결국 **똑같이 투명**해진다 — 시제 c6 이 그 자리를 잡았다
    #    (실패자는 r2 인데 충돌은 r1×r3 이었고 r3 은 한 번도 고려된 적이 없다).
    #    🔴 **멈춘 집합은 한 주기 안에서 단조**다. 라운드마다 뒤바뀌게 두면 고정점에 닿지 못하고
    #    (시제 h1 이 여섯 라운드를 그렇게 헛돌아 **믿을 결과가 하나도 없이** 판이 굳었다),
    #    그러면 「누가 서 있는가」에 모두가 동의하지 못한다. 두 번 실패하면 그 틱은 제자리다 —
    #    한 번에 park 하지 않는 것은 **사슬이 양보로 풀 기회를 한 번은 줘야** 하기 때문이다.
    #    park 된 로봇은 **계획에서 아예 뺀다.** 남들은 정지로 보는데 본인은 움직이면 그 어긋남이
    #    그대로 충돌이다.
    stuck: dict[str, Tube] = {}
    failcnt: dict[str, int] = {}
    why = None
    # 🔴 **씨앗이 없으면 라운드 0 에 이웃 정보가 하나도 없다** — 그러면 전원이 자유롭게 성공해
    #    라운드 하나로 끝나고 **부풀림이 발화할 기회 자체가 없다.** 실제로 그렇게 났다.
    #    직전 «틱»의 공표를 들고 시작해야 「낡은 것을 갖고 있다」가 모형이 된다.
    prev_tubes: dict[str, Tube] = dict(seed_tubes or {})
    # 🔴 **상계를 주는 것은 「단조 집합」이 아니라 아래의 실패 계수기다**(212§212-1-3 반례:
    #    문턱을 무한으로 두면 stuck 은 여전히 단조인데 루프가 안 끝난다 — 시험으로 고정했다).
    #    문턱 η 회 실패에 park 이므로 **실패 사건 ≤ ηN**, 따라서 **라운드 ≤ ηN + 1** 이다.
    #    (예전 주석은 「최악 2N 라운드」였고 코드는 2N+2 를 썼다 — 세 수가 다 달랐다. 212§212-0-3)
    max_rounds = max(prm.rounds, 2 * len(order) + 2)
    for rd in range(max_rounds):
        tubes: dict[str, Tube] = {}
        res: dict[str, Result] = {}
        for r in order:
            if r.key in stuck:
                continue                    # 이 틱은 제자리. 남들에게는 정적 장애물로만 보인다
            upper = base_static + list(stuck.values())
            if stale_rounds > 0:
                # 🔴 직전 라운드 공표만 받은 것으로 둔다 (214-W1 재현)
                upper += [replace(prev_tubes[o.key], age_s=age_per_round)
                          for o in order if o.prio < r.prio and o.key in prev_tubes]
            else:
                upper += [tubes[o.key] for o in order if o.prio < r.prio and o.key in tubes]
            out = plan_robust(r, w, upper, prm, yield_share=share[r.key])
            res[r.key] = out
            if out.kind == "OK":
                tubes[r.key] = to_tube(r, out, prm)
        prev_tubes = dict(tubes)
        fails = [r for r in order if r.key in res and res[r.key].kind != "OK"]
        log.append((rd, {k: v.kind for k, v in res.items()}, dict(share)))
        if not fails:
            # 계획한 전원이 성립했다 — 서 있는 쪽까지 포함해 **모두가 같은 그림**을 본다.
            return {"ok": not stuck, "rounds": rd + 1, "tubes": tubes, "res": res, "log": log,
                    "share": share, "stuck": (sorted(stuck) or None),
                    "why": why if stuck else None}
        for r in fails:
            failcnt[r.key] = failcnt.get(r.key, 0) + 1
            if failcnt[r.key] >= 2:
                stuck[r.key] = static_tube(r.key, r.st.x, r.st.y, prm)
        failed = fails[0]                    # 사슬이 반응하는 것은 가장 위의 실패
        # 🔴 **사슬은 실패 사유를 봐야 한다.** 양보로 풀리는 것은 「막힘」뿐이다.
        #    예산 소진은 해상도·예산 문제라 위쪽이 아무리 비켜도 안 풀린다 — 시제 c6 에서
        #    여섯 라운드가 그렇게 헛돌았다. 사유를 안 보면 「막힘」과 구별이 안 된다.
        cause = res[failed.key].cause
        if cause == "GEOM_NO_WIDTH":
            why = "폭이 애초에 모자란다 — 비켜도 안 된다. 상류로 올릴 자리다"
            continue
        if cause == "BUDGET_EXHAUSTED":
            why = "예산 소진 — 성긴 재시도에도 안 걸렸다면 여기 오지 않는다"
            continue
        # 불가가 위로 — 실패자보다 높은 쪽들이 조금씩 더 비킨다 (CAND-1 · 기저 + 덧붙임)
        higher = [o for o in order if o.prio < failed.prio and o.key not in stuck]
        if not higher:
            why = "가장 위가 막혔다 — 양보로 풀 자리가 없다"
            continue
        # 🔴 **양보에 상한이 있다.** 없으면 사슬이 「안 되면 더 비켜라」만 알고 「더 비킬 데가
        #    없다」를 모른다 — 시제 h3 에서 1위 양보를 1.5 까지 키웠는데 통로 반폭이 1.4 라
        #    벽을 뚫으라는 요구가 됐고, 그 바람에 **원래 되던 2위가 실패로 돌아섰다.**
        #    양보가 판을 고치는 게 아니라 **망치는 구간**이 있다.
        cap = max(0.0, (w.y_hi - w.y_lo) / 2.0 - prm.body_r)
        step = prm.yield_base
        grew = False
        for o in higher:
            sg = default_side(o, w) if abs(share[o.key]) < 1e-9 else math.copysign(1.0, share[o.key])
            want = abs(share[o.key]) + step * (1.0 if o.prio == failed.prio - 1 else 0.5)
            new = min(want, cap)
            if new > abs(share[o.key]) + 1e-9:
                grew = True
            share[o.key] = sg * new          # 🔴 키우되 **편은 안 바꾼다**
        if not grew:
            why = "양보가 상한에 닿았다 — 더 비킬 데가 없다"
    # 🔴 라운드를 다 써도 **직전까지의 결과를 버리지 않는다.** 다만 내보내는 것은 「멈춘 집합에
    #    모두가 동의한」 라운드의 것뿐이다 — 동의 없는 열은 그대로 충돌이다.
    # 여기 오면 단조 집합이 상한 안에 고정점에 못 닿은 것이다 — **도달 불가에 가깝고**,
    # 오면 내보낼 열이 없다. 빈 것을 내보내되 그 사실을 감추지 않는다.
    return {"ok": False, "rounds": max_rounds, "tubes": {}, "res": {}, "log": log,
            "share": share, "stuck": "ROUNDS_EXHAUSTED", "why": why}
