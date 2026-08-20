#!/usr/bin/env python3
"""S5 — 서브골 시나리오 **그림판** (탐색용 시제).

`subgoal_scenarios` 의 판을 돌린 결과를 그림으로 낸다. 로봇 시작 위치, 벽, 금지 영역,
비공표 장애물, 그리고 각 로봇이 공표한 **튜브**(서브골 열을 차체 폭으로 쓴 것)를 그린다.

    python3 subgoal_viz.py                 # 열한 판을 한 장에
    python3 subgoal_viz.py h1 --out h1.png # 한 판만 크게

🔴 **탐색용 시제이며 증거가 아니다.** 값은 전부 시제 전용 임시값이다.
"""

from __future__ import annotations

import argparse
import math

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib.patches import Circle  # noqa: E402

import subgoal_scenarios as S  # noqa: E402
from subgoal_core import SimParams, round_solve  # noqa: E402
from viz import _pick_korean_font  # noqa: E402

PAL = ["#534AB7", "#0F6E56", "#993C1D", "#854F0B", "#993556",
       "#185FA5", "#3B6D11", "#5F5E5A"]


def _setup_font() -> None:
    fam = _pick_korean_font()
    if fam:
        plt.rcParams["font.family"] = fam
    plt.rcParams["axes.unicode_minus"] = False


def draw_case(ax, name: str, budget: int = 4000) -> dict:
    title, fn = S.SCENARIOS[name]
    prm, w, rs = fn()
    prm = SimParams(**{**prm.__dict__, "budget": budget})
    out = round_solve(rs, w, prm)

    xs = [r.st.x for r in rs] + [r.goal[0] for r in rs]
    x0, x1 = min(xs) - 1.0, max(xs) + 1.0
    if x1 - x0 > 14:                       # 목표가 멀면 화면을 로봇 주변으로 좁힌다
        c = sum(r.st.x for r in rs) / len(rs)
        x0, x1 = c - 5.5, c + 5.5

    ax.axhspan(w.y_hi, w.y_hi + 0.25, color="#B4B2A9")
    ax.axhspan(w.y_lo - 0.25, w.y_lo, color="#B4B2A9")
    ax.axhline(0.0, color="#B4B2A9", lw=0.6, ls=(0, (6, 5)))

    for k in w.keepouts:
        ax.add_patch(Circle((k.x, k.y), k.r, fc="#F5C4B3", ec="#993C1D",
                            alpha=0.45, ls="--", lw=1.0))
    for b in w.blobs:
        ax.add_patch(Circle((b.x, b.y), b.r, fc="#888780", ec="#5F5E5A", alpha=0.75))

    for i, r in enumerate(sorted(rs, key=lambda z: z.prio)):
        col = PAL[i % len(PAL)]
        tb = out.get("tubes", {}).get(r.key)
        if tb and len(tb.pts) > 1:
            px = [p[0] for p in tb.pts[:-1]]
            py = [p[1] for p in tb.pts[:-1]]
            ax.plot(px, py, color=col, lw=prm.body_r * 26, alpha=0.22,
                    solid_capstyle="round", zorder=2)
            ax.plot(px, py, color=col, lw=1.4, zorder=3)
            ax.plot(px[-1], py[-1], marker="o", ms=4, color=col, zorder=4)
        ax.add_patch(Circle((r.st.x, r.st.y), prm.body_r, fc=col, ec="white",
                            alpha=0.95, lw=1.0, zorder=5))
        ax.text(r.st.x, r.st.y, str(r.prio), color="white", ha="center",
                va="center", fontsize=7, zorder=6)
        dx = 0.45 * math.cos(r.st.yaw)
        dy = 0.45 * math.sin(r.st.yaw)
        ax.arrow(r.st.x, r.st.y, dx, dy, head_width=0.11, color=col, zorder=5, lw=0.9)

    ok = out["ok"]
    rev = [k for k, v in out.get("res", {}).items() if v.reversed_used]
    tag = f"{'성립' if ok else '미성립'} · R{out['rounds']}"
    if rev:
        tag += " · 후진"
    if not ok:
        tag += f" · {out.get('why', out.get('stuck', ''))[:18]}"
    ax.set_title(f"[{name}] {title}\n{tag}", fontsize=8,
                 color="#27500A" if ok else "#791F1F")
    ax.set_xlim(x0, x1)
    pad = 0.5
    ax.set_ylim(w.y_lo - pad, w.y_hi + pad)
    ax.set_aspect("equal")
    ax.tick_params(labelsize=6)
    for s in ax.spines.values():
        s.set_color("#D3D1C7")
    return out


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("cases", nargs="*", help="판 이름. 비우면 전부")
    ap.add_argument("--out", default="out/subgoal_cases.png")
    ap.add_argument("--budget", type=int, default=4000)
    a = ap.parse_args(argv)
    _setup_font()

    names = a.cases or list(S.SCENARIOS)
    cols = 1 if len(names) == 1 else 2
    rows = (len(names) + cols - 1) // cols
    fig, axes = plt.subplots(rows, cols, figsize=(6.4 * cols, 2.5 * rows))
    axes = [axes] if len(names) == 1 else list(axes.flat)
    for ax, n in zip(axes, names):
        draw_case(ax, n, a.budget)
    for ax in axes[len(names):]:
        ax.axis("off")

    fig.suptitle("서브골 층 시제 — 튜브 공표 결과  (값은 시제 전용 임시값이며 증거가 아니다)",
                 fontsize=9)
    fig.tight_layout(rect=(0, 0, 1, 0.985))
    import os
    os.makedirs(os.path.dirname(a.out) or ".", exist_ok=True)
    fig.savefig(a.out, dpi=150, bbox_inches="tight")
    print(f"[ok] {a.out}  ({len(names)}판)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
