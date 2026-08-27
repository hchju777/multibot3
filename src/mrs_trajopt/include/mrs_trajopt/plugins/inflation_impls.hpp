// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_PLUGINS_INFLATION_IMPLS_HPP
#define MRS_TRAJOPT_PLUGINS_INFLATION_IMPLS_HPP

#include <cmath>

#include "mrs_trajopt/core/i_inflation.hpp"

/// @file inflation_impls.hpp
/// @brief Extension point (iii) implementations (A-N4‴, one radius).
///
/// InflationIsotropicBall (pose-independent margin) and InflationHeadingBiased
/// (larger margin along heading). Both add a fixed margin; heading-biased adds
/// an extra fraction projected on the query heading direction.

namespace mrs_trajopt::plugins
{

/// @brief Isotropic ball inflation: a constant additive margin.
class InflationIsotropicBall : public core::IInflation
{
public:
    /// @brief Construct with a constant margin.
    /// @param margin_m additive inflation margin [m] (>= 0).
    explicit InflationIsotropicBall(double margin_m) : margin_m_(margin_m)
    {
    }

    /// @brief Inflated radius = nominal + constant margin.
    /// @param query the query pose (unused for isotropic).
    /// @param nominal_radius_m the base keepout radius [m].
    /// @return nominal_radius_m + margin.
    double support(const core::Pose2& query, double nominal_radius_m) const override
    {
        (void)query;
        return nominal_radius_m + margin_m_;
    }

private:
    double margin_m_;
};

/// @brief Heading-biased inflation: constant margin plus a heading-scaled term.
class InflationHeadingBiased : public core::IInflation
{
public:
    /// @brief Construct with a base margin and a heading bias fraction.
    /// @param margin_m base additive margin [m] (>= 0).
    /// @param bias_m extra margin scaled by |cos(theta)| [m] (>= 0).
    InflationHeadingBiased(double margin_m, double bias_m) : margin_m_(margin_m), bias_m_(bias_m)
    {
    }

    /// @brief Inflated radius = nominal + margin + bias*|cos(theta)|.
    /// @param query the query pose (heading scales the bias term).
    /// @param nominal_radius_m the base keepout radius [m].
    /// @return the heading-biased inflated radius.
    double support(const core::Pose2& query, double nominal_radius_m) const override
    {
        return nominal_radius_m + margin_m_ + bias_m_ * std::fabs(std::cos(query.theta));
    }

private:
    double margin_m_;
    double bias_m_;
};

}  // namespace mrs_trajopt::plugins

#endif  // MRS_TRAJOPT_PLUGINS_INFLATION_IMPLS_HPP
