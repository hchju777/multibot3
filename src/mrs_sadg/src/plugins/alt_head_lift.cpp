// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/plugins/alt_head_lift.hpp"

#include <string>

namespace mrs_sadg::plugins
{

namespace
{

/// @brief Find a segment by id in a segment vector (nullptr if absent).
const core::Segment* find(const std::vector<core::Segment>& segs, const core::SegmentId& id)
{
    for (const auto& s : segs)
    {
        if (s.id == id)
        {
            return &s;
        }
    }
    return nullptr;
}

}  // namespace

core::GeneratorCapabilities AltHeadLift::capabilities() const
{
    core::GeneratorCapabilities c;
    c.heads_complete = true;
    c.preserves_intra_robot_order = true;
    c.max_alternatives_per_group = cfg_.max_alternatives_per_group;
    return c;
}

core::Result<std::uint32_t, core::CompileError> AltHeadLift::generate(
    const core::VisitOrderView& view,
    const std::vector<core::Segment>& segments,
    std::vector<core::AlternativeSlot>& out) const
{
    if (view.entry == nullptr)
    {
        return core::Result<std::uint32_t, core::CompileError>::err(
            core::CompileError::kMalformedSequence);
    }
    const auto& seq = view.entry->sequence;
    std::uint32_t n = 0;
    for (std::size_t k = 0; k + 1 < seq.size(); ++k)
    {
        const auto& pred = seq[k];
        const auto& succ = seq[k + 1];
        if (pred.robot == succ.robot)
        {
            continue;  // revisit — sequential handles it.
        }
        if (succ.index == 0)
        {
            continue;  // successor at its start vertex — not an ordering.
        }
        core::SegmentId from = pred.robot + "#" + std::to_string(pred.index);
        core::SegmentId to = succ.robot + "#" + std::to_string(succ.index - 1);
        const core::Segment* sf = find(segments, from);
        const core::Segment* st = find(segments, to);
        if (sf == nullptr || st == nullptr)
        {
            return core::Result<std::uint32_t, core::CompileError>::err(
                core::CompileError::kMalformedSequence);
        }
        core::AlternativeSlot slot;
        slot.dep.from = from;
        slot.dep.to = to;
        slot.dep.type = core::DepType::kOrdering;
        slot.opposing = (st->from == sf->to);  // ［D5］ opposing predicate.
        out.push_back(slot);
        ++n;
    }
    return core::Result<std::uint32_t, core::CompileError>::ok_value(n);
}

std::string_view AltHeadLift::key() const
{
    return "mrs_sadg/AltHeadLift";
}

}  // namespace mrs_sadg::plugins
