// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/plugins/alt_adjacent_swap.hpp"

#include <string>

namespace mrs_sadg::plugins
{

namespace
{

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

core::GeneratorCapabilities AltAdjacentSwap::capabilities() const
{
    core::GeneratorCapabilities c;
    c.heads_complete = false;
    c.preserves_intra_robot_order = true;
    c.max_alternatives_per_group = cfg_.max_alternatives_per_group;
    return c;
}

core::Result<std::uint32_t, core::CompileError> AltAdjacentSwap::generate(
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
    // Adjacent pairs — identical contract mapping to the committed order (the
    // checker's seam (c) is a consecutive-pair mapping, so this is the required
    // committed representation).
    for (std::size_t k = 0; k + 1 < seq.size(); ++k)
    {
        const auto& pred = seq[k];
        const auto& succ = seq[k + 1];
        if (pred.robot == succ.robot || succ.index == 0)
        {
            continue;
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
        slot.opposing = (st->from == sf->to);
        out.push_back(slot);
        ++n;
    }
    return core::Result<std::uint32_t, core::CompileError>::ok_value(n);
}

std::string_view AltAdjacentSwap::key() const
{
    return "mrs_sadg/AltAdjacentSwap";
}

}  // namespace mrs_sadg::plugins
