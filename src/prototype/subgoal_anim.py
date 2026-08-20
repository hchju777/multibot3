#!/usr/bin/env python3
"""S5 — 서브골 층 **움직이는 시제**.

`round_solve` 를 매 틱 다시 돌려 로봇을 실제로 굴린다. 한 틱은 이렇게 돈다:

  1. 우선순위 순으로 라운드를 돌려 각자 서브골 열을 정한다(사슬 · 되돌림 포함)
  2. 각 로봇이 자기 열의 **첫 서브골로 한 칸** 이동한다
  3. 다음 틱에 다시 계획한다 — **굴러가는 지평**

🔴 **여기 없는 것**: 이차계획 · 추종 오차 · 통신 지연·유실. 이동은 서브골로의 **순간 이동**이라
동역학이 아니다. 튜브는 그 틱의 **공표물**이며 다음 틱에 새로 그려진다.

    python3 subgoal_anim.py h1 --out out/h1.gif
    python3 subgoal_anim.py c1 c6 c10 --frames 40

🔴 탐색용 시제이며 증거가 아니다.
"""

from __future__ import annotations

import argparse
import math
import os
from dataclasses import replace

import matplotlib

matplotlib.use("Agg")
import matplotlib.animation as animation  # noqa: E402
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib.patches import Circle  # noqa: E402

import subgoal_scenarios as S  # noqa: E402
from subgoal_core import SimParams, State, round_solve, static_tube  # noqa: E402
from subgoal_viz import PAL, _setup_font  # noqa: E402

# 🔴 도착 판정 반경은 **한 틱 이동보다 커야** 한다. 작으면 로봇이 목표 창을 뛰어넘고,
#    그다음엔 「목표에서 멀어지지 않기」가 최소 비용이라 그 자리에 굳는다 — 시제에서 실제로
#    났다(A 가 x=6 목표를 지나 12.4 까지 갔다가 멈췄다). 실물에서는 상류가 다음 목표 정점을
#    주므로 이 문제가 없다. 이 시제에는 그 갈아타기가 없어 반경으로 대신한다.
ARRIVE_M = 1.2

# 🔴 **아무도 사라지지 않는다.** 이탈은 «계획에서 빼는 것»이 아니라 **종료 판정**일 뿐이다.
#    빼면 그 순간 남에게 투명해지고, 그것이 c6 의 r5(0.99 m 옆)와 h1 의 호송 전체를 굳혔다.
#    「멀어질 때까지 기다렸다 뺀다」로 바꿔 봤더니 이번엔 호송이 서로 3 m 안에 있어 **아무도
#    못 나갔다.** 빼는 것 자체가 문제였다.
EXIT_PROGRESS_M = 6.0


def _unit(dx, dy):
    n = math.hypot(dx, dy)
    return (1.0, 0.0) if n < 1e-9 else (dx / n, dy / n)


