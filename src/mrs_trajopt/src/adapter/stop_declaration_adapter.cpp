// SPDX-License-Identifier: Apache-2.0
#include "mrs_trajopt/adapter/stop_declaration_adapter.hpp"

namespace mrs_trajopt::adapter
{

core::Status StopDeclarationAdapter::to_boundary(const std::vector<InternalDeclaration>& decls,
                                                 const std::string& instance_id,
                                                 BoundaryStopDeclarations& out)
{
    out = BoundaryStopDeclarations{};
    out.instance_id = instance_id;
    out.declarations.reserve(decls.size());
    for (const auto& d : decls)
    {
        BoundaryStopDeclaration bd;
        bd.seq = d.seq;
        bd.robot = d.robot;
        bd.blocked_edge.from = d.blocked_from;
        bd.blocked_edge.to = d.blocked_to;
        bd.declared = d.declared;
        bd.reason = core::DeclarationRegulator::to_schema_string(d.reason);
        out.declarations.push_back(std::move(bd));
    }
    return core::Status::kOk;
}

}  // namespace mrs_trajopt::adapter
