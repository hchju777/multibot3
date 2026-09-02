// SPDX-License-Identifier: Apache-2.0
//
// trajopt_node — onboard node/ layer (CN-1/CN-23 Presentation).
//
// 🔴 48차 웨이브 1-B(`_workspace/368_trajopt_tick_wiring.md`) — closes the
// control-tick loop that 47차(P2) left unwired. What THIS round wires (the
// brief's exact 4-item checklist):
//   - the 50 ms control timer (node-clock based, NOT create_wall_timer, so
//     use_sim_time actually governs it — 10_architecture.md §9).
//   - ControlTickService::run_tick, one instance per robot (service/, U48-3).
//   - /{robot}/cmd_vel (geometry_msgs/Twist) publish + /trajectories publish.
//   - /{robot}/odom (nav_msgs/Odometry) subscribe — the ONLY source of
//     TickInput.pose/v this round. No world-observation input is fabricated
//     (OBS-* is the next round's concern, out of scope here).
//
// 🔴 Decisions made to close the loop without inventing values (reported in
// 368_trajopt_tick_wiring.md, not hidden):
//   D1. 🔴 SUPERSEDED by D1' (see 368_p2 delta note below) — the original
//       368 brief explicitly put execution_constraints/discrete_plan/
//       segment_release out of scope, so this round's first pass used
//       goal == first-observed pose as an honest placeholder. That
//       placeholder made start==goal permanently (SubgoalFreeSpaceDisk::
//       generate() always false), which the coordinator's 368_p2 follow-up
//       correctly flagged as defeating the round's own purpose. D1' below
//       replaces this.
//   D2. Search cell = SearchFixedPathGating (no constructor arguments, no
//       numeric parameter needed at all) — chosen over SearchHybridAstar
//       specifically because it needs zero config values, minimizing the
//       "값을 지어내지 마라" surface for a single-robot-corridor wiring round.
//   D3. Subgoal candidate source = SubgoalFreeSpaceDisk(stride_m,
//       window_radius_m). These two numeric parameters have no algorithmic
//       default in this codebase (both plugin ctors require them) so they
//       are read from two NEW config keys with a NaN sentinel (no baked
//       default) and validated once real /robot_specs data arrives (same
//       failure point as the existing SU01-05 checks, not an earlier one) —
//       absent/<=0 throws ContractViolation (CN-15), exactly like
//       core/trajopt.yaml already documents for the pre-existing ten keys.
//   D4. Peer channel = PeerChannelIdeal, ONE shared instance across all
//       robots (it is a cross-robot "board", not a per-robot object).
//   D5. Safety monitor = ONE shared instance with an "always stoppable"
//       predicate — no keepout data is wired this round (needs
//       execution_constraints, out of scope), so this is the honest null
//       default, matching the exact predicate already used by this
//       package's own unit tests (test_control_tick_service.cpp).
//   D6. Instrumentation sink = nullptr (ServiceWiring::instr "may be null",
//       header's own words) — CN-19's 3-piece sink is not implemented this
//       round.
//   D7. control_period_ms is declared with a baked default of 50 — this is
//       NOT an unset algorithm parameter; it is the hard, fleet-wide
//       invariant the brief itself states ("control_period_ms: 50은
//       하드다(불변식 5)"), and it mirrors mrs_sim/clock_node.cpp's own
//       clock_step_ms=50 precedent for the identical invariant. It is
//       declared OUTSIDE method.modules.trajopt.* because it is not a
//       swappable algorithm parameter (CN-6/CN-7 govern algorithm choices,
//       not this system-wide constant).
//   D8. 🔴 SUPERSEDED by 376_p2 (`_workspace/376_observation_consumer_p2.md`,
//       48차 웨이브4-B 재호출) — `/stop_declaration` publication was
//       explicitly OUT of the 368 checklist and stayed that way through
//       376(p1); `TickOutput.has_stop` was surfaced as a WARN log only. That
//       is CLOSED now: see the 376_p2 delta comment block below.
//   D9. SUPERSEDED by 376(p1, `_workspace/376_observation_consumer.md`, 48차
//       웨이브4-B) — the two (void)-discarded DeclarationRegulator::
//       reverse_forbidden_infeasible() call sites now deposit evidence
//       (`pending_q2_evidence_`) and CT24-CT25 (`ControlTickService::
//       run_tick`) now calls `DeclarationRegulator::classify()` for real on
//       the NORMAL path — see `control_tick_service.hpp`/`.cpp` file docs.
//
// Still NOT wired (unchanged from 47차, out of THIS round's scope):
// /discrete_plan, /segment_release subscriptions (no staged constraints
// application, no puncture detection, no release-bit gating — TickInput's
// staged_constraints_fresh/tube_pierced stay at their struct defaults,
// false).
//
// 🔴 368_p2 delta (`_workspace/368_trajopt_tick_wiring_p2.md`) — D1 SUPERSEDED.
// The coordinator's follow-up correctly found that D1's placeholder
// (goal == first-observed pose) makes start==goal permanently, so
// SubgoalFreeSpaceDisk::generate() always returns false and the robot never
// commands nonzero velocity — the round's stated purpose ("로봇이 움직이는
// 것을 본다") was not reachable through wiring alone. This round closes that
// gap:
//   D1' (replaces D1). The segment goal is now sourced from
//       /execution_constraints (3.0.0, `segments[].robot/.from_id/.to_id`)
//       + /roadmap (`nodes[].id/x/y`, newly subscribed here — 🔴 the P2
//       brief's claim that trajopt_node "이미 구독 중"이다 is WRONG, this
//       node had zero /roadmap subscription before this delta; flagged, not
//       silently fixed-and-hidden). For each robot, the FIRST segment (the
//       Segment whose `id` numeric suffix `{robot}#{index}` is minimal among
//       that robot's segments in the latest /execution_constraints message)
//       supplies `to_id`; that id is looked up in the latched /roadmap
//       node table for (x,y). ControlTickService is now constructed lazily
//       once BOTH a real odom pose AND a resolved segment goal are
//       available (TryStartService) — not on first-odom alone.
//   D10. /segment_release is NOT subscribed this round (판정, 신고 — see
//       368_trajopt_tick_wiring_p2.md §2 for the full reasoning). Summary:
//       (a) TickInput/run_tick has no "may I proceed" field to carry a
//       release bit into — CT09's keepout_.rebuild_or_refresh(exec_view_,
//       release_) is unimplemented service-layer machinery, not node
//       wiring; (b) the release bit's contractual job (segment_release.
//       schema.json:"released") is gating ENTRY into a `to`-vertex zone —
//       i.e. it is a keepout-construction input (V^blk), a different job
//       from "which vertex is my goal" that ① asks for. Consuming it here
//       would require inventing a gating mechanism this round's checklist
//       does not list. Narrowed per the brief's own instruction ("애매하면
//       좁게 하고 신고하라").
//   Missing-node handling (질문 in the P2 brief): an execution_constraints
//   segment whose `to_id` is not found in the roadmap node table does NOT
//   throw ContractViolation. Rationale: CN-15 enumerates startup-path
//   refusals (plugin load failure, missing required config key, startup
//   check failure) — /execution_constraints is a live, re-publishable
//   artifact (commit_seq-versioned, can arrive again after a replan), not a
//   one-shot startup gate like /robot_specs. An unresolved node is logged
//   (RCLCPP_ERROR) and that robot's goal is simply NOT updated this message
//   (old resolved goal, if any, is kept; if none yet, the robot stays
//   un-started) — the safe-degrade effect the brief calls "그 틱을 안전
//   정지로": no fabricated coordinate is ever used, and OnControlTick keeps
//   commanding whatever it already knew (zero, if nothing yet).
//   Multi-segment progression (advancing the goal as segments complete) is
//   NOT implemented — known limitation, reported in 368_p2 §"알려진 한계".
//
// 🔴 368_p3 delta (`_workspace/368_trajopt_tick_wiring_p3.md`) — CT15/CT20
// follower fix. `_p2`§5 found (by actually running the code) that even with
// a real goal, `/cmd_vel` stayed at v=0 forever: `control_tick_service.cpp`'s
// `sample_committed` (CT15) unconditionally returned `chain.front()` — the
// committed chain's start anchor, always v=0 — with no notion of elapsed
// time advancing through the chain. This delta:
//   - Adds `RclcppSteadyClock` (below) — a `mrs_core::ISteadyClock` (CN-18,
//     the one clock seam) backed by `this->get_clock()`, so it respects
//     `use_sim_time`. The service NEVER builds its own clock (CN-4/CN-18);
//     this node wires the concrete instance into every robot's
//     `ServiceWiring::clock` in `TryStartService`.
//   - `TrajectoryBuffer::sample_at()` (core/trajectory_buffer.hpp) now
//     advances a zero-order hold through the committed chain by elapsed time
//     since `adopt()` — the CT15 (`322_pseudocode_r43.md:93`
//     `follower_.sample(traj_buf_, tick_seq, now)`) realization. See that
//     header's file doc for why `tick_seq` is not consumed (pseudocode
//     doesn't specify what it would add beyond `now`).
//   - CT20 (`out.cmd = s_now`) is UNCHANGED — the pseudocode's
//     `follower_.track(s_now, limits_)` is realized as a pure feedforward
//     (no separate control law invented): `s_now` already carries v/omega
//     that the velocity profiler capped to `limits_` when the chain was
//     built, so a second clamp would be a no-op in the normal case and
//     pseudocode does not specify anything else `track` would do here.
//   - Zero new config keys — the zero-order-hold reading needs no numeric
//     constant (no gain, no lookahead distance); "값을 지어내지 마라" was
//     satisfied by choosing the narrowest form, not by inventing a sentinel.
//
// 🔴 376 delta (`_workspace/376_observation_consumer.md`, 48차 웨이브4-B) —
// the world-observation CONSUMER side (N1-N4, the publisher side is a
// DIFFERENT round in mrs_sim/mrs_bringup — not touched here):
//   - N1: new subscription `/{robot}/scan` (`sensor_msgs/msg/LaserScan`,
//     BEST_EFFORT/VOLATILE/KeepLast(1) — `357` D23) per robot, registered
//     alongside the existing `/{robot}/odom` subscription in OnRobotSpecs.
//   - `ScanToObservation()` (below, node-layer, `CN-2`: `core/` never
//     includes `sensor_msgs`) converts a scan into a fixed-size
//     `core::WorldObservation` (`OBS-5`) — SIMPLIFIED geometry, see that
//     struct's file doc: a forward-fan occlusion proxy, not exact roadmap
//     edge cross-section (`width_m`) alignment.
//   - Age bookkeeping: a node-level ordinal `global_tick_seq_` (incremented
//     once per OnControlTick, shared across robots — the control tick is a
//     fleet-wide ordinal) plus a per-robot capture-tick stamp gives
//     `WorldObservation::age_ticks` in CONTROL TICKS, never seconds
//     (`221-W2`, `OBS-5` ⑵). The O(1) swap itself (`OBS-5` ⑴) is the
//     scan-callback's plain field overwrite; no parsing happens on the
//     control-tick path (`CN-19`).
//   - Five NEW config keys under `method.modules.trajopt.*` — all
//     [값 부재], no baked default, validated at the SAME failure point as
//     SU01-05 (`StartupChecks::run`, `core/startup_checks.cpp`):
//     `obs_occlusion_range_m` [m], `obs_n_open`/`obs_n_close`/`obs_n_hold`
//     [ticks], `obs_max_age_ticks` [ticks]. Reported to `interface-designer`/
//     the orchestrator for `mrs_bringup/config/trajopt.yaml` — this package
//     does NOT own that file (`CN-6`).
//   - N2/N3/N4 (the evidence -> classify() -> has_stop wiring) live entirely
//     in `service/control_tick_service.{hpp,cpp}` — this node only supplies
//     `TickInput::obs` each tick (see OnControlTick below). `/stop_declaration`
//     boundary publication was OUT of THIS delta's scope (D8) — CLOSED by
//     376_p2 below.
//   - OBS-1 ⑴ (non-attribution) is trivially `true` in `ScanToObservation` —
//     [알려진 한계], see `core/world_observation.hpp` file doc: this build
//     has no onboard channel that consumes neighbor corridors or `V^blk`
//     keepout, so there is nothing to attribute against yet.
//   - `package.xml`/`CMakeLists.txt` gain a `sensor_msgs` dependency — this
//     IS the observation round (`357`§12 flagged `sensor_msgs` addition as
//     belonging to "관측 라운드", not P3/P4; 376 is that round).
//
// 🔴 376_p2 delta (`_workspace/376_observation_consumer_p2.md`, 48차 웨이브4-B
// 재호출) — CLOSES D8: `TickOutput.has_stop`/`stop_reason` now reach the
// boundary `/stop_declaration` topic (`mrs.stop_declaration` 5.0.1):
//   - New publisher `/stop_declaration` (`mrs_msgs::msg::StopDeclarations`,
//     RELIABLE/VOLATILE/KEEP_LAST(32) — `357`§3-3 / `10_architecture.md`§7).
//   - **Event-driven, not per-tick** (`357` D30 "사건 구동이라 매 틱 직렬화가
//     아니다"): a message is published ONLY on a `has_stop` TRANSITION
//     (false->true opens; true->false closes), tracked per robot in
//     `RobotEntry::last_declared_open`. Publishing every tick while blocked
//     would flood the topic and inflate `seq` for no new information.
//   - `blocked_edge` VALUE is `RobotEntry::current_segment_from`/`_to`
//     (cached in `ResolveGoalsFromExecConstraints`, THE SAME point `goal` is
//     resolved from `/execution_constraints`) — NOT new information (schema
//     :35). Frozen at open time; the close event reuses it VERBATIM (schema
//     :35 "declared:false는 개시 때와 글자 그대로 같은 쌍"), stored in
//     `RobotEntry::declared_from`/`_to`.
//   - `seq`: `RobotEntry::stop_seq`, per-robot, 0-based, incremented on EVERY
//     publish (open AND close) — a channel-local space, separate from
//     `mrs.entry_events.seq` (schema :27).
//   - `reason`: the ROS wire type is `uint8` against the SHARED
//     `mrs_msgs::msg::StopReason` vocabulary (legal subset {1,3,5}) — NOT the
//     JSON schema's string enum. `StopReasonToRosConstant()` (below) does
//     that mapping; it is glue (struct-field assignment), not JSON
//     serialization, so it does not touch `io/`. The close event reuses the
//     SAME reason the open event carried (`RobotEntry::declared_reason`) —
//     the most defensible non-fabricated choice given the frozen text is
//     silent on the close event's `reason` value.
//   - 🔴 OBS-8 (reopening with a DIFFERENT reason while still declared open)
//     is NOT implemented (357§5 Q3: needs a stateful multi-reason ledger + a
//     "close old reason / open new reason" two-declaration protocol this
//     round does not build). Detected and reported (WARN, rate-limited to
//     once per open window via `RobotEntry::obs8_conflict_warned`) — NOT
//     silently dropped, NOT force-fit into a fabricated two-declaration
//     sequence either.
//   - CN-19: the ROS-message STRUCT FILL (`PublishStopDeclaration` below) is
//     plain field copies/assignments — no string concatenation, no JSON
//     text assembly (that is `io/json_io`'s job and is NOT called from this
//     path). `rclcpp::Publisher::publish()`'s own DDS serialization is the
//     same middleware step every other publisher in this file already uses
//     (`/cmd_vel`, `/trajectories`) and is outside this package's control.
//     The RCLCPP_WARN/RCLCPP_ERROR calls added here mirror the PRE-EXISTING
//     style in this same callback (the D8 WARN they replace, OnOdom's
//     ERROR) — judgment call, not a new pattern.
//   - `check_layer_layout.py`'s `sensor_msgs` gap (357§3-4 finding) is
//     UNCHANGED by this delta — no new `core/`/`service/` include of any ROS
//     IDL was added (`stop_declaration_adapter.hpp` was already ROS-free).

