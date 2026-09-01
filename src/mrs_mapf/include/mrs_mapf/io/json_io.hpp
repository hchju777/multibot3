// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_IO_JSON_IO_HPP
#define MRS_MAPF_IO_JSON_IO_HPP

#include <string>

#include "mrs_mapf/adapter/boundary_types.hpp"

/// @file json_io.hpp
/// @brief Hand-written JSON (de)serialization for boundary artifacts.
///
/// Kept OUT of `core/` and `adapter/` (CN-2/CN-3): only this `io/` target
/// depends on string/JSON assembly, and `io/` does not include `core/` (CN-1
/// layer-layout check ⑸). Produces documents that satisfy the frozen schemas
/// exactly (`additionalProperties:false` — no extra keys emitted).

namespace mrs_mapf::io
{

/// @brief Serialize a boundary discrete_plan document to schema JSON.
/// @param doc the boundary discrete_plan document.
/// @return a JSON string matching mrs.discrete_plan 4.0.0.
std::string to_json(const adapter::BoundaryDiscretePlan& doc);

/// @brief Parse a boundary discrete_plan document from JSON text.
/// @param text JSON text of an mrs.discrete_plan document (any envelope — the
///   caller/adapter judges version compatibility, not this parser).
/// @return the parsed boundary discrete_plan document.
/// @throws std::runtime_error on malformed input.
adapter::BoundaryDiscretePlan parse_discrete_plan(const std::string& text);

/// @brief Parse a boundary roadmap document from JSON text.
/// @param text JSON text of an mrs.roadmap 3.0.0 document.
/// @return the parsed boundary roadmap document.
/// @throws std::runtime_error on malformed input.
adapter::BoundaryRoadmap parse_roadmap(const std::string& text);

/// @brief Parse a boundary assignment document from JSON text.
/// @param text JSON text of an mrs.assignment 2.0.0 document.
/// @return the parsed boundary assignment document.
/// @throws std::runtime_error on malformed input.
adapter::BoundaryAssignment parse_assignment(const std::string& text);

/// @brief Parse a boundary replan_request document from JSON text.
/// @param text JSON text of an mrs.replan_request 7.0.0 document.
/// @return the parsed boundary replan_request document.
/// @throws std::runtime_error on malformed input.
adapter::BoundaryReplanRequest parse_replan_request(const std::string& text);

/// @brief Parse a boundary robot_specs document from JSON text.
/// @param text JSON text of an mrs.robot_specs 1.1.0 document.
/// @return the parsed boundary robot_specs document.
/// @throws std::runtime_error on malformed input.
adapter::BoundaryRobotSpecs parse_robot_specs(const std::string& text);

}  // namespace mrs_mapf::io

#endif  // MRS_MAPF_IO_JSON_IO_HPP
