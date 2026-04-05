# mapf - Research And Design Notes

This document captures the target MAPF redesign for `multibot3`.
It is intended for researchers, reviewers, and future implementation work.

For practical execution instructions, see [`README.md`](README.md).
Primary design reference:
- [`../concept/MAPF_LAYER.md`](../concept/MAPF_LAYER.md)

Scenario YAML schema:
- [`schema/lifelong-scenario.schema.json`](schema/lifelong-scenario.schema.json)

## Overview

The MAPF layer is responsible for generating collision-free robot paths for the
multibot3 pipeline:

```text
MAPF -> SADG -> Execution
```

The redesign keeps only the lifelong planning flow.
Single-shot planning is not a public target mode anymore.

The planner is built around these principles:
- lifelong planning only
- general graph `G = (V, E)`
- LaCAM-based planner core
- no prioritized fallback
- strict validity-only output
- biconnected validation at the library boundary
- affected-robot replanning with frozen reservations from unaffected robots

## Status

This document should be read as the intended architecture.
Current source files may still contain implementation gaps or legacy remnants.

## Scope

The MAPF library is responsible for:
- preparing and validating the planning graph
- planning initial lifelong assignments
- replanning when goals or execution state change
- enforcing MAPF validity constraints
- returning only valid paths or explicit failure statuses

The MAPF library is not responsible for:
- deciding the affected robot set
- deciding whether to escalate from affected replan to global replan
- deciding when the system should enter safe-stop
- issuing robot control commands

## Core Decisions

### Lifelong-Only Planning

The public interface keeps only lifelong planning and replanning.
Legacy single-shot scenario handling is removed from the target design.

### No Prioritized Fallback

The redesign removes prioritized fallback completely.
The planner core must return:
- a valid solution
- a timeout status with the best valid solution found so far
- or an explicit failure status

The library must never return an invalid emergency path.

### Library-Boundary Graph Validation

Graph preprocessing and biconnected validation are mandatory at the library
boundary.

`prepare_map()` must:
1. load the raw graph
2. apply virtual-lock preprocessing when enabled
3. validate that the resulting planning graph is biconnected
4. fail immediately if validation fails

This keeps invalid maps from leaking into planner internals.

### Affected-Robot Replanning

Replanning is based on the affected robot set.

The target flow is:
1. SADG detects that the current execution plan is invalid or outdated.
2. SADG computes the affected robot set.
3. MAPF receives the current fleet snapshot plus frozen future paths for
   unaffected robots.
4. MAPF replans only the affected robots while treating unaffected robots as
   reserved moving obstacles.

## Planning Model

### Graph

The environment is modeled as a general undirected graph:

```text
G = (V, E)
```

- each vertex is a robot-occupiable location
- each edge is a traversable connection
- vertex ids must be contiguous integers starting at `0`

### Virtual Lock Vertex

Narrow single-capacity corridors may be collapsed into a virtual lock vertex.

This preprocessing has two goals:
- restore biconnected structure where possible
- model narrow corridors as single-capacity resources

After preprocessing, the planning graph is the only graph used by the planner.

### Fleet Snapshot

The planner does not assume a fresh start from time zero in the real system.
Instead it receives a snapshot of the current execution state.

A fleet snapshot contains:
- current planning time
- current planning-graph vertex of each robot
- current goal of each robot, if any
- hold/frozen state if applicable

## Validity Constraints

The MAPF layer must reject any solution that violates these constraints.

### 1. Vertex Collision

Two robots must not occupy the same vertex at the same timestep.

### 2. Swap Collision

Two robots must not exchange vertices in the same timestep.

Example:

```text
robot A: u(t) -> v(t+1)
robot B: v(t) -> u(t+1)
```

### 3. Non-Passing Collision

If one robot traverses edge `(u -> v)` at timestep `t`, another robot must not
traverse `(v -> u)` at timestep `t + 1`.

Example:

```text
robot A: u(t)   -> v(t+1)
robot B: v(t+1) -> u(t+2)
```

This is a delayed reverse-edge collision rule.

### 4. Physical Edge Validity

Every step in every returned path must be either:
- wait at the same vertex
- or a move along a real edge in the planning graph

## Output Format

The returned plan for each robot is a timestep-indexed vertex sequence:

```text
[(vertex_id, timestep), ...]
```

This format is directly suitable for handoff to the SADG layer.

## Objective And Metrics

The redesign does not hard-code a single objective function.
The request selects the objective, while the result always reports both major
metrics.

