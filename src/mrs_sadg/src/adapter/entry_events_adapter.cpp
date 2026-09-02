// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/adapter/entry_events_adapter.hpp"

namespace mrs_sadg::adapter
{

core::Status EntryEventsAdapter::from_boundary(const BoundaryEntryEvents& msg, EntryEventsView& out)
{
    if (msg.schema != BoundaryEntryEvents::kSchema ||
        msg.schema_version != BoundaryEntryEvents::kSchemaVersion)
    {
        return core::Status::kError;  // envelope mismatch — not this consumer's document.
    }
    for (const auto& e : msg.events)
    {
        if (e.entered)
        {
            out.completed.push_back(e.segment);  // arrival at segment.to (mark_completed).
        }
        else
        {
            out.started.push_back(e.segment);  // departure from segment.from (mark_entered).
        }
        auto& next = out.next_seq_by_robot[e.robot];
        if (e.seq + 1 > next)
        {
            next = e.seq + 1;
        }
    }
    return core::Status::kOk;
}

}  // namespace mrs_sadg::adapter
