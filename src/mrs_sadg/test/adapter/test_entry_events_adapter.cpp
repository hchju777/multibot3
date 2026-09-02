// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_sadg/adapter/entry_events_adapter.hpp"
#include "mrs_sadg/core/status.hpp"

using namespace mrs_sadg::adapter;

/// @file test_entry_events_adapter.cpp
/// @brief mrs.entry_events 2.0.0 -> {started, completed, next_seq_by_robot}
/// classification (see entry_events_adapter.hpp file doc for the derivation).

int main()
{
    // entered:false (departure, location==segment.from) -> started (mark_entered).
    {
        BoundaryEntryEvents msg;
        msg.events.push_back({0, "r1", "r1#0", "S1", false});
        EntryEventsView view;
        CHECK(mrs_sadg::core::ok(EntryEventsAdapter::from_boundary(msg, view)));
        CHECK(view.started.size() == 1 && view.started[0] == "r1#0");
        CHECK(view.completed.empty());
        CHECK(view.next_seq_by_robot.at("r1") == 1);
    }

    // entered:true (arrival, location==segment.to) -> completed (mark_completed).
    {
        BoundaryEntryEvents msg;
        msg.events.push_back({5, "r2", "r2#1", "G2", true});
        EntryEventsView view;
        CHECK(mrs_sadg::core::ok(EntryEventsAdapter::from_boundary(msg, view)));
        CHECK(view.completed.size() == 1 && view.completed[0] == "r2#1");
        CHECK(view.started.empty());
        CHECK(view.next_seq_by_robot.at("r2") == 6);
    }

    // envelope mismatch -> kError, view untouched.
    {
        BoundaryEntryEvents msg;
        msg.schema_version = "1.0.0";
        msg.events.push_back({0, "r1", "r1#0", "S1", false});
        EntryEventsView view;
        CHECK(!mrs_sadg::core::ok(EntryEventsAdapter::from_boundary(msg, view)));
        CHECK(view.started.empty());
    }

    // accumulation across two calls into the same view (the node drains across
    // several /entry_events messages between ticks) — next_seq_by_robot folds
    // by max, started/completed append.
    {
        EntryEventsView view;
        BoundaryEntryEvents m1;
        m1.events.push_back({0, "r1", "r1#0", "S1", false});
        BoundaryEntryEvents m2;
        m2.events.push_back({1, "r1", "r1#1", "X", false});
        m2.events.push_back({0, "r2", "r2#0", "S2", true});
        CHECK(mrs_sadg::core::ok(EntryEventsAdapter::from_boundary(m1, view)));
        CHECK(mrs_sadg::core::ok(EntryEventsAdapter::from_boundary(m2, view)));
        CHECK(view.started.size() == 2);
        CHECK(view.completed.size() == 1);
        CHECK(view.next_seq_by_robot.at("r1") == 2);
        CHECK(view.next_seq_by_robot.at("r2") == 1);
    }

    return sadg_test::summary();
}
