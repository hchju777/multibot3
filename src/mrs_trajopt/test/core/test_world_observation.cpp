// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_trajopt/core/world_observation.hpp"

using namespace mrs_trajopt::core;

namespace
{

/// @brief A fresh observation with `n` samples, none occupied (the
/// realistic "unblocked" reading: wall rays present, but none within
/// `obs_occlusion_range_m` — see `world_observation.hpp` file doc's
/// 376_p2 correction).
WorldObservation clear_obs(std::size_t n = 8)
{
    WorldObservation obs;
    obs.fresh = true;
    obs.age_ticks = 0;
    obs.sample_count = n;
    // occupied[] stays all-false (std::array value-init).
    // unattributed stays at its struct default (true).
    return obs;
}

/// @brief `clear_obs()` with exactly one sample collapsed — the realistic
/// "blocked" reading: only the ray aligned with the blocked edge's axis
/// collapses, the rest (wall rays) do not.
WorldObservation one_collapsed_obs(std::size_t n = 8)
{
    WorldObservation obs = clear_obs(n);
    obs.occupied[3] = true;
    return obs;
}

}  // namespace

int main()
{
    // --- HasCollapsedRay (376_p2: "any", not "all" — see file doc for why) ---
    {
        // No samples => never a collapse (안전 쪽 — cannot claim it).
        WorldObservation obs;
        CHECK(!HasCollapsedRay(obs));
    }
    {
        // All samples clear => no collapse.
        CHECK(!HasCollapsedRay(clear_obs(8)));
    }
    {
        // ONE collapsed sample among many clear ones => a collapse IS
        // detected — this is the realistic "blocked" reading (only the
        // edge-aligned ray collapses; wall rays stay finite).
        CHECK(HasCollapsedRay(one_collapsed_obs(8)));
    }
    {
        // All samples collapsed (a degenerate/stronger case) => still a
        // collapse (the predicate is "exists", so "all" is a subset).
        WorldObservation obs = clear_obs(8);
        for (std::size_t i = 0; i < obs.sample_count; ++i)
        {
            obs.occupied[i] = true;
        }
        CHECK(HasCollapsedRay(obs));
    }

    // --- IsQ1RawPredicate ---
    {
        // Fresh, unattributed, one ray collapsed, within age bound => raw true.
        WorldObservation obs = one_collapsed_obs(8);
        CHECK(IsQ1RawPredicate(obs, /*max_age_ticks=*/5));
    }
    {
        // Never captured (fresh=false, the TickInput default) => never raw
        // true, regardless of occupancy — existing callers that never set
        // TickInput::obs must see no behavior change.
        WorldObservation obs = one_collapsed_obs(8);
        obs.fresh = false;
        CHECK(!IsQ1RawPredicate(obs, 5));
    }
    {
        // Stale (age_ticks > max_age_ticks) => cannot confirm, even with a
        // collapsed ray (OBS-5 과소 보고=안전).
        WorldObservation obs = one_collapsed_obs(8);
        obs.age_ticks = 6;
        CHECK(!IsQ1RawPredicate(obs, /*max_age_ticks=*/5));
        obs.age_ticks = 5;  // exactly at the bound => still confirmable.
        CHECK(IsQ1RawPredicate(obs, 5));
    }
    {
        // Attributed (explained by a neighbor/upstream channel) => never Q1,
        // even with a collapsed ray (OBS-1 ⑴).
        WorldObservation obs = one_collapsed_obs(8);
        obs.unattributed = false;
        CHECK(!IsQ1RawPredicate(obs, 5));
    }
    {
        // No collapse at all (a real, fully open gap) => raw predicate
        // false — the negative-control case the module-developer brief
        // requires: the predicate must NOT fire merely because a scan
        // exists.
        WorldObservation obs = clear_obs(8);
        CHECK(!IsQ1RawPredicate(obs, 5));
    }

    return trajopt_test::summary();
}