#include <chrono>
#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "geometry_msgs/msg/twist.hpp"
#include "mrs_core/i_steady_clock.hpp"
#include "mrs_msgs/msg/execution_constraints.hpp"
#include "mrs_msgs/msg/limits.hpp"
#include "mrs_msgs/msg/roadmap.hpp"
#include "mrs_msgs/msg/robot_specs.hpp"
#include "mrs_msgs/msg/robot_trajectory.hpp"
#include "mrs_msgs/msg/segment.hpp"
#include "mrs_msgs/msg/stop_declaration.hpp"
#include "mrs_msgs/msg/stop_declarations.hpp"
#include "mrs_msgs/msg/stop_reason.hpp"
#include "mrs_msgs/msg/trajectories.hpp"
#include "mrs_msgs/msg/trajectory_point.hpp"
#include "mrs_trajopt/adapter/robot_specs_adapter.hpp"
#include "mrs_trajopt/adapter/stop_declaration_adapter.hpp"
#include "mrs_trajopt/adapter/trajectories_adapter.hpp"
#include "mrs_trajopt/core/declaration_regulator.hpp"
#include "mrs_trajopt/core/safety_monitor.hpp"
#include "mrs_trajopt/core/startup_checks.hpp"
#include "mrs_trajopt/core/status.hpp"
#include "mrs_trajopt/core/types.hpp"
#include "mrs_trajopt/core/world_observation.hpp"
#include "mrs_trajopt/plugins/peer_channel_impls.hpp"
#include "mrs_trajopt/plugins/search_fixed_path_gating.hpp"
#include "mrs_trajopt/plugins/subgoal_candidates.hpp"
#include "mrs_trajopt/service/control_tick_service.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

