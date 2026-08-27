// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_SEARCH_SAFE_INTERVAL_TABLE_HPP
#define MRS_TRAJOPT_CORE_SEARCH_SAFE_INTERVAL_TABLE_HPP

#include <vector>

/// @file safe_interval_table.hpp
/// @brief Per-node time-overlap safe intervals (SSIPP, 321b §3-2).
///
/// Time is a **relative offset** in the trajectory layer, never absolute. A
/// safe interval is the complement of the times a neighbor tube / keepout
/// occupies a node — judged as an interval, not a sample (221-P3).

namespace mrs_trajopt::core::search
{

/// @brief A half-open safe interval [lo, hi) of relative time offsets.
struct SafeInterval
{
    double lo = 0.0;  ///< interval start (relative offset).
    double hi = 0.0;  ///< interval end (relative offset).

    /// @brief Does @p t fall inside [lo, hi)?
    /// @param t a relative time offset.
    /// @return true iff lo <= t < hi.
    bool contains(double t) const
    {
        return t >= lo && t < hi;
    }
};

/// @brief Safe intervals per search-node index, capped by safe_interval_max.
class SafeIntervalTable
{
public:
    /// @brief Construct with a per-node interval-count cap (pre-allocation).
    /// @param safe_interval_max max intervals per node; 0 means unbounded.
    explicit SafeIntervalTable(std::size_t safe_interval_max = 0) : cap_(safe_interval_max)
    {
    }

    /// @brief Set the safe intervals for node @p node_index.
    /// @param node_index the search-node index.
    /// @param intervals the safe intervals (truncated to the cap if set).
    void set(std::size_t node_index, std::vector<SafeInterval> intervals)
    {
        if (cap_ != 0 && intervals.size() > cap_)
        {
            intervals.resize(cap_);
        }
        if (node_index >= table_.size())
        {
            table_.resize(node_index + 1);
        }
        table_[node_index] = std::move(intervals);
    }

    /// @brief Safe intervals for node @p node_index.
    /// @param node_index the search-node index.
    /// @return const reference to that node's safe intervals (empty if unset).
    const std::vector<SafeInterval>& at(std::size_t node_index) const
    {
        static const std::vector<SafeInterval> kEmpty;
        return node_index < table_.size() ? table_[node_index] : kEmpty;
    }

private:
    std::size_t cap_;
    std::vector<std::vector<SafeInterval>> table_;
};

}  // namespace mrs_trajopt::core::search

#endif  // MRS_TRAJOPT_CORE_SEARCH_SAFE_INTERVAL_TABLE_HPP
