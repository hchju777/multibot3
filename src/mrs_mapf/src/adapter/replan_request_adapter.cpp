// SPDX-License-Identifier: Apache-2.0
#include "mrs_mapf/adapter/replan_request_adapter.hpp"

namespace mrs_mapf::adapter
{
using mrs_mapf::core::Status;

Status ReplanRequestAdapter::from_boundary(const BoundaryReplanRequest& doc,
                                           ReplanRequestFields& out)
{
    out = ReplanRequestFields{};
    out.reason = doc.reason;
    out.affected_robots = doc.affected_robots;
    out.plan_revision = doc.plan_revision;

    for (const auto& b : doc.blocked)
    {
        core::BlockedEdge e;
        e.from = b.from;
        e.to = b.to;
        out.blocked.push_back(e);
    }
    for (const auto& oc : doc.ordering_constraints)
    {
        core::OrderingConstraint c;
        c.location = oc.location;
        for (const auto& item : oc.sequence)
        {
            core::OrderingPrefixItem it;
            it.robot = item.robot;
            it.index = item.index;
            c.sequence.push_back(it);
        }
        out.ordering_constraints.push_back(c);
    }
    for (const auto& b : doc.baseline)
    {
        core::BaselineEntry e;
        e.robot = b.robot;
        e.committed_segment_count = b.committed_segment_count;
        out.baseline.push_back(e);
    }
    return Status::kOk;
}

}  // namespace mrs_mapf::adapter
