#!/usr/bin/env python3
"""
visualize.py — MAPF solution animator

Usage:
    python3 tools/visualize.py [config/scenarios/lifelong/square.yaml]
    python3 tools/visualize.py --solution build/lifelong_run.json

Controls:
    Space       : play / pause
    ← / →       : step backward / forward
    Slider      : scrub to any timestep

The script runs the C++ solver automatically unless --solution is given.
"""

import argparse
import csv
from collections import defaultdict
import json
import math
import os
from pathlib import Path
import subprocess
import sys
import tempfile

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.animation import FFMpegWriter, FuncAnimation, PillowWriter
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


def resolve_binary_path(cli_binary):
    script_path = Path(__file__).resolve()
    mapf_dir = script_path.parent.parent
    repo_root = mapf_dir.parent

    candidates = []
    if cli_binary:
        raw = Path(cli_binary)
        if raw.is_absolute():
            candidates.append(raw)
        else:
            candidates.append((Path.cwd() / raw).resolve())
            candidates.append((repo_root / raw).resolve())
            candidates.append((mapf_dir / raw).resolve())
    else:
        candidates.extend([
            (repo_root / 'mapf/build/mapf_lifelong_test').resolve(),
            (repo_root / 'build/mapf_lifelong_test').resolve(),
            (mapf_dir / 'build/mapf_lifelong_test').resolve(),
        ])

    seen = set()
    unique_candidates = []
    for candidate in candidates:
        key = str(candidate)
        if key in seen:
            continue
        seen.add(key)
        unique_candidates.append(candidate)

    for candidate in unique_candidates:
        if candidate.exists() and os.access(candidate, os.X_OK):
            return str(candidate)

    checked = '\n'.join(f'- {candidate}' for candidate in unique_candidates)
    raise FileNotFoundError(
        'Could not find the lifelong solver binary.\n'
        f'Checked:\n{checked}\n'
        'Build it first with:\n'
        '  cmake -S mapf -B mapf/build -DBUILD_TESTING=ON\n'
        '  cmake --build mapf/build -j1'
    )


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
    binary = resolve_binary_path(binary)
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


def smooth_alpha(alpha):
    alpha = max(0.0, min(1.0, alpha))
    return alpha * alpha * (3.0 - 2.0 * alpha)


def interpolate_vertex(vertices, start_vid, end_vid, alpha):
    x0, y0 = vertices[start_vid]['x'], vertices[start_vid]['y']
    x1, y1 = vertices[end_vid]['x'], vertices[end_vid]['y']
    eased = smooth_alpha(alpha)
    return (
        x0 + (x1 - x0) * eased,
        y0 + (y1 - y0) * eased,
    )


def analyze_lifelong_conflicts(frames):
    frame_conflicts = defaultdict(list)
    transition_conflicts = defaultdict(list)
    frame_robot_flags = defaultdict(set)
    transition_robot_flags = defaultdict(set)

    if not frames:
        return frame_conflicts, transition_conflicts, frame_robot_flags, transition_robot_flags

    num_robots = len(frames[0]['positions'])

    for frame_idx, frame in enumerate(frames):
        occupancy = defaultdict(list)
        for robot_idx, vid in enumerate(frame['positions']):
            occupancy[vid].append(robot_idx)
        for vid, robot_indices in occupancy.items():
            if len(robot_indices) < 2:
                continue
            label = ', '.join(str(idx) for idx in robot_indices)
            frame_conflicts[frame_idx].append(
                f'vertex conflict at v{vid}: robots {label}'
            )
            frame_robot_flags[frame_idx].update(robot_indices)

    for frame_idx in range(len(frames) - 1):
        current = frames[frame_idx]['positions']
        nxt = frames[frame_idx + 1]['positions']

        for a in range(num_robots):
            for b in range(a + 1, num_robots):
                if current[a] == nxt[a] or current[b] == nxt[b]:
                    continue
                if current[a] == nxt[b] and current[b] == nxt[a]:
                    transition_conflicts[frame_idx].append(
                        f'swap conflict: robots {a} and {b} on {current[a]} <-> {current[b]}'
                    )
                    transition_robot_flags[frame_idx].update((a, b))

    for frame_idx in range(len(frames) - 2):
        prev_positions = frames[frame_idx]['positions']
        current_positions = frames[frame_idx + 1]['positions']
        next_positions = frames[frame_idx + 2]['positions']
        seen_pairs = set()

        for a in range(num_robots):
            u = prev_positions[a]
            v = current_positions[a]
            if u == v:
                continue

            for b in range(num_robots):
                if a == b:
                    continue
                key = tuple(sorted((a, b)))
                if key in seen_pairs:
                    continue
                if current_positions[b] == next_positions[b]:
                    continue
                if current_positions[b] == v and next_positions[b] == u:
                    frame_conflicts[frame_idx + 1].append(
                        f'non-passing conflict near t={frames[frame_idx + 1]["t"]}: '
                        f'robots {a} and {b} on {u} <-> {v}'
                    )
                    frame_robot_flags[frame_idx + 1].update((a, b))
                    seen_pairs.add(key)

    return frame_conflicts, transition_conflicts, frame_robot_flags, transition_robot_flags


