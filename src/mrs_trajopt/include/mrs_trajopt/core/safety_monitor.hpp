// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_SAFETY_MONITOR_HPP
#define MRS_TRAJOPT_CORE_SAFETY_MONITOR_HPP

#include <cstddef>
#include <functional>
#include <vector>

#include "mrs_trajopt/core/types.hpp"

/// @file safety_monitor.hpp
/// @brief The dynamic half of the braking filter (CT16-CT19, 321b §D-0-1).
///
/// Replaces the old QP terminal check. Every control tick it asks: from each
/// remaining sample, can the robot still stop before entering a keepout region?
/// If not, the horizon is truncated at the last stoppable index and a fallback
/// is requested. The keepout test is injected as a predicate so the core stays
/// pure (no ROS, no geometry backend dependency).

namespace mrs_trajopt::core
{

/// @brief Predicate: is a sample's braking envelope free of keepout regions?
///
/// Given a sample and the braking decel, returns true iff the robot can come to
/// rest from that sample without its braking envelope intersecting a keepout.
using StoppablePredicate = std::function<bool(const StateSample&, double brake_decel)>;

/// @brief Evaluates remaining-stoppability over the committed chain.
class SafetyMonitor
{
public:
    /// @brief Construct with the keepout stoppability predicate.
    /// @param pred returns true iff a sample is stoppable clear of keepouts.
    explicit SafetyMonitor(StoppablePredicate pred) : pred_(std::move(pred))
    {
    }

    /// @brief Is every remaining sample stoppable? (CT16, H2)
    /// @param chain the committed state chain.
    /// @param brake_decel the guaranteed-min braking decel [m/s^2].
    /// @return true iff all samples are stoppable clear of keepouts.
    bool remaining_stoppable(const std::vector<StateSample>& chain, double brake_decel) const
    {
        for (const auto& s : chain)
        {
            if (!pred_(s, brake_decel))
            {
                return false;
            }
        }
        return true;
    }

    /// @brief Index of the last sample that is still stoppable (CT18, H3).
    /// @param chain the committed state chain.
    /// @param brake_decel the guaranteed-min braking decel [m/s^2].
    /// @return the last index that is stoppable; 0 if none, chain.size()-1 if all.
    std::size_t last_stoppable_index(const std::vector<StateSample>& chain,
                                     double brake_decel) const
    {
        std::size_t last = 0;
        for (std::size_t i = 0; i < chain.size(); ++i)
        {
            if (pred_(chain[i], brake_decel))
            {
                last = i;
            }
            else
            {
                break;
            }
        }
        return last;
    }

private:
    StoppablePredicate pred_;
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_SAFETY_MONITOR_HPP
