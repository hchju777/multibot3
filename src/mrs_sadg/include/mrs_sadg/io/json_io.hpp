// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_IO_JSON_IO_HPP
#define MRS_SADG_IO_JSON_IO_HPP

#include <string>

#include "mrs_sadg/adapter/boundary_types.hpp"

/// @file json_io.hpp
/// @brief Hand-written JSON (de)serialization for SADG boundary artifacts.
///
/// Kept OUT of `core/` and `adapter/` (CN-1/CN-2/CN-3): only this `io/` target
/// depends on string/JSON assembly, and `io/` does not include `core/` (layer
/// check ⑸). Emits documents matching the frozen schemas exactly
/// (`additionalProperties:false` — no extra keys). The produced
/// execution_constraints is written so that `check_sadg_acyclic.py` passes.

namespace mrs_sadg::io
{

/// @brief Serialize an mrs.execution_constraints 3.0.0 document to schema JSON.
/// @param doc the boundary execution-constraints document.
/// @return a JSON string matching mrs.execution_constraints 3.0.0.
std::string to_json(const adapter::BoundaryExecutionConstraints& doc);

/// @brief Serialize an mrs.discrete_plan 4.0.0 document to schema JSON.
/// @param doc the boundary discrete-plan document.
/// @return a JSON string matching mrs.discrete_plan 4.0.0.
std::string to_json(const adapter::BoundaryDiscretePlan& doc);

/// @brief Parse an mrs.discrete_plan document from JSON text.
/// @param text JSON text of an mrs.discrete_plan document.
/// @return the parsed boundary discrete-plan document.
/// @throws std::runtime_error on malformed input.
adapter::BoundaryDiscretePlan parse_discrete_plan(const std::string& text);

/// @brief Parse an mrs.execution_constraints document from JSON text.
/// @param text JSON text of an mrs.execution_constraints document.
/// @return the parsed boundary execution-constraints document.
/// @throws std::runtime_error on malformed input.
adapter::BoundaryExecutionConstraints parse_execution_constraints(const std::string& text);

/// @brief Serialize an mrs.segment_release 2.0.0 document to schema JSON.
/// @param doc the boundary segment-release document.
/// @return a JSON string matching mrs.segment_release 2.0.0.
std::string to_json(const adapter::BoundarySegmentRelease& doc);

}  // namespace mrs_sadg::io

#endif  // MRS_SADG_IO_JSON_IO_HPP
