// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_EXECUTION_GRAPH_HPP
#define MRS_SADG_CORE_EXECUTION_GRAPH_HPP

#include <cstdint>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "mrs_sadg/core/types.hpp"

/// @file execution_graph.hpp
/// @brief The compiled switchable action-dependency graph plus its runtime
/// accounting state (337 `ExecutionGraph`, 12c §A-2).
///
/// Owns segments, the fixed dependency set E0 (Type-1 sequential + any committed
/// Type-2 ordering), the switch groups (undetermined orderings), and the runtime
/// bookkeeping the tick mutates: committed (entered) segments, release bits
/// (arrival-vertex entry permits — F/§0-3), the externally-supplied blocked edge
/// set V_blk (apply_blocked_delta, §11-2③), per-robot progress deficit D_r,
/// the current escalation rung, and epoch/commit sequence numbers.
///
/// The graph is written by `ExecutionTickService` only (§5-1 ownership). The
/// order-search layer receives it as `const&` (E1: cannot commit).

namespace mrs_sadg::core
{

/// @brief The order in which a chosen edge set is combined for acyclicity.
using Edge = std::pair<std::size_t, std::size_t>;

/// @brief Compiled graph + runtime accounting for one plan revision.
class ExecutionGraph
{
public:
    // ---- construction (ConstraintCompiler fills these) ---------------------

    /// @brief Add a compiled segment. Later lookups use its id.
    /// @param seg the segment to add.
    void add_segment(const Segment& seg);

    /// @brief Add a fixed dependency (E0): sequential chain or committed order.
    /// @param dep the dependency to add.
    void add_fixed_dep(const Dependency& dep);

    /// @brief Add a switch group (undetermined orderings, mutually exclusive).
    /// @param group the switch group to add.
    void add_switch_group(const SwitchGroup& group);

    /// @brief Reset all runtime accounting (keeps the compiled structure).
    void reset_runtime();

    // ---- compiled accessors ------------------------------------------------

    /// @brief All compiled segments.
    /// @return const reference to the segment vector.
    const std::vector<Segment>& segments() const
    {
        return segments_;
    }

    /// @brief Fixed dependencies (E0 + committed ordering).
    /// @return const reference to the fixed dependency vector.
    const std::vector<Dependency>& fixed_deps() const
    {
        return fixed_deps_;
    }

    /// @brief Switch groups.
    /// @return const reference to the switch group vector.
    const std::vector<SwitchGroup>& switch_groups() const
    {
        return groups_;
    }

    /// @brief Does a segment with @p id exist?
    /// @param id segment id.
    /// @return true iff declared.
    bool has_segment(const SegmentId& id) const
    {
        return index_.find(id) != index_.end();
    }

    /// @brief Dense index of a segment id (for edge lists).
    /// @param id segment id (must exist).
    /// @return dense index in `segments()`.
    std::size_t index_of(const SegmentId& id) const;

    /// @brief Find a segment by id.
    /// @param id segment id.
    /// @return pointer to the segment, or nullptr if absent.
    const Segment* find_segment(const SegmentId& id) const;

    // ---- runtime accounting ------------------------------------------------

    /// @brief Mark a segment committed (entered:true = segment start, ET03).
    /// @param id segment id.
    void mark_entered(const SegmentId& id)
    {
        entered_.insert(id);
    }

    /// @brief Has a segment been committed (entered)?
    /// @param id segment id.
    /// @return true iff entered.
    bool is_entered(const SegmentId& id) const
    {
        return entered_.count(id) != 0;
    }

    /// @brief Mark a segment completed (also implies entered).
    /// @param id segment id.
    void mark_completed(const SegmentId& id)
    {
        entered_.insert(id);
        completed_.insert(id);
    }

    /// @brief Has a segment completed?
    /// @param id segment id.
    /// @return true iff completed.
    bool is_completed(const SegmentId& id) const
    {
        return completed_.count(id) != 0;
    }

    /// @brief Record that an ordering edge is opposing (［D5］/［D7］ semantics).
    /// @param from predecessor segment id.
    /// @param to successor segment id.
    void mark_opposing_edge(const SegmentId& from, const SegmentId& to)
    {
        opposing_edges_.insert({from, to});
    }

    /// @brief Is an ordering edge opposing (release waits for completion, ［D7］)?
    /// @param from predecessor segment id.
    /// @param to successor segment id.
    /// @return true iff the edge is opposing.
    bool is_opposing_edge(const SegmentId& from, const SegmentId& to) const
    {
        return opposing_edges_.count({from, to}) != 0;
    }

    /// @brief Set a release bit (arrival-vertex entry permit granted, ET19).
    /// @param id segment id.
    /// @param permit true to grant the arrival-vertex entry permit.
    void set_release(const SegmentId& id, bool permit);

    /// @brief Is a segment's arrival-vertex entry permitted?
    /// @param id segment id.
    /// @return true iff the release bit is set.
    bool is_released(const SegmentId& id) const
    {
        return released_.count(id) != 0;
    }

