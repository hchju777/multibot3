// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_ADAPTER_DISCRETE_PLAN_ADAPTER_HPP
#define MRS_MAPF_ADAPTER_DISCRETE_PLAN_ADAPTER_HPP

#include "mrs_mapf/adapter/boundary_types.hpp"
#include "mrs_mapf/core/status.hpp"
#include "mrs_mapf/core/types.hpp"

/// @file discrete_plan_adapter.hpp
/// @brief Converts between the internal `DiscretePlanDraft` and
/// `mrs.discrete_plan` 4.0.0 — the ONE boundary artifact this module both
/// PRODUCES (every publish) and CONSUMES (its own previous publish, for PX09's
/// `baseline` resolution). Both directions are implemented (CN-9), unlike the
/// other four (consume-only) adapters in this package.

namespace mrs_mapf::adapter
{

/// @brief Adapter for the mrs.discrete_plan boundary artifact.
class DiscretePlanAdapter
{
public:
    /// @brief Internal draft -> boundary document. Fills the envelope constants
    /// (`BoundaryDiscretePlan::kSchema`/`kSchemaVersion`).
    /// @param draft the internal draft (already self-checked by the caller).
    /// @param out the boundary document.
    /// @return Status::kOk always (the draft's shape already matches the schema by construction).
    static core::Status to_boundary(const core::DiscretePlanDraft& draft,
                                    BoundaryDiscretePlan& out);

    /// @brief Boundary document -> internal draft (used to read a previously
    /// published plan back in, e.g. for `baseline` resolution).
    /// @param doc the boundary document.
    /// @param out the internal draft.
    /// @return Status::kError if @p doc.schema/@p doc.schema_version do not
    ///   match the envelope constants; Status::kOk otherwise.
    static core::Status from_boundary(const BoundaryDiscretePlan& doc,
                                      core::DiscretePlanDraft& out);
};

}  // namespace mrs_mapf::adapter

#endif  // MRS_MAPF_ADAPTER_DISCRETE_PLAN_ADAPTER_HPP