def build_frame_samples(frame_count, smooth_steps):
    smooth_steps = max(int(smooth_steps), 1)
    if frame_count <= 1:
        return [0.0]

    samples = []
    for idx in range(frame_count - 1):
        for substep in range(smooth_steps):
            samples.append(idx + (substep / smooth_steps))
    samples.append(float(frame_count - 1))
    return samples


def build_episode_segments(frames, episodes):
    spans = []
    if not frames:
        return spans

    by_index = {episode['index']: episode for episode in episodes}
    start_idx = 0
    current_episode = frames[0].get('episode_index', 0)

    for frame_idx in range(1, len(frames)):
        next_episode = frames[frame_idx].get('episode_index', current_episode)
        if next_episode == current_episode:
            continue
        spans.append({
            'episode_index': current_episode,
            'frame_start': start_idx,
            'frame_end': frame_idx - 1,
            'episode': by_index.get(current_episode, {}),
        })
        start_idx = frame_idx
        current_episode = next_episode

    spans.append({
        'episode_index': current_episode,
        'frame_start': start_idx,
        'frame_end': len(frames) - 1,
        'episode': by_index.get(current_episode, {}),
    })
    return spans


def classify_conflict(detail):
    if detail.startswith('vertex conflict'):
        return 'vertex'
    if detail.startswith('swap conflict'):
        return 'swap'
    if detail.startswith('non-passing conflict'):
        return 'non_passing'
    return 'unknown'


def build_robot_stats(robots, frames):
    stats = []
    if not frames:
        return stats

    robot_count = len(frames[0]['positions'])
    for robot_idx in range(robot_count):
        robot = robots[robot_idx] if robot_idx < len(robots) else {'id': str(robot_idx)}
        positions = [frame['positions'][robot_idx] for frame in frames]
        goals = [frame['goals'][robot_idx] for frame in frames]

        move_count = 0
        wait_count = 0
        for frame_idx in range(len(frames) - 1):
            if positions[frame_idx] == positions[frame_idx + 1]:
                wait_count += 1
            else:
                move_count += 1

        goal_change_count = 0
        for frame_idx in range(1, len(goals)):
            if goals[frame_idx] != goals[frame_idx - 1]:
                goal_change_count += 1

        active_goal_frames = sum(1 for goal in goals if goal is not None)
        idle_frames = len(goals) - active_goal_frames

        stats.append({
            'robot_index': robot_idx,
            'robot_id': robot.get('id', str(robot_idx)),
            'start_vertex': positions[0],
            'end_vertex': positions[-1],
            'final_goal': goals[-1],
            'move_count': move_count,
            'wait_count': wait_count,
            'goal_change_count': goal_change_count,
            'active_goal_frames': active_goal_frames,
            'idle_frames': idle_frames,
            'unique_vertex_count': len(set(positions)),
        })

    return stats


