// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_sadg/plugins/policy_constant_rung.hpp"
#include "mrs_sadg/plugins/policy_deficit_ladder.hpp"

using namespace mrs_sadg::core;
using mrs_sadg::plugins::PolicyConstantRung;
using mrs_sadg::plugins::PolicyDeficitLadder;

/// @file test_escalation_policy.cpp
/// @brief EP## — the five ladder rules, each exercised.

namespace
{

DeficitView no_deficit()
{
    DeficitView d;
    d.stall_threshold = 0.0;  // deficits never trip; the trigger is blocked-count.
    return d;
}

BlockedView blocked(std::size_t n)
{
    BlockedView b;
    b.count = n;
    return b;
}

}  // namespace

int main()
{
    // Rule 1 (one step): from kNone with a trigger -> kReorder, never skips to 2.
    {
        PolicyDeficitLadder p;
        CHECK(p.decide(no_deficit(), blocked(1), Rung::kNone) == Rung::kReorder);
    }

    // Rule 2 (no downward assignment): no trigger -> stays at cur.
    {
        PolicyDeficitLadder p;
        CHECK(p.decide(no_deficit(), blocked(0), Rung::kPartialReplan) == Rung::kPartialReplan);
    }

    // Rule 3 (no escalation while pending): after escalating, the same call holds.
    {
        PolicyDeficitLadder p;
        CHECK(p.decide(no_deficit(), blocked(1), Rung::kNone) == Rung::kReorder);
        CHECK(p.decide(no_deficit(), blocked(1), Rung::kReorder) == Rung::kReorder);  // pending.
    }

    // Rule 4 (no duplicate situation): same fingerprint does not re-escalate even
    // after the pending action resolves.
    {
        PolicyDeficitLadder p;
        CHECK(p.decide(no_deficit(), blocked(1), Rung::kNone) == Rung::kReorder);
        p.mark_resolved();
        CHECK(p.decide(no_deficit(), blocked(1), Rung::kReorder) == Rung::kReorder);  // dup sig.
        p.mark_resolved();
        CHECK(p.decide(no_deficit(), blocked(2), Rung::kReorder) ==
              Rung::kPartialReplan);  // new sig escalates.
    }

    // Rule 5 (<= 3 per episode): the fourth distinct escalation is refused;
    // reset_episode re-enables it.
    {
        PolicyDeficitLadder p;
        Rung cur = Rung::kNone;
        cur = p.decide(no_deficit(), blocked(1), cur);  // 1st
        p.mark_resolved();
        cur = p.decide(no_deficit(), blocked(2), cur);  // 2nd
        p.mark_resolved();
        cur = p.decide(no_deficit(), blocked(3), cur);  // 3rd
        p.mark_resolved();
        Rung after = p.decide(no_deficit(), blocked(4), cur);  // 4th refused.
        CHECK(after == cur);
        p.reset_episode();
        // After reset the 3-count clears; a lower cur can escalate one step again.
        CHECK(p.decide(no_deficit(), blocked(4), Rung::kReorder) != Rung::kReorder);
    }

    // Baseline: constant-rung policy never escalates.
    {
        PolicyConstantRung p;
        CHECK(p.decide(no_deficit(), blocked(9), Rung::kNone) == Rung::kNone);
    }

    return sadg_test::summary();
}
