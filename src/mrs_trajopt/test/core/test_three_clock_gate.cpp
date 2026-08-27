// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_trajopt/core/three_clock_gate.hpp"

using mrs_trajopt::core::ThreeClockGate;

int main()
{
    // Nesting: subgoal tick ⊂ trajectory tick ⊂ control tick (RQ5-2).
    ThreeClockGate g(2, 3);  // n^traj=2, n^re=3 => subgoal every 6 control ticks.
    CHECK(g.valid());
    CHECK(g.subgoal_period_ticks() == 6);

    // Trajectory tick fires at multiples of n^traj.
    CHECK(!g.is_trajectory_tick(1));
    CHECK(g.is_trajectory_tick(2));
    CHECK(g.is_trajectory_tick(4));
    CHECK(g.is_trajectory_tick(6));

    // Subgoal tick fires at multiples of n^traj*n^re; always also a trajectory tick.
    CHECK(!g.is_subgoal_tick(2));
    CHECK(!g.is_subgoal_tick(4));
    CHECK(g.is_subgoal_tick(6));
    CHECK(g.is_trajectory_tick(6));  // nesting invariant.
    CHECK(g.is_subgoal_tick(12));
    CHECK(g.is_trajectory_tick(12));

    // Zero period is invalid (SU01/SU02): a startup refusal, never UB.
    ThreeClockGate bad0(0, 3);
    CHECK(!bad0.valid());
    CHECK(!bad0.is_trajectory_tick(4));  // guarded — no tick_seq % 0.
    ThreeClockGate bad1(2, 0);
    CHECK(!bad1.valid());
    CHECK(!bad1.is_subgoal_tick(4));

    // n^traj = n^re = 1: every control tick is also a subgoal tick.
    ThreeClockGate all(1, 1);
    CHECK(all.is_trajectory_tick(1));
    CHECK(all.is_subgoal_tick(1));

    return trajopt_test::summary();
}
