// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_I_INFLATION_HPP
#define MRS_TRAJOPT_CORE_I_INFLATION_HPP

#include "mrs_trajopt/core/types.hpp"

/// @file i_inflation.hpp
/// @brief Extension point (iii): sigma inflation of the keepout (CN-10).
///
/// Unchanged by the redesign (321b §3-1, A-N4‴, one radius). The search consumes
/// it as `const IInflation&`. Implementations: InflationIsotropicBall /
/// InflationHeadingBiased.

namespace mrs_trajopt::core
{

/// @brief Inflation support: how much to inflate a keepout at a query pose.
class IInflation
{
public:
    virtual ~IInflation() = default;

    /// @brief Inflation radius [m] to add to a keepout, given the query pose.
    /// @param query the pose at which inflation is evaluated.
    /// @param nominal_radius_m the base keepout radius [m].
    /// @return the inflated radius [m] (>= nominal_radius_m).
    virtual double support(const Pose2& query, double nominal_radius_m) const = 0;
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_I_INFLATION_HPP
