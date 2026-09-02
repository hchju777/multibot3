// SPDX-License-Identifier: Apache-2.0
#include "mrs_trajopt/core/startup_checks.hpp"

#include <algorithm>
#include <cctype>

namespace mrs_trajopt::core
{

bool StartupChecks::is_forbidden_eta_key(const std::string& key)
{
    std::string lower;
    lower.reserve(key.size());
    for (char c : key)
    {
        lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
    }
    return lower.find("eta_slf") != std::string::npos ||
           lower.find("self_trigger") != std::string::npos;
}

Status StartupChecks::run(const std::vector<RobotLimits>& specs,
                          const FleetLimits& fleet,
                          const TrajoptConfig& cfg)
{
    // SU01/SU02: periods >= 1 (a zero period makes tick_seq % 0 UB).
    if (cfg.traj_replan_period_ticks < 1)
    {
        throw ContractViolation("SU01: traj_replan_period_ticks (n^traj) must be >= 1");
    }
    if (cfg.subgoal_replan_period_trajcycles < 1)
    {
        throw ContractViolation("SU02: subgoal_replan_period_trajcycles (n^re) must be >= 1");
    }
    // SU03-SU05: budgets >= 1.
    if (cfg.round_cap_budget < 1)
    {
        throw ContractViolation("SU03: round_cap_budget must be >= 1");
    }
    if (cfg.backtrack_budget < 1)
    {
        throw ContractViolation("SU04: backtrack_budget must be >= 1");
    }
    if (cfg.peer_board_rounds_max < 1)
    {
        throw ContractViolation("SU05: peer_board_rounds_max must be >= 1");
    }
    // 🆕 376: exogenous-block observation hysteresis + staleness bound +
    // occlusion-range threshold, all [값 부재] — refused with the same
    // no-baked-default rule as SU01-05 (no SU number assigned here; that
    // registry belongs to 322, not this round — reported, not invented).
    if (cfg.obs_n_open < 1)
    {
        throw ContractViolation("OBS: obs_n_open (n^open, OBS7-NZ O1) must be >= 1");
    }
    if (cfg.obs_n_close < 1)
    {
        throw ContractViolation("OBS: obs_n_close (n^close, OBS7-NZ O1) must be >= 1");
    }
    if (cfg.obs_n_hold < 1)
    {
        throw ContractViolation("OBS: obs_n_hold (n^hold, OBS7-NZ O2) must be >= 1");
    }
    if (cfg.obs_max_age_ticks < 1)
    {
        throw ContractViolation("OBS: obs_max_age_ticks (Δk^obs_max, OBS-5) must be >= 1");
    }
    if (!(cfg.obs_occlusion_range_m > 0.0))
    {
        throw ContractViolation("OBS: obs_occlusion_range_m must be > 0");
    }
    // SU06: no independent eta_slf / self_trigger* config key (▲7). It is a
    // derived value (235-C1); a key would let it drift from eta_lead & n_re.
    for (const auto& key : cfg.config_key_names)
    {
        if (is_forbidden_eta_key(key))
        {
            throw ContractViolation("SU06: forbidden derived-value config key present: " + key);
        }
    }
    // SU08: the six robot-scoped fields present and valid per robot.
    if (specs.empty())
    {
        throw ContractViolation("SU08: robot_specs.robots is empty (minItems 1)");
    }
    for (const auto& r : specs)
    {
        if (r.robot.empty())
        {
            throw ContractViolation("SU08: a robot spec has an empty identifier");
        }
        if (!(r.v_max >= 0.0))
        {
            throw ContractViolation("SU08: v_max invalid for " + r.robot);
        }
        if (!(r.brake_decel_min_mps2 > 0.0))
        {
            throw ContractViolation("SU08: brake_decel_min_mps2 must be > 0 for " + r.robot);
        }
        if (!(r.curvature_max_invm > 0.0))
        {
            throw ContractViolation("SU08: curvature_max_invm must be > 0 for " + r.robot);
        }
        if (!(r.avg_traversal_speed_mps > 0.0))
        {
            throw ContractViolation("SU08: avg_traversal_speed_mps must be > 0 for " + r.robot);
        }
        if (!(r.circumradius_m > 0.0))
        {
            throw ContractViolation("SU08: circumradius_m must be > 0 for " + r.robot);
        }
    }
    // SU11: d_sep_min >= 2 * min circumradius (C-41, pose-free non-collision).
    double min_cir = specs.front().circumradius_m;
    for (const auto& r : specs)
    {
        min_cir = std::min(min_cir, r.circumradius_m);
    }
    if (fleet.min_separation_m < 2.0 * min_cir)
    {
        throw ContractViolation("SU11: min_separation_m < 2 * min circumradius_m (C-41)");
    }
    return Status::kOk;
}

}  // namespace mrs_trajopt::core
