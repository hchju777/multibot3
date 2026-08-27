// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_trajopt/core/round_ledger.hpp"

using namespace mrs_trajopt::core;

int main()
{
    // --- Ledger: fail holds a robot; held robots are skipped; no_fail detects convergence. ---
    {
        RoundLedger led;
        led.reset();
        led.begin_round();
        CHECK(led.no_fail());
        CHECK(!led.is_held("r1"));
        led.fail("r1", FailCause::kEmptyCandidates, OriginCause::kBudgetExhausted);
        CHECK(led.is_held("r1"));
        CHECK(!led.no_fail());
        CHECK(led.fail_records().size() == 1);
        // Origin cause is kept apart from the immediate cause (TR-C1).
        CHECK(led.fail_records()[0].origin == OriginCause::kBudgetExhausted);
        CHECK(led.fail_records()[0].cause == FailCause::kEmptyCandidates);
        // A new round clears the per-round flag but keeps the hold.
        led.begin_round();
        CHECK(led.no_fail());
        CHECK(led.is_held("r1"));
        led.reset();
        CHECK(!led.is_held("r1"));
    }

    // --- YieldHandles: quota is monotone non-decreasing and capped (221-P6). ---
    {
        YieldHandles y(1.0);  // cap = 1.0 m.
        CHECK_NEAR(y.quota_of("r1"), 0.0, 1e-12);
        double a = y.raise_quota("r1", 0.3);
        CHECK_NEAR(a, 0.3, 1e-12);
        double b = y.raise_quota("r1", 0.4);
        CHECK_NEAR(b, 0.7, 1e-12);
        CHECK(b >= a);  // monotone.
        // A negative step never decreases the quota (208-T1 "얹기만").
        double c = y.raise_quota("r1", -5.0);
        CHECK_NEAR(c, 0.7, 1e-12);
        // Raising past the cap clamps to the cap (finite-convergence lever).
        double d = y.raise_quota("r1", 10.0);
        CHECK_NEAR(d, 1.0, 1e-12);
        double e = y.raise_quota("r1", 10.0);
        CHECK_NEAR(e, 1.0, 1e-12);  // stays at cap.
    }

    // --- Finite rounds: monotone quota + cap => a fixed number of raises reaches cap. ---
    {
        YieldHandles y(1.0);
        int rounds = 0;
        const double step = 0.25;
        while (y.quota_of("r2") < y.cap() && rounds < 1000)
        {
            y.raise_quota("r2", step);
            ++rounds;
        }
        // cap 1.0 / step 0.25 => reached in 4 rounds; the loop terminates (finite).
        CHECK(rounds == 4);
        CHECK_NEAR(y.quota_of("r2"), 1.0, 1e-12);
    }

    return trajopt_test::summary();
}
