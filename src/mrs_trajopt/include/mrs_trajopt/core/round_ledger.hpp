// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_ROUND_LEDGER_HPP
#define MRS_TRAJOPT_CORE_ROUND_LEDGER_HPP

#include <cstdint>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

/// @file round_ledger.hpp
/// @brief Inter-robot back-off ledger (U42-8, ST08-ST23). Records lower-robot
/// failures so an upper robot re-selects (quota / timing / candidate column).
///
/// 🔴 This is NOT PIBT "execution": no on-the-fly order change, no chained
/// pushing (고정 결정 3). Priority order is invariant (ST04, U37-1); only the
/// **direction** of back-off (lower fail -> upper re-select) is borrowed.
/// Finite convergence rests on a monotone quota with a cap -> finite rounds
/// (U42-8, the termination proper is 212-T1; the quota is only "progress").

namespace mrs_trajopt::core
{

/// @brief Why a lower robot's subgoal search failed (ST09, TR-C1).
enum class FailCause
{
    kEmptyCandidates,  ///< candidate tree exhausted.
    kBrakeReject,      ///< SG-BRAKE filter rejected every candidate.
    kGeomNoWidth,      ///< no geometric width (TR-W1).
    kBlockedCoarse     ///< blocked at the coarse layer.
};

/// @brief The *origin* cause, kept apart so a coarse retry never overwrites it.
enum class OriginCause
{
    kNone,            ///< no distinct origin.
    kBudgetExhausted  ///< the failure originated in budget exhaustion (TR-C1).
};

/// @brief Outcome of the round loop (ST12/ST13).
enum class RoundOutcome
{
    kConverged,  ///< a round completed with no failures.
    kTruncated   ///< the cap was reached with failures still present (sequential downgrade).
};

/// @brief The inter-robot round loop state: holds, failures, and the outcome.
///
/// A held robot appears to others only as a static disk (RQ-11) and is skipped
/// (ST05). `no_fail()` after a full round => converged (ST12).
class RoundLedger
{
public:
    /// @brief Clear all held/failed state for a fresh subgoal tick (ST02).
    void reset()
    {
        held_.clear();
        failed_this_round_.clear();
        fail_records_.clear();
    }

    /// @brief Is robot @p r currently held (skipped this round)? (ST05)
    /// @param r robot identifier.
    /// @return true iff @p r is in the hold set.
    bool is_held(const std::string& r) const
    {
        return held_.count(r) != 0;
    }

    /// @brief Record a lower-robot failure (ST09). Adds @p r to the hold set.
    /// @param r the robot that failed.
    /// @param cause the immediate failure cause.
    /// @param origin the origin cause; a coarse retry must not overwrite it.
    void fail(const std::string& r, FailCause cause, OriginCause origin)
    {
        held_.insert(r);
        failed_this_round_.insert(r);
        fail_records_.push_back({r, cause, origin});
    }

    /// @brief Did the round just finished have zero failures? (ST12)
    /// @return true iff no fail() was called since the last begin_round().
    bool no_fail() const
    {
        return failed_this_round_.empty();
    }

    /// @brief Mark the start of a new round (clears the per-round failure flag).
    void begin_round()
    {
        failed_this_round_.clear();
    }

    /// @brief One recorded failure (for instrumentation n_sg_fail_by_cause[]).
    struct FailRecord
    {
        std::string robot;   ///< the failed robot.
        FailCause cause;     ///< immediate cause.
        OriginCause origin;  ///< origin cause (TR-C1).
    };

    /// @brief All failure records accumulated since reset().
    /// @return const reference to the failure record list.
    const std::vector<FailRecord>& fail_records() const
    {
        return fail_records_;
    }

private:
    std::set<std::string> held_;
    std::set<std::string> failed_this_round_;
    std::vector<FailRecord> fail_records_;
};

/// @brief The three re-selection handles applied on the next round (ST09/ST19).
///
/// 🔴 Quota is monotone non-decreasing with a cap (221-P6): cap = corridor
/// half-width - circumradius_m. This is the finite-convergence lever
/// (monotone + cap => finite rounds).
class YieldHandles
{
public:
    /// @brief Construct with the quota cap.
    /// @param quota_cap_m cap = corridor half-width - circumradius_m [m] (>= 0).
    explicit YieldHandles(double quota_cap_m) : cap_(quota_cap_m < 0.0 ? 0.0 : quota_cap_m)
    {
    }

    /// @brief Current lateral yield quota for a robot [m].
    /// @param robot robot identifier.
    /// @return the quota, in [0, cap].
    double quota_of(const std::string& robot) const
    {
        auto it = quota_.find(robot);
        return it == quota_.end() ? 0.0 : it->second;
    }

    /// @brief Handle ①: raise the quota by @p step, monotone and capped (ST09①).
    /// @param robot robot whose quota rises.
    /// @param step_m non-negative increment [m]. Negative steps are clamped to 0
    ///        (the quota only "얹기만" — 208-T1, never decreases).
    /// @return the new quota after clamping to [0, cap].
    double raise_quota(const std::string& robot, double step_m)
    {
        double s = step_m < 0.0 ? 0.0 : step_m;
        double next = quota_of(robot) + s;
        if (next > cap_)
        {
            next = cap_;
        }
        quota_[robot] = next;
        return next;
    }

    /// @brief The quota cap [m].
    /// @return cap = corridor half-width - circumradius_m.
    double cap() const
    {
        return cap_;
    }

private:
    double cap_;
    std::unordered_map<std::string, double> quota_;
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_ROUND_LEDGER_HPP
