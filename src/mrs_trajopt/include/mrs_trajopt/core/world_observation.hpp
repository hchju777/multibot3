// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_WORLD_OBSERVATION_HPP
#define MRS_TRAJOPT_CORE_WORLD_OBSERVATION_HPP

#include <array>
#include <cstddef>
#include <cstdint>

/// @file world_observation.hpp
/// @brief The exogenous-block observation channel
/// (`_workspace/355_trajopt_exogenous_observation.md` `OBS-1`/`OBS-3`/`OBS-5`,
/// `_workspace/357_arch_observation_seam.md` §3).
///
/// This is the SOLE input the Q1 witness (`DeclarationEvidence::
/// edge_impassable_for_any_robot`) is computed from. It is a **tool-layer,
/// fixed-size POD structure** — NOT a boundary artifact. `355`§4-2/`357`§3-1
/// place the seam at `/{robot}/scan` (`sensor_msgs/msg/LaserScan`), outside
/// `mrs_msgs/schema/`. `core/` never includes `sensor_msgs` (`CN-2`); the
/// `LaserScan` -> `WorldObservation` conversion is a private function in
/// `node/` (`357`§3-4) — this header only knows the POD shape.
///
/// 🔴 376 SIMPLIFIED geometry (reported, not hidden — see `mrs_trajopt` 20d
/// "알려진 한계"): exact alignment against a roadmap edge's `width_m`
/// cross-section (`364_formulation_anchors_p2.md`'s Z7 `xsec(e)`) is NOT
/// implemented this round. `occupied[i]` marks whether sample `i` of a FIXED
/// FORWARD ANGULAR FAN is within `obs_occlusion_range_m` (config,
/// `[값 부재]`) — a conservative proxy for "the gap ahead has collapsed".
///
/// 🔴 **376_p2 correction (found via RUNTIME verification, `mrs_sim` attached
/// — see `_workspace/376_observation_consumer_p2.md`§ "found bug"):** the
/// AGGREGATION across samples is "does AT LEAST ONE sample report a
/// collapse" (`HasCollapsedRay`), NOT "do ALL samples report a collapse".
/// The 376(p1) design required ALL — that is UNREACHABLE for any
/// finite-width corridor, because `mrs_sim/observation_node.cpp`'s actual ray
/// model only collapses rays aligned with the blocked edge's AXIS (within
/// `forward_cone_half_angle_rad` of it); the REMAINING rays in the same fan
/// are "wall rays" that report the corridor's (always-finite, unaffected by
/// blocking) half-width. Requiring every sample to be < `obs_occlusion_range_m`
/// therefore also requires the corridor to be narrower than that threshold —
/// never true (`trajopt.yaml`'s own derivation table picked
/// `obs_occlusion_range_m` specifically to sit ABOVE the scan's collapsed-
/// range floor and BELOW normal wall/open distances, which only makes sense
/// under an "exists" aggregation). Runtime confirmed 0 declarations under
/// the ALL rule; the ANY rule is what the config comment already assumed.
/// It never claims full occlusion from zero samples (과소 보고=안전,
/// `13_interface_contract_p2.md`:149), but it does not claim exact
/// cross-section-width fidelity either way — that is future work.

namespace mrs_trajopt::core
{

/// @brief One control tick's exogenous world observation (`OBS-5` fixed size).
struct WorldObservation
{
    /// Fixed capacity (`OBS-5` ⑴: no per-tick allocation, `CN-19`).
    static constexpr std::size_t kMaxSamples = 64;

    /// Per-sample occupancy in the forward fan, indices `[0, sample_count)`.
    std::array<bool, kMaxSamples> occupied{};

    /// How many of `occupied` are valid this tick (`<= kMaxSamples`).
    std::size_t sample_count = 0;

    /// `OBS-1` ⑴ (non-attribution): true iff this observation is NOT
    /// explained by a declared neighbor corridor or an upstream `V^blk`
    /// keepout region (`357` D24: computed onboard by intersecting the two,
    /// NOT carried by the scan itself). 🔴 [알려진 한계] this build has no
    /// onboard channel that CONSUMES neighbor corridors or `V^blk` keepout
    /// (`355`§8 finding X3), so the producer of this struct always leaves
    /// this at its default `true` — trivially satisfied, not yet a genuine
    /// attribution test. See `mrs_trajopt` 20d "알려진 한계".
    bool unattributed = true;

    /// `OBS-5` ⑵: ordinal age in CONTROL TICKS since this observation was
    /// captured (never seconds — `221-W2`). `0` means captured this tick.
    std::int64_t age_ticks = 0;

    /// `OBS-5` ⑶: has ANY observation ever been captured for this robot?
    /// (Same shape as `TickInput::staged_constraints_fresh` — a presence
    /// bit. Staleness is judged separately, from `age_ticks` against a
    /// configured bound in `IsQ1RawPredicate`.)
    bool fresh = false;
};

/// @brief Has ANY sample in the observation's forward fan collapsed THIS
/// tick? 🔴 376_p2: renamed/re-aggregated from the unreachable "ALL samples"
/// rule (see file doc) — a single collapsed ray IS the "gap ahead collapsed"
/// signal; the other samples in the same fan measure an unrelated quantity
/// (wall distance) that stays finite regardless of block status.
/// @param obs the observation.
/// @return true iff `obs` has at least one sample and some sample in
///         `[0, sample_count)` is occupied (collapsed).
inline bool HasCollapsedRay(const WorldObservation& obs)
{
    if (obs.sample_count == 0)
    {
        return false;  // no samples => cannot claim a collapse (안전 쪽).
    }
    for (std::size_t i = 0; i < obs.sample_count; ++i)
    {
        if (obs.occupied[i])
        {
            return true;
        }
    }
    return false;
}

/// @brief The raw (pre-hysteresis) Q1 predicate for one tick — `OBS-1` ⑴+⑵
/// plus the `OBS-5` staleness guard. Persistence (`OBS-1` ⑶) is
/// `DeclarationLedger`'s job, not this function's.
/// @param obs the observation.
/// @param max_age_ticks the staleness bound (`Δk^obs_max`, config,
///        `[값 부재]`). A stale observation can never confirm Q1 (과소
///        보고=안전, `13_interface_contract_p2.md`:149).
/// @return true iff `obs.fresh`, not stale (`age_ticks <= max_age_ticks`),
///         `obs.unattributed`, and some ray in the forward fan has collapsed.
inline bool IsQ1RawPredicate(const WorldObservation& obs, std::int64_t max_age_ticks)
{
    if (!obs.fresh)
    {
        return false;
    }
    if (obs.age_ticks > max_age_ticks)
    {
        return false;  // stale => cannot confirm (OBS-5, safety-side).
    }
    return obs.unattributed && HasCollapsedRay(obs);
}

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_WORLD_OBSERVATION_HPP
