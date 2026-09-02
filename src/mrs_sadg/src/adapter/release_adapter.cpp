// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/adapter/release_adapter.hpp"

namespace mrs_sadg::adapter
{

core::Status ReleaseAdapter::to_boundary(const core::ExecutionGraph& g,
                                         const std::string& robot,
                                         BoundarySegmentRelease& out)
{
    out.schema = BoundarySegmentRelease::kSchema;
    out.schema_version = BoundarySegmentRelease::kSchemaVersion;
    out.robot = robot;
    out.segments.clear();
    for (const auto& seg : g.segments())
    {
        if (seg.robot != robot)
        {
            continue;
        }
        BoundarySegmentReleaseItem item;
        item.id = seg.id;
        item.released = g.is_released(seg.id);
        out.segments.push_back(item);
    }
    return core::Status::kOk;
}

}  // namespace mrs_sadg::adapter
