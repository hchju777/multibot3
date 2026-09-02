// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_TRAJECTORY_BUFFER_HPP
#define MRS_TRAJOPT_CORE_TRAJECTORY_BUFFER_HPP

#include <cstddef>
#include <vector>

#include "mrs_trajopt/core/types.hpp"

/// @file trajectory_buffer.hpp
/// @brief The committed state chain ③ and the INV-2 fallback source.
///
/// `hold_previous()` keeps the last committed remainder (INV-2 a1): a partial
/// solve is never adopted (BT-FAIL-SOUND). `adopt()` replaces it with a new
/// chain (only inside the tube or via the commit gate). `truncate()` cuts the
/// horizon at the last stoppable index (CT19).
///
/// 🔴 368_p3 (`_workspace/368_trajopt_tick_wiring_p3.md`) — `sample_at()` is
/// the CT15 (`322_pseudocode_r43.md:93` `follower_.sample(traj_buf_, tick_seq,
/// now)`) realization: it advances through the committed chain by elapsed
/// time since the chain was last `adopt()`-ed, instead of always returning
/// `.front()`. The pseudocode's `follower_.sample` signature does not specify
/// an interpolation scheme, so this implements the narrowest reading: a
/// zero-order hold on the last waypoint whose `.t <= elapsed` (no blending
/// between samples is invented). `tick_seq` (also a `follower_.sample`
/// parameter in the pseudocode) is not consumed here — nothing in the
/// pseudocode specifies what it would add beyond `now`, and inventing a use
/// for it would not be "옮기는 것" but designing.

namespace mrs_trajopt::core
{

/// @brief Holds the committed trajectory and supports hold/adopt/truncate.
class TrajectoryBuffer
{
public:
    /// @brief Replace the committed chain with @p chain (ST29 / TT18).
    /// @param chain the new state chain (copied).
    /// @param now_s the steady-clock time (seconds, `mrs_core::ISteadyClock`)
    ///        at which this chain is adopted — the CT15 reference point that
    ///        `sample_at()` measures elapsed time from.
    void adopt(const std::vector<StateSample>& chain, double now_s)
    {
        committed_ = chain;
        adopted_at_s_ = now_s;
    }

    /// @brief Keep the previous committed chain unchanged (INV-2 a1).
    ///
    /// A no-op by design — the previous remainder stays, and so does its
    /// adoption timestamp (368_p3: time keeps advancing against the SAME
    /// reference point, so `sample_at()` correctly runs off the end of an
    /// un-refreshed chain and holds at its final — safe, v=0 braking-tail —
    /// sample, rather than resetting the clock and re-starting it).
    void hold_previous()
    {
        // Intentionally does nothing: the committed remainder is the fallback.
    }

    /// @brief Cut the horizon at index @p k inclusive (CT19).
    /// @param k the last stoppable index; samples after k are dropped.
    void truncate(std::size_t k)
    {
        if (k + 1 < committed_.size())
        {
            committed_.resize(k + 1);
        }
    }

    /// @brief Read-only view of the committed chain.
    /// @return const reference to the committed state chain.
    const std::vector<StateSample>& view() const
    {
        return committed_;
    }

    /// @brief Is the committed chain empty?
    /// @return true iff no chain has been adopted.
    bool empty() const
    {
        return committed_.empty();
    }

    /// @brief CT15 — the state sample to track right now (368_p3).
    ///
    /// Zero-order hold on the last committed-chain waypoint whose `.t` does
    /// not exceed the time elapsed since this chain was `adopt()`-ed. Empty
    /// chain falls back to @p fallback with v=omega=0 (the pre-368_p3
    /// behavior, preserved for that case).
    /// @param now_s the current steady-clock time (seconds).
    /// @param fallback the pose to report (v=omega=0) if the chain is empty.
    /// @return the state sample to command this tick.
    StateSample sample_at(double now_s, const Pose2& fallback) const
    {
        if (committed_.empty())
        {
            StateSample s;
            s.x = fallback.x;
            s.y = fallback.y;
            s.theta = fallback.theta;
            return s;
        }
        const double elapsed = now_s - adopted_at_s_;
        // 368_p3: `now_s` and `adopted_at_s_` are independent clock readings
        // (not derived from the same arithmetic expression in the general
        // case), so `elapsed` can land an ULP below a chain waypoint's `.t`
        // that it was, in the real world, meant to reach exactly (observed:
        // a `10.0 + t) - 10.0` round trip lost ~1e-16 relative precision and
        // flipped a `<=` at a chain boundary during the 368_p3 regression
        // test). A tiny epsilon absorbs that without inventing a real
        // (physical) tolerance value — this is floating-point hygiene, not
        // an algorithm parameter, so it is not routed through config.
        constexpr double kBoundaryEpsS = 1e-9;
        std::size_t idx = 0;
        for (std::size_t i = 0; i < committed_.size(); ++i)
        {
            if (committed_[i].t <= elapsed + kBoundaryEpsS)
            {
                idx = i;
            }
            else
            {
                break;
            }
        }
        return committed_[idx];
    }

private:
    std::vector<StateSample> committed_;
    double adopted_at_s_ = 0.0;
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_TRAJECTORY_BUFFER_HPP
