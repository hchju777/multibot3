#!/usr/bin/env python3
"""이산 계획 애니메이션 — **공칭 속도 재생이며 본 시스템의 실행이 아니다.**

🔴 두 가지를 먼저 못 박는다.

1. **`mrs.discrete_plan`에는 시각이 없다**(계약 §1-2(1): index는 타임스텝이 아니다).
   연속 시간은 `mrs.trajectories`에만 있다. 그래서 이 재생의 타이밍은 **우리가 지어낸 것**이다.
2. **경계 아티팩트만으로는 충돌 없이 재생할 수 없다.** 대기가 접혀 사라졌기 때문이다.
   그래서 이 도구는 계획기를 같은 인자로 **다시 돌려 내부 타임라인(대기 포함)**을 얻어 재생한다.
   이 사실 자체가 «타이밍은 실행 계층(SADG)의 몫»이라는 계약 설계의 실물 증거다.

속도는 `config/scale.yaml:robot.nominal_speed_mps`에서 온다. 코드에 박혀 있지 않다.
`ffmpeg`가 없으면 GIF로 물러선다(설치하지 않는다).
"""

from __future__ import annotations

import argparse
import json
import os

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib import animation  # noqa: E402

from boundary_adapter import plan_to_boundary  # noqa: E402
from gen_plan import load_inputs  # noqa: E402
from plan_core import PlanParams, plan_instance  # noqa: E402
from protoscale import load_scale  # noqa: E402
from viz import ROBOT_COLORS, draw_static_layer  # noqa: E402

HERE = os.path.dirname(os.path.abspath(__file__))
CAPTION = (
    "공칭 속도 재생 — 실행 계층(SADG) 없음. 실제 시스템의 타이밍이 아니다.\n"
    "이산 계획에는 시각이 없다(계약 §1-2). 대기를 포함한 내부 타임라인을 재생한 것이다."
)


def _sample(model, timeline, tick, frames_per_tick, v_nom, tick_seconds):
    """한 로봇의 타임라인을 프레임 좌표열로 편다."""
    nodes = model.node_map()
    lengths = {}
    for e in model.edges:
        lengths[(e.u, e.v)] = e.length_m
        lengths[(e.v, e.u)] = e.length_m
    out: list[tuple[float, float]] = []
    for t in range(tick):
        u = timeline[min(t, len(timeline) - 1)]
        v = timeline[min(t + 1, len(timeline) - 1)]
        a, b = nodes[u], nodes[v]
        span = lengths.get((u, v), 0.0)
        move_ticks = (span / v_nom) / tick_seconds if span else 0.0
        for f in range(frames_per_tick):
            ratio = (f / frames_per_tick) / move_ticks if move_ticks > 0 else 1.0
            ratio = min(1.0, ratio)
            out.append((a.x_m + (b.x_m - a.x_m) * ratio, a.y_m + (b.y_m - a.y_m) * ratio))
    out.append((nodes[timeline[-1]].x_m, nodes[timeline[-1]].y_m))
    return out


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="이산 계획 애니메이션 (공칭 속도 재생)")
    ap.add_argument("--roadmap", default=os.path.join(HERE, "out", "roadmap.json"))
    ap.add_argument("--assignment", default=os.path.join(HERE, "out", "assignment.json"))
    ap.add_argument("--plan", default=os.path.join(HERE, "out", "discrete_plan.json"),
                    help="대조용. 다시 계획한 결과와 다르면 경고한다")
    ap.add_argument("--priority", default="most_goals")
    ap.add_argument("--horizon", type=int, default=96)
    ap.add_argument("--seed", type=int, default=0)
    ap.add_argument("--fps", type=int, default=10)
    ap.add_argument("--frames-per-tick", type=int, default=8)
    ap.add_argument("--scale", default=None)
    ap.add_argument("--out", default=os.path.join(HERE, "out", "replay.gif"))
    args = ap.parse_args(argv)

    cfg = load_scale(args.scale)
    model, assignment = load_inputs(args.roadmap, args.assignment)
    result = plan_instance(
        model,
        assignment,
        PlanParams(priority=args.priority, horizon=args.horizon, seed=args.seed),
    )
    if os.path.exists(args.plan):
        with open(args.plan, "r", encoding="utf-8") as handle:
            saved = json.load(handle)
        if saved != plan_to_boundary(result):
            print("⚠️ 저장된 계획과 다시 계획한 결과가 다르다 — 인자를 맞춰라(우선순위·지평·시드)")

    v_nom = cfg.nominal_speed_mps
    tick_seconds = max(e.length_m for e in model.edges) / v_nom
    ticks = max(len(p.timeline) for p in result.paths)
    tracks = {
        p.robot: _sample(model, p.timeline, ticks, args.frames_per_tick, v_nom, tick_seconds)
        for p in result.paths
    }
    total_frames = ticks * args.frames_per_tick + 1

    fig, ax = plt.subplots(figsize=(12.5, 8.5))
    draw_static_layer(ax, model, assignment, show_goal_labels=True)
    dots, tags = {}, {}
    for i, path in enumerate(sorted(result.paths, key=lambda p: p.robot)):
        color = ROBOT_COLORS[i % len(ROBOT_COLORS)]
        (dot,) = ax.plot([], [], marker="o", markersize=15, color=color,
                         markeredgecolor="black", markeredgewidth=1.0, zorder=12)
        dots[path.robot] = dot
        tags[path.robot] = ax.text(0, 0, path.robot, fontsize=8, fontweight="bold",
                                   color="black", zorder=13, ha="center", va="center")
    ax.set_title(f"{result.instance_id} — 탐색용 시제 (증거 아님)", fontsize=11)
    ax.text(0.5, -0.10, CAPTION, transform=ax.transAxes, ha="center", va="top",
            fontsize=9, color="#b00020")
    clock = ax.text(0.015, 0.975, "", transform=ax.transAxes, ha="left", va="top", fontsize=9,
                    zorder=20, bbox=dict(boxstyle="round,pad=0.3", fc="white", ec="#888888"))
    fig.subplots_adjust(left=0.07, right=0.74, top=0.93, bottom=0.17)

    def update(frame: int):
        for robot, dot in dots.items():
            x, y = tracks[robot][min(frame, len(tracks[robot]) - 1)]
            dot.set_data([x], [y])
            tags[robot].set_position((x, y))
        tick = frame // args.frames_per_tick
        clock.set_text(
            f"이산 단계 {tick}/{ticks - 1}   ·   재생 시각 {tick * tick_seconds:.1f} s "
            f"(공칭 {v_nom:.2f} m/s · 지어낸 타이밍)"
        )
        return list(dots.values()) + list(tags.values()) + [clock]

    anim = animation.FuncAnimation(fig, update, frames=total_frames, interval=1000 / args.fps,
                                   blit=False)
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    writers = animation.writers.list()
    if args.out.endswith(".mp4") and "ffmpeg" in writers:
        anim.save(args.out, writer="ffmpeg", fps=args.fps, dpi=110)
    else:
        if args.out.endswith(".mp4"):
            args.out = args.out[:-4] + ".gif"
            print("ffmpeg가 없어 GIF로 물러섰다 (설치하지 않았다)")
        anim.save(args.out, writer=animation.PillowWriter(fps=args.fps), dpi=90)
    plt.close(fig)
    print(f"사용 가능한 writer : {writers}")
    print(f"이산 단계 수       : {ticks}")
    print(f"단계 하나의 재생 시간: {tick_seconds:.2f} s (= 최장 간선 {max(e.length_m for e in model.edges):.1f} m / {v_nom:.2f} m/s)")
    print(f"wrote              : {os.path.abspath(args.out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
