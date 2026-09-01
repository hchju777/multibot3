// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_CORE_TYPES_HPP
#define MRS_MRTA_CORE_TYPES_HPP

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#include "mrs_mrta/core/status.hpp"

/// @file types.hpp
/// @layer core [Clean Architecture: Domain].
/// @brief Pure entities: no ROS, no JSON, no `mrs_msgs` (CN-2/CN-3/CN-23).
///
/// This header carries the module's central claim (`336_mrta_canon.md` §336-3,
/// "없는 연산 여섯"): `AssignmentState`'s *only* mutating operations are
/// `push_tail` and `pop_head`. There is no `erase`, `insert_at`, `swap`,
/// `move_to_robot`, `sort`, or `replace` — those six member functions are
/// simply never declared. `test/core/test_assignment_state.cpp` locks this
/// with a compile-time SFINAE probe in addition to behavioural tests.

namespace mrs_mrta::core
{

/// @brief Opaque run-scoped goal identifier (leg-level, per `92` A-TA3/A-TA4).
/// This module never mints one — it is copied verbatim from the boundary.
using GoalId = std::string;

/// @brief Index into the fixed robot roster established at `configure()` time.
using RobotIndex = std::size_t;

/// @brief Sentinel for "no robot" / "not found".
inline constexpr RobotIndex kNoRobot = static_cast<RobotIndex>(-1);

/// @brief The subset of `mrs.roadmap` this module reads
/// (`12a_arch_mrta.md` §4: "endpoints_ 비트셋 + 공유표 핸들"). This module
/// does not build its own distance table (owned by `mrs_core`, absent from
/// this checkout, `12a`§8-3) — this view exists for the startup checks
/// (`C-10`/`C-11`) a node-layer wiring would run, not for the algorithm core.
struct RoadmapView
{
    std::vector<std::string> node_ids;
    std::vector<std::string> endpoints;

    /// @brief Is @p vertex a known roadmap node?
    /// @param vertex Vertex name.
    /// @return true iff @p vertex is in node_ids.
    bool has_node(const std::string& vertex) const;

    /// @brief Is @p vertex an endpoint (V^ep)?
    /// @param vertex Vertex name.
    /// @return true iff @p vertex is in endpoints.
    bool is_endpoint(const std::string& vertex) const;
};

/// @brief One queued goal element (a "leg" of a task — pickup or delivery).
///
/// `id` and `location` are copied from the boundary without modification
/// (adapter identity, assumption A-TA5, `336_mrta_canon_p2.md` §336-5-3).
struct GoalRecord
{
    GoalId id;             ///< Opaque, stable within the run.
    std::string task;      ///< Grouping key (a task may have >1 leg).
    std::string location;  ///< Roadmap endpoint name.

    bool operator==(const GoalRecord& other) const
    {
        return id == other.id && task == other.task && location == other.location;
    }
};

/// @brief A task released but not yet placed on any robot's queue.
struct PendingTask
{
    std::string task;
    std::vector<GoalRecord> goals;
};

/// @brief One task-release event (`mrs.task_release` after `from_boundary`).
struct ReleaseEvent
{
    std::uint64_t release_index = 0;  ///< Run-global sequence, gap = loss.
    std::string task;
    std::vector<GoalRecord> goals;  ///< Order is the within-task leg order (regime, 2.0.0).
};

/// @brief One goal-completion event (`mrs.goal_completion` after `from_boundary`).
struct CompletionEvent
{
    std::string robot;
    GoalId goal_id;
    std::uint64_t seq = 0;       ///< Run-global 0-based monotonic, gap = loss.
    std::uint64_t revision = 0;  ///< Echo of the `mrs.assignment.revision` it was judged against.
};

/// @brief One operator `goal_override` event (`mrs.operator_command` after filtering
/// to `verb == goal_override` and `from_boundary`). Other verbs (`hold`,
/// `speed_cap`) are not this module's concern (`336b`§2-1) and never reach here.
struct OperatorEvent
{
    std::string robot;
    bool active = false;
    std::string goal_location;  ///< Only meaningful when active == true.
    std::uint64_t seq = 0;      ///< Per-robot 0-based monotonic, gap = loss.
};

/// @brief FIFO of tasks that found no eligible robot at release time
/// (`336_mrta_canon.md` §336-3-3: "대기 풀 순서 = 릴리스 순서(FIFO)"). Composed
/// into `AssignmentState` (`12a_arch_mrta.md` §0-3 class diagram).
class PendingPool
{
public:
    /// @brief Append a task to the back of the pool.
    /// @param task The unplaced task.
    void push_back(PendingTask task);

    /// @brief Is the pool empty?
    /// @return true iff no task is pending.
    bool empty() const noexcept
    {
        return tasks_.empty();
    }

