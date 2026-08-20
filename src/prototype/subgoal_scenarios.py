"""S5 — 서브골 층 시나리오 실행기 (탐색용 시제).

`subgoal_core` 를 써서 지금까지 **도상으로만** 검토한 판들을 실제로 돌린다.
🔴 **값은 전부 시제 전용 임시값이다**(`subgoal_core.provenance()`). 결론을 값에 의존시키지 마라.
🔴 **이 실행기는 판정하지 않는다.** 각 판의 산출(성공 여부·라운드 수·양보 몫·후진 사용)만 적고
「통과/실패」 어휘를 쓰지 않는다.

돌리기: `python3 subgoal_scenarios.py [이름]`
"""

from __future__ import annotations

import math
import sys

from subgoal_core import (Disk, Robot, SimParams, State, World, provenance,
                          round_solve)


def _fmt(out) -> str:
    head = f"라운드 {out['rounds']}  결과 {'성립' if out['ok'] else '미성립'}"
    if not out["ok"]:
        head += f"  멈춘 자리 {out.get('stuck')}"
    sh = {k: round(v, 2) for k, v in out["share"].items() if v > 0}
    if sh:
        head += f"  양보 몫 {sh}"
    rev = [k for k, r in out.get("res", {}).items() if r.reversed_used]
    if rev:
        head += f"  후진 사용 {rev}"
    return head


def _paths(out) -> str:
    lines = []
    for k, t in sorted(out.get("tubes", {}).items()):
        pts = ", ".join(f"({p[0]:.2f},{p[1]:+.2f})" for p in t.pts[:-1])
        lines.append(f"    {k}: {pts}")
    return "\n".join(lines)


# --------------------------------------------------------------------------- 판

def c1_headon_wide(prm=None):
    """1 — 정면 대면 · 폭 여유. 노드는 양 끝이고 둘 다 중앙선으로 진입한다."""
    prm = prm or SimParams()
    w = World(y_lo=-1.2, y_hi=1.2)
    rs = [Robot("A", State(-3, 0, 0, 1.0), (6, 0), 0, ref_y=0.0),
          Robot("B", State(3, 0, math.pi, 1.0), (-6, 0), 1, ref_y=0.0)]
    return prm, w, rs


def c2_headon_tight(prm=None):
    """2 — 정면 대면 · 폭 빠듯. 차체 둘 + 이격을 겨우 넘는다."""
    prm = prm or SimParams()
    half = prm.body_r + prm.d_sep / 2 + 0.05
    w = World(y_lo=-half, y_hi=half)
    rs = [Robot("A", State(-3, 0, 0, 1.0), (6, 0), 0, ref_y=0.0),
          Robot("B", State(3, 0, math.pi, 1.0), (-6, 0), 1, ref_y=0.0)]
    return prm, w, rs


def c4_convoy_block(prm=None):
    """4 — 같은 방향 호송 셋 + 앞에 사람(비공표 장애물)."""
    prm = prm or SimParams()
    w = World(y_lo=-1.2, y_hi=1.2, blobs=[Disk(2.0, 0.0, 0.3)])
    rs = [Robot("A", State(-1.0, 0, 0, 1.0), (6, 0), 0, ref_y=0.0),
          Robot("B", State(-2.5, 0, 0, 1.0), (6, 0), 1, ref_y=0.0),
          Robot("C", State(-4.0, 0, 0, 1.0), (6, 0), 2, ref_y=0.0)]
    return prm, w, rs


def c6_swap8(prm=None):
    """6 — 원형 교환 8대. 모든 직선이 중심을 지난다."""
    prm = prm or SimParams()
    # 🔴 **열린 공간이라면 상자가 열려 있어야 한다.** ±6 으로 두면 교환을 마친 로봇들이
    #    벽에 붙어 끝나고, 그것을 「열린 공간에서의 거동」으로 읽게 된다.
    w = World(y_lo=-20.0, y_hi=20.0)
    R, rs = 4.0, []
    for i in range(8):
        th = TAU_ = i * (2 * math.pi / 8)
        x, y = R * math.cos(th), R * math.sin(th)
        rs.append(Robot(f"r{i}", State(x, y, th + math.pi, 1.0), (-x, -y), i, ref_y=None))
    return prm, w, rs


def c7_node_keepout(prm=None):
    """7 — 앞 노드가 미릴리스라 금지 영역이다."""
    prm = prm or SimParams()
    w = World(y_lo=-1.2, y_hi=1.2, keepouts=[Disk(2.0, 0.0, 0.6)])
    rs = [Robot("A", State(-2.0, 0, 0, 1.0), (6, 0), 0, ref_y=0.0)]
    return prm, w, rs