def build_robot_timeline_stats(robots, frames):
    timelines = []
    if not frames:
        return timelines

    robot_count = len(frames[0]['positions'])
    for robot_idx in range(robot_count):
        robot = robots[robot_idx] if robot_idx < len(robots) else {'id': str(robot_idx)}
        entries = []
        move_count = 0
        wait_count = 0
        goal_change_count = 0

        for frame_idx, frame in enumerate(frames):
            current_goal = frame['goals'][robot_idx]
            current_vertex = frame['positions'][robot_idx]
            if frame_idx > 0:
                prev_frame = frames[frame_idx - 1]
                prev_vertex = prev_frame['positions'][robot_idx]
                prev_goal = prev_frame['goals'][robot_idx]
                if current_vertex == prev_vertex:
                    wait_count += 1
                else:
                    move_count += 1
                if current_goal != prev_goal:
                    goal_change_count += 1

            entries.append({
                'robot_index': robot_idx,
                'robot_id': robot.get('id', str(robot_idx)),
                'frame_index': frame_idx,
                't': frame['t'],
                'current_vertex': current_vertex,
                'current_goal': current_goal,
                'move_count_so_far': move_count,
                'wait_count_so_far': wait_count,
                'goal_change_count_so_far': goal_change_count,
            })
        timelines.append(entries)

    return timelines


def resolve_focus_robot_indices(focus_value, robots):
    if not focus_value:
        return []

    id_to_index = {robot['id']: idx for idx, robot in enumerate(robots)}
    indices = []
    for token in [part.strip() for part in focus_value.split(',') if part.strip()]:
        if token in id_to_index:
            indices.append(id_to_index[token])
            continue
        if token.isdigit():
            robot_idx = int(token)
            if 0 <= robot_idx < len(robots):
                indices.append(robot_idx)
                continue
        raise ValueError(
            f"Unknown focus robot '{token}'. "
            f"Use robot id or zero-based index."
        )

    return sorted(set(indices))


def build_lifelong_report(sol_data, frames, episodes, frame_conflicts, transition_conflicts):
    robot_stats = build_robot_stats(sol_data.get('robots', []), frames)
    conflict_entries = []
    for frame_idx, details in sorted(frame_conflicts.items()):
        frame = frames[frame_idx]
        for detail in details:
            conflict_entries.append({
                'kind': classify_conflict(detail),
                'phase': 'frame',
                'frame_index': frame_idx,
                't': frame['t'],
                'episode_index': frame.get('episode_index'),
                'detail': detail,
            })

    for frame_idx, details in sorted(transition_conflicts.items()):
        frame = frames[frame_idx]
        next_t = frames[frame_idx + 1]['t'] if frame_idx + 1 < len(frames) else frame['t']
        for detail in details:
            conflict_entries.append({
                'kind': classify_conflict(detail),
                'phase': 'transition',
                'frame_index': frame_idx,
                't': frame['t'],
                'next_t': next_t,
                'episode_index': frame.get('episode_index'),
                'detail': detail,
            })

    event_entries = []
    for frame_idx, frame in enumerate(frames):
        for event_id in frame.get('activated_event_ids', []):
            event_entries.append({
                'frame_index': frame_idx,
                't': frame['t'],
                'episode_index': frame.get('episode_index'),
                'event_id': event_id,
            })

    report = {
        'kind': 'lifelong_visualization_report_v1',
        'scenario_path': sol_data.get('scenario_path'),
        'map_path': sol_data.get('map_path'),
        'safe_stop_required': sol_data.get('safe_stop_required', False),
        'stop_reason': sol_data.get('stop_reason'),
        'summary': {
            'frame_count': len(frames),
            'episode_count': len(episodes),
            'event_count': len(event_entries),
            'conflict_count': len(conflict_entries),
            'robot_count': len(robot_stats),
            'total_moves': sum(stat['move_count'] for stat in robot_stats),
            'total_waits': sum(stat['wait_count'] for stat in robot_stats),
        },
        'episodes': episodes,
        'events': event_entries,
        'conflicts': conflict_entries,
        'robot_stats': robot_stats,
    }
    return report


