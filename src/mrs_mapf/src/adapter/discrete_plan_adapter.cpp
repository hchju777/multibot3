// SPDX-License-Identifier: Apache-2.0
#include "mrs_mapf/adapter/discrete_plan_adapter.hpp"

namespace mrs_mapf::adapter
{
using mrs_mapf::core::Status;

namespace
{

std::string terminal_to_string(core::Terminal t)
{
    switch (t)
    {
        case core::Terminal::kGoalReached:
            return "goal_reached";
        case core::Terminal::kPlanTruncated:
            return "plan_truncated";
        case core::Terminal::kNoProgress:
        default:
            return "no_progress";
    }
}

core::Terminal terminal_from_string(const std::string& s)
{
    if (s == "goal_reached")
    {
        return core::Terminal::kGoalReached;
    }
    if (s == "plan_truncated")
    {
        return core::Terminal::kPlanTruncated;
    }
    return core::Terminal::kNoProgress;
}

}  // namespace

Status DiscretePlanAdapter::to_boundary(const core::DiscretePlanDraft& draft,
                                        BoundaryDiscretePlan& out)
{
    out = BoundaryDiscretePlan{};
    out.schema = BoundaryDiscretePlan::kSchema;
    out.schema_version = BoundaryDiscretePlan::kSchemaVersion;
    out.instance_id = draft.instance_id;
    out.plan_revision = draft.plan_revision;

    for (const auto& p : draft.plans)
    {
        BoundaryRobotPlan bp;
        bp.robot = p.robot;
        bp.terminal = terminal_to_string(p.terminal);
        for (const auto& s : p.steps)
        {
            BoundaryStep bs;
            bs.index = s.index;
            bs.location = s.location;
            bp.steps.push_back(bs);
        }
        out.plans.push_back(bp);
    }

    for (const auto& v : draft.visit_order)
    {
        BoundaryVisitOrderEntry bv;
        bv.location = v.location;
        for (const auto& it : v.sequence)
        {
            BoundaryVisitItem bi;
            bi.robot = it.robot;
            bi.index = it.index;
            bv.sequence.push_back(bi);
        }
        out.visit_order.push_back(bv);
    }
    return Status::kOk;
}

Status DiscretePlanAdapter::from_boundary(const BoundaryDiscretePlan& doc,
                                          core::DiscretePlanDraft& out)
{
    out = core::DiscretePlanDraft{};
    if (doc.schema != BoundaryDiscretePlan::kSchema ||
        doc.schema_version != BoundaryDiscretePlan::kSchemaVersion)
    {
        return Status::kError;  // 봉투 불일치 — 함정 ③, 옛 판본을 오늘의 술어로 재지 않는다.
    }
    out.instance_id = doc.instance_id;
    out.plan_revision = doc.plan_revision;

    for (const auto& p : doc.plans)
    {
        core::RobotPlan rp;
        rp.robot = p.robot;
        rp.terminal = terminal_from_string(p.terminal);
        for (const auto& s : p.steps)
        {
            core::StepEntry se;
            se.index = s.index;
            se.location = s.location;
            rp.steps.push_back(se);
        }
        out.plans.push_back(rp);
    }

    for (const auto& v : doc.visit_order)
    {
        core::VisitOrderEntry ve;
        ve.location = v.location;
        for (const auto& it : v.sequence)
        {
            core::VisitItem vi;
            vi.robot = it.robot;
            vi.index = it.index;
            ve.sequence.push_back(vi);
        }
        out.visit_order.push_back(ve);
    }
    return Status::kOk;
}

}  // namespace mrs_mapf::adapter