```cpp
enum class ObjectiveType {
    SOC,
    MAKESPAN
};

struct PlanMetrics {
    int soc;
    int makespan;
};
```

Recommended default:
- use `SOC` as the default optimization objective in v1
- always report both `SOC` and `makespan`

## Timeout Semantics

### Initial Planning

If timeout occurs:
- return the best valid solution found so far, if one exists
- otherwise return `TIMEOUT_NO_SOLUTION`

### Replanning

If timeout occurs during affected-robot replanning:
- return the best valid replanning result, if one exists
- otherwise return `TIMEOUT_NO_SOLUTION`

The planner itself does not choose the next system action after replan failure.
That decision belongs to the runtime policy layer.

## Runtime Policy Split

The redesign separates responsibilities into three layers.

### Planner Core

Responsible for:
- graph search
- collision avoidance
- validity guarantees
- returning solved or failed planning results

Not responsible for:
- policy escalation
- execution control

### Scenario Runtime

Responsible for:
- calling affected-robot replanning first
- escalating to global replanning if policy requires it
- surfacing `SAFE_STOP_REQUIRED` if no valid plan can be produced

### SADG

Responsible for:
- detecting invalid or stale execution plans
- choosing the affected robot set
- requesting replanning from the runtime layer

## Frozen Reservations

Affected-robot replanning depends on reservations from robots that are not being
replanned.

Two reservation types are required.

### Frozen Plan Reservation

For unaffected robots that keep following their current valid future plan:
- reserve every future vertex occupancy
- reserve every future directed edge usage
- after the path ends, reserve the last vertex indefinitely

### Hold Reservation

For robots currently held in place:
- reserve the current vertex from the current time onward
- when needed, also reserve a one-step reverse-edge cooldown after the robot's
  last move to preserve the non-passing rule

This is intentionally conservative and safety-first.

## Public API Sketch

The public class-facing API is centered on `MapfPlanner`.
Low-level `PlanningRequest` is treated as an internal type.

```cpp
PreparedMap prepared = PreparedMap::from_graph(raw_graph, preprocessing);
MapfPlanner planner(std::move(prepared), params);

PlanResult initial = planner.plan_initial(InitialPlanningRequest{
    .snapshot = snapshot,
    .assignments = assignments,
    .params_override = std::nullopt,
});

PlanResult replanned = planner.replan_affected(AffectedReplanningRequest{
    .snapshot = snapshot,
    .assignments = updated_assignments,
    .affected_robot_indices = affected_set,
    .frozen = frozen_paths,
    .params_override = std::nullopt,
});
```

```cpp
enum class PlanningStatus {
    SOLVED,
    TIMEOUT_WITH_SOLUTION,
    TIMEOUT_NO_SOLUTION,
    UNSAT,
    INVALID_MAP,
    INVALID_REQUEST,
    SAFE_STOP_REQUIRED
};

struct PlanResult {
    PlanningStatus status;
    Solution solution;
    PlanMetrics metrics;
    std::string reason;
};
```

## Failure Semantics

The redesign uses explicit failure states.

- `INVALID_MAP`
  - preprocessing failed
  - vertex ids are malformed
  - planning graph is not biconnected

- `INVALID_REQUEST`
  - duplicate starts
  - duplicate goals where not allowed
  - unknown robot id
  - malformed replanning input

- `TIMEOUT_NO_SOLUTION`
  - timeout occurred before any valid solution was found

- `TIMEOUT_WITH_SOLUTION`
  - timeout occurred after at least one valid solution was found

- `UNSAT`
  - the request is infeasible under the current constraints

- `SAFE_STOP_REQUIRED`
  - runtime-level escalation failed and the system must stay halted until a new
    valid plan is available

## Recommended Replanning Policy

When SADG reports that the current execution plan has become invalid, the
recommended runtime policy is:

```text
safe hold
-> affected replan
-> if failed, global replan
-> if failed, safe stop
```

This keeps the planner clean while still giving the full system a clear failure
recovery path.

In the current codebase this policy lives in `ScenarioRuntime`, while
`MapfPlanner` remains the public planning API.

## Migration Notes

The rewrite should remove or migrate these legacy pieces:
- single-shot planners and configs
- prioritized fallback logic
- runner-only map validity checks
- public APIs that assume full-fleet planning on every request

The long-term goal is a small, explicit MAPF library centered on:
- prepared maps
- lifelong snapshots
- affected replanning
- strict valid-path output
