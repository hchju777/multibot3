#!/usr/bin/env python3
"""S4 애니메이션 — **릴리스 판정이 화면에 보인다.**

    python3 anim_release.py --block-robot r0 --block-at 4 --block-seconds 45

매 틱(50 ms) 릴리스 판정을 돌리고, **릴리스된 세그먼트만** 물리 계층이 시작한다.
오른쪽 상자에 «지금 무엇이 놓였고, 놓이지 않은 것은 **왜** 아직인가»가 줄마다 적힌다.

🔴 그림 안 빨간 글씨로 유지하는 것: 지어낸 타이밍 · 재선택(스위칭) 없음 · 계측 없음 ·
`theta_esc` 값 부재라 승급 트리거가 존재하지 않음.
"""

from __future__ import annotations

import argparse
import os

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib import animation  # noqa: E402

from boundary_adapter import assignment_from_boundary  # noqa: E402
from exec_sim import Blockage  # noqa: E402
from protoscale import load_scale  # noqa: E402
from release_sim import load_bundle, run_loop  # noqa: E402
from tick_core import DELTA_T_T0_S  # noqa: E402
from viz import ROBOT_COLORS, draw_static_layer  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
CAPTION = (
    "[!] 지어낸 타이밍 — 공칭 속도 × 간선 길이. 사전등록 상수가 아니다. 탐색용 시제이며 증거가 아니다.\n"
    "매 틱(50 ms) 릴리스 판정을 돌리고 놓인 세그먼트만 물리 계층이 시작한다. "
    "순서는 고정이며 재선택(스위칭)이 없다.\n"
    "[!] 계측(링버퍼·분위수·예산)이 없다. theta_esc가 값 부재이므로 승급 트리거가 존재하지 않는다."
)
STATE_STYLE = {"move": ("o", 1.0, "black"), "wait": ("o", 0.4, "#333333"),
               "block": ("X", 1.0, "#b00020"), "done": ("s", 0.5, "black")}


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="S4 릴리스 판정 애니메이션")
    ap.add_argument("--roadmap", default=os.path.join(HERE, "out", "roadmap.json"))
    ap.add_argument("--constraints", default=os.path.join(HERE, "out", "execution_constraints.json"))
    ap.add_argument("--assignment", default=os.path.join(HERE, "out", "assignment.json"))
    ap.add_argument("--block-robot", default=None)
    ap.add_argument("--block-at", type=int, default=0)
    ap.add_argument("--block-seconds", type=float, default=45.0)
    ap.add_argument("--drop-entry-nth", type=int, default=None)
    ap.add_argument("--fps", type=int, default=10)
    ap.add_argument("--frames", type=int, default=150)
    ap.add_argument("--scale", default=None)
    ap.add_argument("--out", default=os.path.join(HERE, "out", "replay_release.gif"))
    args = ap.parse_args(argv)

    cfg = load_scale(args.scale)
    model, graph = load_bundle(args.roadmap, args.constraints)
    assignment = None
    if args.assignment and os.path.exists(args.assignment):
        import json

        with open(args.assignment, "r", encoding="utf-8") as handle:
            assignment = assignment_from_boundary(json.load(handle))

    blockages = (
        [Blockage(args.block_robot, args.block_at, args.block_seconds)] if args.block_robot else []
    )
    res = run_loop(graph, model, cfg.nominal_speed_mps, blockages,
                   drop_entry_nth=args.drop_entry_nth)
    frames = res.frames
    stride = max(1, len(frames) // args.frames)
    picked = frames[::stride]
    robots = sorted({s.robot for s in graph.segments})

    print(f"틱 {res.ticks}개 · 시뮬 {res.makespan_s:.2f} s · 프레임 {len(picked)}개 (stride {stride})")
    print(f"교차검증·불변식 위반 {len(res.problems)}건 · 정점 충돌 {len(res.vertex_conflicts)}건")
    print(f"재발행 발화 {len(res.republish_log)}회 {res.republish_log[:4]}")

    fig, ax = plt.subplots(figsize=(12.5, 8.5))
    draw_static_layer(ax, model, assignment, show_goal_labels=True)
    dots, tags, colors = {}, {}, {}
    for i, robot in enumerate(robots):
        colors[robot] = ROBOT_COLORS[i % len(ROBOT_COLORS)]
        (dot,) = ax.plot([], [], marker="o", markersize=15, color=colors[robot],
                         markeredgecolor="black", markeredgewidth=1.0, zorder=12)
        dots[robot] = dot
        tags[robot] = ax.text(0, 0, robot, fontsize=8, fontweight="bold", color="black",
                              zorder=13, ha="center", va="center")
    ax.set_title(f"{graph.instance_id} — 릴리스 판정 루프 (탐색용 시제 · 증거 아님)", fontsize=11)
    ax.text(0.5, -0.10, CAPTION, transform=ax.transAxes, ha="center", va="top",
            fontsize=8.5, color="#b00020")
    clock = ax.text(0.015, 0.985, "", transform=ax.transAxes, ha="left", va="top", fontsize=9,
                    zorder=20, bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="#888888"))
    panel = ax.text(1.005, 0.40, "", transform=ax.transAxes, ha="left", va="top", fontsize=7.6,
                    zorder=20, color="#b00020",
                    bbox=dict(boxstyle="round,pad=0.3", fc="#fffbe6", ec="#b00020"))
    fig.subplots_adjust(left=0.07, right=0.74, top=0.93, bottom=0.20)

    def update(i: int):
        f = picked[i]
        for robot in robots:
            pos = f.positions.get(robot)
            if pos is None:
                continue
            x, y, mode = pos
            marker, alpha, edge = STATE_STYLE[mode]
            dots[robot].set_data([x], [y])
            dots[robot].set_marker(marker)
            dots[robot].set_alpha(alpha)
            dots[robot].set_markeredgecolor(edge)
            dots[robot].set_markeredgewidth(2.5 if mode == "block" else 1.0)
            tags[robot].set_position((x, y))
        lines = [f"놓인 세그먼트: {[f'{r}#{n}' for r, n in f.released] or '(없음)'}"]
        if f.waiting:
            lines.append("아직 놓이지 않은 것과 그 이유")
            for robot in sorted(f.waiting):
                front, wait = f.waiting[robot]
                lines.append(f"  {front[0]}#{front[1]} <- {wait[0]}#{wait[1]} 완료 대기")
        else:
            lines.append("대기 중인 프런티어 없음")
        late = [f"{r}:{v:+.1f}" for r, v in sorted(f.deficits.items()) if abs(v) > 0.05]
        lines.append("D_r(s) " + (" ".join(late) if late else "전부 0") + "  (theta_esc 값 부재)")
        clock.set_text(f"틱 {f.tick_seq:5d}  ·  시뮬 {f.t_s:6.2f} s  "
                       f"(주기 {DELTA_T_T0_S * 1000:.0f} ms)  [!] 지어낸 타이밍")
        panel.set_text("\n".join(lines))
        return list(dots.values()) + list(tags.values()) + [clock, panel]

    anim = animation.FuncAnimation(fig, update, frames=len(picked),
                                   interval=1000 / args.fps, blit=False)
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    out = args.out
    if out.endswith(".mp4") and "ffmpeg" not in animation.writers.list():
        out = out[:-4] + ".gif"
        print("ffmpeg가 없어 GIF로 물러섰다 (설치하지 않았다)")
    anim.save(out, writer=animation.PillowWriter(fps=args.fps), dpi=90)
    plt.close(fig)
    print(f"wrote              : {os.path.abspath(out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