def save_report(report_path, report_data):
    report_path = Path(report_path)
    suffix = report_path.suffix.lower()
    report_path.parent.mkdir(parents=True, exist_ok=True)

    if suffix == '.json':
        with open(report_path, 'w') as f:
            json.dump(report_data, f, indent=2)
            f.write('\n')
    elif suffix == '.csv':
        fieldnames = [
            'category', 'kind', 'phase', 'frame_index', 't', 'next_t',
            'episode_index', 'detail', 'event_id', 'mode', 'status',
            'used_global_replan', 'executed_until_timestep',
            'activated_event_ids', 'safe_stop_required', 'stop_reason',
            'robot_id', 'robot_index', 'start_vertex', 'end_vertex',
            'final_goal', 'move_count', 'wait_count', 'goal_change_count',
            'active_goal_frames', 'idle_frames', 'unique_vertex_count',
        ]
        with open(report_path, 'w', newline='') as f:
            writer = csv.DictWriter(f, fieldnames=fieldnames)
            writer.writeheader()

            for stat in report_data.get('robot_stats', []):
                writer.writerow({
                    'category': 'robot_stat',
                    'robot_id': stat.get('robot_id'),
                    'robot_index': stat.get('robot_index'),
                    'start_vertex': stat.get('start_vertex'),
                    'end_vertex': stat.get('end_vertex'),
                    'final_goal': stat.get('final_goal'),
                    'move_count': stat.get('move_count'),
                    'wait_count': stat.get('wait_count'),
                    'goal_change_count': stat.get('goal_change_count'),
                    'active_goal_frames': stat.get('active_goal_frames'),
                    'idle_frames': stat.get('idle_frames'),
                    'unique_vertex_count': stat.get('unique_vertex_count'),
                    'safe_stop_required': report_data.get('safe_stop_required'),
                    'stop_reason': report_data.get('stop_reason'),
                })

            for episode in report_data.get('episodes', []):
                writer.writerow({
                    'category': 'episode',
                    'episode_index': episode.get('index'),
                    'mode': episode.get('mode'),
                    'status': episode.get('status'),
                    'used_global_replan': episode.get('used_global_replan'),
                    'executed_until_timestep': episode.get('executed_until_timestep'),
                    'activated_event_ids': ','.join(episode.get('activated_event_ids', [])),
                    'safe_stop_required': report_data.get('safe_stop_required'),
                    'stop_reason': report_data.get('stop_reason'),
                })

            for event in report_data.get('events', []):
                writer.writerow({
                    'category': 'event',
                    'frame_index': event.get('frame_index'),
                    't': event.get('t'),
                    'episode_index': event.get('episode_index'),
                    'event_id': event.get('event_id'),
                    'safe_stop_required': report_data.get('safe_stop_required'),
                    'stop_reason': report_data.get('stop_reason'),
                })

            for conflict in report_data.get('conflicts', []):
                writer.writerow({
                    'category': 'conflict',
                    'kind': conflict.get('kind'),
                    'phase': conflict.get('phase'),
                    'frame_index': conflict.get('frame_index'),
                    't': conflict.get('t'),
                    'next_t': conflict.get('next_t'),
                    'episode_index': conflict.get('episode_index'),
                    'detail': conflict.get('detail'),
                    'safe_stop_required': report_data.get('safe_stop_required'),
                    'stop_reason': report_data.get('stop_reason'),
                })
    else:
        raise ValueError(
            f'Unsupported report format: {report_path.suffix}. '
            'Use .json or .csv'
        )

    print(f'[viz] Saved report: {report_path}')


def save_frame_sequence(fig, update_fn, frame_samples, frames_dir, dpi):
    frames_dir = Path(frames_dir)
    frames_dir.mkdir(parents=True, exist_ok=True)

    total = len(frame_samples)
    width = max(4, len(str(max(total - 1, 0))))
    manifest = []

    print(f'[viz] Saving frame sequence to {frames_dir} ({total} frames, dpi={dpi})')
    for frame_index, frame_pos in enumerate(frame_samples):
        update_fn(frame_pos)
        filename = f'frame_{frame_index:0{width}d}.png'
        output_path = frames_dir / filename
        fig.savefig(output_path, dpi=dpi, bbox_inches='tight')
        manifest.append({
            'frame_index': frame_index,
            'sample_position': frame_pos,
            'filename': filename,
        })

    manifest_path = frames_dir / 'manifest.json'
    with open(manifest_path, 'w') as f:
        json.dump({
            'kind': 'lifelong_frame_sequence_v1',
            'frame_count': total,
            'dpi': dpi,
            'frames': manifest,
        }, f, indent=2)
        f.write('\n')

    print(f'[viz] Saved frame sequence: {frames_dir}')
    print(f'[viz] Saved manifest: {manifest_path}')


