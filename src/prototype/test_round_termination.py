"""라운드 루프 종료성의 **반증기 둘** — `212`§212-6-2 조건 6.

🔴 **두 시험이 서로의 대우를 잡는다.**

| | 무엇 | 왜 |
|:-:|------|-----|
| **A** | 정상 문턱에서 `ROUNDS_EXHAUSTED` 가 **나오면 안 된다** (`212-C1`) | 정리가 주장하는 것 |
| **B** | 문턱을 무한으로 두면 **나와야 한다** (`212`§212-1-3 반례) | 🔴 **A가 「검사 0건」과 구별되게 만드는 것** |

**B가 없으면 A는 통과인지 미실행인지 알 수 없다** — 이 레포에서 반복된 형태다.
그리고 B는 정리의 **귀속**을 코드로 고정한다: 상계를 주는 것은 「단조 집합」이 아니라
**실패 계수기**다. 문턱을 무한으로 두면 `stuck` 은 여전히 단조(늘 공집합)인데 루프는 안 끝난다.

🔴 **부분 통과를 「통과」로 보고하지 않는다.** 시험 하나라도 실패하면 종료 코드 1 이고,
그때 `N passed` 를 인용하는 것은 **거짓이다** — 실제로 그렇게 인용해 관문이 잡았다(`206` F-1).

돌리기: `python3 -m pytest test_round_termination.py -q`
"""
from __future__ import annotations

import pathlib

import pytest

import subgoal_scenarios as S
from subgoal_anim import simulate
from subgoal_core import Disk, Robot, SimParams, State, World

FRAMES = 60
SCAN_FRAMES = 120       # 정체 틱을 찾을 때 훑는 범위
CASES = ["c1", "c2", "c4", "c6", "c7", "c8", "c10", "h1", "h2", "h3", "h4"]
STALLING = ["c4", "c7", "h4"]          # 정체가 실제로 나는 판 — 반례가 발화할 자리


def _patched_module(threshold: str):
    """제자리 고정 문턱만 바꾼 사본을 만든다. 🔴 원본 파일은 건드리지 않는다."""
    src = pathlib.Path(__file__).with_name("subgoal_core.py").read_text()
    # 🔴 «바뀌었는가»가 아니라 «그 줄이 있는가»를 본다 — 문턱 2 를 2 로 두는 대조군에서는
    #    치환이 항등이라 「안 바뀌었다」가 정상이다. 낡음을 잡는 것은 **줄의 존재**다.
    assert "if failcnt[r.key] >= 2:" in src, "문턱 줄을 못 찾았다 — 시험이 낡았다"
    patched = src.replace("if failcnt[r.key] >= 2:", f"if failcnt[r.key] >= {threshold}:")
    ns: dict = {}
    exec(compile(patched, "patched_core", "exec"), ns)
    return ns


def _stalled(name):
    """🔴 **정체 상태를 «찾는다».** 프레임 수를 박아 두면 코어가 바뀔 때 전제가 조용히 깨진다.

    실제로 그렇게 났다 — 시험을 쓴 뒤 `subgoal_core.py` 를 세 번 고쳤고(`ρ` · 포함 봉투 ·
    `stale_rounds`), 60프레임의 `h4` 가 더는 정체가 아니게 되어 **정리가 깨진 것처럼** 실패했다.
    전제 미충족과 정리 반증은 **다른 사건**이고 시험이 그 둘을 갈라 말해야 한다.
    """
    def _held(st):
        # 🔴 「멈춤」이 두 형태로 온다 — 제자리 집합은 **목록**, 상향 통지 경로는 **문자열**(로봇 키).
        #    목록만 보면 후자를 놓치고 「정체가 없다」로 읽는다.
        return bool((isinstance(st, list) and st)
                    or (isinstance(st, str) and st != "ROUNDS_EXHAUSTED"))

    _t, _prm, _w, _rs, hist = simulate(name, SCAN_FRAMES, 4000)
    # 🔴 **마지막 틱만 보면 안 된다** — h4 는 120틱 중 **2틱**만 정체이고 끝에는 안 걸린다.
    #    전체를 훑고, 걸린 틱까지 **다시 굴려** 상태를 그 틱에 맞춘다.
    for i, h in enumerate(hist):
        if _held(h["stuck"]):
            _t, prm, w, rs, _h2 = simulate(name, max(1, i), 4000)
            return prm, w, rs, i, h["stuck"]
    return None


