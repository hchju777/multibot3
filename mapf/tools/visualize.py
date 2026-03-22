#!/usr/bin/env python3
"""
visualize.py — MAPF solution animator

Usage:
    python3 tools/visualize.py [config/scenarios/single_shot/square.yaml]
    python3 tools/visualize.py [config/scenarios/single_shot/square.yaml] [config/params/default.yaml]
    python3 tools/visualize.py [config/scenarios/single_shot/square.yaml] --solution solution.json
    python3 tools/visualize.py --scenario [config/scenarios/lifelong/square.yaml]

Controls:
    Space       : play / pause
    ← / →       : step backward / forward
    Slider      : scrub to any timestep

The script runs the C++ solver automatically unless --solution is given.
"""

import argparse
import json
import math
import os
from pathlib import Path
import subprocess
import sys
import tempfile

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.animation import FuncAnimation
from matplotlib.widgets import Slider
import yaml

# ── 색상 팔레트 (로봇별) ──────────────────────────────────────
COLORS = [
    '#e74c3c', '#3498db', '#2ecc71', '#f39c12',
    '#9b59b6', '#1abc9c', '#e67e22', '#e91e63',
    '#00bcd4', '#8bc34a',
]


# ── YAML 맵 로드 ──────────────────────────────────────────────
def load_map(path):
    with open(path) as f:
        data = yaml.safe_load(f)

    g = data['graph']
    vertices = {}
    for v in g['vertices']:
        vid = v['id']
        vertices[vid] = {
            'x': v.get('x'),
            'y': v.get('y'),
        }

    edges = [(int(e[0]), int(e[1])) for e in g['edges']]

    robots = sorted(data.get('robots', []), key=lambda r: r['id'])

    return vertices, edges, robots


def load_single_shot_scenario(path):
    with open(path) as f:
        data = yaml.safe_load(f)

    scenario_path = Path(path).resolve()
    if 'map' not in data:
        vertices, edges, robots = load_map(path)
        return {
            'scenario_path': str(scenario_path),
            'map_path': str(scenario_path),
            'params_path': None,
            'vertices': vertices,
            'edges': edges,
            'robots': robots,
        }

    map_path = resolve_path_from(scenario_path.parent, data['map'])
    params_path = None
    if 'params' in data:
        params_path = resolve_path_from(scenario_path.parent, data['params'])

    vertices, edges, _ = load_map(map_path)
    robots = sorted(data.get('robots', []), key=lambda r: r['id'])
    return {
        'scenario_path': str(scenario_path),
        'map_path': map_path,
        'params_path': params_path,
        'vertices': vertices,
        'edges': edges,
        'robots': robots,
    }


# ── 위치 없으면 자동 배치 ──────────────────────────────────────
def auto_layout(vertices, edges):
    try:
        import networkx as nx
        G = nx.Graph()
        for vid in vertices:
            G.add_node(vid)
        for u, v in edges:
            G.add_edge(u, v)
        pos = nx.spring_layout(G, seed=42, k=2.0)
        for vid, (x, y) in pos.items():
            vertices[vid]['x'] = float(x)
            vertices[vid]['y'] = float(y)
    except ImportError:
        # networkx 없으면 원형 배치
        n = len(vertices)
        for i, vid in enumerate(sorted(vertices)):
            angle = 2 * math.pi * i / n
            vertices[vid]['x'] = math.cos(angle)
            vertices[vid]['y'] = math.sin(angle)
    return vertices


# ── JSON solution 로드 ────────────────────────────────────────
def load_solution(path):
    with open(path) as f:
        return json.load(f)


def trim_copy(value):
    return value.strip()


def unquote_copy(value):
    if len(value) >= 2 and value[0] == value[-1] and value[0] in ("'", '"'):
        return value[1:-1]
    return value


def find_dotenv(start_dir):
    current = Path(start_dir).resolve()
    while True:
        candidate = current / '.env'
        if candidate.exists():
            return candidate
        if current.parent == current:
            return None
        current = current.parent


def load_dotenv_value(dotenv_path, key):
    with open(dotenv_path) as f:
        for line in f:
            trimmed = trim_copy(line)
            if not trimmed or trimmed.startswith('#'):
                continue
            if '=' not in trimmed:
                continue
            lhs, rhs = trimmed.split('=', 1)
            if trim_copy(lhs) != key:
                continue
            return unquote_copy(trim_copy(rhs))
    return None


