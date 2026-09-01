// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/adapter/constraints_adapter.hpp"

namespace mrs_sadg::adapter
{

namespace
{

/// @brief Boundary spelling of a DepType.
const char* dep_type_str(core::DepType t)
{
    return t == core::DepType::kSequential ? "sequential" : "ordering";
}

}  // namespace

core::Status ConstraintsAdapter::to_boundary(const core::ExecutionGraph& g,
                                             BoundaryExecutionConstraints& out)
{
    out = BoundaryExecutionConstraints{};
    out.instance_id = g.instance_id();
    out.commit_seq = g.commit_seq();

    for (const auto& s : g.segments())
    {
        BoundarySegment bs;
        bs.id = s.id;
        bs.robot = s.robot;
        bs.from = s.from;
        bs.to = s.to;
        out.segments.push_back(bs);
    }
    for (const auto& d : g.fixed_deps())
    {
        BoundaryDependency bd;
        bd.from = d.from;
        bd.to = d.to;
        bd.type = dep_type_str(d.type);
        out.dependencies.push_back(bd);
    }
    for (const auto& grp : g.switch_groups())
    {
        BoundarySwitchGroup bg;
        bg.id = grp.id;
        bg.location = grp.location;
        for (const auto& alt : grp.alternatives)
        {
            BoundaryAlternative ba;
            ba.id = alt.id;
            for (const auto& d : alt.deps)
            {
                BoundaryDependency bd;
                bd.from = d.from;
                bd.to = d.to;
                bd.type = dep_type_str(d.type);
                ba.dependencies.push_back(bd);
            }
            bg.alternatives.push_back(ba);
        }
        out.switch_groups.push_back(bg);
    }
    return core::Status::kOk;
}

}  // namespace mrs_sadg::adapter
