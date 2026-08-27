// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_trajopt/core/safety_monitor.hpp"

using namespace mrs_trajopt::core;

int main()
{
    // Keepout predicate: a sample is NOT stoppable iff x >= 3.0 (a virtual wall).
    auto pred = [](const StateSample& s, double brake_decel) -> bool
    {
        (void)brake_decel;
        return s.x < 3.0;
    };
    SafetyMonitor mon(pred);

    std::vector<StateSample> chain;
    for (int i = 0; i <= 5; ++i)
    {
        StateSample s;
        s.t = static_cast<double>(i);
        s.x = static_cast<double>(i);  // x = 0,1,2,3,4,5.
        chain.push_back(s);
    }

    // Not all stoppable (x=3,4,5 fail).
    CHECK(!mon.remaining_stoppable(chain, 1.0));
    // Last stoppable index is 2 (x=2 < 3, x=3 fails).
    CHECK(mon.last_stoppable_index(chain, 1.0) == 2u);

    // A fully-clear chain is all stoppable, last index = end.
    std::vector<StateSample> clear;
    for (int i = 0; i < 3; ++i)
    {
        StateSample s;
        s.x = static_cast<double>(i) * 0.5;  // 0, 0.5, 1.0 — all < 3.
        clear.push_back(s);
    }
    CHECK(mon.remaining_stoppable(clear, 1.0));
    CHECK(mon.last_stoppable_index(clear, 1.0) == 2u);

    return trajopt_test::summary();
}