def c8_slow_leader(prm=None):
    """8 — 느린 선행 + 뒤차. 추월이 비용에서 나오는가."""
    prm = prm or SimParams()
    w = World(y_lo=-1.2, y_hi=1.2)
    # 🔴 목표를 통로 **밖**으로 뺀다. 안에 두면 먼저 도착한 쪽이 한가운데 서서 뒤차의 길을
    #    막는데, 실물에서는 상류가 다음 노드를 주어 그 로봇이 **떠난다.** 이 시제엔 그
    #    갈아타기가 없어 「도착 = 영구 주차」가 된다 — 판이 보려던 추월을 가린다.
    # 🔴 **선행이 계속 느려야 추월할 일이 생긴다.** 예전엔 v=0.15 로 «출발»만 시켰는데
    #    가속 상한이 1.0 이라 **한 틱 만에 따라잡아 계속 앞섰다** — 추월이 필요 없는 판이었고
    #    그런데도 「추월 판」이라 이름 붙어 있었다.
    rs = [Robot("A", State(0.0, 0, 0, 0.15), (12, 0), 0, ref_y=0.0, v_cap=0.30),
          Robot("B", State(-1.6, 0, 0, 1.0), (12, 0), 1, ref_y=0.0)]
    return prm, w, rs


def c10_throat(prm=None):
    """10 — 목구멍 이탈. 노드 영역 **안에** 정지해 있고 앞은 막혔다.

    🔴 후진이라야 풀리는 자리(205). reverse_ok 를 끄고 켜서 갈리는지 본다.
    """
    prm = prm or SimParams(reverse_ok=True)
    w = World(y_lo=-1.0, y_hi=1.0, keepouts=[Disk(1.2, 0.0, 0.9)])
    # 🔴 목표는 **금지 영역과 출발 노드 사이**에 둔다. 예전엔 목표가 출발 노드 **너머**라
    #    「가려면 반드시 재진입해야 하는데 재진입은 위반」이 되어 **판 자체가 모순**이었다.
    #    목구멍 이탈이 보려던 것은 «나오는 것»이지 «되돌아가는 것»이 아니다.
    rs = [Robot("A", State(0.55, 0.0, 0.0, 0.0), (-2.0, 0), 0, ref_y=0.0,
                start_node=(-3.0, 0.0, 0.5))]
    return prm, w, rs


SCENARIOS = {
    "c1": ("정면 대면 · 폭 여유", c1_headon_wide),
    "c2": ("정면 대면 · 폭 빠듯", c2_headon_tight),
    "c4": ("호송 + 일시 차단", c4_convoy_block),
    "c6": ("원형 교환 8대", c6_swap8),
    "c7": ("미릴리스 노드 금지 영역", c7_node_keepout),
    "c8": ("느린 선행 · 추월", c8_slow_leader),
    "c10": ("목구멍 이탈 · 후진", c10_throat),
}


def run(name: str, show_paths: bool = True):
    title, fn = SCENARIOS[name]
    prm, w, rs = fn()
    out = round_solve(rs, w, prm)
    print(f"[{name}] {title}")
    print("   ", _fmt(out))
    if show_paths and out.get("tubes"):
        print(_paths(out))
    return out


def main() -> int:
    print(provenance())
    print()
    names = sys.argv[1:] or list(SCENARIOS)
    for n in names:
        if n not in SCENARIOS:
            print(f"[skip] 모르는 판: {n}")
            continue
        run(n)
        print()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())


# --------------------------------------------------------------------------- 어려운 판

def h1_deep_convoy(prm=None):
    """H1 — 호송 다섯이 마주 오는 하나와 좁은 통로에서. 사슬이 깊어야 풀린다."""
    prm = prm or SimParams(budget=4000)
    half = prm.body_r + prm.d_sep / 2 + 0.08
    w = World(y_lo=-half, y_hi=half)
    rs = [Robot("A", State(-1.0, 0, 0, 1.0), (8, 0), 0, ref_y=0.0)]
    for i in range(1, 5):
        rs.append(Robot(f"C{i}", State(-1.0 - 1.5 * i, 0, 0, 1.0), (8, 0), i + 1, ref_y=0.0))
    rs.append(Robot("B", State(3.0, 0, math.pi, 1.0), (-8, 0), 1, ref_y=0.0))
    return prm, w, rs


