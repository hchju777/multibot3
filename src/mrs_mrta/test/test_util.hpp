// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_TEST_TEST_UTIL_HPP
#define MRS_MRTA_TEST_TEST_UTIL_HPP

#include <cstdio>
#include <cstdlib>
#include <map>
#include <optional>
#include <string>
#include <utility>

#include "mrs_mrta/ports/i_distance_oracle.hpp"
#include "mrs_mrta/ports/i_override_goal_id_resolver.hpp"

/// @file test_util.hpp
/// @brief Test-only scaffolding shared by mrs_mrta's ctest binaries.
///
/// 🔴 `MapDistanceOracle` is **not** a production shortest-path implementation
/// — `12a_arch_mrta.md` §8-3 places the real shared distance table in
/// `mrs_core` (absent from this checkout). This is an exact-lookup stub a
/// test populates explicitly so tie-break assertions are deterministic and
/// legible, not an approximation of Dijkstra living inside `mrs_mrta`.

namespace mrs_mrta::testutil
{

/// @brief Minimal assert macro that prints context and aborts non-zero —
/// avoids a gtest dependency (20d precedent, `mrs_trajopt` test binaries).
#define MRTA_CHECK(cond)                                                                        \
    do                                                                                          \
    {                                                                                           \
        if (!(cond))                                                                            \
        {                                                                                       \
            std::fprintf(stderr, "MRTA_CHECK failed: %s (%s:%d)\n", #cond, __FILE__, __LINE__); \
            std::exit(1);                                                                       \
        }                                                                                       \
    } while (0)

/// @brief Exact-lookup distance oracle a test populates explicitly.
class MapDistanceOracle : public ports::IDistanceOracle
{
public:
    /// @brief Register a distance for one (from, to) pair.
    /// @param from Origin vertex.
    /// @param to Endpoint.
    /// @param mm Cost in millimetres.
    void set(const std::string& from, const std::string& to, std::uint64_t mm)
    {
        table_[{from, to}] = mm;
    }

    std::optional<std::uint64_t> dist_to_endpoint(const std::string& from_vertex,
                                                  const std::string& to_endpoint) const override
    {
        const auto it = table_.find({from_vertex, to_endpoint});
        if (it == table_.end())
        {
            return std::nullopt;
        }
        return it->second;
    }

private:
    std::map<std::pair<std::string, std::string>, std::uint64_t> table_;
};

/// @brief Fixed-answer override goal_id resolver — stands in for the
/// [결정 부재] generator reservation (`336b`§336b-4-2). Test-only: the value
/// returned is fixture data, not a claim about what the real reservation
/// rule will be.
class FixedOverrideGoalIdResolver : public ports::IOverrideGoalIdResolver
{
public:
    /// @brief Register the goal_id to return for one (robot, goal_location).
    /// @param robot Robot identifier.
    /// @param goal_location Roadmap endpoint.
    /// @param goal_id The fixture goal_id.
    void set(const std::string& robot, const std::string& goal_location, std::string goal_id)
    {
        table_[{robot, goal_location}] = std::move(goal_id);
    }

    std::optional<core::GoalId> resolve(const std::string& robot,
                                        const std::string& goal_location) override
    {
        const auto it = table_.find({robot, goal_location});
        if (it == table_.end())
        {
            return std::nullopt;
        }
        return it->second;
    }

private:
    std::map<std::pair<std::string, std::string>, std::string> table_;
};

}  // namespace mrs_mrta::testutil

#endif  // MRS_MRTA_TEST_TEST_UTIL_HPP
