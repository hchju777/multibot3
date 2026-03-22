# mapf — Multi-Agent Path Finding Layer

A standalone C++17 implementation of **LaCAM★** for the multibot3 project.
Produces collision-free full paths for all agents, which are fed into the downstream **SADG layer**.
The planner library owns both primary LaCAM★ search and prioritized fallback search, while the CLI
runner performs strict final physical-validity checks before printing or exporting results.

> Design reference: [`concept/MAPF_LAYER.md`](../concept/MAPF_LAYER.md)

---

## Table of Contents

1. [Overview](#overview)
2. [Requirements](#requirements)
3. [Build](#build)
4. [Run](#run)
5. [Visualization](#visualization)
6. [Configuration](#configuration)
   - [Map YAML](#map-yaml)
   - [Params YAML](#params-yaml)
   - [Lifelong Scenario YAML](#lifelong-scenario-yaml)
7. [File Structure](#file-structure)
8. [Architecture](#architecture)
9. [Algorithm](#algorithm)
10. [For Developers](#for-developers)
11. [References](#references)

---

## Overview

The MAPF layer solves the **Multi-Agent Path Finding** problem on a general graph G = (V, E):

```
Given : G=(V,E),  starts s₁…sₙ,  goals g₁…gₙ
Find  : collision-free paths π₁…πₙ  (vertex sequence per timestep)
```

Three collision types are enforced (including the non-standard **non-passing** constraint required by the SADG layer):

| # | Name | Condition |
|---|------|-----------|
| ① | Vertex collision | Two robots at same vertex at same timestep |
| ② | Swap collision | Two robots exchange positions in same timestep |
| ③ | Non-passing collision | Same edge traversed in opposite directions in consecutive timesteps |

The output format — `[(vertex_id, timestep), …]` per robot — is directly compatible with the SADG layer (SE-ADG, Berndt et al. TRO-2024).

In addition to graph-theoretic collisions, the runner validates that:
- every move uses a real edge (or wait),
- no vertex collision exists at any timestep,
- no swap collision exists in the same timestep,
- no non-passing collision exists across consecutive timesteps,
- every robot starts at its declared start and ends at its declared goal.

---

## Requirements

### C++ Build

| Dependency | Version | Purpose |
|---|---|---|
| CMake | ≥ 3.16 | Build system |
| GCC / Clang | C++17 support | Compiler |
| [yaml-cpp](https://github.com/jbeder/yaml-cpp) | 0.8+ | YAML config parsing |
| [spdlog](https://github.com/gabime/spdlog) | 1.x | Structured runtime logging |

Install on Ubuntu:
```bash
sudo apt install cmake g++ libyaml-cpp-dev libspdlog-dev
```

### Python Visualization (optional)

| Package | Install |
|---|---|
| Python 3 | pre-installed on Ubuntu |
| matplotlib | `sudo apt install python3-matplotlib` |
| pyyaml | `sudo apt install python3-yaml` |
| networkx *(optional)* | `sudo apt install python3-networkx` — used for auto-layout if vertex positions are not in the map YAML |

---

## Build

```bash
cd mapf
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# optional: run all automated tests
ctest --output-on-failure
```

This produces:
- `build/mapf_test` — the test/runner executable
- `build/mapf_lifelong_test` — scripted lifelong event-stream runner
- `build/libmapf_lib.a` — static library for linking into other modules (e.g. ROS2)
- `build/mapf_unit_tests` — core unit-test executable
- `build/mapf_regression_tests` — shipped-config regression executable

---

## Run

The binaries must be run from **`mapf/build/`**.

Config categories are now:
- `config/params/` — planner / preprocessing params only
- `config/maps/` — graph only
- `config/scenarios/single_shot/` — one-shot MAPF problems
- `config/scenarios/lifelong/` — event-stream lifelong problems

If positional arguments are omitted, `mapf_test` resolves the **single-shot scenario** path in this order:
- explicit CLI `single_shot_scenario.yaml`
- `MAPF_SINGLE_SHOT_SCENARIO_PATH` from the process environment
- `MAPF_SINGLE_SHOT_SCENARIO_PATH` from the nearest `.env`
- legacy aliases `MAPF_MAP_PATH` from env / `.env`
- built-in default `../config/scenarios/single_shot/square.yaml`

If the params override is omitted, `mapf_test` resolves params in this order:
- explicit CLI `params.yaml`
- `MAPF_PARAMS_PATH` from env / `.env`
- `params:` field inside the single-shot scenario
- built-in default `../config/params/default.yaml`

If the scenario path is omitted, `mapf_lifelong_test` resolves it in this order:
- explicit CLI `lifelong_scenario.yaml`
- `MAPF_LIFELONG_SCENARIO_PATH` from env / `.env`
- built-in default `../config/scenarios/lifelong/square.yaml`

The repository root can define these values in `.env`, for example:

```dotenv
MAPF_SINGLE_SHOT_SCENARIO_PATH=mapf/config/scenarios/single_shot/square.yaml
MAPF_PARAMS_PATH=mapf/config/params/default.yaml
MAPF_LIFELONG_SCENARIO_PATH=mapf/config/scenarios/lifelong/corridor_n8.yaml
```

```bash
cd mapf/build

# default — single-shot scenario and params come from .env or scenario defaults
./mapf_test

# use a shipped single-shot scenario
./mapf_test ../config/scenarios/single_shot/corridor_n8.yaml

# override params explicitly
./mapf_test ../config/scenarios/single_shot/corridor_n8.yaml \
    ../config/params/default.yaml

# write solution to JSON
./mapf_test ../config/scenarios/single_shot/corridor.yaml --out solution.json

# enable runtime logging
./mapf_test ../config/scenarios/single_shot/grid_4x4.yaml --log-level debug

# scripted lifelong scenario replay
./mapf_lifelong_test ../config/scenarios/lifelong/square.yaml --log-level info

# omit scenario path and use MAPF_LIFELONG_SCENARIO_PATH from .env
# the repository .env currently points to lifelong/corridor_n8.yaml
./mapf_lifelong_test

# export lifelong timeline JSON
./mapf_lifelong_test ../config/scenarios/lifelong/square.yaml --out lifelong_run.json
```

### Output

```
[20:34:12.418] [info] Scenario: /abs/path/to/mapf/config/scenarios/single_shot/square.yaml
[20:34:12.418] [info] Map     : /abs/path/to/mapf/config/maps/square.yaml
[20:34:12.418] [info] Params  : /abs/path/to/mapf/config/params/default.yaml
[20:34:12.419] [info] [OK] solution found
[20:34:12.419] [info] [OK] solution validation passed
[20:34:12.419] [info] === Solution ===
[20:34:12.419] [info] Robot 0: v0(t=0) v1(t=1) v3(t=2)
[20:34:12.419] [info] Robot 1: v3(t=0) v2(t=1) v0(t=2)
```

Async fallback is mode-dependent:

- `INITIAL`: after `initial_async_fallback_delay_ms`, the planner may start prioritized fallback
  on another thread. If fallback finds a valid path first, that path becomes the incumbent, but
  primary LaCAM★ still gets `initial_primary_grace_ms` to try to beat it before returning.
- `REPLAN`: after `replan_async_fallback_delay_ms`, the planner may start prioritized fallback,
  and the first valid solution can win immediately for lower latency.

If async fallback is disabled for a mode, the planner falls back synchronously only after primary
LaCAM★ search fails to produce a valid path within that mode's stop condition.

All runner messages are emitted through `spdlog`.
Logging defaults to `info`. You can override it with `--log-level` or `MAPF_LOG_LEVEL`.
Supported levels: `trace`, `debug`, `info`, `warn`, `error`, `critical`, `off`.

---

## Visualization

`tools/visualize.py` runs the solver automatically and opens an interactive animation.
**Must be run from the `mapf/` directory**. It supports both:
- single-shot scenarios through `build/mapf_test`
- lifelong event-stream scenarios through `build/mapf_lifelong_test`

If the single-shot scenario path is omitted, `visualize.py` resolves it in this order:
- explicit CLI `single_shot_scenario.yaml`
- `MAPF_SINGLE_SHOT_SCENARIO_PATH` from env / `.env`
- legacy aliases `MAPF_MAP_PATH` from env / `.env`
- built-in default `config/scenarios/single_shot/square.yaml`

If the params override is omitted, `visualize.py` resolves it in this order:
- explicit CLI `params.yaml`
- `MAPF_PARAMS_PATH` from env / `.env`
- `params:` field inside the single-shot scenario

> **Requirement:** A graphical display is required (`DISPLAY` must be set on Linux).
> On a headless server, use `--out` to generate a JSON solution file and visualize it locally.

```bash
cd mapf

# run a single-shot scenario automatically and open animation
python3 tools/visualize.py config/scenarios/single_shot/square.yaml

# omit params.yaml and use MAPF_PARAMS_PATH from .env
python3 tools/visualize.py config/scenarios/single_shot/square.yaml

# omit both scenario and params and use .env / scenario defaults
python3 tools/visualize.py

# corridor scenario
python3 tools/visualize.py config/scenarios/single_shot/corridor.yaml

# lifelong event-stream scenario
python3 tools/visualize.py --scenario config/scenarios/lifelong/square.yaml

# scenario from MAPF_LIFELONG_SCENARIO_PATH in .env
# the repository .env currently points to lifelong/corridor_n8.yaml
python3 tools/visualize.py --scenario

# generate solution JSON first, then visualize
./build/mapf_test config/scenarios/single_shot/corridor.yaml --out solution.json
python3 tools/visualize.py config/scenarios/single_shot/corridor.yaml \
    --solution build/solution.json

# generate lifelong timeline JSON first, then visualize
./build/mapf_lifelong_test config/scenarios/lifelong/square.yaml --out lifelong_run.json
python3 tools/visualize.py --solution build/lifelong_run.json

# slower animation (ms per frame)
python3 tools/visualize.py config/scenarios/single_shot/corridor.yaml --interval 1000
```

> **Note:** For single-shot replay, the `--solution` JSON must have been generated from the
> **same scenario / underlying map** passed as the first argument. Using a solution from a
> different scenario causes a vertex-id mismatch error.

For lifelong exports, the JSON already contains the planning graph and per-frame goal changes.
When possible, the visualizer derives vertex coordinates from the original map referenced by the
scenario; otherwise it falls back to auto layout.

**Controls:**

| Input | Action |
|---|---|
| `Space` | Play / Pause |
| `←` | Step one timestep backward |
| `→` | Step one timestep forward |
| Slider | Scrub to any timestep |

**What is shown:**

- Gray circles — vertices (labeled by id)
- Colored circles — robots (move along their path)
- Colored `×` — each robot's goal vertex
- Legend (top-right) — robot id, start → goal mapping

If vertex `x, y` coordinates are not present in the map YAML, the script falls back to:
- `networkx` spring layout (if installed)
- Circular layout (always available)

---

## Configuration

### Map YAML

`config/maps/*.yaml` contains **graph only**.

```yaml
graph:
  vertices:
    - id: 0        # required — must be 0-indexed, contiguous
      x: 0.0       # optional — used for visualization
      y: 1.0       # optional — used for visualization
    - id: 1
      x: 1.0
      y: 1.0
    # ...
  edges:
    - [0, 1]       # undirected edge
    - [1, 3]
    # ...
```

**Rules:**
- Vertex ids must be **contiguous starting from 0**.
- The graph **must be biconnected** (verified at startup). If it is not, the planner exits with `[FAIL]`.
- For narrow corridors (bridge chains), enable `virtual_lock: true` in `params/default.yaml` — see [Virtual Lock Vertex](#virtual-lock-vertex).

### Single-Shot Scenario YAML

`config/scenarios/single_shot/*.yaml` describes one MAPF problem instance.

```yaml
version: 1
map: ../../maps/square.yaml
params: ../../params/default.yaml

robots:
  - id: 0
    start: 0
    goal: 3
  - id: 1
    start: 3
    goal: 0
```

This keeps graph structure in `maps/` and robot assignments in `scenarios/single_shot/`.

### Params YAML

`config/params/default.yaml` — algorithm tuning, independent of the map.

```yaml
planner:
  # Initial planning — quality-first
  initial_quality_threshold: 1.2   # stop when cost ≤ optimal × 1.2
  initial_timeout_ms: 5000         # hard limit (ms)

  # Replanning — speed-first (first solution wins)
  replan_timeout_ms: 500           # hard limit (ms)

  seed: 0                          # RNG seed (affects PIBT ordering)

  # INITIAL async fallback policy
  initial_async_fallback: true
  initial_async_fallback_delay_ms: 50
  initial_primary_grace_ms: 100

  # REPLAN async fallback policy
  replan_async_fallback: true
  replan_async_fallback_delay_ms: 0

preprocessing:
  virtual_lock: true               # collapse narrow corridors (§5.3)
```

All keys are **optional** — missing entries use the defaults shown above.

Backward-compatible aliases are still accepted:

```yaml
planner:
  async_fallback: true
  async_fallback_delay_ms: 50
```

Those legacy keys seed both INITIAL and REPLAN fallback settings unless the mode-specific keys are
also present.

The current policy split is:

- `INITIAL`: quality-aware. Fallback can start early, but primary gets a short grace window before
  the incumbent fallback result is returned.
- `REPLAN`: latency-oriented. The first valid solution can win immediately.

This keeps dense-case runtime low while avoiding the old behavior where INITIAL mode could return a
fallback solution too aggressively before primary had a final chance to improve cost.

---

### Lifelong Scenario YAML

`config/scenarios/lifelong/square.yaml` demonstrates the current scripted lifelong format.
All shipped maps also have matching event-stream scenarios under `config/scenarios/lifelong/`.

```yaml
version: 1
map: ../../maps/square.yaml
params: ../../params/default.yaml

robots:
  - id: R0
    start: 0
    home: 0
  - id: R1
    start: 3
    home: 3

scenario:
  replan_policy: replan_on_assignment
  events:
    - id: e0
      type: assign_goal
      robot: R0
      goal: 3
      when: {type: at_start}
    - id: e2
      type: assign_goal
      robot: R0
      goal: 0
      when: {type: on_robot_idle, robot: R0}
```

Current v1 rules:

- canonical scenario format is **event stream**
- supported event type: `assign_goal`
- supported triggers: `at_start`, `on_robot_idle`
- replanning policy: event-triggered **global replanning**
- runtime state is kept on the **planning graph** after preprocessing, so replanning can continue
  even when virtual-lock vertices are in use

Use the runner from `mapf/build`:

```bash
# explicit scenario
./mapf_lifelong_test ../config/scenarios/lifelong/square.yaml --log-level info

# scenario from .env / built-in default
./mapf_lifelong_test
```

The current implementation is still scripted. Later, the same `assign_goal(robot, goal)` event
model can be fed from a live external system instead of YAML replay.

---

## File Structure

```
mapf/
├── CMakeLists.txt
├── README.md
│
├── include/                  ← public headers (linked into ROS2 later)
│   ├── graph.hpp             ← Vertex, Graph (biconnected undirected)
│   ├── dist_table.hpp        ← BFS distance table per agent
│   ├── collision.hpp         ← CollisionChecker (①②③)
│   ├── pibt.hpp              ← PIBT configuration generator
│   ├── solution.hpp          ← Solution type (SADG-compatible)
│   ├── solution_validation.hpp ← shared physical-validity checker
│   ├── stop_condition.hpp    ← StopCondition (INITIAL / REPLAN)
│   ├── planner.hpp           ← LaCAM★ planner, Instance, HNode, LNode
│   ├── params.hpp            ← Params (loaded from params/*.yaml)
│   ├── logger.hpp            ← spdlog setup helpers
│   ├── map_loader.hpp        ← graph-only YAML loader
│   ├── prepared_map.hpp      ← persistent preprocessed map + vertex remap
│   ├── goal_distance_cache.hpp ← reusable BFS cache by planning-goal vertex
│   ├── lifelong_types.hpp    ← lifelong scenario / runtime data types
│   ├── scenario_loader.hpp   ← event-stream scenario YAML loader
│   ├── lifelong_planner.hpp  ← scripted lifelong runtime
│   ├── graph_preprocessor.hpp ← Virtual lock vertex transform
│   └── config_loader.hpp     ← single-shot scenario YAML → Graph + Instance
│
├── src/                      ← implementations
│   ├── graph.cpp
│   ├── dist_table.cpp
│   ├── collision.cpp
│   ├── pibt.cpp
│   ├── solution_validation.cpp
│   ├── solution.cpp
│   ├── stop_condition.cpp
│   ├── planner.cpp
│   ├── params.cpp
│   ├── logger.cpp
│   ├── map_loader.cpp
│   ├── prepared_map.cpp
│   ├── goal_distance_cache.cpp
│   ├── scenario_loader.cpp
│   ├── lifelong_planner.cpp
│   ├── graph_preprocessor.cpp
│   └── config_loader.cpp
│
├── config/
│   ├── params/
│   │   └── default.yaml
│   ├── maps/
│   │   ├── square.yaml
│   │   ├── corridor.yaml
│   │   ├── grid_4x4.yaml
│   │   ├── ring_24.yaml
│   │   └── warehouse_5x4.yaml
│   └── scenarios/
│       ├── single_shot/
│       │   ├── square.yaml
│       │   ├── corridor.yaml
│       │   ├── corridor_n4.yaml
│       │   ├── corridor_n6.yaml
│       │   ├── corridor_n8.yaml
│       │   ├── grid_4x4.yaml
│       │   ├── ring_24.yaml
│       │   └── warehouse_5x4.yaml
│       └── lifelong/
│           ├── square.yaml
│           ├── corridor.yaml
│           ├── corridor_n4.yaml
│           ├── corridor_n6.yaml
│           ├── corridor_n8.yaml
│           ├── grid_4x4.yaml
│           ├── ring_24.yaml
│           └── warehouse_5x4.yaml
│
├── test/
│   ├── smoke_test.cpp        ← mapf_test executable (CLI runner)
│   ├── lifelong_smoke_test.cpp ← mapf_lifelong_test executable
│   ├── unit_tests.cpp        ← core unit tests
│   ├── regression_tests.cpp  ← shipped-config regression tests
│   └── test_harness.hpp      ← lightweight shared test harness
│
└── tools/
    └── visualize.py          ← interactive solution animator
```

---

## Architecture

```
config/scenarios/single_shot/square.yaml   config/params/default.yaml
                │                                     │
          ConfigLoader                          Params::load()
                │                                     │
                ├── map -> config/maps/square.yaml    PlannerParams
                ├── preprocess()                      PreprocessingParams
                │   (GraphPreprocessor virtual lock)
                │
                ├── Graph  ──────── DistTable   StopCondition
                │   (biconnected    (BFS per        (INITIAL /
                │    undirected)     agent)          REPLAN)
                │
                └── Instance ──────────────────── Planner (LaCAM★)
                        (G, starts, goals)              │
                                                   CollisionChecker
                                                        │
                                                   PIBT (config gen)
                                                        │
                                                   Solution
                                             [(vertex_id, t), ...]
                                                        │
                                                  SADG Layer ──► Execution
```

---

## Algorithm

### LaCAM★ (Two-Level Search)

**High-level search** — DFS over the configuration space:
- Each `HNode` holds a joint configuration C = (v₁, …, vₙ) and a low-level queue of `LNode`s.
- Popping an HNode from `OPEN` triggers one `expand()` call.
- After expansion, HNode is **re-queued** if its low-level queue is non-empty (so all constraint alternatives are explored).
- A* f-value pruning (`g + h ≥ best_cost`) is applied.

**Low-level search** — PIBT constraint tree:
- Each `LNode` represents one additional constraint: "agent `who` must not use vertex `where`".
- A chain of LNodes (root → leaf) defines the full constraint set for one PIBT call.
- Collision detection after each PIBT call generates child LNodes for the next HNode.

**PIBT (configuration generator):**
1. Apply forced constraints (constrained agents placed first).
2. Process remaining agents in random order via `funcPIBT()`.
3. `funcPIBT()` uses precomputed candidate ordering by BFS distance to goal; equal-distance groups are randomized once per planner instance.
4. Priority inheritance: if a candidate is occupied, the occupant is moved first (recursion).
5. Non-passing constraint ③ is checked during candidate evaluation.

### Async Fallback Race

- Primary LaCAM★ search starts immediately in both modes.
- In `INITIAL`, if `initial_async_fallback` is enabled and no solution has been found after
  `initial_async_fallback_delay_ms`, the planner launches prioritized fallback on a separate
  thread.
- If fallback wins in `INITIAL`, it becomes the incumbent, but primary continues for
  `initial_primary_grace_ms` before the planner returns.
- In `REPLAN`, if `replan_async_fallback` is enabled and no solution has been found after
  `replan_async_fallback_delay_ms`, the planner launches prioritized fallback and the first valid
  solution can win immediately.
- This split keeps `REPLAN` latency-oriented while making `INITIAL` less likely to give up too
  early on a potentially better primary solution.

### Virtual Lock Vertex

Narrow corridors (bridge chains) are collapsed before planning:

```
Before:  [Zone A] — a — b — c — [Zone B]   ← bridge chain (3 intermediate vertices)
After:   [Zone A] — [V_lock] — [Zone B]    ← single virtual vertex
```

1. Tarjan iterative DFS finds all bridge edges.
2. Vertices whose **all** edges are bridges = "corridor vertices".
3. Connected components of corridor vertices → each becomes one `V_lock`.
4. Starts/goals on corridor vertices are remapped to the corresponding `V_lock`.

This prevents PIBT livelock in narrow corridors and models them as capacity-1 resources (two robots cannot occupy the same vertex simultaneously).

### Anytime Behaviour

| Mode | Stop when |
|---|---|
| `INITIAL` | cost ≤ optimal × `quality_threshold` **or** `initial_timeout_ms` exceeded |
| `REPLAN` | first solution found **or** `replan_timeout_ms` exceeded |

### Planner And Runner Responsibilities

`Planner::solve()` owns planning policy:

1. Run primary LaCAM★ search.
2. Optionally start prioritized fallback according to the current `INITIAL` / `REPLAN` async policy.
3. Accept only physically valid planner-side solutions as incumbents.

`mapf_test` owns CLI-side safety:

1. Load the single-shot scenario and params, apply preprocessing, and build the planning instance.
2. Invoke `Planner::solve()`.
3. Re-validate the returned solution with the shared physical-validity checker.
4. Export JSON only if the final path passes validation.

This separation avoids duplicated fallback logic in the runner and prevents invalid or stale
solution files from being treated as successful outputs.

### Current Regression / Smoke-Test Results

The following configs are covered by the shipped regression flow and were independently checked for
physical validity:

| Config | Result | Makespan |
|---|---|---|
| `scenarios/single_shot/square.yaml` | valid | 2 |
| `scenarios/single_shot/corridor.yaml` | valid | 6 |
| `scenarios/single_shot/corridor_n4.yaml` | valid | 7 |
| `scenarios/single_shot/corridor_n6.yaml` | valid | 7 |
| `scenarios/single_shot/corridor_n8.yaml` | valid | 8 |
| `scenarios/single_shot/grid_4x4.yaml` | valid | 9 |
| `scenarios/single_shot/ring_24.yaml` | valid | 12 |
| `scenarios/single_shot/warehouse_5x4.yaml` | valid | 9 |

---

## For Developers

### Running Tests

```bash
cd mapf/build
ctest --output-on-failure
```

Current automated test split:

- `mapf_unit_tests`: low-level component tests for graph connectivity, distance table,
  collision detection, physical solution validation, params parsing, and stop-condition logic.
- `mapf_regression_tests`: planner integration tests over all shipped configs, with final
  physical-validity checks on the returned solutions, including the shipped lifelong scenarios.

You can also run the suites directly while debugging:

```bash
cd mapf/build
./mapf_unit_tests
./mapf_regression_tests
./mapf_lifelong_test ../config/scenarios/lifelong/square.yaml --log-level info
```

### Adding a New Map

1. Create `config/maps/mymap.yaml` following the [Map YAML](#map-yaml) format.
2. Create a matching single-shot scenario in `config/scenarios/single_shot/`.
3. Ensure the graph is biconnected — from `mapf/build`, run `./mapf_test ../config/scenarios/single_shot/mycase.yaml` to verify.
4. If not biconnected, either:
   - Redesign the graph, or
   - Set `virtual_lock: true` in `params/default.yaml` to collapse corridors first (result must still be biconnected).

### Linking `mapf_lib` in Another CMake Target

```cmake
add_subdirectory(path/to/mapf)
target_link_libraries(my_target PRIVATE mapf_lib)
```

`mapf_lib` exports `include/` as a public include directory.

### Key Extension Points

| File | How to extend |
|---|---|
| `collision.hpp/.cpp` | Add new collision constraint type — implement check in `CollisionChecker`, enforce in `pibt.cpp:funcPIBT()` |
| `stop_condition.hpp/.cpp` | Add new `PlanMode` or change termination logic |
| `planner.cpp:solve()` | Change search strategy (DFS stack → priority queue for A*) |
| `graph_preprocessor.cpp` | Extend preprocessing (e.g. dead-end removal, zone partitioning) |
| `config_loader.cpp` | Add new YAML fields (e.g. vertex capacity, edge weights) |

### Graph Requirements

The planner **requires** a biconnected undirected graph:

- Biconnected = no articulation points = no vertex whose removal disconnects the graph.
- PIBT's reachability guarantee holds only on biconnected graphs.
- `Graph::is_biconnected()` uses Tarjan's DFS and is called at startup.
- Vertex ids must be **contiguous from 0**: `V[i]->id == i` is always assumed.

### SADG Interface

The `Solution` type is directly compatible with SADG Algorithm 1 input:

```cpp
using Solution = std::vector<AgentPlan>;
using AgentPlan = std::vector<std::pair<int, int>>;  // (vertex_id, timestep)
```

`sol[i]` is the complete path for agent `i`:
`sol[i][t] = {vertex_id, t}` means agent `i` occupies `vertex_id` at timestep `t`.

---

## References

| Paper | Relevance |
|---|---|
| Okumura et al. (2022). *PIBT: Priority Inheritance with Backtracking*. AIJ. | PIBT algorithm |
| Okumura (2023). *LaCAM: Search-Based Algorithm for Quick MAPF*. AAAI. | LaCAM base |
| Okumura (2023). *Improving LaCAM for Scalable Eventually Optimal MAPF*. IJCAI. | LaCAM★ (anytime) |
| Okumura (2024). *Engineering LaCAM★*. AAMAS. | LaCAM★ engineering |
| Berndt et al. (2024). *Receding Horizon Re-Ordering of Multi-Agent Execution Schedules*. TRO. | SE-ADG / SADG layer |
