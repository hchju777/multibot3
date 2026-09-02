// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/adapter/stop_declarations_adapter.hpp"

namespace mrs_sadg::adapter
{

core::Status StopDeclarationsAdapter::from_boundary(
    const BoundaryStopDeclarations& msg,
    std::vector<std::pair<std::string, std::string>>& out)
{
    if (msg.schema != BoundaryStopDeclarations::kSchema ||
        msg.schema_version != BoundaryStopDeclarations::kSchemaVersion)
    {
        return core::Status::kError;  // envelope mismatch — not this consumer's document.
    }
    for (const auto& d : msg.declarations)
    {
        if (d.declared)
        {
            out.emplace_back(d.blocked_from, d.blocked_to);
        }
        // declared:false: no removal API — see file-doc scope note.
    }
    return core::Status::kOk;
}

}  // namespace mrs_sadg::adapter
