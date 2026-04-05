# multibot3

## [multibot3 wiki](https://github.com/hchju777/multibot3/wiki)
### [Requirements](https://github.com/hchju777/multibot3/wiki/Requirements)
### [Architecture](https://github.com/hchju777/multibot3/wiki/Architecture)
## [Kanban Board(Collaborator Only)](https://github.com/users/hchju777/projects/3)

## MAPF Quick Start

The current MAPF flow is lifelong-only.
Execution guide:
- [mapf/README.md](mapf/README.md)

Research and design notes:
- [mapf/RESEARCH.md](mapf/RESEARCH.md)

Build from the repository root:

```bash
cmake -S mapf -B mapf/build -DBUILD_TESTING=ON
cmake --build mapf/build -j1
```

Run a lifelong scenario:

```bash
./mapf/build/mapf_lifelong_test mapf/config/scenarios/lifelong/corridor_n6.yaml
```

Visualize it with Python:

```bash
python3 mapf/tools/visualize.py \
  mapf/config/scenarios/lifelong/corridor_n6.yaml \
  --binary mapf/build/mapf_lifelong_test
```

If the scenario path is omitted, both the runner and the visualizer read
`MAPF_LIFELONG_SCENARIO_PATH` from the environment or the repo `.env`.