def simulate(name: str, frames: int, budget: int, on_tick=None,
             stale_rounds: int = 0, age_per_round: float = 0.0,
             replan_every: int = 1):
    """틱마다 재계획하고 한 칸 이동. 프레임별 상태를 모아 돌려준다.

    🔴 **틱 루프는 여기 하나뿐이다.** 진단 스크립트가 이 루프를 복사해 두면 수정이 한쪽에만
    들어가 **같은 판이 도구에 따라 다른 답을 낸다** — 실제로 후진 규약 수정이 이 파일에만
    들어가 `h4` 진단이 오염됐다. 진단은 `hist` 를 읽는다.
    """
    title, fn = S.SCENARIOS[name]
    prm, w, rs = fn()
    prm = SimParams(**{**prm.__dict__, "budget": budget})
    hist = []
    last_tubes: dict = {}       # 직전 틱의 공표 — stale_rounds 의 씨앗
    done = set()
    arrived = set()
    lo_y, hi_y = w.y_lo + prm.body_r, w.y_hi - prm.body_r
    for r in rs:
        r.exit_dir = _unit(r.goal[0] - r.st.x, r.goal[1] - r.st.y)
        r.home_goal = r.goal

    def _progress(r):
        return ((r.st.x - r.home_goal[0]) * r.exit_dir[0]
                + (r.st.y - r.home_goal[1]) * r.exit_dir[1])
    # 🔴 **이탈 판정은 「누구도 가까이 없을 때」만 참이다.**
    #    나간 로봇은 계획에서 빠지므로 그 순간 남에게 **투명**해진다. 그래서 거리를 조건에
    #    넣지 않으면 **막 나간 로봇이 뒤차 코앞에서 유령처럼 굳는다** — c6 의 r5(0.99 m 옆)와
    #    h1 의 호송 전체(선두 A 가 x≈16 에서 유령으로 남아 C1 을 막았다)가 같은 자리였다.
    #    🔴 그리고 판정을 `|x|` 로 하면 **위아래로 가는 로봇은 영영 못 나간다** — c6 의 r2 는
    #    연장 목표가 (0,-24) 로 **벽 바깥**이라 y=-19.6 벽에 붙어 v=0.25 로 기었다.
    #    진행 방향 **진척**으로 재고, 이웃이 멀어야 비로소 뺀다.
    stored: dict = {}               # key -> [튜브, 다음에 쓸 색인]
    for tick in range(frames):
        live = list(rs)                 # 🔴 아무도 빼지 않는다
        # 🔴 실험은 **이 루프를 복사하지 않는다** — 훅으로 들어온다. 복사본은 수정이 한쪽에만
        #    들어가 «같은 판이 도구에 따라 다른 답을 내는» 자리를 만든다(§211-13 발견 16).
        if on_tick is not None:
            on_tick(tick, rs, w, prm)
        # 🔴 **도착해 계획에서 빠진 로봇도 여전히 그 자리를 차지한다.** 안 넘기면 남에게
        #    투명해져 뒤차가 그대로 밟고 지나간다 — 「공표 없는 것은 장애물이 아니다」의
        #    두 번째 얼굴이다.
        out = round_solve(live, w, prm, stale_rounds=stale_rounds,
                          age_per_round=age_per_round,
                          seed_tubes=last_tubes) if live else {"ok": True, "rounds": 0,
                                                      "tubes": {}, "res": {}, "share": {}}
        hist.append({
            "pos": {r.key: (r.st.x, r.st.y, r.st.yaw) for r in rs},
            "v": {r.key: r.st.v for r in rs},
            "tubes": {k: [(p[0], p[1]) for p in t.pts[:-1]] for k, t in out.get("tubes", {}).items()},
            "ok": out["ok"], "rounds": out["rounds"],
            "rev": [k for k, v in out.get("res", {}).items() if v.reversed_used],
            "done": set(done), "arrived": set(arrived),
            "stuck": out.get("stuck"), "why": out.get("why"),
            "share": dict(out.get("share", {})),
            "res": {k: (v.kind if v.kind == "OK" else v.cause) for k, v in out.get("res", {}).items()},
        })
        last_tubes = dict(out.get("tubes", {}))
        moved_from = {r.key: (r.st.x, r.st.y) for r in rs}
        # 🔴 **서브골 층의 주기를 제어 주기에서 뗀다.** `replan_every` 틱마다 한 번만 다시 짜고
        #    그 사이에는 **이미 정한 열을 그대로 실행**한다. 예산이 풀리는 대신 **이웃 소식이
        #    낡는다** — 그 낡음이 정확히 오늘 미해결인 「나이」다.
        if tick % replan_every == 0:
            stored = {k: [t, 1] for k, t in out.get("tubes", {}).items()}
        for r in live:
            ent = stored.get(r.key)
            tb = ent[0] if ent else None
            if tb is not None and ent[1] < len(tb.pts) - 1:
                nx0, ny0, _ = tb.pts[ent[1]]
                ent[1] += 1
                tb = type(tb)(tb.owner, [(r.st.x, r.st.y, 0.0), (nx0, ny0, prm.dt_s)],
                              tb.radius, tb.kind)
            else:
                tb = None
            if not tb or len(tb.pts) < 2:
                # 🔴 **제자리에 선 로봇은 속력이 0 이다.** 건너뛰기만 하면 **옛 속력이 그대로
                #    남아** ① 다음 틱 부채꼴이 실제보다 멀리 그려지고 ② 후진 부채꼴은 «정지
                #    선행»(205)을 요구하므로 **영영 안 열린다.** 후진을 허용해도 h2 가 한 틱도
                #    안 쓴 이유가 이것이었다 — 「허용했는데 안 쓴다」는 **기능이 없는 것과
                #    구별되지 않는다.**
                r.st = State(r.st.x, r.st.y, r.st.yaw, 0.0)
                continue
            nx, ny, _ = tb.pts[1]
            dx, dy = nx - r.st.x, ny - r.st.y
            step = math.hypot(dx, dy)
            yaw = math.atan2(dy, dx) if step > 1e-6 else r.st.yaw
            back = step > 1e-6 and (dx * math.cos(r.st.yaw) + dy * math.sin(r.st.yaw)) < 0
            # 후진이면 헤딩을 뒤집지 않는다 — 차체는 앞을 향한 채 물러난다
            if back:
                yaw = r.st.yaw
            # 🔴 **후진은 속력 0 으로 기록한다** — 계획기 내부(`nxt = State(..., 0.0 if back ...)`)와
            #    같은 규약이다. 이동량을 그대로 속력에 넣으면 v>0 이 되고, 후진 부채꼴은 «정지
            #    선행»(205)을 요구하므로 **한 번 물러난 뒤 영영 못 물러난다.** 시제 c10 이 정확히
            #    그 자리에서 틱1에 굳었다 — 계획기는 맞았고 **틱 루프만 어긋나 있었다.**
            r.st = State(nx, ny, yaw, 0.0 if back else step / prm.dt_s)
            # 🔴 **도착은 「그 자리에 영구 주차」가 아니라 「다음 노드로 넘어감」이다.** 주차로
            #    두면 도착한 로봇이 통로 한가운데 서서 뒤차의 길을 막고, 그것이 판이 보려던
            #    것(추월·교행)을 통째로 가린다 — 시제 c8 과 h1 의 꼬리가 전부 이 인공물이었다.
            #    실물에서는 상류가 다음 유도 정점을 준다. 여기서는 **같은 방향으로 목표를
            #    늘려** 그 갈아타기를 흉내 낸다. 장면 밖으로 나가야 비로소 계획에서 뺀다.
            if r.key not in arrived and math.hypot(nx - r.goal[0], ny - r.goal[1]) < ARRIVE_M:
                arrived.add(r.key)
                # 🔴 연장 목표를 **벽 안으로 물린다.** 밖에 두면 로봇이 닿을 수 없는 것을 쫓아
                #    벽에 붙어 긴다 — c6 의 r2 가 목표 (0,-24)·벽 -20 에서 y=-19.6 에 붙어
                #    v=0.25 로 기었다.
                gx = r.goal[0] + 10.0 * r.exit_dir[0]
                gy = min(hi_y, max(lo_y, r.goal[1] + 10.0 * r.exit_dir[1]))
                r.goal = (gx, gy)
        for r in rs:
            if _progress(r) >= EXIT_PROGRESS_M:
                done.add(r.key)         # 종료 판정 전용 — 계획에서 빼지 않는다
        if len(done) == len(rs):
            break
    return title, prm, w, rs, hist