def save_animation(fig, update_fn, frame_samples, save_path, fps, speed):
    save_path = Path(save_path)
    suffix = save_path.suffix.lower()
    effective_fps = max(1, int(round(float(fps) * max(float(speed), 0.01))))

    anim = FuncAnimation(
        fig,
        update_fn,
        frames=frame_samples,
        interval=max(10, int(1000 / effective_fps)),
        cache_frame_data=False,
    )

    if suffix == '.gif':
        writer = PillowWriter(fps=effective_fps)
    elif suffix in ('.mp4', '.m4v'):
        if not FFMpegWriter.isAvailable():
            raise RuntimeError(
                'Saving .mp4 requires ffmpeg, but it is not available. '
                'Install ffmpeg or save as .gif instead.'
            )
        writer = FFMpegWriter(fps=effective_fps)
    else:
        raise ValueError(
            f'Unsupported save format: {save_path.suffix}. '
            'Use .gif or .mp4'
        )

    print(f'[viz] Saving animation to {save_path} at {effective_fps} fps')
    anim.save(str(save_path), writer=writer)
    print(f'[viz] Saved animation: {save_path}')
    return anim


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
    episodes = sol_data.get('episodes', [])
    if not frames:
        print('[viz] Lifelong export contains no frames.')
        sys.exit(1)

    (frame_conflicts,
     transition_conflicts,
     frame_robot_flags,
     transition_robot_flags) = analyze_lifelong_conflicts(frames)
    robot_timeline_stats = build_robot_timeline_stats(robots, frames)
    focus_indices = resolve_focus_robot_indices(args.focus_robot, robots)
    if focus_indices:
        robot_panel_indices = focus_indices[:max(args.robot_panel_max, 1)]
    else:
        robot_panel_indices = list(range(min(max(args.robot_panel_max, 1), len(robots))))
    episode_spans = build_episode_segments(frames, episodes)
    episodes_by_index = {episode['index']: episode for episode in episodes}
    report_data = build_lifelong_report(
        sol_data,
        frames,
        episodes,
        frame_conflicts,
        transition_conflicts,
    )

    fig, ax = plt.subplots(figsize=(9, 7))
    plt.subplots_adjust(bottom=0.24)
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
    trail_lines = []

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
        trail_line, = ax.plot([], [], color=col, lw=3,
                              alpha=0.22, zorder=4)
        robot_circles.append(circ)
        robot_texts.append(txt)
        goal_markers.append(goal_marker)
        trail_lines.append(trail_line)

    total_t = frames[-1]['t']
    title = ax.set_title(f't = {frames[0]["t"]}  /  {total_t}',
                         fontsize=13, pad=10)
    warning_text = ax.text(
        0.02, 0.98, '',
        transform=ax.transAxes,
        ha='left', va='top',
        fontsize=9,
        color='#b00020',
        fontweight='bold',
        bbox={
            'boxstyle': 'round,pad=0.35',
            'facecolor': '#fff2f2',
            'edgecolor': '#d66',
            'alpha': 0.9,
        },
        zorder=10,
    )
    warning_text.set_visible(False)

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

    timeline_ax = plt.axes([0.15, 0.11, 0.70, 0.05])
    timeline_ax.set_xlim(0, len(frames) - 1)
    timeline_ax.set_ylim(0, 1)
    timeline_ax.set_yticks([])
    timeline_ax.set_xticks([])
    timeline_ax.set_facecolor('#f8f8f8')
    for spine in timeline_ax.spines.values():
        spine.set_visible(False)

    if episode_spans:
        for span in episode_spans:
            episode = span['episode']
            mode = episode.get('mode', 'REPLAN')
            used_global = episode.get('used_global_replan', False)
            if mode == 'INITIAL':
                color = '#d7ebff'
            elif used_global:
                color = '#ffe0d9'
            else:
                color = '#fff0c9'
            timeline_ax.axvspan(
                span['frame_start'],
                span['frame_end'] + 1e-6,
                0.15,
                0.85,
                color=color,
                ec='#c9c9c9',
                lw=0.6,
                zorder=1,
            )
            timeline_ax.text(
                (span['frame_start'] + span['frame_end']) / 2.0,
                0.5,
                f'ep{span["episode_index"]}',
                ha='center',
                va='center',
                fontsize=8,
                color='#333333',
                zorder=2,
            )

    event_frames = [
        frame_idx for frame_idx, frame in enumerate(frames)
        if frame.get('activated_event_ids')
    ]
    if event_frames:
        timeline_ax.scatter(
            event_frames,
            [0.92] * len(event_frames),
            s=22,
            color='#1f78b4',
            marker='o',
            zorder=3,
        )

    current_cursor = timeline_ax.axvline(0, color='#202020', lw=2.0, zorder=4)
    timeline_ax.text(
        0.0, 1.08,
        'Episode timeline',
        transform=timeline_ax.transAxes,
        ha='left', va='bottom',
        fontsize=9,
        color='#444444',
    )

    ax_sl = plt.axes([0.15, 0.05, 0.70, 0.03])
    slider = Slider(ax_sl, 'time', 0, len(frames) - 1,
                    valinit=0, color='#3498db')
    runtime_text = ax.text(
        0.02, 0.02, '',
        transform=ax.transAxes,
        ha='left', va='bottom',
        fontsize=9,
        color='#222222',
        bbox={
            'boxstyle': 'round,pad=0.35',
            'facecolor': '#ffffff',
            'edgecolor': '#cccccc',
            'alpha': 0.95,
        },
        zorder=10,
    )
    robot_panel_text = ax.text(
        0.98, 0.02, '',
        transform=ax.transAxes,
        ha='right', va='bottom',
        fontsize=9,
        color='#222222',
        bbox={
            'boxstyle': 'round,pad=0.35',
            'facecolor': '#ffffff',
            'edgecolor': '#cccccc',
            'alpha': 0.95,
        },
        zorder=10,
    )
    safe_stop_text = ax.text(
        0.98, 0.98, '',
        transform=ax.transAxes,
        ha='right', va='top',
        fontsize=10,
        color='#7a0012',
        fontweight='bold',
        bbox={
            'boxstyle': 'round,pad=0.35',
            'facecolor': '#fff1f3',
            'edgecolor': '#d66',
            'alpha': 0.95,
        },
        zorder=10,
    )
    if sol_data.get('safe_stop_required', False):
        stop_reason = sol_data.get('stop_reason') or 'unknown'
        safe_stop_text.set_text(f'SAFE STOP\nreason: {stop_reason}')
        safe_stop_text.set_visible(True)
    else:
        safe_stop_text.set_visible(False)

    def redraw(frame_pos):
        frame_pos = max(0.0, min(float(frame_pos), len(frames) - 1))
        base_idx = min(int(math.floor(frame_pos)), len(frames) - 1)
        next_idx = min(base_idx + 1, len(frames) - 1)
        alpha = frame_pos - base_idx if next_idx > base_idx else 0.0
        frame = frames[base_idx]
        next_frame = frames[next_idx]
        actual_t = frame['t'] + (next_frame['t'] - frame['t']) * alpha
        current_cursor.set_xdata([frame_pos, frame_pos])

        highlighted = set(frame_robot_flags.get(base_idx, set()))
        if alpha > 0.0:
            highlighted.update(transition_robot_flags.get(base_idx, set()))

        for i, robot in enumerate(robots):
            start_vid = frame['positions'][i]
            end_vid = next_frame['positions'][i]
            x, y = interpolate_vertex(vertices, start_vid, end_vid, alpha)
            robot_circles[i].center = (x, y)
            robot_texts[i].set_position((x, y))
            is_focused = not focus_indices or i in focus_indices
            if args.hide_unfocused and focus_indices and not is_focused:
                robot_circles[i].set_visible(False)
                robot_texts[i].set_visible(False)
                goal_markers[i].set_visible(False)
                trail_lines[i].set_visible(False)
                continue

            robot_circles[i].set_visible(True)
            robot_texts[i].set_visible(True)
            goal_markers[i].set_visible(True)
            trail_lines[i].set_visible(True)

            if i in highlighted:
                robot_circles[i].set_edgecolor('#b00020')
                robot_circles[i].set_linewidth(3.0)
                robot_circles[i].set_alpha(0.98 if is_focused else 0.45)
                robot_texts[i].set_alpha(1.0 if is_focused else 0.5)
            else:
                robot_circles[i].set_edgecolor('#222222' if is_focused else '#7d7d7d')
                robot_circles[i].set_linewidth(2.2 if focus_indices and is_focused else 1.5)
                robot_circles[i].set_alpha(0.92 if is_focused else 0.22)
                robot_texts[i].set_alpha(1.0 if is_focused else 0.35)

            goal_vid = frame['goals'][i]
            if goal_vid is None:
                goal_markers[i].set_data([], [])
            else:
                gx, gy = vertices[goal_vid]['x'], vertices[goal_vid]['y']
                goal_markers[i].set_data([gx], [gy])
            goal_markers[i].set_alpha(0.55 if is_focused else 0.15)

            trail_start = max(0, base_idx - max(args.trail_length, 0) + 1)
            trail_points = []
            for hist_idx in range(trail_start, base_idx + 1):
                hist_vid = frames[hist_idx]['positions'][i]
                trail_points.append(
                    (vertices[hist_vid]['x'], vertices[hist_vid]['y'])
                )
            if alpha > 0.0 or not trail_points:
                trail_points.append((x, y))
            xs = [point[0] for point in trail_points]
            ys = [point[1] for point in trail_points]
            trail_lines[i].set_data(xs, ys)
            trail_lines[i].set_alpha(0.22 if is_focused else 0.05)

        events = ', '.join(frame.get('activated_event_ids', []))
        suffix = f'  events: {events}' if events else ''
        title.set_text(f't = {actual_t:.2f}  /  {total_t}  ep={frame["episode_index"]}{suffix}')
        episode = episodes_by_index.get(frame['episode_index'], {})
        mode = episode.get('mode', 'UNKNOWN')
        status = episode.get('status', 'UNKNOWN')
        replan_kind = 'global' if episode.get('used_global_replan', False) else 'affected/local'
        executed_until = episode.get('executed_until_timestep', '-')
        episode_events = ', '.join(episode.get('activated_event_ids', [])) or '-'
        runtime_text.set_text(
            f'episode: {frame["episode_index"]}\n'
            f'mode: {mode}\n'
            f'status: {status}\n'
            f'replan: {replan_kind}\n'
            f'executed_until: {executed_until}\n'
            f'events: {episode_events}'
        )
        panel_lines = []
        for robot_idx in robot_panel_indices:
            snapshot = robot_timeline_stats[robot_idx][base_idx]
            goal_text = snapshot['current_goal']
            if goal_text is None:
                goal_text = '-'
            panel_lines.append(
                f"{snapshot['robot_id']}  v={snapshot['current_vertex']}  g={goal_text}"
            )
            panel_lines.append(
                f"  moves={snapshot['move_count_so_far']}  waits={snapshot['wait_count_so_far']}  "
                f"goalchg={snapshot['goal_change_count_so_far']}"
            )
        if focus_indices:
            prefix = 'Focus robots'
        else:
            prefix = 'Robot snapshot'
        robot_panel_text.set_text(prefix + '\n' + '\n'.join(panel_lines))

        active_warnings = list(frame_conflicts.get(base_idx, []))
        if alpha > 0.0:
            active_warnings.extend(transition_conflicts.get(base_idx, []))
        if active_warnings:
            warning_text.set_text('\n'.join(active_warnings[:3]))
            warning_text.set_visible(True)
        else:
            warning_text.set_visible(False)
        fig.canvas.draw_idle()

    slider.on_changed(redraw)
    redraw(0)

    frame_samples = build_frame_samples(len(frames), args.smooth_steps)

    if args.report:
        save_report(args.report, report_data)
    if args.frames_dir:
        save_frame_sequence(
            fig,
            redraw,
            frame_samples,
            args.frames_dir,
            args.dpi,
        )

    playing = [False]
    anim = [None]

    def toggle_play(_event=None):
        playing[0] = not playing[0]
        if playing[0]:
            def step(_frame):
                current = float(slider.val)
                next_value = current + (1.0 / max(args.smooth_steps, 1))
                if next_value < len(frames) - 1:
                    slider.set_val(next_value)
                else:
                    slider.set_val(len(frames) - 1)
                    playing[0] = False
            anim[0] = FuncAnimation(fig, step,
                                    interval=max(10, int(args.interval / max(args.smooth_steps, 1))),
                                    cache_frame_data=False)
        else:
            if anim[0]:
                anim[0].event_source.stop()
        fig.canvas.draw_idle()

    def on_key(event):
        if event.key == ' ':
            toggle_play()
        elif event.key == 'right':
            slider.set_val(min(math.floor(float(slider.val)) + 1, len(frames) - 1))
        elif event.key == 'left':
            slider.set_val(max(math.ceil(float(slider.val)) - 1, 0))

    fig.canvas.mpl_connect('key_press_event', on_key)
    print('[viz] Controls: Space=play/pause  ←→=step  Slider=scrub')
    print(f'[viz] Smooth playback: {args.smooth_steps} substeps per discrete timestep')

    if args.save:
        anim[0] = save_animation(
            fig,
            redraw,
            frame_samples,
            args.save,
            args.fps,
            args.speed,
        )

    if args.no_show:
        plt.close(fig)
        return

    plt.show()


