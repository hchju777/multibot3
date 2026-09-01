// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/adapter/plan_adapter.hpp"

namespace mrs_sadg::adapter
{

core::Status PlanAdapter::from_boundary(const BoundaryDiscretePlan& msg, core::PlanView& out)
{
    if (msg.schema != BoundaryDiscretePlan::kSchema ||
        msg.schema_version != BoundaryDiscretePlan::kSchemaVersion)
    {
        return core::Status::kError;  // envelope mismatch — not this consumer's document.
    }
    out = core::PlanView{};
    out.instance_id = msg.instance_id;
    out.plan_revision = msg.plan_revision;
    for (const auto& p : msg.plans)
    {
        core::RobotSteps r;
        r.robot = p.robot;
        r.terminal = p.terminal;
        // steps are index-ordered on the wire; preserve given order (io keeps it sorted).
        for (const auto& s : p.steps)
        {
            r.locations.push_back(s.location);
        }
        out.robots.push_back(r);
    }
    for (const auto& v : msg.visit_order)
    {
        core::VisitEntry e;
        e.location = v.location;
        for (const auto& it : v.sequence)
        {
            e.sequence.push_back({it.robot, it.index});
        }
        out.visit_order.push_back(e);
    }
    return core::Status::kOk;
}

core::Status PlanAdapter::to_boundary(const core::PlanView& view, BoundaryDiscretePlan& out)
{
    out = BoundaryDiscretePlan{};
    out.instance_id = view.instance_id;
    out.plan_revision = view.plan_revision;
    for (const auto& r : view.robots)
    {
        BoundaryRobotPlan p;
        p.robot = r.robot;
        p.terminal = r.terminal;
        for (std::size_t k = 0; k < r.locations.size(); ++k)
        {
            BoundaryStep s;
            s.index = static_cast<std::uint32_t>(k);
            s.location = r.locations[k];
            p.steps.push_back(s);
        }
        out.plans.push_back(p);
    }
    for (const auto& e : view.visit_order)
    {
        BoundaryVisitOrderEntry v;
        v.location = e.location;
        for (const auto& it : e.sequence)
        {
            v.sequence.push_back({it.robot, it.index});
        }
        out.visit_order.push_back(v);
    }
    return core::Status::kOk;
}

}  // namespace mrs_sadg::adapter