def resolve_path_from(base_dir, raw_path):
    path = Path(raw_path)
    if path.is_absolute():
        return str(path.resolve())
    return str((base_dir / path).resolve())


def resolve_config_path(cli_path, env_keys, default_path=None):
    if cli_path:
        return cli_path

    if isinstance(env_keys, str):
        env_keys = [env_keys]

    for env_key in env_keys:
        env_path = trim_copy(os.getenv(env_key, ''))
        if env_path:
            return env_path

    dotenv_path = find_dotenv(os.getcwd())
    if dotenv_path is not None:
        for env_key in env_keys:
            dotenv_value = load_dotenv_value(dotenv_path, env_key)
            if dotenv_value:
                return resolve_path_from(dotenv_path.parent, dotenv_value)

    return default_path


# ── C++ 바이너리 실행 → JSON 생성 ─────────────────────────────
def run_solver(scenario_path, params_path, binary, out_path):
    cmd = [binary, scenario_path]
    if params_path:
        cmd.append(params_path)
    cmd.extend(['--out', out_path])
    print(f"[viz] Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, capture_output=False, text=True)
    if result.returncode != 0:
        raise RuntimeError(f"Solver exited with code {result.returncode}")


def run_lifelong_solver(scenario_path, binary, out_path):
    cmd = [binary, scenario_path, '--out', out_path]
    print(f"[viz] Running: {' '.join(cmd)}")
    result = subprocess.run(cmd, capture_output=False, text=True)
    if result.returncode != 0:
        raise RuntimeError(f"Lifelong solver exited with code {result.returncode}")


def build_lifelong_graph(sol_data):
    planning_graph = sol_data['planning_graph']
    original_vertices = {}
    map_path = sol_data.get('map_path')
    if map_path and Path(map_path).exists():
        original_vertices, _, _ = load_map(map_path)

    vertices = {}
    for vertex in planning_graph['vertices']:
        vid = vertex['id']
        backing = vertex.get('backing_vertices', [])
        coords = []
        for orig_vid in backing:
            if orig_vid not in original_vertices:
                continue
            ox = original_vertices[orig_vid].get('x')
            oy = original_vertices[orig_vid].get('y')
            if ox is None or oy is None:
                continue
            coords.append((ox, oy))

        if coords:
            x = sum(c[0] for c in coords) / len(coords)
            y = sum(c[1] for c in coords) / len(coords)
        else:
            x = None
            y = None

        vertices[vid] = {
            'x': x,
            'y': y,
            'backing_vertices': backing,
            'label': ','.join(str(v) for v in backing) if backing else str(vid),
        }

    edges = [(int(e[0]), int(e[1])) for e in planning_graph['edges']]
    return vertices, edges


def animate_single_solution(args, sol_data):
    single_shot = load_single_shot_scenario(args.single_shot)
    vertices = single_shot['vertices']
    edges = single_shot['edges']
    robots = single_shot['robots']

    has_positions = all(v['x'] is not None for v in vertices.values())
    if not has_positions:
        print('[viz] No x/y in YAML — using auto layout (install networkx for better results)')
        vertices = auto_layout(vertices, edges)

    if not sol_data.get('solved', False):
        print('[viz] Solver found no solution.')
        sys.exit(1)

    all_sol_vids = {v for path in sol_data['paths'] for v in path}
    all_sol_vids |= {r['goal'] for r in sol_data.get('robots', [])}
    unknown = all_sol_vids - set(vertices.keys())
    if unknown:
        print(f'[viz] ERROR: solution contains vertex id(s) {sorted(unknown)} '
              f'not found in {single_shot["map_path"]}.')
        print('[viz] This can happen when virtual_lock preprocessing remaps vertex ids.')
        print('[viz] Make sure the scenario/map YAML and solution JSON come from the same run.')
        sys.exit(1)

    paths = sol_data['paths']
    n_bots = len(paths)
    makespan = max(len(p) for p in paths)
    for p in paths:
        while len(p) < makespan:
            p.append(p[-1])

    sol_robots = sol_data.get('robots', [{'id': i, 'start': paths[i][0],
                                           'goal': paths[i][-1]}
                                          for i in range(n_bots)])

    fig, ax = plt.subplots(figsize=(9, 7))
    plt.subplots_adjust(bottom=0.15)
    ax.set_aspect('equal')
    ax.axis('off')
    fig.patch.set_facecolor('#f8f8f8')
    ax.set_facecolor('#f8f8f8')

    xs = [v['x'] for v in vertices.values()]
    ys = [v['y'] for v in vertices.values()]
    margin = 0.6
    ax.set_xlim(min(xs) - margin, max(xs) + margin)
    ax.set_ylim(min(ys) - margin, max(ys) + margin)

    for u, v in edges:
        x0, y0 = vertices[u]['x'], vertices[u]['y']
        x1, y1 = vertices[v]['x'], vertices[v]['y']
        ax.plot([x0, x1], [y0, y1], color='#aaaaaa', lw=2, zorder=1)

    VR = 0.13
    for vid, pos in vertices.items():
        c = plt.Circle((pos['x'], pos['y']), VR,
                        color='white', ec='#555555', lw=1.5, zorder=2)
        ax.add_patch(c)
        ax.text(pos['x'], pos['y'], str(vid),
                ha='center', va='center', fontsize=7,
                color='#333333', fontweight='bold', zorder=3)

    for r in sol_robots:
        gv = r['goal']
        gx = vertices[gv]['x']
        gy = vertices[gv]['y']
        col = COLORS[r['id'] % len(COLORS)]
        ax.plot(gx, gy, 'x', color=col, ms=15, mew=2.5,
                zorder=4, alpha=0.55)

    RR = 0.18
    robot_circles = []
    robot_texts = []
    for r in sol_robots:
        col = COLORS[r['id'] % len(COLORS)]
        sv = paths[r['id']][0]
        sx, sy = vertices[sv]['x'], vertices[sv]['y']
        circ = plt.Circle((sx, sy), RR,
                           color=col, ec='#222222', lw=1.5,
                           zorder=5, alpha=0.92)
        ax.add_patch(circ)
        txt = ax.text(sx, sy, str(r['id']),
                      ha='center', va='center',
                      fontsize=9, color='white',
                      fontweight='bold', zorder=6)
        robot_circles.append(circ)
        robot_texts.append(txt)

    title = ax.set_title('t = 0  /  ' + str(makespan - 1),
                         fontsize=13, pad=10)

    legend_patches = [
        mpatches.Patch(
            color=COLORS[r['id'] % len(COLORS)],
            label=f"R{r['id']}  s={r['start']} → g={r['goal']}"
        )
        for r in sol_robots
    ]
    ax.legend(handles=legend_patches,
              loc='upper right', fontsize=8,
              framealpha=0.8)

    ax_sl = plt.axes([0.15, 0.05, 0.70, 0.03])
    slider = Slider(ax_sl, 't', 0, makespan - 1,
                    valinit=0, valstep=1, color='#3498db')

    def redraw(t):
        t = int(t)
        for i, r in enumerate(sol_robots):
            vid = paths[r['id']][t]
            x, y = vertices[vid]['x'], vertices[vid]['y']
            robot_circles[i].center = (x, y)
            robot_texts[i].set_position((x, y))
        title.set_text(f't = {t}  /  {makespan - 1}')
        fig.canvas.draw_idle()

    slider.on_changed(redraw)
    redraw(0)

    playing = [False]
    anim = [None]

    def toggle_play(_event=None):
        playing[0] = not playing[0]
        if playing[0]:
            def step(_frame):
                t = int(slider.val)
                if t < makespan - 1:
                    slider.set_val(t + 1)
                else:
                    playing[0] = False
            anim[0] = FuncAnimation(fig, step,
                                    interval=args.interval,
                                    cache_frame_data=False)
        else:
            if anim[0]:
                anim[0].event_source.stop()
        fig.canvas.draw_idle()

    def on_key(event):
        if event.key == ' ':
            toggle_play()
        elif event.key == 'right':
            slider.set_val(min(int(slider.val) + 1, makespan - 1))
        elif event.key == 'left':
            slider.set_val(max(int(slider.val) - 1, 0))

    fig.canvas.mpl_connect('key_press_event', on_key)
    print('[viz] Controls: Space=play/pause  ←→=step  Slider=scrub')
    plt.show()


def animate_lifelong_solution(args, sol_data):
    vertices, edges = build_lifelong_graph(sol_data)
    if not all(v['x'] is not None for v in vertices.values()):
        print('[viz] No planning-graph coordinates available — using auto layout')
        vertices = auto_layout(vertices, edges)

    robots = sol_data['robots']
    frames = sol_data['frames']
    if not frames:
        print('[viz] Lifelong export contains no frames.')
        sys.exit(1)

    fig, ax = plt.subplots(figsize=(9, 7))
    plt.subplots_adjust(bottom=0.15)
    ax.set_aspect('equal')
    ax.axis('off')
    fig.patch.set_facecolor('#f8f8f8')
    ax.set_facecolor('#f8f8f8')

    xs = [v['x'] for v in vertices.values()]
    ys = [v['y'] for v in vertices.values()]
    margin = 0.6
    ax.set_xlim(min(xs) - margin, max(xs) + margin)
    ax.set_ylim(min(ys) - margin, max(ys) + margin)

    for u, v in edges:
        x0, y0 = vertices[u]['x'], vertices[u]['y']
        x1, y1 = vertices[v]['x'], vertices[v]['y']
        ax.plot([x0, x1], [y0, y1], color='#aaaaaa', lw=2, zorder=1)

    VR = 0.13
    for vid, pos in vertices.items():
        c = plt.Circle((pos['x'], pos['y']), VR,
                       color='white', ec='#555555', lw=1.5, zorder=2)
        ax.add_patch(c)
        ax.text(pos['x'], pos['y'], pos.get('label', str(vid)),
                ha='center', va='center', fontsize=7,
                color='#333333', fontweight='bold', zorder=3)

    RR = 0.18
    robot_circles = []
    robot_texts = []
    goal_markers = []

    for i, robot in enumerate(robots):
        col = COLORS[i % len(COLORS)]
        start_vid = frames[0]['positions'][i]
        sx, sy = vertices[start_vid]['x'], vertices[start_vid]['y']
        circ = plt.Circle((sx, sy), RR,
                          color=col, ec='#222222', lw=1.5,
                          zorder=5, alpha=0.92)
        ax.add_patch(circ)
        txt = ax.text(sx, sy, robot['id'],
                      ha='center', va='center',
                      fontsize=9, color='white',
                      fontweight='bold', zorder=6)
        goal_marker, = ax.plot([], [], 'x', color=col, ms=15, mew=2.5,
                               zorder=4, alpha=0.55)
        robot_circles.append(circ)
        robot_texts.append(txt)
        goal_markers.append(goal_marker)

    total_t = frames[-1]['t']
    title = ax.set_title(f't = {frames[0]["t"]}  /  {total_t}',
                         fontsize=13, pad=10)

    legend_patches = []
    for i, robot in enumerate(robots):
        start = robot.get('start')
        home = robot.get('home')
        legend_patches.append(
            mpatches.Patch(
                color=COLORS[i % len(COLORS)],
                label=f"{robot['id']}  start={start} home={home}"
            )
        )
    ax.legend(handles=legend_patches,
              loc='upper right', fontsize=8,
              framealpha=0.8)

    ax_sl = plt.axes([0.15, 0.05, 0.70, 0.03])
    slider = Slider(ax_sl, 'frame', 0, len(frames) - 1,
                    valinit=0, valstep=1, color='#3498db')

    def redraw(frame_idx):
        frame_idx = int(frame_idx)
        frame = frames[frame_idx]
        for i, robot in enumerate(robots):
            vid = frame['positions'][i]
            x, y = vertices[vid]['x'], vertices[vid]['y']
            robot_circles[i].center = (x, y)
            robot_texts[i].set_position((x, y))

            goal_vid = frame['goals'][i]
            if goal_vid is None:
                goal_markers[i].set_data([], [])
            else:
                gx, gy = vertices[goal_vid]['x'], vertices[goal_vid]['y']
                goal_markers[i].set_data([gx], [gy])

        events = ', '.join(frame.get('activated_event_ids', []))
        suffix = f'  events: {events}' if events else ''
        title.set_text(
            f't = {frame["t"]}  /  {total_t}  ep={frame["episode_index"]}{suffix}'
        )
        fig.canvas.draw_idle()

    slider.on_changed(redraw)
    redraw(0)

    playing = [False]
    anim = [None]

    def toggle_play(_event=None):
        playing[0] = not playing[0]
        if playing[0]:
            def step(_frame):
                current = int(slider.val)
                if current < len(frames) - 1:
                    slider.set_val(current + 1)
                else:
                    playing[0] = False
            anim[0] = FuncAnimation(fig, step,
                                    interval=args.interval,
                                    cache_frame_data=False)
        else:
            if anim[0]:
                anim[0].event_source.stop()
        fig.canvas.draw_idle()

    def on_key(event):
        if event.key == ' ':
            toggle_play()
        elif event.key == 'right':
            slider.set_val(min(int(slider.val) + 1, len(frames) - 1))
        elif event.key == 'left':
            slider.set_val(max(int(slider.val) - 1, 0))

    fig.canvas.mpl_connect('key_press_event', on_key)
    print('[viz] Controls: Space=play/pause  ←→=step  Slider=scrub')
    plt.show()


# ── 메인 ─────────────────────────────────────────────────────
def main():
    parser = argparse.ArgumentParser(description='MAPF solution visualizer')
    parser.add_argument(
        'single_shot',
        nargs='?',
        default=None,
        help='Single-shot scenario YAML (default: MAPF_SINGLE_SHOT_SCENARIO_PATH from env/.env, else config/scenarios/single_shot/square.yaml)',
    )
    parser.add_argument(
        'params',
        nargs='?',
        default=None,
        help='Optional params override YAML (default: MAPF_PARAMS_PATH from env/.env, else scenario params)',
    )
    parser.add_argument(
        '--scenario',
        nargs='?',
        const='',
        default=None,
        help='Lifelong scenario YAML (default with flag only: MAPF_LIFELONG_SCENARIO_PATH from env/.env, else config/scenarios/lifelong/square.yaml)',
    )
    parser.add_argument('--solution', '-s',
                        help='Solution JSON — skip running the solver')
    parser.add_argument('--binary', '-b',
                        default='build/mapf_test',
                        help='C++ solver binary (default: build/mapf_test)')
    parser.add_argument('--lifelong-binary',
                        default='build/mapf_lifelong_test',
                        help='Lifelong C++ solver binary (default: build/mapf_lifelong_test)')
    parser.add_argument('--interval', type=int, default=600,
                        help='Animation frame interval in ms (default: 600)')
    args = parser.parse_args()

    if args.solution:
        sol_data = load_solution(args.solution)
    elif args.scenario is not None:
        args.scenario = resolve_config_path(
            args.scenario if args.scenario else None,
            'MAPF_LIFELONG_SCENARIO_PATH',
            'config/scenarios/lifelong/square.yaml',
        )
        tmp = tempfile.NamedTemporaryFile(suffix='.json', delete=False)
        tmp.close()
        try:
            run_lifelong_solver(args.scenario, args.lifelong_binary, tmp.name)
            sol_data = load_solution(tmp.name)
        finally:
            os.unlink(tmp.name)
    else:
        args.single_shot = resolve_config_path(
            args.single_shot,
            ['MAPF_SINGLE_SHOT_SCENARIO_PATH', 'MAPF_MAP_PATH'],
            'config/scenarios/single_shot/square.yaml',
        )
        single_shot = load_single_shot_scenario(args.single_shot)
        args.params = resolve_config_path(args.params, 'MAPF_PARAMS_PATH', single_shot['params_path'])
        tmp = tempfile.NamedTemporaryFile(suffix='.json', delete=False)
        tmp.close()
        try:
            run_solver(args.single_shot, args.params, args.binary, tmp.name)
            sol_data = load_solution(tmp.name)
        finally:
            os.unlink(tmp.name)

    if sol_data.get('kind') == 'lifelong_run_v1':
        animate_lifelong_solution(args, sol_data)
    else:
        if args.scenario is not None and 'paths' not in sol_data:
            print('[viz] ERROR: expected lifelong export JSON for --scenario mode.')
            sys.exit(1)
        if not args.single_shot:
            args.single_shot = resolve_config_path(
                args.single_shot,
                ['MAPF_SINGLE_SHOT_SCENARIO_PATH', 'MAPF_MAP_PATH'],
                'config/scenarios/single_shot/square.yaml',
            )
        animate_single_solution(args, sol_data)


if __name__ == '__main__':
    main()
