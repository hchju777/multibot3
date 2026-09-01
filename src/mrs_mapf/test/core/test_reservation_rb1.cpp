// SPDX-License-Identifier: Apache-2.0
#include "../fixtures.hpp"
#include "../test_util.hpp"
#include "mrs_mapf/core/reservation.hpp"

using namespace mrs_mapf::core;

/// @file test_reservation_rb1.cpp
/// @brief `RB-1` — 🔴 함정 ④: the tick is [값 부재]; this test proves the
/// unquantized path never invents a number, and (when a tick IS supplied to
/// the pure function directly, exercising the math) the outward-rounding
/// invariant `reserved ⊇ nominal` holds with a per-segment error bounded by one
/// tick REGARDLESS of path length (the "총 상수, 방문 수 무관" claim, §255-5-3).

int main()
{
    Roadmap rm = mrs_mapf::test::make_line_roadmap();  // A-B-C-D, 5.0 m each edge.
    std::vector<StepEntry> steps = {{0, "A"}, {1, "B"}, {2, "C"}, {3, "D"}};

    // --- exact_arrival_schedule: correct cumulative sum, origin "now" = 0 ---
    {
        std::vector<double> arrival = exact_arrival_schedule(steps, rm, 1.0);
        CHECK(arrival.size() == 4);
        CHECK_NEAR(arrival[0], 0.0, 1e-9);
        CHECK_NEAR(arrival[1], 5.0, 1e-9);
        CHECK_NEAR(arrival[2], 10.0, 1e-9);
        CHECK_NEAR(arrival[3], 15.0, 1e-9);
    }

    // --- no tick supplied => unquantized, no value invented ---
    {
        ReservedInterval r = reserve_interval(1.23, 4.56, std::nullopt);
        CHECK(!r.quantized);
        CHECK_NEAR(r.reserved_entry_s, 1.23, 1e-9);
        CHECK_NEAR(r.reserved_exit_s, 4.56, 1e-9);
    }

    // --- tick supplied (as a pure-math exercise, not a module default) =>
    //     entry floors down, exit ceils up, reserved ⊇ nominal ---
    {
        const double tick = 0.5;
        ReservedInterval r = reserve_interval(1.1, 4.3, tick);
        CHECK(r.quantized);
        CHECK_NEAR(r.reserved_entry_s, 1.0, 1e-9);  // floor(1.1/0.5)*0.5 = 1.0
        CHECK_NEAR(r.reserved_exit_s, 4.5, 1e-9);   // ceil(4.3/0.5)*0.5 = 4.5
        CHECK(r.reserved_entry_s <= r.nominal_entry_s + 1e-9);
        CHECK(r.reserved_exit_s >= r.nominal_exit_s - 1e-9);
        // error bound: <= 1 tick on each endpoint.
        CHECK(r.nominal_entry_s - r.reserved_entry_s <= tick + 1e-9);
        CHECK(r.reserved_exit_s - r.nominal_exit_s <= tick + 1e-9);
    }

    // --- reconstruct_reservations: exact cumsum FIRST, endpoints rounded only
    //     at the very end — error does NOT grow with path length (a long,
    //     4-segment path still has <= 1 tick error per endpoint) ---
    {
        const double tick = 0.7;
        auto reservations = reconstruct_reservations(steps, rm, 1.0, tick);
        CHECK(reservations.size() == 3);
        for (const auto& r : reservations)
        {
            CHECK(r.quantized);
            CHECK(r.nominal_entry_s - r.reserved_entry_s <= tick + 1e-9);
            CHECK(r.reserved_exit_s - r.nominal_exit_s <= tick + 1e-9);
            CHECK(r.reserved_entry_s <= r.nominal_entry_s + 1e-9);
            CHECK(r.reserved_exit_s >= r.nominal_exit_s - 1e-9);
        }
        // The nominal endpoints themselves are the EXACT cumulative sum
        // (15.0 m total / 1.0 m/s), not a sum of already-rounded segments —
        // the "잉여 누적" this file's header docs forbid.
        CHECK_NEAR(reservations.back().nominal_exit_s, 15.0, 1e-9);
    }

    // --- unquantized reconstruction (tick absent) — the [값 부재] default path ---
    {
        auto reservations = reconstruct_reservations(steps, rm, 1.0, std::nullopt);
        CHECK(reservations.size() == 3);
        for (const auto& r : reservations)
        {
            CHECK(!r.quantized);
        }
    }

    return mapf_test::summary();
}