    /// @brief Number of tasks pending.
    /// @return the pool size.
    std::size_t size() const noexcept
    {
        return tasks_.size();
    }

    /// @brief Peek the front (oldest-released) task without removing it.
    /// @return pointer to the front task, or nullptr if empty.
    const PendingTask* front() const;

    /// @brief Remove and return the front task.
    /// @return the removed task.
    /// @throws ContractViolation if the pool is empty.
    PendingTask pop_front();

    /// @brief All pending task identifiers (release order — NOT the order the
    /// boundary emits them in; the adapter re-sorts ascending, `336`§336-6-1).
    /// @return the task ids, oldest-first.
    std::vector<std::string> task_ids() const;

private:
    std::vector<PendingTask> tasks_;
};

/// @brief Fixed-capacity, per-robot FIFO queue set — the module's core claim.
///
/// Physical capacity is `goal_queue_capacity_ta + 1` per robot (the "+1 슬롯"
/// of `12a_arch_mrta.md` §2-3: it exists so `faulty_overflow`-style probes can
/// *attempt* to exceed the policy capacity `Q^ta` and be caught, rather than
/// having no representation for overflow at all). `AssignmentState` itself
/// refuses only the physical limit — it does not enforce `Q^ta`; that is the
/// assigner plugin's job (`12a` §2-3, "정상 플러그인이 자기 카운터로 지킨다").
///
/// 🔴 The two mutating members below are the entire public write surface.
/// No `erase`, `insert_at`, `swap`, `move_to_robot`, `sort`, or `replace`
/// exists on this type — see the compile-time probe in
/// `test/core/test_assignment_state.cpp`.
class AssignmentState
{
public:
    /// @brief Construct with a fixed roster and fixed queue capacities.
    /// @param robot_names Robot roster, ascending lexicographic order
    ///        (this fixes the tie-break tier-4 ordering and the "robot index"
    ///        space used by `EventRing::pop_all_ordered`'s completion sort).
    /// @param goal_queue_capacity_ta Policy capacity Q^ta (`336_mrta_canon_p3.md`
    ///        §336-9 cites Q^ta = 4; the value is not hard-coded here).
    /// @throws ContractViolation if @p robot_names is empty, contains a
    ///         duplicate, or is not sorted ascending (a startup-path check —
    ///         never called from a runtime callback, CN-15).
    AssignmentState(std::vector<std::string> robot_names, std::size_t goal_queue_capacity_ta);

    /// @brief Number of robots in the fixed roster.
    /// @return the roster size.
    std::size_t robot_count() const noexcept
    {
        return robot_names_.size();
    }

    /// @brief The fixed robot roster (ascending lexicographic order).
    /// @return a const reference to the roster.
    const std::vector<std::string>& robot_names() const noexcept
    {
        return robot_names_;
    }

    /// @brief Policy capacity Q^ta this instance was configured with.
    /// @return Q^ta.
    std::size_t goal_queue_capacity() const noexcept
    {
        return goal_queue_capacity_ta_;
    }

    /// @brief Physical capacity (Q^ta + 1) per robot.
    /// @return the physical slot count.
    std::size_t physical_capacity() const noexcept
    {
        return goal_queue_capacity_ta_ + 1;
    }

    /// @brief Look up a robot's index by name.
    /// @param robot Robot identifier.
    /// @return the index, or kNoRobot if @p robot is not in the roster.
    RobotIndex index_of(const std::string& robot) const noexcept;

    /// @brief Number of goals currently queued for robot @p r (0..physical_capacity()).
    /// @param r Robot index (must be < robot_count()).
    /// @return the residual queue length.
    std::size_t residual_len(RobotIndex r) const;

    /// @brief The one and only insertion operation (canon `S03`).
    /// @param r Robot index (must be < robot_count()).
    /// @param goal The goal to append to robot @p r's tail.
    /// @return Status::kError if robot @p r is already at physical_capacity()
    ///         (a memory-safety refusal, not a policy decision — see class doc).
    Status push_tail(RobotIndex r, GoalRecord goal);

    /// @brief The one and only removal operation (canon `S04`).
    /// @param r Robot index (must be < robot_count()).
    /// @return the popped record, or std::nullopt if robot @p r's queue is empty.
    std::optional<GoalRecord> pop_head(RobotIndex r);

    /// @brief Read-only peek at the head goal (does not remove).
    /// @param r Robot index (must be < robot_count()).
    /// @return pointer to the head record, or nullptr if empty. Valid until
    ///         the next mutating call on this state.
    const GoalRecord* peek_head(RobotIndex r) const;