def render(name: str, out_path: str, frames: int, fps: int, budget: int) -> str:
    title, prm, w, rs, hist = simulate(name, frames, budget)
    _setup_font()
    order = sorted(rs, key=lambda z: z.prio)
    col = {r.key: PAL[i % len(PAL)] for i, r in enumerate(order)}

    # 화면은 **원래 판**에 맞춘다 — 이탈선까지 담으면 통로가 실오라기처럼 얇아진다.
    allx = ([r.st.x for r in rs] + [g for r in rs for g in (r.home_goal[0],)]
            + [p[0] for h in hist[:1] for p in h["pos"].values()])
    x0, x1 = min(allx) - 1.5, max(allx) + 1.5
    fig, ax = plt.subplots(figsize=(8.2, max(2.4, (w.y_hi - w.y_lo + 1.4) * 0.85)))

    def update(i):
        ax.clear()
        h = hist[i]
        ax.axhspan(w.y_hi, w.y_hi + 0.25, color="#B4B2A9")
        ax.axhspan(w.y_lo - 0.25, w.y_lo, color="#B4B2A9")
        ax.axhline(0.0, color="#B4B2A9", lw=0.6, ls=(0, (6, 5)))
        for k in w.keepouts:
            ax.add_patch(Circle((k.x, k.y), k.r, fc="#F5C4B3", ec="#993C1D",
                                alpha=0.45, ls="--", lw=1.0))
        for b in w.blobs:
            ax.add_patch(Circle((b.x, b.y), b.r, fc="#888780", ec="#5F5E5A", alpha=0.8))
        for r in order:
            c = col[r.key]
            tb = h["tubes"].get(r.key)
            if tb and len(tb) > 1:
                ax.plot([p[0] for p in tb], [p[1] for p in tb], color=c,
                        lw=prm.body_r * 26, alpha=0.20, solid_capstyle="round", zorder=2)
                ax.plot([p[0] for p in tb], [p[1] for p in tb], color=c, lw=1.3, zorder=3)
            x, y, yaw = h["pos"][r.key]
            fin = r.key in h["done"]
            ax.add_patch(Circle((x, y), prm.body_r, fc=c, ec="white", lw=1.0,
                                alpha=0.35 if fin else 0.95, zorder=5))
            ax.text(x, y, str(r.prio), color="white", ha="center", va="center",
                    fontsize=7, zorder=6)
            if not fin:
                ax.arrow(x, y, 0.42 * math.cos(yaw), 0.42 * math.sin(yaw),
                         head_width=0.1, color=c, lw=0.9, zorder=5)
        tag = f"틱 {i}  ·  라운드 {h['rounds']}"
        if h["rev"]:
            tag += "  ·  후진 " + ",".join(h["rev"])
        if not h["ok"]:
            # 🔴 «미성립»만 적으면 **무엇이 안 됐는지 읽히지 않는다.** 대개는 「누가 제자리에
            #    선다」이고 그것은 실패가 아니라 **양보**다. 이름을 적는다.
            who = h["stuck"]
            tag += "  ·  제자리 " + (",".join(who) if isinstance(who, list) else str(who))
        ax.set_title(f"[{name}] {title}\n{tag}", fontsize=9,
                     color="#27500A" if h["ok"] else "#791F1F")
        ax.set_xlim(x0, x1)
        ax.set_ylim(w.y_lo - 0.5, w.y_hi + 0.5)
        ax.set_aspect("equal")
        ax.tick_params(labelsize=6)
        for s in ax.spines.values():
            s.set_color("#D3D1C7")

    anim = animation.FuncAnimation(fig, update, frames=len(hist),
                                   interval=1000 / fps, blit=False)
    os.makedirs(os.path.dirname(out_path) or ".", exist_ok=True)
    anim.save(out_path, writer=animation.PillowWriter(fps=fps), dpi=90)
    plt.close(fig)
    return f"{out_path}  ({len(hist)}틱)"


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("cases", nargs="*", default=None)
    ap.add_argument("--out", default=None)
    ap.add_argument("--frames", type=int, default=34)
    ap.add_argument("--fps", type=int, default=4)
    ap.add_argument("--budget", type=int, default=4000)
    a = ap.parse_args(argv)
    names = a.cases or ["c1", "c6", "c8", "c10", "h1"]
    for n in names:
        if n not in S.SCENARIOS:
            print(f"[skip] 모르는 판: {n}")
            continue
        out = a.out if (a.out and len(names) == 1) else f"out/{n}.gif"
        print("[ok]", render(n, out, a.frames, a.fps, a.budget))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
