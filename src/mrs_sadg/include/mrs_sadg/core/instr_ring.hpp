// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_INSTR_RING_HPP
#define MRS_SADG_CORE_INSTR_RING_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <map>
#include <string>

/// @file instr_ring.hpp
/// @brief CN-19 instrumentation triple: a fixed-size ring of latency samples + a
/// fixed-bucket histogram + a drop counter, plus the named counters (337 §7-2:
/// n_hcons_reject, n_evac_replans, n_budget_exceeded, n_wc1_detected, ...). No
/// file/log/string assembly happens inside the tick (CN-19) — the ring is read
/// off-tick by the 1 Hz diagnostics timer. No clock is read here (CN-18):
/// samples are supplied by the caller.

namespace mrs_sadg::core
{

/// @brief Fixed-size instrumentation ring + counters (heap-free, tick-safe).
class InstrRing
{
public:
    static constexpr std::size_t kRingSize = 256;  ///< fixed ring capacity.
    static constexpr std::size_t kBuckets = 8;     ///< fixed histogram buckets.

    /// @brief Increment a named counter (e.g. "n_hcons_reject").
    /// @param key the counter name.
    void count(const std::string& key)
    {
        ++counters_[key];
    }

    /// @brief Read a named counter (0 if never incremented).
    /// @param key the counter name.
    /// @return the counter value.
    std::uint64_t counter(const std::string& key) const;

    /// @brief Record a latency sample (ms). Overwrites oldest; bumps drop count.
    /// @param key the metric name (kept for the diagnostics label).
    /// @param ms the sample in milliseconds.
    void observe(const std::string& key, double ms);

    /// @brief How many samples were dropped by ring overwrite.
    /// @return the drop counter.
    std::uint64_t drops() const
    {
        return drops_;
    }

    /// @brief Number of live samples in the ring (<= kRingSize).
    /// @return the live sample count.
    std::size_t size() const
    {
        return filled_ ? kRingSize : head_;
    }

private:
    std::map<std::string, std::uint64_t> counters_;
    std::array<double, kRingSize> ring_{};
    std::array<std::uint64_t, kBuckets> hist_{};
    std::size_t head_ = 0;
    bool filled_ = false;
    std::uint64_t drops_ = 0;
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_INSTR_RING_HPP