namespace mrs_trajopt::node
{

/// @brief 368_p3 — the CN-18 clock seam's concrete, node-layer realization.
///
/// `mrs_core::ISteadyClock` is pure (no ROS); this adapter is the one place
/// (composition root) that backs it with `rclcpp::Clock`, so it follows
/// `use_sim_time` like every other time source in this pipeline
/// (`10_architecture.md` §9). `core/`/`service/` never construct this
/// themselves (CN-4/CN-18) — the node builds one and injects a pointer to it
/// via `ServiceWiring::clock`.
class RclcppSteadyClock : public mrs_core::ISteadyClock
{
public:
    /// @brief Construct with the ROS clock to read through.
    /// @param clock the node's clock (`this->get_clock()`).
    explicit RclcppSteadyClock(rclcpp::Clock::SharedPtr clock) : clock_(std::move(clock))
    {
    }

    /// @brief Current time in seconds (`rclcpp::Clock::now()`, sim-time-aware).
    /// @return monotonically non-decreasing seconds (ISteadyClock contract).
    double NowSeconds() const override
    {
        return clock_->now().seconds();
    }

private:
    rclcpp::Clock::SharedPtr clock_;
};

/// @brief 376 (N1) — `sensor_msgs::msg::LaserScan` -> `core::WorldObservation`
/// conversion. `core/` never includes `sensor_msgs` (`CN-2`); this is the
/// ONE place that reads it, matching `357`§3-4's disposition (a private
/// `node/` function, NOT `adapter/` — this is a tool-layer channel, not a
/// boundary artifact, `CN-8` does not apply).
///
/// SIMPLIFIED (reported, not hidden — see `core/world_observation.hpp` file
/// doc and `mrs_trajopt` 20d "알려진 한계"): marks sample `i` occupied iff its
/// range is finite, positive, and `<= occlusion_range_m`. 🔴 376_p2: per-
/// sample occupancy is aggregated with `HasCollapsedRay` ("exists a
/// collapsed ray"), NOT "every sample" — see `world_observation.hpp` file
/// doc for the runtime-found reason (only edge-aligned rays ever collapse;
/// "wall" rays in the same fan stay finite regardless of block status).
/// `unattributed` is left at the struct default `true` (no onboard channel
/// consumes neighbor corridors / `V^blk` keepout in this build — `355`§8 X3).
/// @param scan the raw scan.
/// @param occlusion_range_m the occlusion-range threshold [m] (config,
///        `[값 부재]`, validated `> 0` by `StartupChecks::run`).
/// @return a fixed-size `WorldObservation` with `fresh=true`, `age_ticks=0`
///         (the caller stamps age from its own capture-tick bookkeeping).
mrs_trajopt::core::WorldObservation ScanToObservation(const sensor_msgs::msg::LaserScan& scan,
                                                      double occlusion_range_m)
{
    mrs_trajopt::core::WorldObservation obs;
    obs.fresh = true;
    obs.age_ticks = 0;  // capture-time value; the reader (OnControlTick) adds
                        // elapsed control ticks via the per-robot capture stamp.
    const std::size_t n =
        std::min(scan.ranges.size(), mrs_trajopt::core::WorldObservation::kMaxSamples);
    obs.sample_count = n;
    for (std::size_t i = 0; i < n; ++i)
    {
        const float r = scan.ranges[i];
        obs.occupied[i] =
            std::isfinite(r) && r > 0.0f && r <= static_cast<float>(occlusion_range_m);
    }
    // obs.unattributed stays at its struct default (true) — see file doc.
    return obs;
}

/// @brief 376_p2 — `core::StopReason` -> the ROS wire's `uint8` constant
/// (`mrs_msgs::msg::StopReason`, the SHARED numeric vocabulary — legal
/// subset `{1,3,5}` for this channel, `StopDeclaration.msg` file doc). This
/// is a DIFFERENT encoding from `DeclarationRegulator::to_schema_string()`
/// (the JSON schema's string enum, used by `io/json_io`) — the ROS `.msg`
/// never carries the string form. Node-layer glue (`CN-9` does not apply:
/// this is not an `adapter/` boundary-struct conversion, it is the
/// boundary-struct -> ROS-wire-type fill that `PublishTrajectories` already
/// does for `mrs.trajectories`).
/// @param r the classified reason.
/// @return the matching `mrs_msgs::msg::StopReason::REASON_*` constant.
std::uint8_t StopReasonToRosConstant(mrs_trajopt::core::StopReason r)
{
    switch (r)
    {
        case mrs_trajopt::core::StopReason::kExogenousBlock:
            return mrs_msgs::msg::StopReason::REASON_EXOGENOUS_BLOCK;
        case mrs_trajopt::core::StopReason::kInfeasibleSubgoal:
            return mrs_msgs::msg::StopReason::REASON_INFEASIBLE_SUBGOAL;
        case mrs_trajopt::core::StopReason::kUnresolvableLocally:
            return mrs_msgs::msg::StopReason::REASON_UNRESOLVABLE_LOCALLY;
    }
    return mrs_msgs::msg::StopReason::REASON_UNRESOLVABLE_LOCALLY;
}

class TrajoptNode : public rclcpp::Node
{
public:
    TrajoptNode() : rclcpp::Node("trajopt_node")
    {
        // Fleet-folded limits + config (method.modules.trajopt.*, CN-6/CN-7 —
        // belongs in mrs_bringup/config, not this package's config/; declared
        // here as a stand-in, P3 out of scope this round). Unchanged from
        // 47차 — see multibot3/src/mrs_bringup/config/trajopt.yaml for why
        // these are all [값 부재] today (SU01-05 will refuse once
        // /robot_specs arrives; that halt point predates this round).
        fleet_.a_max = this->declare_parameter<double>("method.modules.trajopt.a_max", 0.0);
        fleet_.j_max = this->declare_parameter<double>("method.modules.trajopt.j_max", 0.0);
        fleet_.omega_max_radps =
            this->declare_parameter<double>("method.modules.trajopt.omega_max_radps", 0.0);
        fleet_.yaw_accel_max_radps2 =
            this->declare_parameter<double>("method.modules.trajopt.yaw_accel_max_radps2", 0.0);
        fleet_.min_separation_m =
            this->declare_parameter<double>("method.modules.trajopt.min_separation_m", 0.0);

        cfg_.traj_replan_period_ticks =
            this->declare_parameter<int>("method.modules.trajopt.traj_replan_period_ticks", 0);
        cfg_.subgoal_replan_period_trajcycles =
            this->declare_parameter<int>("method.modules.trajopt.subgoal_replan_period_trajcycles",
                                         0);
        cfg_.round_cap_budget =
            this->declare_parameter<int>("method.modules.trajopt.round_cap_budget", 0);
        cfg_.backtrack_budget =
            this->declare_parameter<int>("method.modules.trajopt.backtrack_budget", 0);
        cfg_.peer_board_rounds_max =
            this->declare_parameter<int>("method.modules.trajopt.peer_board_rounds_max", 0);

        // 🆕 D3 — subgoal candidate-source numeric parameters. NaN sentinel =
        // [값 부재]; no baked default (this round's rule) — validated once
        // real /robot_specs data arrives (OnRobotSpecs), the SAME failure
        // point as SU01-05, not an earlier one.
        subgoal_stride_m_ =
            this->declare_parameter<double>("method.modules.trajopt.subgoal_stride_m",
                                            std::numeric_limits<double>::quiet_NaN());
        subgoal_window_radius_m_ =
            this->declare_parameter<double>("method.modules.trajopt.subgoal_window_radius_m",
                                            std::numeric_limits<double>::quiet_NaN());

        // 🆕 376 — exogenous-block observation config (OBS-1/OBS-5/OBS-7).
        // Baked-0/0.0 sentinel (same pattern as round_cap_budget etc.) — ALL
        // [값 부재], refused by StartupChecks::run at the SAME failure point
        // as SU01-05 (no earlier gate invented for these).
        cfg_.obs_n_open = this->declare_parameter<int>("method.modules.trajopt.obs_n_open", 0);
        cfg_.obs_n_close = this->declare_parameter<int>("method.modules.trajopt.obs_n_close", 0);
        cfg_.obs_n_hold = this->declare_parameter<int>("method.modules.trajopt.obs_n_hold", 0);
        cfg_.obs_max_age_ticks =
            this->declare_parameter<int>("method.modules.trajopt.obs_max_age_ticks", 0);
        cfg_.obs_occlusion_range_m =
            this->declare_parameter<double>("method.modules.trajopt.obs_occlusion_range_m", 0.0);

        // SU06 scans this list for a forbidden independent eta_slf/self_trigger*
        // key — every method.modules.trajopt.* key this node declared, so far.
        cfg_.config_key_names = {"method.modules.trajopt.a_max",
                                 "method.modules.trajopt.j_max",
                                 "method.modules.trajopt.omega_max_radps",
                                 "method.modules.trajopt.yaw_accel_max_radps2",
                                 "method.modules.trajopt.min_separation_m",
                                 "method.modules.trajopt.traj_replan_period_ticks",
                                 "method.modules.trajopt.subgoal_replan_period_trajcycles",
                                 "method.modules.trajopt.round_cap_budget",
                                 "method.modules.trajopt.backtrack_budget",
                                 "method.modules.trajopt.peer_board_rounds_max",
                                 "method.modules.trajopt.subgoal_stride_m",
                                 "method.modules.trajopt.subgoal_window_radius_m",
                                 "method.modules.trajopt.obs_n_open",
                                 "method.modules.trajopt.obs_n_close",
                                 "method.modules.trajopt.obs_n_hold",
                                 "method.modules.trajopt.obs_max_age_ticks",
                                 "method.modules.trajopt.obs_occlusion_range_m"};

        // D7 — bare param (outside method.modules.trajopt.*), default 50 =
        // the hard invariant itself, mirrors mrs_sim/clock_node.cpp.
        const int control_period_ms = this->declare_parameter<int>("control_period_ms", 50);

        // mirrors mrta_node.cpp's own "instance_id" precedent (bare param,
        // empty-string default — not an algorithm parameter).
        instance_id_ = this->declare_parameter<std::string>("instance_id", "");

        rclcpp::QoS latched_qos(rclcpp::KeepLast(1));
        latched_qos.reliable().transient_local();

        robot_specs_sub_ = this->create_subscription<mrs_msgs::msg::RobotSpecs>(
            "/robot_specs",
            latched_qos,
            [this](mrs_msgs::msg::RobotSpecs::ConstSharedPtr msg)
            {
                this->OnRobotSpecs(msg);
            });

        // 🆕 D1' — /roadmap: node id -> (x,y) table, needed to resolve a
        // segment's `to_id` into a goal Pose2. QoS mirrors mapf_node's own
        // /roadmap subscriber (RELIABLE/TRANSIENT_LOCAL/KeepLast(1),
        // 10_architecture.md §7, 실측 확인 361_p2§7).
        roadmap_sub_ = this->create_subscription<mrs_msgs::msg::Roadmap>(
            "/roadmap",
            latched_qos,
            [this](mrs_msgs::msg::Roadmap::ConstSharedPtr msg)
            {
                this->OnRoadmap(msg);
            });

        // 🆕 D1' — /execution_constraints: this robot's segments[]. QoS
        // RELIABLE/TRANSIENT_LOCAL/KeepLast(1) — 10_architecture.md §7,
        // 361_p2§7 실측(발행측 sadg_t0_node 확인).
        exec_constraints_sub_ = this->create_subscription<mrs_msgs::msg::ExecutionConstraints>(
            "/execution_constraints",
            latched_qos,
            [this](mrs_msgs::msg::ExecutionConstraints::ConstSharedPtr msg)
            {
                this->OnExecutionConstraints(msg);
            });

        // /trajectories — latched like the sibling upstream boundary artifacts
        // (/roadmap, /plan, /execution_constraints; 10_architecture.md §7).
        // 🔴 [결정] that table does not list /trajectories explicitly; mirroring
        // the sibling artifact topics (rather than inventing a fresh profile)
        // is the choice made here.
        traj_pub_ =
            this->create_publisher<mrs_msgs::msg::Trajectories>("/trajectories", latched_qos);

        // 🆕 376_p2 — /stop_declaration: RELIABLE/VOLATILE/KEEP_LAST(32)
        // (`357`§3-3 / `10_architecture.md`§7 D30 — a lost declaration must
        // NOT be tolerated the way a lost scan is; this is an event, not a
        // state stream). Event-driven publish (see `PublishStopDeclaration`).
        rclcpp::QoS stop_declaration_qos(rclcpp::KeepLast(32));
        stop_declaration_qos
            .reliable();  // durability defaults to VOLATILE (no .transient_local()).
        stop_declaration_pub_ =
            this->create_publisher<mrs_msgs::msg::StopDeclarations>("/stop_declaration",
                                                                    stop_declaration_qos);

        // The 50 ms control timer — node-clock based (NOT create_wall_timer,
        // 10_architecture.md §9 "create_wall_timer 금지"), so use_sim_time
        // actually governs its cadence, per multibot3/CLAUDE.md invariant 5.
        control_timer_ = this->create_timer(std::chrono::milliseconds(control_period_ms),
                                            [this]()
                                            {
                                                this->OnControlTick();
                                            });

        // 368_p3 — CN-18 clock seam, node-owned (service never builds its
        // own). Injected per-robot into ServiceWiring::clock in
        // TryStartService.
        steady_clock_ = std::make_unique<RclcppSteadyClock>(this->get_clock());
    }

private:
    /// @brief Per-robot onboard state: physical limits, owned plugin cells,
    /// the lazily-built tick service (D1), latest odom observation, and this
    /// robot's ROS endpoints.
    struct RobotEntry
    {
        RobotEntry(core::RobotLimits lim_in, double stride_m, double window_radius_m)
            : lim(std::move(lim_in)), subgoals(stride_m, window_radius_m)
        {
        }

