// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/plugins/layer_branch_bound.hpp"
#include "mrs_sadg/plugins/layer_fixed_order.hpp"
#include "mrs_sadg/service/order_search_service.hpp"

using namespace mrs_sadg::core;
using namespace mrs_sadg::service;
using mrs_sadg::plugins::LayerBranchBound;
using mrs_sadg::plugins::LayerFixedOrder;

/// @file test_order_search.cpp
/// @brief OS## — branch-and-bound finds an improving order under a node budget,
/// falls back to z_prev when the budget is spent with no gain; the fixed-order
/// baseline always returns the original order.

namespace
{

ExecutionGraph make_switch_graph()
{
    ExecutionGraph g;
    g.add_segment({"r1#0", "r1", "S1", "X"});
    g.add_segment({"r1#1", "r1", "X", "G1"});
    g.add_segment({"r2#0", "r2", "S2", "X"});
    g.add_segment({"r2#1", "r2", "X", "G2"});
    g.add_fixed_dep({"r1#0", "r1#1", DepType::kSequential});
    g.add_fixed_dep({"r2#0", "r2#1", DepType::kSequential});
    SwitchGroup grp;
    grp.id = "gX";
    grp.location = "X";
    grp.alternatives = {{"r1_first", {{"r1#1", "r2#0", DepType::kOrdering}}},
                        {"r2_first", {{"r2#1", "r1#0", DepType::kOrdering}}}};
    g.add_switch_group(grp);
    return g;
}

}  // namespace

int main()
{
    // FixedOrder baseline: always the original order (combo all zeros).
    {
        ExecutionGraph g = make_switch_graph();
        LayerFixedOrder layer;
        OrderSearchService svc(layer);
        ReorderRequest req;
        req.graph = &g;
        req.budget.max_expansions = 100;
        auto r = svc.run(req);
        CHECK(r.ok());
        CHECK(r.value().combo == std::vector<std::size_t>{0});
    }

    // BranchBound finds the improving order: with r2's deficit high, ordering r2
    // first (alt 1) lowers the delay proxy J.
    {
        ExecutionGraph g = make_switch_graph();
        g.set_deficit("r2", 10.0);
        g.set_deficit("r1", 1.0);
        LayerBranchBound layer;  // theta_gain defaults to 0 (config-injected in prod).
        OrderSearchService svc(layer);
        ReorderRequest req;
        req.graph = &g;
        req.budget.max_expansions = 100;
        auto r = svc.run(req);
        CHECK(r.ok());
        CHECK(r.value().combo == std::vector<std::size_t>{1});  // r2_first improves.
    }

    // BranchBound falls back to z_prev (SolveError) when the budget is spent with
    // no strict improvement (equal deficits -> no gain).
    {
        ExecutionGraph g = make_switch_graph();
        g.set_deficit("r1", 5.0);
        g.set_deficit("r2", 5.0);
        LayerBranchBound layer;
        OrderSearchService svc(layer);
        ReorderRequest req;
        req.graph = &g;
        req.budget.max_expansions = 1;  // just enough to visit the one non-z0 combo.
        auto r = svc.run(req);
        CHECK(!r.ok());
        CHECK(r.error() == SolveError::kBudgetExhausted);
    }

    return sadg_test::summary();
}
