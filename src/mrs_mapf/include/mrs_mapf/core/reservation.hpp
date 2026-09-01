// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_CORE_RESERVATION_HPP
#define MRS_MAPF_CORE_RESERVATION_HPP

#include <optional>
#include <string>
#include <vector>

#include "mrs_mapf/core/types.hpp"

/// @file reservation.hpp
/// @brief `RB-1` — reservation-interval quantization for OUT-OF-SCOPE robots
/// (`255`§255-5 / §255-8-3 "밖 로봇 지선"). Time never leaves the boundary
/// (`255`§255-1: "시간을 계산하지도, 내보내지도, 시간으로 판정하지도 않는다") — this
/// is a purely internal working structure the search plugin (and only the
/// search plugin) may consult when treating out-of-scope robots' previously
/// published paths as moving obstacles.
///
/// Invariant (RB-1, §255-5-1(ii)): reserved interval ⊇ nominal occupancy
/// interval. The nominal occupancy for segment i of a robot with average speed
/// v is `[t_i, t_i + length_i / v]` where `t_i` is the EXACT cumulative sum of
/// prior segment durations (never the tick-rounded value — rounding per-segment
/// and then accumulating lets the error grow with path length, which RB-1
/// forbids: "올림한 지속시간을 더해 누적하는 구현은 금지한다").
///
/// 🔴 **함정 ④(발주서): the tick (전역 동기 눈금) VALUE is [값 부재]** — `255`§255-11
/// found nine blockers to a value and left all nine open; the value's home is a
/// system-wide config + preregistration row, never a module default
/// (§255-11-5). This module therefore takes `tick_s` as `std::optional<double>`
/// with **no default** — when absent, the reservation is left UNQUANTIZED
/// (exact endpoints) and `ReservedInterval::quantized` is false, so a caller can
/// tell "no tick supplied" apart from "tick applied". No numeric default is
/// invented anywhere in this file.

namespace mrs_mapf::core
{

/// @brief One segment's exact (unquantized) occupancy interval, and — if a tick
/// was supplied — its RB-1-rounded reservation interval.
struct ReservedInterval
{
    double nominal_entry_s =
        0.0;                      ///< exact cumulative arrival time at the segment's source vertex.
    double nominal_exit_s = 0.0;  ///< nominal_entry_s + length/speed.
    double reserved_entry_s = 0.0;  ///< == nominal_entry_s if !quantized, else floor to tick.
    double reserved_exit_s = 0.0;   ///< == nominal_exit_s if !quantized, else ceil to tick.
    bool quantized = false;         ///< was a tick supplied for this call?
};

/// @brief Compute the exact cumulative arrival-time schedule for one robot's
/// steps, given the roadmap (for segment length) and that robot's average
/// speed. Time origin is "now" (§255-6-1): `arrival[0] == 0`.
/// @param steps the robot's step sequence (>= 1 entries).
/// @param roadmap the roadmap (for `length_m` lookups; a missing arc yields no entry).
/// @param avg_speed_mps the robot's average traversal speed (> 0).
/// @return arrival[i] = exact time the robot reaches steps[i].location, for i in
///   [0, steps.size()). Empty if @p avg_speed_mps <= 0 or a required arc is missing.
std::vector<double> exact_arrival_schedule(const std::vector<StepEntry>& steps,
                                           const Roadmap& roadmap,
                                           double avg_speed_mps);

/// @brief RB-1 for a single segment: exact nominal interval, then (if @p tick_s
/// is present) round the two ENDPOINTS outward — entry floor, exit ceil. Never
/// rounds per-edge durations before summing (that is the forbidden construction
/// §255-5-1(ii) names explicitly).
/// @param nominal_entry_s exact entry time (from `exact_arrival_schedule`).
/// @param nominal_exit_s exact exit time.
/// @param tick_s the tick size in seconds, if known; `std::nullopt` if [값 부재].
/// @return the reservation interval; `.quantized` reflects whether @p tick_s was used.
ReservedInterval reserve_interval(double nominal_entry_s,
                                  double nominal_exit_s,
                                  std::optional<double> tick_s);

/// @brief Reconstruct RB-1 reservation intervals for every segment of an
/// out-of-scope robot's (previously published) plan.
/// @param steps the robot's retained step sequence.
/// @param roadmap the roadmap.
/// @param avg_speed_mps that robot's average speed.
/// @param tick_s the tick size, if known ([값 부재] otherwise).
/// @return one `ReservedInterval` per segment (size == steps.size()-1, or empty if
/// steps.size()<=1).
std::vector<ReservedInterval> reconstruct_reservations(const std::vector<StepEntry>& steps,
                                                       const Roadmap& roadmap,
                                                       double avg_speed_mps,
                                                       std::optional<double> tick_s);

}  // namespace mrs_mapf::core

#endif  // MRS_MAPF_CORE_RESERVATION_HPP