        core::RobotLimits lim;
        plugins::SearchFixedPathGating search;             // D2.
        plugins::SubgoalFreeSpaceDisk subgoals;            // D3.
        std::unique_ptr<service::ControlTickService> svc;  // built once odom+goal both land (D1').
        bool has_odom = false;
        core::Pose2 latest_pose;
        double latest_v = 0.0;
        bool has_goal = false;  // D1' — resolved from /execution_constraints + /roadmap.
        core::Pose2 goal;
        rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub;
        rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub;
        // 🆕 376 (N1) — latest world observation (O(1) swap, OBS-5 ⑴) + the
        // node-level tick at which it was captured (-1 = never captured).
        // `TickInput::obs.age_ticks` is derived from
        // `global_tick_seq_ - obs_capture_tick` each control tick.
        core::WorldObservation latest_obs;
        std::int64_t obs_capture_tick = -1;
        rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr scan_sub;

        // 🆕 376_p2 (`/stop_declaration` publish) — the CURRENT segment's
        // `from_id`/`to_id`, cached alongside `goal` (same resolution point,
        // `ResolveGoalsFromExecConstraints`). `blocked_edge`'s VALUE comes
        // from here — "이 로봇이 선언 시점에 보유한 세그먼트의 from/to", NOT
        // new information (`13_p2` schema :35). D1' known limitation
        // (multi-segment progression unimplemented) applies here too: this
        // stays fixed at the first-resolved segment.
        std::string current_segment_from;
        std::string current_segment_to;