    /// @brief Read-only peek at the tail goal (does not remove) — this is the
    /// position the greedy cost model measures from ("꼬리 목표에서 태스크
    /// 픽업 지점까지의 로드맵 최단경로 비용", `336_mrta_canon.md` §336-2-2).
    /// @param r Robot index (must be < robot_count()).
    /// @return pointer to the tail record, or nullptr if the queue is empty
    ///         (in which case the cost model uses the robot's current vertex).
    const GoalRecord* peek_tail(RobotIndex r) const;

    /// @brief Residual queue contents for robot @p r, head..tail order.
    /// @param r Robot index (must be < robot_count()).
    /// @return a copy of the residual queue (read-only view — this is the
    ///         only way the queue leaves this class; there is no iterator
    ///         exposing internal ring indices).
    std::vector<GoalRecord> residual_view(RobotIndex r) const;

    /// @brief Endpoint exclusivity probe (A8): is @p location the location of
    /// *any* currently-queued (uncompleted) goal, on any robot?
    /// @param location Roadmap endpoint name.
    /// @return true iff some robot's residual queue holds a goal at @p location.
    bool location_in_use(const std::string& location) const;

    // -- Robot vertex read model. Not part of Γ_r and not one of the two
    //    queue-mutating operations — a parallel field the cost model reads
    //    (canon `D04`/`B14`: "유휴 기준 정점" — where the greedy distance
    //    query starts from when a robot's queue is empty). Updated from
    //    `on_robot_states`, which never triggers `drain_once`
    //    (`336_mrta_canon.md` §336-0 #6) and is therefore not itself one of
    //    the two mutating queue operations either.

    /// @brief Set robot @p r's last-known roadmap vertex.
    /// @param r Robot index (must be < robot_count()).
    /// @param vertex The vertex name.
    void set_robot_vertex(RobotIndex r, std::string vertex);

    /// @brief Robot @p r's last-known roadmap vertex.
    /// @param r Robot index (must be < robot_count()).
    /// @return the vertex name, or an empty string if never set.
    const std::string& robot_vertex(RobotIndex r) const;

    // -- Pending pool (composed member, `12a_arch_mrta.md` §0-3 class diagram:
    //    `AssignmentState { -PendingPool pool_; }`). Distinct from the queue
    //    above: a task here has not been placed on any robot yet, so FIFO
    //    push/pop on this pool is not one of the "없는 연산 여섯" — that
    //    restriction is scoped to the per-robot queue (Γ_r) only.

    /// @brief Append a task to the back of the pending pool.
    /// @param task The unplaced task.
    void push_pending(PendingTask task);

    /// @brief Is the pending pool empty?
    /// @return true iff no task is pending.
    bool pending_empty() const noexcept;

    /// @brief Number of tasks pending.
    /// @return the pool size.
    std::size_t pending_size() const noexcept;

    /// @brief Peek the oldest-released pending task without removing it.
    /// @return pointer to the front task, or nullptr if empty.
    const PendingTask* peek_pending_front() const;

    /// @brief Remove and return the oldest-released pending task.
    /// @return the removed task.
    /// @throws ContractViolation if the pool is empty.
    PendingTask pop_pending_front();

    /// @brief All pending task identifiers, oldest-first (release order — the
    /// adapter re-sorts ascending for the boundary, `336`§336-6-1).
    /// @return the task ids.
    std::vector<std::string> pending_task_ids() const;

private:
    std::size_t goal_queue_capacity_ta_;
    std::vector<std::string> robot_names_;
    std::vector<std::vector<GoalRecord>> ring_;  ///< ring_[r] preallocated to physical_capacity().
    std::vector<std::size_t> head_;              ///< head_[r]: index of the current head slot.
    std::vector<std::size_t> count_;             ///< count_[r]: number of occupied slots.
    PendingPool pool_;
    std::vector<std::string>
        robot_vertex_;  ///< robot_vertex_[r]: last-known vertex (D04 read model).
};

/// @brief One robot's goal列 as it will be published — either the base
/// residual queue, or (while an operator override is active for that robot)
/// the single projected override goal (`336b_goal_override_design.md` §336b-3).
struct RobotGoalsView
{
    std::string robot;
    std::string start;
    std::vector<GoalRecord> goals;
};

/// @brief The read-only, boundary-shaped snapshot `AssignmentAdapter::to_boundary`
/// consumes. Built by `service::AssignmentService` — never by `core` or a plugin
/// (`12a_arch_mrta.md` §4 "내부 자료구조가 경계로 새지 않음의 근거").
/// This type carries *no* solver cost, tie-break state, or pool internals —
/// only what the `mrs.assignment` 2.0.0 schema needs.
struct AssignmentView
{
    std::vector<RobotGoalsView> robots;
    std::vector<std::string> unassigned_tasks;  ///< Caller sorts ascending before this is built.
    std::uint64_t revision = 0;
};

}  // namespace mrs_mrta::core

#endif  // MRS_MRTA_CORE_TYPES_HPP
