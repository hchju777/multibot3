// SPDX-License-Identifier: Apache-2.0
//
// 🔴 STUB — this executable is a SKELETON, not the checker `10_architecture_p2.md`
// §13 describes (C-2/C-3/C-4: edge-length vs min_separation_m, waiting-endpoint
// connectivity W1, node clearance radius W4, required-key presence). Those
// checks read `mrs.roadmap` + `config.yaml`, and `mrs_bringup` (which owns
// `config/`, CN-6) and the roadmap file path do not exist yet in this round
// (P0~P2 only — U47-1, `348`). Building the real checks here would mean
// inventing a roadmap file format/location that is `mrs_bringup`'s decision,
// not this round's.
//
// This binary exists only so the `startup_checker` TARGET is present (D14
// asked for it as one of the "four" P1 items) and so its exit-code contract
// is fixed now: 0 = all checks passed, 1 = a contract violation was found
// (CN-15 — startup path only, no fallback). `21_integration.md` marks this
// stub explicitly; do not treat a green run of THIS binary as evidence that
// C-2/C-3/C-4 hold.
#include <cstdio>

#include "mrs_core/status.hpp"

int main()
{
    std::fprintf(stderr,
                 "startup_checker: STUB (integration round 47, P1) — C-2/C-3/C-4 "
                 "(roadmap vs config.yaml, W1 connectivity, W4 clearance, required-key "
                 "presence) are NOT implemented. Real implementation needs "
                 "mrs_bringup/config (P3, out of this round's scope, U47-1).\n");
    return mrs_core::ok(mrs_core::Status::kOk) ? 0 : 1;
}
