#!/usr/bin/env python3
"""S3 애니메이션 — **막힘과 회복**. 의존 그래프가 허락할 때만 세그먼트가 진행한다.

    python3 anim_exec.py --block-robot r0 --block-at 4 --block-seconds 45

🔴 두 가지를 먼저 못 박는다.

1. **타이밍은 우리가 지어낸 것이다.** 공칭 속도 × 간선 길이이며 사전등록 상수가 아니다.
2. **이것은 릴리스 판정의 완전한 형태가 아니다 — 그것은 S4다.** 여기에는 재선택(스위칭)·
   커밋 게이트·진입 사건·신선도·계측·승급 사다리가 **하나도 없다.** 순서는 고정이며
   그림이 보여 주는 것은 *"막혔을 때 남은 로봇들이 `visit_order`가 정한 순서를 지키며
   기다렸다가 회복한다"* 하나뿐이다.

`anim.py`(S2)와 다른 점: **계획기를 다시 돌리지 않는다.** 경계 아티팩트 셋만으로 재생한다.
`ffmpeg`가 없으므로 GIF로 낸다. **설치하지 않는다.**
"""

from __future__ import annotations

import argparse
import json
import os

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib import animation  # noqa: E402

from boundary_adapter import constraints_from_boundary, roadmap_from_boundary  # noqa: E402
from exec_sim import (  # noqa: E402
    Blockage,
    blocking_witness,
    robot_position,
    segment_durations,
    simulate,
    vertex_occupancy_conflicts,
)
from protoscale import load_scale  # noqa: E402
from viz import ROBOT_COLORS, draw_static_layer  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))

