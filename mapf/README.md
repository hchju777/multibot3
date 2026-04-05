# mapf - Execution Guide

This document is the practical execution guide for the `mapf` module.

For deeper design and research context, see:
- [`RESEARCH.md`](RESEARCH.md)
- [`../concept/MAPF_LAYER.md`](../concept/MAPF_LAYER.md)

Scenario YAML schema:
- [`schema/lifelong-scenario.schema.json`](schema/lifelong-scenario.schema.json)
- lifelong scenario files may reference it directly via
  `# yaml-language-server: $schema=../../../schema/lifelong-scenario.schema.json`

## Overview

The current MAPF flow is lifelong-only.

Main entry points:
- C++ runner: `mapf/build/mapf_lifelong_test`
- Python visualizer: `mapf/tools/visualize.py`

## Build

From the repository root:

```bash
cmake -S mapf -B mapf/build -DBUILD_TESTING=ON
cmake --build mapf/build -j1
```

`-j1` is the recommended default inside VS Code because some scenarios are
CPU-heavy and can make the IDE feel unresponsive.

## Run The C++ Solver

Run a lifelong scenario:

```bash
./mapf/build/mapf_lifelong_test mapf/config/scenarios/lifelong/corridor_n6.yaml
```

Export a JSON run for later inspection:

```bash
./mapf/build/mapf_lifelong_test \
  mapf/config/scenarios/lifelong/corridor_n6.yaml \
  --out /tmp/lifelong_run.json
```

If no scenario path is passed, the runner uses:
- `MAPF_LIFELONG_SCENARIO_PATH` from the environment
- or `MAPF_LIFELONG_SCENARIO_PATH` from the repo `.env`

## Visualize With Python

Install the Python packages once:

```bash
python3 -m pip install matplotlib pyyaml networkx
```

- `matplotlib`: required
- `pyyaml`: required
- `networkx`: optional but improves auto-layout when a map has no vertex
  coordinates

Run the solver and open the animation:

```bash
python3 mapf/tools/visualize.py \
  mapf/config/scenarios/lifelong/corridor_n6.yaml
```

By default the visualizer auto-detects the solver binary and checks:
- `mapf/build/mapf_lifelong_test`
- `build/mapf_lifelong_test`

You can still override it explicitly:

```bash
python3 mapf/tools/visualize.py \
  mapf/config/scenarios/lifelong/corridor_n6.yaml \
  --binary mapf/build/mapf_lifelong_test
```

Or reuse a previously exported JSON:

```bash
python3 mapf/tools/visualize.py --solution /tmp/lifelong_run.json
```

## Save Outputs

Save a video-like animation:

```bash
python3 mapf/tools/visualize.py \
  mapf/config/scenarios/lifelong/corridor_n6.yaml \
  --save /tmp/corridor_n6.gif \
  --fps 20 \
  --speed 1.25 \
  --no-show
```

- use `.gif` for the simplest portable export
- use `.mp4` when `ffmpeg` is available on the system

Save a machine-readable report:

```bash
python3 mapf/tools/visualize.py \
  mapf/config/scenarios/lifelong/corridor_n6.yaml \
  --report /tmp/corridor_n6_report.json \
  --no-show
```

- `.json` keeps full episode, event, conflict, and robot-stat structure
- `.csv` writes a flat report with `robot_stat`, `episode`, `event`, and
  `conflict` rows

Save a PNG frame sequence:

```bash
python3 mapf/tools/visualize.py \
  mapf/config/scenarios/lifelong/corridor_n6.yaml \
  --frames-dir /tmp/corridor_n6_frames \
  --dpi 160 \
  --no-show
```

- each frame is saved as `frame_0000.png`, `frame_0001.png`, ...
- `manifest.json` is written alongside the images

## Debugging Options

Focus on specific robots:

```bash
python3 mapf/tools/visualize.py \
  mapf/config/scenarios/lifelong/corridor_n6.yaml \
  --focus-robot robot0,robot1
```

Hide every unfocused robot:

```bash
python3 mapf/tools/visualize.py \
  mapf/config/scenarios/lifelong/corridor_n6.yaml \
  --focus-robot 0,1 \
  --hide-unfocused
```

- `--focus-robot` accepts robot ids or zero-based indices
- the viewer shows a live panel with current vertex, goal, moves, waits, and
  goal changes

If no scenario path is given, the visualizer also falls back to
`MAPF_LIFELONG_SCENARIO_PATH` from the environment or `.env`.

## Recommended Workflow

1. Build with `cmake -S mapf -B mapf/build -DBUILD_TESTING=ON`
2. Compile with `cmake --build mapf/build -j1`
3. Run `./mapf/build/mapf_lifelong_test ...` to verify the scenario
4. Run `python3 mapf/tools/visualize.py ...` to inspect it
5. Optionally save `gif/mp4/report/png` outputs for review or sharing