# ── 메인 ─────────────────────────────────────────────────────
def main():
    parser = argparse.ArgumentParser(description='MAPF solution visualizer')
    parser.add_argument(
        'scenario',
        nargs='?',
        default=None,
        help='Lifelong scenario YAML (default: MAPF_LIFELONG_SCENARIO_PATH from env/.env, else config/scenarios/lifelong/square.yaml)',
    )
    parser.add_argument('--solution', '-s',
                        help='Solution JSON — skip running the solver')
    parser.add_argument('--binary', '-b',
                        default=None,
                        help='Lifelong C++ solver binary (auto-detects mapf/build/mapf_lifelong_test by default)')
    parser.add_argument('--interval', type=int, default=600,
                        help='Animation frame interval in ms (default: 600)')
    parser.add_argument('--smooth-steps', type=int, default=10,
                        help='Substeps per discrete timestep for video-like playback (default: 10)')
    parser.add_argument('--trail-length', type=int, default=8,
                        help='Visible history length in timesteps for each robot trail (default: 8)')
    parser.add_argument('--save',
                        help='Save animation to .gif or .mp4')
    parser.add_argument('--fps', type=int, default=20,
                        help='Base frames per second when saving animation (default: 20)')
    parser.add_argument('--speed', type=float, default=1.0,
                        help='Playback speed multiplier when saving animation (default: 1.0)')
    parser.add_argument('--no-show', action='store_true',
                        help='Do not open the interactive window after saving')
    parser.add_argument('--report',
                        help='Write a visualization report as .json or .csv')
    parser.add_argument('--frames-dir',
                        help='Write a PNG frame sequence into this directory')
    parser.add_argument('--dpi', type=int, default=140,
                        help='DPI for saved PNG frames (default: 140)')
    parser.add_argument('--focus-robot',
                        help='Comma-separated robot ids or zero-based indices to emphasize in the viewer')
    parser.add_argument('--hide-unfocused', action='store_true',
                        help='Hide robots that are not in --focus-robot')
    parser.add_argument('--robot-panel-max', type=int, default=4,
                        help='Maximum number of robots to show in the on-screen robot panel (default: 4)')
    args = parser.parse_args()

    if args.solution:
        sol_data = load_solution(args.solution)
    else:
        args.scenario = resolve_config_path(
            args.scenario,
            'MAPF_LIFELONG_SCENARIO_PATH',
            'config/scenarios/lifelong/square.yaml')
        tmp = tempfile.NamedTemporaryFile(suffix='.json', delete=False)
        tmp.close()
        try:
            run_lifelong_solver(args.scenario, args.binary, tmp.name)
            sol_data = load_solution(tmp.name)
        finally:
            os.unlink(tmp.name)

    if sol_data.get('kind') != 'lifelong_run_v1':
        print('[viz] ERROR: expected lifelong export JSON.')
        sys.exit(1)
    animate_lifelong_solution(args, sol_data)


if __name__ == '__main__':
    main()
