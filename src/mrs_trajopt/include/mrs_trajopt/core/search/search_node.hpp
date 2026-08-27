// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_SEARCH_SEARCH_NODE_HPP
#define MRS_TRAJOPT_CORE_SEARCH_SEARCH_NODE_HPP

#include "mrs_trajopt/core/types.hpp"

/// @file search_node.hpp
/// @brief SSIPP search node ② (U42-9, RQ5-9). **No speed dimension** — geometry
/// only. Edge speeds and times are analytic; the profiler fills them (TT04).

namespace mrs_trajopt::core::search
{

/// @brief A search node: (position, heading[, reverse]). No velocity (RQ5-9).
struct SearchNode
{
    Pose2 pose;            ///< position + heading (grain ②, 수십 cm).
    bool reverse = false;  ///< true iff reached by a reverse edge (RS transition).
};

}  // namespace mrs_trajopt::core::search

#endif  // MRS_TRAJOPT_CORE_SEARCH_SEARCH_NODE_HPP