        // 🆕 376_p2 — edge-triggered `/stop_declaration` publish state
        // (357 D30: "사건 구동이라 매 틱 직렬화가 아니다" — publish ONLY on a
        // has_stop TRANSITION, not every tick it stays true). The declared
        // pair is FROZEN at open time and reused verbatim at close
        // (schema :35 "declared:false는 개시 때와 글자 그대로 같은 쌍").
        bool last_declared_open = false;
        std::string declared_from;
        std::string declared_to;
        core::StopReason declared_reason = core::StopReason::kUnresolvableLocally;
        std::uint64_t stop_seq = 0;  // per-robot, 0-based, monotonic (separate space, schema :27).
        // OBS-8 (reopen with a different reason while still open) is NOT
        // implemented (357§5 Q3 — needs a stateful multi-reason ledger + a
        // "close old / open new" two-declaration protocol). This flag rate-
        // limits the WARN below to once per open window instead of every
        // control tick the mismatch persists.
        bool obs8_conflict_warned = false;
    };

    void OnRobotSpecs(mrs_msgs::msg::RobotSpecs::ConstSharedPtr msg)
    {
        adapter::BoundaryRobotSpecs boundary;
        boundary.instance_id = msg->instance_id;
        boundary.robots.reserve(msg->robots.size());
        for (const auto& r : msg->robots)
        {
            boundary.robots.push_back({r.robot,
                                       r.avg_traversal_speed_mps,
                                       r.v_max,
                                       r.brake_decel_min_mps2,
                                       r.curvature_max_invm,
                                       r.reverse_motion_allowed,
                                       r.circumradius_m});
        }

        std::vector<core::RobotLimits> specs;
        adapter::RobotSpecsAdapter::from_boundary(boundary, specs);  // always kOk.

        // Throws ContractViolation (CN-15) on any SU00-SU13 refusal — startup
        // path only. main() below turns that into exit code 1. Unchanged from
        // 47차 (this is the pre-existing halt point documented in trajopt.yaml).
        core::StartupChecks::run(specs, fleet_, cfg_);

        // 🆕 D3 validation — same failure point as SU01-05 above, not an
        // earlier one: only checked once real /robot_specs data has arrived.
        if (std::isnan(subgoal_stride_m_) || subgoal_stride_m_ <= 0.0 ||
            std::isnan(subgoal_window_radius_m_) || subgoal_window_radius_m_ <= 0.0)
        {
            throw mrs_core::ContractViolation(
                "trajopt_node: method.modules.trajopt.subgoal_stride_m / "
                "subgoal_window_radius_m missing or <= 0 ([값 부재] — no fabricated "
                "default per this round's rule; SubgoalFreeSpaceDisk cannot be built).");
        }

        rclcpp::QoS cmd_vel_qos(rclcpp::KeepLast(1));
        cmd_vel_qos.reliable();  // matches mrs_sim/state_integrator's subscriber side.

        rclcpp::QoS odom_qos(rclcpp::KeepLast(1));
        odom_qos.best_effort();  // matches mrs_sim/state_integrator's publisher side.

        // 🆕 376 (N1) — `/{robot}/scan`: BEST_EFFORT/VOLATILE/KeepLast(1),
        // `357` D23 (a stale/lost scan must never latch a false "still
        // blocked"; the age-staleness guard in `IsQ1RawPredicate` is the
        // OTHER half of that defense).
        rclcpp::QoS scan_qos(rclcpp::KeepLast(1));
        scan_qos.best_effort();

        for (const auto& lim : specs)
        {
            if (robots_.count(lim.robot) > 0)
            {
                // 🔴 known limitation — no support for a revised robot roster
                // arriving on a later /robot_specs publication this round.
                RCLCPP_WARN(this->get_logger(),
                            "trajopt_node: robot '%s' already registered — skipping "
                            "re-registration (revised /robot_specs not supported this round).",
                            lim.robot.c_str());
                continue;
            }

            robots_.emplace(lim.robot,
                            RobotEntry(lim, subgoal_stride_m_, subgoal_window_radius_m_));
            RobotEntry& entry = robots_.at(lim.robot);

            entry.cmd_vel_pub =
                this->create_publisher<geometry_msgs::msg::Twist>("/" + lim.robot + "/cmd_vel",
                                                                  cmd_vel_qos);
            entry.odom_sub = this->create_subscription<nav_msgs::msg::Odometry>(
                "/" + lim.robot + "/odom",
                odom_qos,
                [this, robot = lim.robot](nav_msgs::msg::Odometry::ConstSharedPtr odom_msg)
                {
                    this->OnOdom(robot, odom_msg);
                });
            entry.scan_sub = this->create_subscription<sensor_msgs::msg::LaserScan>(
                "/" + lim.robot + "/scan",
                scan_qos,
                [this, robot = lim.robot](sensor_msgs::msg::LaserScan::ConstSharedPtr scan_msg)
                {
                    this->OnScan(robot, scan_msg);
                });
        }

        RCLCPP_INFO(this->get_logger(),
                    "trajopt_node: startup checks passed for %zu robot(s). Control-tick "
                    "loop wired — waiting on first /odom per robot before that robot's "
                    "ControlTickService starts (D1).",
                    specs.size());
    }

