#!/usr/bin/env python3
"""정적 시각화 (탐색용 시제).

로드맵과 초기 배치·목표를 그림 하나로 그린다. 애니메이션은 `anim.py`가 한다.
입력은 경계 아티팩트(JSON)이며 어댑터로 내부 표현으로 되돌린 뒤 그린다 —
그리기 코드가 경계 키를 직접 만지지 않는다.

    python3 viz.py --roadmap out/roadmap.json --assignment out/assignment.json --out out/scene.png

🔴 탐색용 시제이며 증거가 아니다.
"""

from __future__ import annotations

import argparse
import json
import os
from collections import defaultdict

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt  # noqa: E402
from matplotlib import font_manager  # noqa: E402

from boundary_adapter import assignment_from_boundary, roadmap_from_boundary  # noqa: E402


def _pick_korean_font() -> str | None:
    """한글 글리프를 가진 폰트를 고른다. 없으면 None(그때는 라벨이 네모로 나온다)."""
    installed = {f.name for f in font_manager.fontManager.ttflist}
    for candidate in (
        "Noto Sans CJK KR", "Noto Sans CJK JP", "NanumGothic",
        "Noto Sans KR", "Malgun Gothic",
    ):
        if candidate in installed:
            return candidate
    return None


_KO_FONT = _pick_korean_font()
if _KO_FONT:
    plt.rcParams["font.family"] = [_KO_FONT, "DejaVu Sans"]
plt.rcParams["axes.unicode_minus"] = False

HERE = os.path.dirname(os.path.abspath(__file__))
COLOR_WIDE = "#1f77b4"  # 교행 가능
COLOR_NARROW = "#d62728"  # 교행 불가 통로 묶음
COLOR_STUB = "#999999"  # 엔드포인트 지선
ROBOT_COLORS = [
    "#e6194b", "#3cb44b", "#f58231", "#911eb4",
    "#0082c8", "#f032e6", "#808000", "#008080",
]


