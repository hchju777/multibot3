// SPDX-License-Identifier: Apache-2.0
#include "../fixtures.hpp"
#include "../test_util.hpp"
#include "mrs_mapf/core/visit_order.hpp"

using namespace mrs_mapf::core;

/// @file test_visit_order.cpp
/// @brief `visit_order` construction: the "items >= 2" filter (not "robots >=
/// 2" — a robot's own revisit counts), the contract tie-break (robot id UTF-8
/// ascending, then that robot's index ascending) when nominal times coincide,
/// and consumption point ② (§255-4-3): a received `ordering_constraints`
/// prefix is emitted first, verbatim, with new visits appended after.

int main()
{
    Roadmap rm = mrs_mapf::test::make_line_roadmap();
    std::map<std::string, double> speed = {{"a", 1.0}, {"b", 1.0}};

    // --- items >= 2 filter: a robot's own revisit is listed even alone ---
    {
        RobotPlan p;
        p.robot = "a";
        p.terminal = Terminal::kGoalReached;
        p.steps = {{0, "A"}, {1, "B"}, {2, "A"}};  // revisits A.
        auto vo = build_visit_order({p}, speed, rm, {});
        bool found = false;
        for (const auto& e : vo)
        {
            if (e.location == "A")
            {
                found = true;
                CHECK(e.sequence.size() == 2);
            }
        }
        CHECK(found);
        // B was visited once — must NOT appear.
        for (const auto& e : vo)
        {
            CHECK(e.location != "B");
        }
    }

    // --- tie-break when nominal times coincide: robot id ascending, then index ---
    {
        // Both robots start AT A with a 0-length first step is not representable
        // (folding rule) — instead, engineer a genuine simultaneous arrival: two
        // robots of equal speed converging on B from symmetric distances is not
        // directly constructible on this line roadmap, so we test the
        // comparator's documented behavior directly via two robots visiting the
        // SAME location at index 0 (both starts) — the canonical simultaneous
        // case (steps[0] is a visit per contract 4.0.0, and both start at the
        // same instant t=0).
        RobotPlan pb;
        pb.robot = "b";
        pb.terminal = Terminal::kGoalReached;
        pb.steps = {{0, "A"}, {1, "B"}};
        RobotPlan pa;
        pa.robot = "a";
        pa.terminal = Terminal::kGoalReached;
        pa.steps = {{0, "A"}, {1, "B"}};  // only steps[0]=A matters to this assertion.
        auto vo = build_visit_order({pb, pa}, speed, rm, {});
        for (const auto& e : vo)
        {
            if (e.location == "A")
            {
                CHECK(e.sequence.size() == 2);
                CHECK(e.sequence[0].robot == "a");  // "a" < "b" (UTF-8 codepoint ascending).
                CHECK(e.sequence[1].robot == "b");
            }
        }
    }

    // --- consumption point ②: received prefix emitted first, verbatim ---
    {
        RobotPlan pa;
        pa.robot = "a";
        pa.terminal = Terminal::kGoalReached;
        pa.steps = {{0, "A"}, {1, "B"}, {2, "C"}};
        RobotPlan pb;
        pb.robot = "b";
        pb.terminal = Terminal::kGoalReached;
        pb.steps = {{0, "D"}, {1, "C"}, {2, "B"}};
        OrderingConstraint oc;
        oc.location = "B";
        oc.sequence = {{"b", 2}};  // "b already realized its visit to B" — must lead.
        auto vo = build_visit_order({pa, pb}, speed, rm, {oc});
        bool found = false;
        for (const auto& e : vo)
        {
            if (e.location == "B")
            {
                found = true;
                CHECK(!e.sequence.empty());
                CHECK(e.sequence.front().robot == "b");
                CHECK(e.sequence.front().index == 2);
            }
        }
        CHECK(found);
    }

    return mapf_test::summary();
}