    /// @brief Fill the blocked edge set from an external delta (ET17, §11-2③).
    /// @param edges (from-location, to-location) pairs trajopt reported blocked.
    void apply_blocked_delta(const std::vector<std::pair<std::string, std::string>>& edges);

    /// @brief Is a roadmap edge in the blocked set V_blk?
    /// @param from from-location.
    /// @param to to-location.
    /// @return true iff blocked.
    bool is_blocked_edge(const std::string& from, const std::string& to) const;

    /// @brief Number of blocked edges currently held.
    /// @return |V_blk|.
    std::size_t blocked_count() const
    {
        return blocked_.size();
    }

    /// @brief Set a robot's progress deficit D_r (ET20). Time stays analysis-only.
    /// @param robot the robot.
    /// @param deficit D_r = now - tau_nom (a plain scalar; no absolute clock).
    void set_deficit(const std::string& robot, double deficit)
    {
        deficit_[robot] = deficit;
    }

    /// @brief A robot's progress deficit D_r (0 if unset).
    /// @param robot the robot.
    /// @return D_r.
    double deficit(const std::string& robot) const;

    /// @brief The current escalation rung (0..3).
    /// @return the rung.
    int cur_rung() const
    {
        return cur_rung_;
    }

    /// @brief Set the current escalation rung.
    /// @param rung the rung (0..3).
    void set_cur_rung(int rung)
    {
        cur_rung_ = rung;
    }

    /// @brief Tick sequence number (last run_tick).
    /// @return tick_seq.
    std::uint64_t tick_seq() const
    {
        return tick_seq_;
    }

    /// @brief Set the tick sequence number.
    /// @param seq the tick sequence number.
    void set_tick_seq(std::uint64_t seq)
    {
        tick_seq_ = seq;
    }

    /// @brief Commit sequence number (bumped on each all-or-nothing commit).
    /// @return commit_seq.
    std::uint64_t commit_seq() const
    {
        return commit_seq_;
    }

    /// @brief Advance the commit sequence (one commit epoch, ET12).
    void bump_commit_seq()
    {
        ++commit_seq_;
    }

    /// @brief The scenario instance id (set by the compiler).
    /// @return instance_id.
    const std::string& instance_id() const
    {
        return instance_id_;
    }

    /// @brief Set the scenario instance id.
    /// @param id the instance id.
    void set_instance_id(const std::string& id)
    {
        instance_id_ = id;
    }

    /// @brief Plan revision this graph was compiled for.
    /// @return plan_revision.
    std::uint64_t plan_revision() const
    {
        return plan_revision_;
    }

    /// @brief Set the plan revision (ConstraintCompiler).
    /// @param rev the plan revision.
    void set_plan_revision(std::uint64_t rev)
    {
        plan_revision_ = rev;
    }

    // ---- graph queries (used by gates / envelope / search) -----------------

    /// @brief Edge list (dense indices) for fixed deps + one alternative per
    /// group chosen by @p combo (fixed only if @p combo is empty).
    /// @param combo per-group chosen alternative index; empty selects none.
    /// @return the combined edge list.
    std::vector<Edge> edges_for(const std::vector<std::size_t>& combo) const;

    /// @brief Is the combined graph (fixed + chosen alternatives) acyclic?
    /// @param combo per-group chosen alternative index (empty = fixed only).
    /// @return true iff acyclic (Kahn).
    bool is_acyclic(const std::vector<std::size_t>& combo) const;

    /// @brief Record the currently committed switch-group combination (ET12).
    /// @param combo per-group chosen alternative index.
    void set_committed_combo(const std::vector<std::size_t>& combo)
    {
        committed_combo_ = combo;
    }

    /// @brief The currently committed switch-group combination.
    /// @return the committed combo (empty until a reorder commits).
    const std::vector<std::size_t>& committed_combo() const
    {
        return committed_combo_;
    }

    /// @brief All ordering deps in force = fixed ordering + committed alternatives.
    /// @return the effective committed ordering dependencies.
    std::vector<Dependency> committed_ordering_deps() const;

private:
    std::vector<Segment> segments_;
    std::unordered_map<SegmentId, std::size_t> index_;
    std::vector<Dependency> fixed_deps_;
    std::vector<SwitchGroup> groups_;

    std::set<SegmentId> entered_;
    std::set<SegmentId> completed_;
    std::set<std::pair<SegmentId, SegmentId>> opposing_edges_;
    std::set<SegmentId> released_;
    std::set<std::pair<std::string, std::string>> blocked_;
    std::map<std::string, double> deficit_;
    std::vector<std::size_t> committed_combo_;
    std::string instance_id_;
    int cur_rung_ = 0;
    std::uint64_t tick_seq_ = 0;
    std::uint64_t commit_seq_ = 0;
    std::uint64_t plan_revision_ = 0;
};

/// @brief Kahn acyclicity over a dense edge list.
/// @param n number of nodes.
/// @param edges (u, v) directed edges over [0, n).
/// @return true iff acyclic.
bool is_dag(std::size_t n, const std::vector<Edge>& edges);

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_EXECUTION_GRAPH_HPP