def _port(ns, prm, w, rs):
    """상태를 사본 모듈의 타입으로 옮긴다."""
    w2 = ns["World"](w.y_lo, w.y_hi,
                     [ns["Disk"](k.x, k.y, k.r) for k in w.keepouts],
                     [ns["Disk"](b.x, b.y, b.r) for b in w.blobs])
    rs2 = [ns["Robot"](r.key, ns["State"](r.st.x, r.st.y, r.st.yaw, r.st.v),
                       r.goal, r.prio, ref_y=r.ref_y, start_node=r.start_node)
           for r in rs]
    return ns["SimParams"](**{**prm.__dict__}), w2, rs2


@pytest.mark.parametrize("name", CASES)
def test_a_no_rounds_exhausted(name):
    """A(`212-C1`) — 정상 문턱에서는 라운드 상한을 소진하지 않는다."""
    _t, _prm, _w, _rs, hist = simulate(name, FRAMES, 4000)
    bad = [i for i, h in enumerate(hist) if h["stuck"] == "ROUNDS_EXHAUSTED"]
    assert not bad, f"{name}: 라운드 소진이 틱 {bad[:5]} 에서 났다 — 정리의 반증기가 발화했다"


@pytest.mark.parametrize("name", CASES)
def test_a2_round_bound(name):
    """A′ — 관측된 라운드 수가 상계 `η·N + 1`(문턱 2) 안인가."""
    _t, _prm, _w, rs, hist = simulate(name, FRAMES, 4000)
    bound = 2 * len(rs) + 1
    worst = max(h["rounds"] for h in hist)
    assert worst <= bound, f"{name}: 라운드 {worst} 가 상계 {bound} 를 넘었다"


@pytest.mark.parametrize("name", STALLING)
def test_b_infinite_threshold_exhausts(name):
    """B(§212-1-3 반례) — 🔴 문턱을 무한으로 두면 **반드시** 라운드를 소진해야 한다.

    나오지 않으면 §212-1-3의 반례가 틀렸거나, 시험이 그 체제를 안 밟은 것이다.
    어느 쪽이든 **A의 통과를 믿을 수 없게 된다.**
    """
    got = _stalled(name)
    assert got is not None, (
        f"{name}: 어느 프레임에서도 «정체 상태»가 안 나온다 — 🔴 **전제 미충족이지 정리 반증이 아니다.** "
        f"판이 그 체제를 더는 안 밟거나 코어가 바뀐 것이다. 판을 바꾸거나 이 판을 STALLING 에서 빼라"
    )
    prm, w, rs, _f, _st = got
    ns = _patched_module("10**9")
    p2, w2, rs2 = _port(ns, prm, w, rs)
    out = ns["round_solve"](rs2, w2, p2)
    assert out["stuck"] == "ROUNDS_EXHAUSTED", (
        f"{name}: 문턱을 무한으로 뒀는데 {out['rounds']} 라운드에 끝났다 "
        f"(멈춤={out.get('stuck')}) — 상계가 계수기에서 온다는 귀속이 시험되지 않았다"
    )
    assert not out["tubes"], f"{name}: 소진했는데 열을 내보냈다 — 동의 없는 열이다"


@pytest.mark.parametrize("name", STALLING)
def test_b2_normal_threshold_is_the_difference(name):
    """B′ — 같은 상태에서 **정상 문턱이면** 소진하지 않는다. 둘의 차이가 문턱뿐임을 고정한다."""
    got = _stalled(name)
    assert got is not None, f"{name}: 정체 상태가 안 나온다 — 전제 미충족"
    prm, w, rs, _f, _st = got
    ns = _patched_module("2")                                 # 원본과 같은 값 — 사본 경로만 확인
    p2, w2, rs2 = _port(ns, prm, w, rs)
    out = ns["round_solve"](rs2, w2, p2)
    assert out["stuck"] != "ROUNDS_EXHAUSTED", f"{name}: 정상 문턱인데 소진했다"
