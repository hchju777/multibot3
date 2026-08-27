// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_IO_JSON_IO_HPP
#define MRS_TRAJOPT_IO_JSON_IO_HPP

#include <string>

#include "mrs_trajopt/adapter/boundary_types.hpp"

/// @file json_io.hpp
/// @brief Hand-written JSON (de)serialization for boundary artifacts.
///
/// Kept OUT of `core/` and `adapter/` (CN-2/CN-3): only this `io/` target
/// depends on string/JSON assembly. Produces documents that satisfy the frozen
/// schemas exactly (additionalProperties:false — no extra keys emitted).

namespace mrs_trajopt::io
{

/// @brief Serialize a boundary trajectories document to schema JSON.
/// @param doc the boundary trajectories document.
/// @return a JSON string matching mrs.trajectories 2.0.0.
std::string to_json(const adapter::BoundaryTrajectories& doc);

/// @brief Serialize a boundary stop-declaration document to schema JSON.
/// @param doc the boundary stop-declaration document.
/// @return a JSON string matching mrs.stop_declaration 5.0.0.
std::string to_json(const adapter::BoundaryStopDeclarations& doc);

/// @brief Parse a boundary robot_specs document from JSON text.
///
/// Minimal parser — accepts the required seven fields per robot. Throws
/// std::runtime_error on malformed input.
/// @param text JSON text of an mrs.robot_specs 1.1.0 document.
/// @return the parsed boundary robot_specs document.
/// @throws std::runtime_error on parse failure or missing required fields.
adapter::BoundaryRobotSpecs parse_robot_specs(const std::string& text);

}  // namespace mrs_trajopt::io

#endif  // MRS_TRAJOPT_IO_JSON_IO_HPP
