// SPDX-License-Identifier: Apache-2.0
#include "mrs_mrta/adapter/operator_command_adapter.hpp"

namespace mrs_mrta::adapter
{

core::Status OperatorCommandAdapter::from_boundary(const BoundaryOperatorCommand& msg,
                                                   core::OperatorEvent& out)
{
    if (msg.verb != "goal_override")
    {
        return core::Status::kError;
    }
    if (msg.active && msg.goal_location.empty())
    {
        // "" is the wire absence sentinel (schema §7-2, corridor:null 관례) —
        // not a distinct "unset" flag. Endpoints are "L##"-shaped and never
        // collide with "" (F46-1 fix).
        return core::Status::kError;
    }

    out.robot = msg.robot;
    out.active = msg.active;
    out.goal_location = msg.active ? msg.goal_location : std::string{};
    out.seq = msg.seq;
    return core::Status::kOk;
}

}  // namespace mrs_mrta::adapter