CAPTION = (
    "[!] 지어낸 타이밍 — 공칭 속도 × 간선 길이. 사전등록 상수가 아니다. 탐색용 시제이며 증거가 아니다.\n"
    "의존 그래프가 허락할 때만 세그먼트가 진행한다. 순서는 고정이며 재선택(스위칭)이 없다.\n"
    "[!] 이것은 릴리스 판정의 완전한 형태가 아니다 — 그것은 S4다"
    "(커밋 게이트·진입 사건·신선도·계측·승급 사다리 전부 없음)."
)
STATE_STYLE = {
    "move": ("o", 1.0, None),
    "wait": ("o", 0.45, "#333333"),
    "block": ("X", 1.0, "#b00020"),
    "done": ("s", 0.5, None),
}


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="실행 의존 그래프 재생 — 막힘과 회복")
    ap.add_argument("--roadmap", default=os.path.join(HERE, "out", "roadmap.json"))
    ap.add_argument("--constraints", default=os.path.join(HERE, "out", "execution_constraints.json"))
    ap.add_argument("--assignment", default=os.path.join(HERE, "out", "assignment.json"),
                    help="목표 라벨용(선택). 없으면 로드맵만 그린다")
    ap.add_argument("--block-robot", default=None, help="세울 로봇. 없으면 차단 없는 재생")
    ap.add_argument("--block-at", type=int, default=0, help="그 로봇의 몇 번째 세그먼트 위에서")
    ap.add_argument("--block-seconds", type=float, default=45.0, help="몇 초 동안")
    ap.add_argument("--fps", type=int, default=10)
    ap.add_argument("--frames", type=int, default=140)
    ap.add_argument("--scale", default=None)
    ap.add_argument("--out", default=os.path.join(HERE, "out", "replay_exec.gif"))
    args = ap.parse_args(argv)

    cfg = load_scale(args.scale)
    with open(args.roadmap, "r", encoding="utf-8") as handle:
        model = roadmap_from_boundary(json.load(handle))
    with open(args.constraints, "r", encoding="utf-8") as handle:
        graph = constraints_from_boundary(json.load(handle))
    assignment = None
    if args.assignment and os.path.exists(args.assignment):
        from boundary_adapter import assignment_from_boundary

        with open(args.assignment, "r", encoding="utf-8") as handle:
            assignment = assignment_from_boundary(json.load(handle))

    durations = segment_durations(graph, model, cfg.nominal_speed_mps)
    free = simulate(graph, durations)
    blockages = []
    if args.block_robot:
        blockages = [Blockage(args.block_robot, args.block_at, args.block_seconds)]
    sim = simulate(graph, durations, blockages)

    robots = sorted({s.robot for s in graph.segments})
    conflicts = vertex_occupancy_conflicts(graph, sim)

    print(f"세그먼트           : {len(graph.segments)}개 · 로봇 {len(robots)}대")
    print(f"공칭 속도          : {cfg.nominal_speed_mps:.2f} m/s ({cfg.source_path})")
    print(f"차단 없는 완료 시각: {free.makespan:.1f} s   🔴 지어낸 타이밍")
    if blockages:
        b = blockages[0]
        print(f"차단 주입          : {b.robot} 세그먼트 #{b.segment_number} 위에서 {b.seconds:.0f} s")
        print(f"차단 후 완료 시각  : {sim.makespan:.1f} s (+{sim.makespan - free.makespan:.1f} s)")
        print("로봇별 지연        :")
        for r in robots:
            keys = [s.key for s in graph.segments if s.robot == r]
            if not keys:
                continue
            delay = max(sim.end[k] for k in keys) - max(free.end[k] for k in keys)
            print(f"   - {r}: +{delay:6.1f} s")
    print(f"정점 점유 충돌     : {len(conflicts)}건 (자체 점검이며 판정이 아니다)")
    if conflicts:
        print(f"   예: {conflicts[:3]}")

    fig, ax = plt.subplots(figsize=(12.5, 8.5))
    draw_static_layer(ax, model, assignment, show_goal_labels=True)
    dots, tags = {}, {}
    colors = {}
    for i, robot in enumerate(robots):
        colors[robot] = ROBOT_COLORS[i % len(ROBOT_COLORS)]
        (dot,) = ax.plot([], [], marker="o", markersize=15, color=colors[robot],
                         markeredgecolor="black", markeredgewidth=1.0, zorder=12)
        dots[robot] = dot
        tags[robot] = ax.text(0, 0, robot, fontsize=8, fontweight="bold", color="black",
                              zorder=13, ha="center", va="center")
    ax.set_title(f"{graph.instance_id} — 실행 의존 그래프 재생 (탐색용 시제 · 증거 아님)", fontsize=11)
    ax.text(0.5, -0.10, CAPTION, transform=ax.transAxes, ha="center", va="top",
            fontsize=8.5, color="#b00020")
    clock = ax.text(0.015, 0.985, "", transform=ax.transAxes, ha="left", va="top", fontsize=9,
                    zorder=20, bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="#888888"))
    status = ax.text(1.005, 0.34, "", transform=ax.transAxes, ha="left", va="top", fontsize=8,
                     zorder=20, color="#b00020",
                     bbox=dict(boxstyle="round,pad=0.3", fc="#fffbe6", ec="#b00020"))
    fig.subplots_adjust(left=0.07, right=0.74, top=0.93, bottom=0.19)

    def update(frame: int):
        t = sim.makespan * frame / max(1, args.frames - 1)
        lines = []
        for robot in robots:
            pos = robot_position(graph, sim, model, robot, t)
            if pos is None:
                continue
            x, y, state = pos
            marker, alpha, edge = STATE_STYLE[state]
            dots[robot].set_data([x], [y])
            dots[robot].set_marker(marker)
            dots[robot].set_alpha(alpha)
            dots[robot].set_markeredgecolor(edge or "black")
            dots[robot].set_markeredgewidth(2.5 if edge == "#b00020" else 1.0)
            tags[robot].set_position((x, y))
            if state == "block":
                lines.append(f"{robot}: [차단] 세워 두었다")
            elif state == "wait":
                witness = blocking_witness(graph, sim, robot, t)
                lines.append(
                    f"{robot}: 대기 — {witness[0]}#{witness[1]} 완료를 기다린다"
                    if witness else f"{robot}: 대기"
                )
        clock.set_text(f"재생 시각 {t:6.1f} / {sim.makespan:.1f} s   [!] 지어낸 타이밍")
        status.set_text("순서를 지키며 기다리는 로봇\n" + ("\n".join(lines) if lines else "(없음)"))
        return list(dots.values()) + list(tags.values()) + [clock, status]

    anim = animation.FuncAnimation(fig, update, frames=args.frames,
                                   interval=1000 / args.fps, blit=False)
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    writers = animation.writers.list()
    out = args.out
    if out.endswith(".mp4") and "ffmpeg" not in writers:
        out = out[:-4] + ".gif"
        print("ffmpeg가 없어 GIF로 물러섰다 (설치하지 않았다)")
    anim.save(out, writer=animation.PillowWriter(fps=args.fps), dpi=90)
    plt.close(fig)
    print(f"사용 가능한 writer : {writers}")
    print(f"wrote              : {os.path.abspath(out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
