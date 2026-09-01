// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/core/instr_ring.hpp"

namespace mrs_sadg::core
{

std::uint64_t InstrRing::counter(const std::string& key) const
{
    auto it = counters_.find(key);
    return it == counters_.end() ? 0 : it->second;
}

void InstrRing::observe(const std::string& key, double ms)
{
    (void)key;  // label used only by the off-tick diagnostics reader.
    if (filled_)
    {
        ++drops_;  // overwriting a live sample — CN-19 drop counter.
    }
    ring_[head_] = ms;
    // Fixed-bucket histogram: 0-1, 1-2, ..., 6-7, >=7 ms (kBuckets = 8).
    std::size_t b = ms < 0.0 ? 0 : static_cast<std::size_t>(ms);
    if (b >= kBuckets)
    {
        b = kBuckets - 1;
    }
    ++hist_[b];
    ++head_;
    if (head_ >= kRingSize)
    {
        head_ = 0;
        filled_ = true;
    }
}

}  // namespace mrs_sadg::core