    void OnOdom(const std::string& robot, nav_msgs::msg::Odometry::ConstSharedPtr msg)
    {
        try  // module-developer convention: every callback top level is try-wrapped.
        {
            auto it = robots_.find(robot);
            if (it == robots_.end())
            {
                return;  // odom for an unregistered robot — should not happen.
            }
            RobotEntry& entry = it->second;

            // Planar yaw recovery: mrs_sim/state_integrator emits a pure Z-axis
            // rotation quaternion (qx=qy=0), so theta = 2*atan2(qz, qw) is exact.
            const double qz = msg->pose.pose.orientation.z;
            const double qw = msg->pose.pose.orientation.w;
            entry.latest_pose.x = msg->pose.pose.position.x;
            entry.latest_pose.y = msg->pose.pose.position.y;
            entry.latest_pose.theta = 2.0 * std::atan2(qz, qw);
            entry.latest_v = msg->twist.twist.linear.x;
            entry.has_odom = true;

            TryStartService(robot);
        }
        catch (const std::exception& e)
        {
            RCLCPP_ERROR(this->get_logger(), "OnOdom(%s): %s", robot.c_str(), e.what());
        }
    }

    /// @brief 376 (N1) — `/{robot}/scan` callback: O(1) overwrite of the
    /// robot's latest observation + the node-level capture-tick stamp used
    /// to derive `age_ticks` in `OnControlTick`. No parsing happens on the
    /// control-tick path (`CN-19`) — the conversion happens HERE, in a
    /// DIFFERENT `MutuallyExclusive` group from the control timer
    /// (`357`§6, the same disposition as `/execution_constraints`).
    void OnScan(const std::string& robot, sensor_msgs::msg::LaserScan::ConstSharedPtr msg)
    {
        try  // module-developer convention: every callback top level is try-wrapped.
        {
            auto it = robots_.find(robot);
            if (it == robots_.end())
            {
                return;  // scan for an unregistered robot — should not happen.
            }
            RobotEntry& entry = it->second;
            entry.latest_obs = ScanToObservation(*msg, cfg_.obs_occlusion_range_m);
            entry.obs_capture_tick = global_tick_seq_;
        }
        catch (const std::exception& e)
        {
            RCLCPP_ERROR(this->get_logger(), "OnScan(%s): %s", robot.c_str(), e.what());
        }
    }

    /// @brief D1' — /roadmap latch: build the node id -> (x,y) table, then
    /// retry resolving any pending segment goals (arrival order between
    /// /roadmap and /execution_constraints is not guaranteed).
    void OnRoadmap(mrs_msgs::msg::Roadmap::ConstSharedPtr msg)
    {
        try
        {
            roadmap_nodes_.clear();
            for (const auto& n : msg->nodes)
            {
                core::Pose2 p;
                p.x = n.x;
                p.y = n.y;
                p.theta = 0.0;  // roadmap carries no heading; direction comes from start->goal.
                roadmap_nodes_[n.id] = p;
            }
            ResolveGoalsFromExecConstraints();
        }
        catch (const std::exception& e)
        {
            RCLCPP_ERROR(this->get_logger(), "OnRoadmap: %s", e.what());
        }
    }

    /// @brief D1' — /execution_constraints latch: cache the message, then
    /// (re)resolve each known robot's first-segment goal.
    void OnExecutionConstraints(mrs_msgs::msg::ExecutionConstraints::ConstSharedPtr msg)
    {
        try
        {
            last_exec_constraints_ = msg;
            ResolveGoalsFromExecConstraints();
        }
        catch (const std::exception& e)
        {
            RCLCPP_ERROR(this->get_logger(), "OnExecutionConstraints: %s", e.what());
        }
    }