def h2_pinch(prm=None):
    """H2 — 마주 오는 둘 + 통로 한쪽을 먹은 장애물. 1위가 크게 비켜야만 풀린다."""
    # 🔴 **후진을 켜고 후진 경계를 준다.** 핀치(자유 폭 0.20 m)는 동시 통과가 기하로
    #    불가능해 **순차 통과**뿐인데, 그러려면 한쪽이 «물러나» 줘야 한다. 후진이 바로 그
    #    종 방향 수단이다. 🔬 실험: 금지 0/2 · 허용하되 경계 없음 0/2 · **허용 + 경계 2/2(55틱,
    #    후진 12틱)**. 경계가 없으면 `rear_ok` 가 후진 자체를 막으므로(205) 셋째가 필요하다.
    prm = prm or SimParams(budget=4000, reverse_ok=True)
    # 사람을 **벽에 붙인다**(0.62 → 0.85). 예전 자리는 필요 이격 1.30 이 중심띠 바닥
    # (-0.65)을 **3 cm 넘겨** 판이 불가능했다. 통로가 완전히 막히는 판은 c4 가 맡는다.
    w = World(y_lo=-1.0, y_hi=1.0, blobs=[Disk(0.6, 0.85, 0.30)])
    rs = [Robot("A", State(-2.6, 0, 0, 1.0), (8, 0), 0, ref_y=0.0,
                start_node=(-6.0, 0.0, 0.5)),
          Robot("B", State(2.6, 0, math.pi, 1.0), (-8, 0), 1, ref_y=0.0,
                start_node=(6.0, 0.0, 0.5))]
    return prm, w, rs


def h3_two_pairs(prm=None):
    """H3 — 두 쌍이 동시에 교차. 우선순위가 서로 얽힌다."""
    prm = prm or SimParams(budget=4000)
    w = World(y_lo=-1.4, y_hi=1.4)
    rs = [Robot("A", State(-3.0, -0.4, 0, 1.0), (8, -0.4), 0, ref_y=-0.4),
          Robot("B", State(3.0, 0.4, math.pi, 1.0), (-8, 0.4), 1, ref_y=0.4),
          Robot("C", State(-3.6, 0.4, 0, 1.0), (8, 0.4), 2, ref_y=0.4),
          Robot("D", State(3.6, -0.4, math.pi, 1.0), (-8, -0.4), 3, ref_y=-0.4)]
    return prm, w, rs


def h4_reverse_needed(prm=None):
    """H4 — 1위가 금지 영역 안에 갇혔는데 뒤에 호송이 붙어 있다.

    🔴 후진이 필요한데 뒤가 막혀 있다 — 후진과 사슬이 동시에 걸리는 판.
    """
    prm = prm or SimParams(budget=4000, reverse_ok=True)
    w = World(y_lo=-0.9, y_hi=0.9, keepouts=[Disk(1.3, 0.0, 0.95)])
    # 🔴 c10 과 같은 모순을 닫는다 — 목표를 출발 노드 **앞**에 둔다.
    rs = [Robot("A", State(0.55, 0.0, 0.0, 0.0), (-2.5, 0), 0, ref_y=0.0,
                start_node=(-4.0, 0.0, 0.5)),
          # 🔴 **호송에게도 후진 경계를 준다.** 없으면 `rear_ok` 가 후진을 막아(205) 「물러나
          #    달라」는 요청 자체가 성립하지 않는다 — 그러면 갇힌 1위가 영영 못 나온다.
          #    🔬 경계 없음 0/3 → 경계 있음 **1/3**(A 탈출, 후진 139틱). 나머지 둘은 미릴리스
          #    노드가 통로를 완전히 막아(필요 1.30 · 중심띠 ±0.55) **기다리는 것이 정답**이다.
          Robot("C1", State(-0.9, 0.0, 0.0, 0.0), (8, 0), 1, ref_y=0.0,
                start_node=(-6.0, 0.0, 0.5)),
          Robot("C2", State(-2.2, 0.0, 0.0, 0.0), (8, 0), 2, ref_y=0.0,
                start_node=(-7.0, 0.0, 0.5))]
    return prm, w, rs


SCENARIOS.update({
    "h1": ("호송 다섯 + 마주 하나 · 좁음", h1_deep_convoy),
    "h2": ("마주 둘 + 한쪽 장애물", h2_pinch),
    "h3": ("두 쌍 동시 교차", h3_two_pairs),
    "h4": ("갇힌 1위 + 뒤 호송", h4_reverse_needed),
})
