// SPDX-License-Identifier: Apache-2.0
#include "../test_util.hpp"
#include "mrs_trajopt/core/declaration_ledger.hpp"

using namespace mrs_trajopt::core;

int main()
{
    // --- OBS7-NZ (O1): n^open consecutive true ticks are required to open. ---
    {
        DeclarationLedger ledger(DeclarationLedgerConfig{/*n_open=*/3,
                                                         /*n_close=*/2,
                                                         /*n_hold=*/1});
        CHECK(!ledger.update(true));  // 1/3.
        CHECK(!ledger.update(true));  // 2/3.
        CHECK(!ledger.confirmed_open());
        CHECK(ledger.update(true));  // 3/3 => confirmed.
        CHECK(ledger.confirmed_open());
    }

    // --- A broken run resets the open counter (no partial credit). ---
    {
        DeclarationLedger ledger(DeclarationLedgerConfig{3, 2, 1});
        CHECK(!ledger.update(true));   // 1/3.
        CHECK(!ledger.update(false));  // resets.
        CHECK(!ledger.update(true));   // 1/3 again.
        CHECK(!ledger.update(true));   // 2/3.
        CHECK(!ledger.confirmed_open());
    }

    // --- OBS7-NZ (O2): minimum hold blocks a premature close, even after
    // n^close consecutive false ticks. 364_p2's correction: n_hold is what
    // the non-Zeno bound rests on, NOT the open/close asymmetry. ---
    {
        DeclarationLedger ledger(DeclarationLedgerConfig{/*n_open=*/1,
                                                         /*n_close=*/1,
                                                         /*n_hold=*/3});
        CHECK(ledger.update(true));  // opens immediately (n_open=1).
        CHECK(ledger.confirmed_open());
        // n_close=1 would close on the very next false tick if not for the
        // hold — verify it does NOT close before n_hold ticks have elapsed.
        CHECK(ledger.update(false));  // ticks_since_open=1 < n_hold=3.
        CHECK(ledger.confirmed_open());
        CHECK(ledger.update(false));  // ticks_since_open=2 < 3.
        CHECK(ledger.confirmed_open());
        CHECK(!ledger.update(false));  // ticks_since_open=3 >= 3 AND close_run>=1 => closes.
        CHECK(!ledger.confirmed_open());
    }

    // --- Close requires n^close consecutive false ticks AFTER the hold. ---
    {
        DeclarationLedger ledger(DeclarationLedgerConfig{/*n_open=*/1,
                                                         /*n_close=*/2,
                                                         /*n_hold=*/1});
        CHECK(ledger.update(true));   // opens.
        CHECK(ledger.update(false));  // hold elapsed (1>=1), but close_run=1 < n_close=2.
        CHECK(ledger.confirmed_open());
        CHECK(!ledger.update(false));  // close_run=2 >= 2 => closes.
        CHECK(!ledger.confirmed_open());
    }

    // --- A true tick after opening does not close (resets close_run). ---
    {
        DeclarationLedger ledger(DeclarationLedgerConfig{1, 2, 1});
        CHECK(ledger.update(true));
        CHECK(ledger.update(false));  // close_run=1.
        CHECK(ledger.update(true));   // interrupts close_run => resets to 0.
        CHECK(ledger.confirmed_open());
        CHECK(ledger.update(false));  // close_run=1 again (not 2).
        CHECK(ledger.confirmed_open());
    }

    return trajopt_test::summary();
}
