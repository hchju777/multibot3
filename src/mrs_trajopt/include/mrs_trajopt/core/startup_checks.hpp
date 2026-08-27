// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_STARTUP_CHECKS_HPP
#define MRS_TRAJOPT_CORE_STARTUP_CHECKS_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "mrs_trajopt/core/status.hpp"
#include "mrs_trajopt/core/types.hpp"

/// @file startup_checks.hpp
/// @brief Startup checks SU00-SU13 (322 §322-5). Run once, on latched inputs.
///
/// Throws ContractViolation (CN-15) on refusal — startup path only. Covers:
///   SU01/SU02 periods >= 1 (a zero period makes tick_seq % 0 UB),
///   SU03-SU05 budgets >= 1,
///   SU06 forbid an independent eta_slf config key (it is derived, ▲7),
///   SU08 the six robot-scoped fields present per robot,
///   SU11 d_sep_min >= 2 * min circumradius (C-41, pose-free non-collision).
/// SU09/SU12 (A33 consumption enforcement) is a runtime-path check approximated
/// elsewhere — noted as [미확인] and not asserted here.

namespace mrs_trajopt::core
{

/// @brief Trajopt algorithm parameters (config/trajopt.yaml, method.modules.trajopt.*).
struct TrajoptConfig
{
    std::int64_t traj_replan_period_ticks = 0;          ///< n^traj (SU01).
    std::int64_t subgoal_replan_period_trajcycles = 0;  ///< n^re (SU02).
    std::int64_t round_cap_budget = 0;                  ///< SU03.
    std::int64_t backtrack_budget = 0;                  ///< SU04.
    std::int64_t peer_board_rounds_max = 0;             ///< SU05.
    /// All config key names present under method.modules.trajopt.* — SU06 scans
    /// this for forbidden eta_slf / self_trigger* keys.
    std::vector<std::string> config_key_names;
};

/// @brief Runs the startup checks once against latched inputs.
class StartupChecks
{
public:
    /// @brief Run SU00-SU13. Throws ContractViolation on the first refusal.
    /// @param specs per-robot limits (the six fields must be present/valid).
    /// @param fleet fleet-folded limits (min_separation_m for SU11).
    /// @param cfg trajopt config (periods, budgets, key names).
    /// @return Status::kOk if all checks pass.
    /// @throws ContractViolation on any refusal (period 0, missing field, etc.).
    static Status run(const std::vector<RobotLimits>& specs,
                      const FleetLimits& fleet,
                      const TrajoptConfig& cfg);

private:
    static bool is_forbidden_eta_key(const std::string& key);
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_STARTUP_CHECKS_HPP