    /// @brief D1' — for each registered robot, find its FIRST segment
    /// (minimal `{robot}#{index}` suffix) in the last-received
    /// /execution_constraints message and resolve `to_id` via the roadmap
    /// node table. Missing-node handling (368_p2 brief question, answered in
    /// the file-doc "Missing-node handling" note above): NOT a
    /// ContractViolation — logged and left unresolved/unchanged for that
    /// robot (safe degrade, no fabricated coordinate). Multi-segment
    /// progression is NOT implemented: once a robot has a resolved goal it
    /// is never overwritten by a later message (known limitation).
    void ResolveGoalsFromExecConstraints()
    {
        if (last_exec_constraints_ == nullptr)
        {
            return;  // nothing received yet.
        }

        for (auto& kv : robots_)
        {
            const std::string& robot = kv.first;
            RobotEntry& entry = kv.second;
            if (entry.has_goal)
            {
                continue;  // D1' known limitation: first-resolved goal sticks.
            }

            const mrs_msgs::msg::Segment* first = nullptr;
            long long first_index = -1;
            for (const auto& seg : last_exec_constraints_->segments)
            {
                if (seg.robot != robot)
                {
                    continue;
                }
                const auto hash_pos = seg.id.find('#');
                if (hash_pos == std::string::npos)
                {
                    RCLCPP_ERROR(this->get_logger(),
                                 "ResolveGoalsFromExecConstraints: segment id '%s' does not "
                                 "match the {robot}#{index} pattern — skipped.",
                                 seg.id.c_str());
                    continue;
                }
                long long idx = 0;
                try
                {
                    idx = std::stoll(seg.id.substr(hash_pos + 1));
                }
                catch (const std::exception&)
                {
                    RCLCPP_ERROR(this->get_logger(),
                                 "ResolveGoalsFromExecConstraints: segment id '%s' index is "
                                 "not parseable — skipped.",
                                 seg.id.c_str());
                    continue;
                }
                if (first == nullptr || idx < first_index)
                {
                    first = &seg;
                    first_index = idx;
                }
            }

            if (first == nullptr)
            {
                continue;  // this robot has zero segments right now — contract §2-2 item 7, legal.
            }

            auto node_it = roadmap_nodes_.find(first->to_id);
            if (node_it == roadmap_nodes_.end())
            {
                // Missing-node handling (see file-doc note): not a
                // ContractViolation, no fabricated coordinate — log and wait
                // for a roadmap/execution_constraints update that resolves.
                RCLCPP_ERROR(this->get_logger(),
                             "%s: first segment '%s' to_id '%s' not found in /roadmap nodes "
                             "(roadmap has %zu node(s) so far) — goal NOT set this message.",
                             robot.c_str(),
                             first->id.c_str(),
                             first->to_id.c_str(),
                             roadmap_nodes_.size());
                continue;
            }

            entry.goal = node_it->second;
            entry.has_goal = true;
            // 376_p2 — cache the segment pair `blocked_edge` will reuse
            // verbatim if/when this robot declares a stop (schema :35).
            entry.current_segment_from = first->from_id;
            entry.current_segment_to = first->to_id;
            RCLCPP_INFO(this->get_logger(),
                        "%s: goal resolved from segment '%s' -> node '%s' (%.3f, %.3f).",
                        robot.c_str(),
                        first->id.c_str(),
                        first->to_id.c_str(),
                        entry.goal.x,
                        entry.goal.y);
            TryStartService(robot);
        }
    }

    /// @brief Construct this robot's ControlTickService once both a real
    /// odom pose AND a resolved segment goal are available (D1'). No-op if
    /// already started or still waiting on either input.
    void TryStartService(const std::string& robot)
    {
        auto it = robots_.find(robot);
        if (it == robots_.end())
        {
            return;
        }
        RobotEntry& entry = it->second;
        if (entry.svc != nullptr || !entry.has_odom || !entry.has_goal)
        {
            return;
        }

        service::ServiceWiring wiring;
        wiring.search = &entry.search;
        wiring.subgoals = &entry.subgoals;
        wiring.channel = &peer_channel_;     // D4 — shared board.
        wiring.instr = nullptr;              // D6.
        wiring.safety = &safety_monitor_;    // D5 — shared, "always stoppable".
        wiring.clock = steady_clock_.get();  // 368_p3 — CN-18 seam, node-owned.

        entry.svc = std::make_unique<service::ControlTickService>(robot,
                                                                  wiring,
                                                                  cfg_,
                                                                  entry.lim,
                                                                  fleet_,
                                                                  entry.goal);
    }

    void OnControlTick()
    {
        try  // CN-16: no exception escapes the tick path.
        {
            // 376 (N1) — a fleet-wide control-tick ordinal (OBS-5 ⑵ "나이는
            // 순서수", 221-W2). Incremented once per OnControlTick, shared
            // across robots (the control tick itself is fleet-wide).
            global_tick_seq_ += 1;

            std::vector<std::vector<core::StateSample>> chains;
            std::vector<std::string> robots_out;
            chains.reserve(robots_.size());
            robots_out.reserve(robots_.size());

            for (auto& kv : robots_)
            {
                const std::string& robot = kv.first;
                RobotEntry& entry = kv.second;
                if (entry.svc == nullptr)
                {
                    continue;  // no /odom yet for this robot — honest 0 cmd_vel messages.
                }

                service::TickInput in;
                in.pose = entry.latest_pose;
                in.v = entry.latest_v;
                in.staged_constraints_fresh =
                    false;                // no /execution_constraints wiring this round.
                in.tube_pierced = false;  // no puncture detector this round (needs keepout).
                // 376 (N1) — this tick's world observation. `age_ticks` is
                // derived HERE from the capture-tick stamp (O(1), no parsing
                // on this path, CN-19); a robot with no scan yet gets
                // `fresh=false` (struct default) so IsQ1RawPredicate() can
                // never confirm Q1 for it (과소 보고=안전).
                in.obs = entry.latest_obs;
                if (entry.obs_capture_tick >= 0)
                {
                    in.obs.age_ticks = global_tick_seq_ - entry.obs_capture_tick;
                }
                else
                {
                    in.obs.fresh = false;  // never captured yet.
                }

                service::TickOutput out = entry.svc->run_tick(in);

                geometry_msgs::msg::Twist cmd;
                cmd.linear.x = out.cmd.v;
                cmd.angular.z = out.cmd.omega;
                entry.cmd_vel_pub->publish(cmd);

                // 376_p2 (D8 closed) — edge-triggered /stop_declaration
                // publish. See HandleStopDeclarationTransition's doc for why
                // this is NOT "every tick has_stop is true" (357 D30).
                HandleStopDeclarationTransition(robot, entry, out);

                chains.push_back(entry.svc->committed());
                robots_out.push_back(robot);
            }

            if (!robots_out.empty())
            {
                PublishTrajectories(chains, robots_out);
            }
        }
        catch (const std::exception& e)
        {
            RCLCPP_ERROR(this->get_logger(), "OnControlTick: %s", e.what());
        }
    }

    /// @brief 376_p2 — detects a `has_stop` TRANSITION for one robot this
    /// tick and publishes the matching `/stop_declaration` event, if any.
    /// No-op (no publish) when there is no transition — this is the "사건
    /// 구동" half of D30 (see file doc).
    /// @param robot this robot's identifier.
    /// @param entry this robot's mutable bookkeeping (declaration state).
    /// @param out this tick's `ControlTickService::run_tick()` output.
    void HandleStopDeclarationTransition(const std::string& robot,
                                         RobotEntry& entry,
                                         const service::TickOutput& out)
    {
        if (out.has_stop && !entry.last_declared_open)
        {
            // OPEN — freeze the (from,to) pair from the CURRENTLY held
            // segment (schema :35, "새 정보가 아니다"). No fabrication if
            // that segment isn't resolved yet (should not happen once
            // entry.svc exists, since TryStartService requires has_goal —
            // defensive, not the expected path).
            if (entry.current_segment_from.empty() || entry.current_segment_to.empty())
            {
                RCLCPP_ERROR(this->get_logger(),
                             "%s: has_stop but no resolved segment (from/to) to attribute "
                             "blocked_edge to — /stop_declaration NOT published this tick "
                             "(no fabricated edge).",
                             robot.c_str());
                return;
            }
            entry.declared_from = entry.current_segment_from;
            entry.declared_to = entry.current_segment_to;
            entry.declared_reason = out.stop_reason;
            entry.last_declared_open = true;
            entry.obs8_conflict_warned = false;
            PublishStopDeclaration(robot, entry, /*declared=*/true);
        }
        else if (!out.has_stop && entry.last_declared_open)
        {
            // CLOSE — the SAME pair (and reason) as at open time (schema :35).
            PublishStopDeclaration(robot, entry, /*declared=*/false);
            entry.last_declared_open = false;
        }
        else if (out.has_stop && entry.last_declared_open &&
                 out.stop_reason != entry.declared_reason)
        {
            // OBS-8 (reopen with a DIFFERENT reason while still open) — NOT
            // implemented (see file doc). Reported once per open window, not
            // silently dropped, not force-fit.
            if (!entry.obs8_conflict_warned)
            {
                RCLCPP_WARN(
                    this->get_logger(),
                    "%s: stop reason changed from %s to %s while still declared "
                    "open — OBS-8 (reopen with a different reason) is NOT "
                    "implemented (376_p2 known limitation); the boundary "
                    "declaration keeps the OLD reason.",
                    robot.c_str(),
                    core::DeclarationRegulator::to_schema_string(entry.declared_reason).c_str(),
                    core::DeclarationRegulator::to_schema_string(out.stop_reason).c_str());
                entry.obs8_conflict_warned = true;
            }
        }
        // else: no transition, no conflict -> no publish (사건 구동, D30).
    }

