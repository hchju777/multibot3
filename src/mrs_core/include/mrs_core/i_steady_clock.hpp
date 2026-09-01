// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_CORE_I_STEADY_CLOCK_HPP
#define MRS_CORE_I_STEADY_CLOCK_HPP

/// @file i_steady_clock.hpp
/// @brief Common seam (CN-4/CN-18): the one clock every module reads through.
///
/// `multibot3/CLAUDE.md` CN-18 — "시계는 mrs_core::ISteadyClock 하나". Modules
/// do not redefine this seam (CN-4). This header is pure (`mrs_core_pure`,
/// CN-2/CN-3): no ROS, no rclcpp::Clock, no `std::chrono` clock call baked in
/// — an adapter in the node/ layer wires a concrete implementation (e.g. one
/// backed by `rclcpp::Clock` with `use_sim_time`) at composition time.

namespace mrs_core
{

/// @brief Monotonic time source seam. CN-18: no direct `std::chrono` /
/// `rclcpp::Clock::now()` calls inside `core/`/`plugins/` — go through this.
class ISteadyClock
{
public:
    virtual ~ISteadyClock() = default;

    /// @brief Current time in seconds since an implementation-defined epoch.
    ///
    /// The epoch is not specified — only monotonicity within a run is
    /// guaranteed. Callers take differences, not absolute readings.
    /// @return Monotonically non-decreasing seconds.
    virtual double NowSeconds() const = 0;
};

}  // namespace mrs_core

#endif  // MRS_CORE_I_STEADY_CLOCK_HPP
