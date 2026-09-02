// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_ADAPTER_ENTRY_EVENTS_ADAPTER_HPP
#define MRS_SADG_ADAPTER_ENTRY_EVENTS_ADAPTER_HPP

#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "mrs_sadg/adapter/boundary_types.hpp"
#include "mrs_sadg/core/status.hpp"

/// @file entry_events_adapter.hpp
/// @brief Converts mrs.entry_events 2.0.0 (boundary, upstream trajopt->sadg_t0)
/// into the two internal predicates the tick service consumes (CN-8/CN-9).
///
/// 🔴 The split is DERIVED, not invented: `entry_events.schema.json`'s own
/// field-level constraint on `location` reads "entered:true이면 location ==
/// segment.to, false이면 location == segment.from" — i.e. an event tagged
/// `entered:true` reports that `segment`'s ARRIVAL vertex (`to`) was just
/// occupied, and `entered:false` reports that its DEPARTURE vertex (`from`) was
/// just vacated. Cross-referencing the two internal predicates the core already
/// exposes settles which one each case feeds:
///   - `ExecutionGraph::mark_completed` (`execution_graph.hpp`: "also implies
///     entered") is used for the OPPOSING (［D7]) release rule which
///     `release_checker.cpp` documents as "opens only on predecessor COMPLETE" —
///     i.e. "arrived at its destination". That is the `entered:true` case.
///   - `ExecutionGraph::mark_entered` (`commit_arbiter.hpp`: "entered:true =
///     segment start") is used for the FOLLOWING (［D6]) release rule
///     (`release_checker.cpp`: "opens once r1#1 has STARTED (entered)"), which
///     `release_checker.cpp`'s own inline comment glosses as "departed the
///     shared vertex" — i.e. "left its origin". That is the `entered:false`
///     case.
/// No new vocabulary is introduced; this header only names the reading.

namespace mrs_sadg::adapter
{

/// @brief The two internal predicates + per-robot next-expected-seq, derived
/// from one mrs.entry_events 2.0.0 drain. Output-only accumulator: the caller
/// merges `started`/`completed` into its own per-tick drain buffer and folds
/// `next_seq_by_robot` into a persistent (never-cleared) monotonic map — the
/// three fields have different lifetimes and this struct does not own either.
struct EntryEventsView
{
    std::vector<std::string> started;    ///< segment ids -> TickInput.entry_events (mark_entered).
    std::vector<std::string> completed;  ///< segment ids -> TickInput.completions (mark_completed).
    std::map<std::string, std::uint64_t>
        next_seq_by_robot;  ///< 1 + max(seq) seen this call, per robot.
};

/// @brief mrs.entry_events 2.0.0 -> EntryEventsView adapter (CN-8).
class EntryEventsAdapter
{
public:
    /// @brief Boundary -> internal (pure classification; no graph mutation here).
    /// @param msg the boundary entry-events document.
    /// @param out [out] the split view. Appends to any existing `started`/
    ///            `completed` contents; folds `next_seq_by_robot` by max, so
    ///            repeated calls into the same `out` accumulate correctly.
    /// @return Status::kOk on success, kError if the envelope is wrong.
    static core::Status from_boundary(const BoundaryEntryEvents& msg, EntryEventsView& out);
};

}  // namespace mrs_sadg::adapter

#endif  // MRS_SADG_ADAPTER_ENTRY_EVENTS_ADAPTER_HPP