def draw_static_layer(ax, model, plan=None, show_goal_labels: bool = True) -> None:
    """로드맵(+배정)의 정적 층을 축에 그린다. viz.py와 anim.py가 함께 쓴다.

    목표 라벨은 **정점마다 한 뭉치로 쌓는다** — 로봇별로 찍으면 같은 엔드포인트를 가리키는
    라벨들이 서로 겹쳐 읽을 수 없다(1단계 그림의 결함).
    """
    nodes = model.node_map()

    for e in model.edges:
        a, b = nodes[e.u], nodes[e.v]
        if e.corridor_key:
            color, style, lw = COLOR_NARROW, (0, (6, 3)), 2.0 + 1.6 * e.width_m
        elif e.capacity_robots == 1:
            color, style, lw = COLOR_STUB, "solid", 1.0 + 1.2 * e.width_m
        else:
            color, style, lw = COLOR_WIDE, "solid", 2.0 + 1.6 * e.width_m
        ax.plot([a.x_m, b.x_m], [a.y_m, b.y_m], color=color, linestyle=style, linewidth=lw,
                solid_capstyle="round", zorder=1, alpha=0.8)
        mx, my = (a.x_m + b.x_m) / 2.0, (a.y_m + b.y_m) / 2.0
        ax.text(mx, my, f"c{e.capacity_robots}", fontsize=6, color="#333333",
                ha="center", va="center", zorder=4,
                bbox=dict(boxstyle="round,pad=0.12", fc="white", ec="none", alpha=0.7))

    eps = set(model.endpoints)
    for n in model.nodes:
        marker, size, color = ("s", 9, "#2ca02c") if n.key in eps else ("o", 6, "#444444")
        ax.plot(n.x_m, n.y_m, marker=marker, markersize=size, color=color, zorder=3)
        ax.text(n.x_m, n.y_m + 0.4, n.key, fontsize=6, ha="center", color="#222222", zorder=5)

    for c in model.corridors:
        pts = [nodes[k] for pair in c.edge_keys for k in pair]
        cx = sum(p.x_m for p in pts) / len(pts) + 0.9
        cy = sum(p.y_m for p in pts) / len(pts) + 0.9
        ax.text(cx, cy, c.key, fontsize=8, color=COLOR_NARROW, ha="center", va="center", zorder=6,
                bbox=dict(boxstyle="round,pad=0.2", fc="white", ec=COLOR_NARROW, alpha=0.9))

    if plan is not None:
        per_node: dict[str, list[tuple[str, str]]] = defaultdict(list)
        for i, rp in enumerate(sorted(plan.robots, key=lambda p: p.robot)):
            color = ROBOT_COLORS[i % len(ROBOT_COLORS)]
            s = nodes[rp.start]
            ax.plot(s.x_m, s.y_m, marker="*", markersize=17, color=color,
                    markeredgecolor="black", markeredgewidth=0.5, zorder=7)
            ax.text(s.x_m - 0.6, s.y_m - 0.6, rp.robot, fontsize=8, color=color,
                    fontweight="bold", zorder=7)
            for k, g in enumerate(rp.goals):
                per_node[g.location].append((f"{rp.robot}:{k}({g.task})", color))
        for location, labels in sorted(per_node.items()):
            t = nodes[location]
            ax.plot(t.x_m, t.y_m, marker="X", markersize=11, color="#222222",
                    markerfacecolor="none", markeredgewidth=1.4, zorder=7)
            if not show_goal_labels:
                continue
            for row, (text, color) in enumerate(labels):  # 정점마다 아래로 쌓는다
                ax.text(t.x_m + 0.45, t.y_m - 0.55 - 0.5 * row, text,
                        fontsize=6, color=color, zorder=8)

    handles = [
        plt.Line2D([], [], color=COLOR_WIDE, lw=4, label="교행 가능 (capacity>=2)"),
        plt.Line2D([], [], color=COLOR_NARROW, lw=4, ls=(0, (6, 3)),
                   label="교행 불가 통로 묶음 (capacity=1 + corridor)"),
        plt.Line2D([], [], color=COLOR_STUB, lw=2, label="엔드포인트 지선 (capacity=1, 통로 없음)"),
        plt.Line2D([], [], color="#2ca02c", marker="s", ls="", label="endpoint"),
        plt.Line2D([], [], color="#444444", marker="o", ls="", label="정점"),
        plt.Line2D([], [], color="black", marker="*", ls="", label="로봇 시작"),
        plt.Line2D([], [], color="black", marker="X", ls="", markerfacecolor="none",
                   label="목표 (라벨은 정점마다 쌓음)"),
    ]
    ax.legend(handles=handles, loc="upper left", bbox_to_anchor=(1.005, 1.0), fontsize=8)
    ax.set_aspect("equal", adjustable="datalim")
    ax.set_xlabel("x [m]")
    ax.set_ylabel("y [m]")
    ax.grid(True, alpha=0.15)


def draw(roadmap_path: str, assignment_path: str | None, out_path: str, title: str | None) -> str:
    """로드맵·배정 아티팩트를 읽어 정적 그림을 저장하고 경로를 돌려준다."""
    with open(roadmap_path, "r", encoding="utf-8") as handle:
        model = roadmap_from_boundary(json.load(handle))
    plan = None
    if assignment_path and os.path.exists(assignment_path):
        with open(assignment_path, "r", encoding="utf-8") as handle:
            plan = assignment_from_boundary(json.load(handle))

    fig, ax = plt.subplots(figsize=(11.0, 8.0))
    draw_static_layer(ax, model, plan)
    ax.set_title(title or f"{model.instance_id}  —  탐색용 시제 (증거 아님)", fontsize=11)
    fig.tight_layout()
    os.makedirs(os.path.dirname(os.path.abspath(out_path)), exist_ok=True)
    fig.savefig(out_path, dpi=150, bbox_inches="tight")
    plt.close(fig)
    return os.path.abspath(out_path)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="로드맵·배정 정적 시각화 (탐색용 시제)")
    ap.add_argument("--roadmap", default=os.path.join(HERE, "out", "roadmap.json"))
    ap.add_argument("--assignment", default=os.path.join(HERE, "out", "assignment.json"))
    ap.add_argument("--out", default=os.path.join(HERE, "out", "scene.png"))
    ap.add_argument("--title", default=None)
    args = ap.parse_args(argv)
    print(f"wrote: {draw(args.roadmap, args.assignment, args.out, args.title)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
