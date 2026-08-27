// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_LAYER_CAPABILITIES_HPP
#define MRS_TRAJOPT_CORE_LAYER_CAPABILITIES_HPP

#include <cstdint>

/// @file layer_capabilities.hpp
/// @brief Capability declaration of a trajectory-search cell (321b §3-1).
///
/// A **declaration**, not a config knob and not a boundary field. It states what
/// a cell can do so the service can branch (e.g. whether a cell realizes lateral
/// yield inside the tube, or must stop and escalate).
///
/// 🔴 F-4 [잠정]: `handles_lateral_yield` is placed here provisionally per the
/// order (328 §328-1, 205c §4-2). The **final placement (which class/header)
/// belongs to system-architect** — 328 fixed only that it must exist as a
/// capability declaration and be observable. See 20d "F-4 잠정".

namespace mrs_trajopt::core
{

/// @brief What a trajectory-search cell is capable of.
struct LayerCapabilities
{
    bool supports_warm_start = false;  ///< warm-start across trajectory ticks (TT01 premise).
    bool handles_reverse = false;      ///< can handle reverse=false robots (TT12 path).
    bool emits_reachability_witness = false;  ///< produces the per-alternative reachability bit.
    std::int64_t node_budget_max = 0;         ///< search node budget (pre-allocation, §3-2).

    /// 🔴 F-4 [잠정] Capability: realizes an assigned lateral-yield quota by a
    /// tube-internal lateral maneuver. true for the search cell, false for the
    /// gating cell (which stops and escalates instead). This flag decides
    /// "does the cell take the escalation path", and a verifier checks both
    /// cells share the same escalation path (328 FC-4).
    bool handles_lateral_yield = false;
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_LAYER_CAPABILITIES_HPP
