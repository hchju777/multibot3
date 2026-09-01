// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_PORTS_I_DISTANCE_ORACLE_HPP
#define MRS_MRTA_PORTS_I_DISTANCE_ORACLE_HPP

#include <cstdint>
#include <optional>
#include <string>

/// @file i_distance_oracle.hpp
/// @layer ports [Clean Architecture: Application, driven seam].
/// @brief Module-local seam onto the shared roadmap distance table (CN-4).
///
/// `12a_arch_mrta.md` §8-3 places the *table itself* in `mrs_core` (a process-
/// wide, roadmap-hash-indexed, build-once cache shared with `mrs_mapf` — this
/// module never builds its own table). Since `mrs_core` does not exist in this
/// checkout, this header only defines the seam `mrta` queries through; a real
/// implementation (`SharedRoadmapDistances`) is `mrs_core`'s to provide at
/// integration time. Unit tests inject a small stand-in (see
/// `test/core/test_util.hpp`) — that stand-in is test-only scaffolding, not a
/// production shortest-path implementation living inside `mrs_mrta` (which
/// would recreate the per-process duplication `12a` §8-3 rejected).
namespace mrs_mrta::ports
{

/// @brief Read-only query interface onto a precomputed roadmap distance table.
class IDistanceOracle
{
public:
    virtual ~IDistanceOracle() = default;

    /// @brief Shortest-path cost from a vertex to an endpoint, in millimetres.
    /// @param from_vertex Roadmap vertex the query originates at.
    /// @param to_endpoint Roadmap endpoint (must be a member of
    ///        `mrs.roadmap.endpoints`) the query targets.
    /// @return the integer-millimetre cost, or std::nullopt if @p to_endpoint
    ///         is unreachable from @p from_vertex (e.g. no path exists).
    virtual std::optional<std::uint64_t> dist_to_endpoint(const std::string& from_vertex,
                                                          const std::string& to_endpoint) const = 0;
};

}  // namespace mrs_mrta::ports

#endif  // MRS_MRTA_PORTS_I_DISTANCE_ORACLE_HPP