    /// @brief 376_p2 — builds ONE `mrs.stop_declaration` 5.0.1 message
    /// (a single declaration) and publishes it. Struct-fill only — no JSON
    /// text assembly (`CN-19`; see the 376_p2 file-doc block for the
    /// judgment call).
    /// @param robot this robot's identifier.
    /// @param entry this robot's bookkeeping — `seq` is incremented here.
    /// @param declared true = open, false = close (schema :48).
    void PublishStopDeclaration(const std::string& robot, RobotEntry& entry, bool declared)
    {
        adapter::InternalDeclaration decl;
        decl.seq = entry.stop_seq;
        entry.stop_seq += 1;  // per-robot, 0-based, monotonic (this channel's own space).
        decl.robot = robot;
        decl.blocked_from = entry.declared_from;
        decl.blocked_to = entry.declared_to;
        decl.declared = declared;
        decl.reason = entry.declared_reason;

        adapter::BoundaryStopDeclarations boundary;
        if (!mrs_core::ok(
                adapter::StopDeclarationAdapter::to_boundary({decl}, instance_id_, boundary)))
        {
            RCLCPP_ERROR(this->get_logger(), "PublishStopDeclaration: adapter to_boundary failed.");
            return;
        }

        mrs_msgs::msg::StopDeclarations msg;
        msg.schema = "mrs.stop_declaration";
        msg.schema_version = "5.0.1";
        msg.instance_id = boundary.instance_id;
        msg.declarations.reserve(boundary.declarations.size());
        for (const auto& bd : boundary.declarations)
        {
            mrs_msgs::msg::StopDeclaration sd;
            sd.seq = bd.seq;
            sd.robot = bd.robot;
            sd.blocked_edge.from_id = bd.blocked_edge.from;
            sd.blocked_edge.to_id = bd.blocked_edge.to;
            sd.declared = bd.declared;
            sd.reason = StopReasonToRosConstant(entry.declared_reason);
            msg.declarations.push_back(sd);
        }
        stop_declaration_pub_->publish(msg);
    }

    void PublishTrajectories(const std::vector<std::vector<core::StateSample>>& chains,
                             const std::vector<std::string>& robots)
    {
        adapter::BoundaryTrajectories boundary;
        if (!mrs_core::ok(adapter::TrajectoriesAdapter::to_boundary(chains,
                                                                    robots,
                                                                    fleet_,
                                                                    instance_id_,
                                                                    boundary)))
        {
            RCLCPP_ERROR(this->get_logger(), "PublishTrajectories: adapter to_boundary failed.");
            return;
        }

        mrs_msgs::msg::Trajectories msg;
        msg.schema = "mrs.trajectories";  // origin producer — no upstream to copy from
        msg.schema_version = "2.0.0";     // (mirrors mrta_node's own /assignment precedent).
        msg.instance_id = boundary.instance_id;
        msg.limits.a_max = boundary.limits.a_max;
        msg.limits.j_max = boundary.limits.j_max;
        msg.limits.min_separation_m = boundary.limits.min_separation_m;
        msg.limits.omega_max_radps = boundary.limits.omega_max_radps;
        msg.limits.yaw_accel_max_radps2 = boundary.limits.yaw_accel_max_radps2;

        msg.trajectories.reserve(boundary.trajectories.size());
        for (const auto& bt : boundary.trajectories)
        {
            mrs_msgs::msg::RobotTrajectory rt;
            rt.robot = bt.robot;
            rt.points.reserve(bt.points.size());
            for (const auto& p : bt.points)
            {
                mrs_msgs::msg::TrajectoryPoint tp;
                tp.t = p.t;
                tp.x = p.x;
                tp.y = p.y;
                tp.yaw = p.yaw;
                tp.has_v = p.has_v;
                tp.v = p.v;
                tp.has_a = p.has_a;
                tp.a = p.a;
                rt.points.push_back(tp);
            }
            msg.trajectories.push_back(std::move(rt));
        }
        traj_pub_->publish(msg);
    }

    core::FleetLimits fleet_;
    core::TrajoptConfig cfg_;
    double subgoal_stride_m_ = std::numeric_limits<double>::quiet_NaN();
    double subgoal_window_radius_m_ = std::numeric_limits<double>::quiet_NaN();
    std::string instance_id_;

    plugins::PeerChannelIdeal peer_channel_;  // D4 — shared cross-robot board.
    core::SafetyMonitor safety_monitor_{
        [](const core::StateSample&, double)
        {
            return true;
        }};  // D5 — no keepout wiring this round -> "always stoppable" placeholder.
    std::unique_ptr<mrs_core::ISteadyClock> steady_clock_;  // 368_p3 — CN-18 seam.

    std::int64_t global_tick_seq_ = 0;  // 376 (N1) — fleet-wide control-tick ordinal.

    std::map<std::string, RobotEntry> robots_;
    std::map<std::string, core::Pose2> roadmap_nodes_;  // D1' — node id -> (x,y).
    mrs_msgs::msg::ExecutionConstraints::ConstSharedPtr last_exec_constraints_;  // D1' cache.
    rclcpp::Subscription<mrs_msgs::msg::RobotSpecs>::SharedPtr robot_specs_sub_;
    rclcpp::Subscription<mrs_msgs::msg::Roadmap>::SharedPtr roadmap_sub_;
    rclcpp::Subscription<mrs_msgs::msg::ExecutionConstraints>::SharedPtr exec_constraints_sub_;
    rclcpp::Publisher<mrs_msgs::msg::Trajectories>::SharedPtr traj_pub_;
    rclcpp::Publisher<mrs_msgs::msg::StopDeclarations>::SharedPtr stop_declaration_pub_;  // 376_p2.
    rclcpp::TimerBase::SharedPtr control_timer_;
};

}  // namespace mrs_trajopt::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    try
    {
        auto node = std::make_shared<mrs_trajopt::node::TrajoptNode>();
        rclcpp::spin(node);
    }
    catch (const mrs_core::ContractViolation& cv)
    {
        RCLCPP_FATAL(rclcpp::get_logger("trajopt_node"),
                     "startup contract violation: %s",
                     cv.what());
        rclcpp::shutdown();
        return 1;
    }
    rclcpp::shutdown();
    return 0;
}
