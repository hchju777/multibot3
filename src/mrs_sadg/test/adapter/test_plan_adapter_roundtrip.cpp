// SPDX-License-Identifier: Apache-2.0
#include "../fixtures.hpp"
#include "../test_util.hpp"
#include "mrs_sadg/adapter/plan_adapter.hpp"
#include "mrs_sadg/io/json_io.hpp"

using namespace mrs_sadg::core;
using namespace mrs_sadg::adapter;

/// @file test_plan_adapter_roundtrip.cpp
/// @brief Adapter round-trip: PlanView -> boundary -> JSON -> boundary ->
/// PlanView preserves meaning (steps, terminals, visit order).

int main()
{
    const PlanView original = mrs_sadg::test::make_following_plan();

    BoundaryDiscretePlan doc;
    CHECK(ok(PlanAdapter::to_boundary(original, doc)));
    const std::string json = mrs_sadg::io::to_json(doc);
    const BoundaryDiscretePlan parsed = mrs_sadg::io::parse_discrete_plan(json);

    PlanView back;
    CHECK(ok(PlanAdapter::from_boundary(parsed, back)));

    CHECK(back.instance_id == original.instance_id);
    CHECK(back.robots.size() == original.robots.size());
    for (std::size_t i = 0; i < original.robots.size(); ++i)
    {
        CHECK(back.robots[i].robot == original.robots[i].robot);
        CHECK(back.robots[i].terminal == original.robots[i].terminal);
        CHECK(back.robots[i].locations == original.robots[i].locations);
    }
    CHECK(back.visit_order.size() == original.visit_order.size());
    for (std::size_t i = 0; i < original.visit_order.size(); ++i)
    {
        CHECK(back.visit_order[i].location == original.visit_order[i].location);
        CHECK(back.visit_order[i].sequence.size() == original.visit_order[i].sequence.size());
        for (std::size_t j = 0; j < original.visit_order[i].sequence.size(); ++j)
        {
            CHECK(back.visit_order[i].sequence[j].robot ==
                  original.visit_order[i].sequence[j].robot);
            CHECK(back.visit_order[i].sequence[j].index ==
                  original.visit_order[i].sequence[j].index);
        }
    }

    // Envelope mismatch is rejected (not this consumer's document).
    BoundaryDiscretePlan wrong = doc;
    wrong.schema_version = "3.0.0";
    PlanView ignored;
    CHECK(!ok(PlanAdapter::from_boundary(wrong, ignored)));

    return sadg_test::summary();
}
