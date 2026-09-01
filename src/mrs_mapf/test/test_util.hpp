// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MAPF_TEST_TEST_UTIL_HPP
#define MRS_MAPF_TEST_TEST_UTIL_HPP

#include <cmath>
#include <cstdio>
#include <string>

/// @file test_util.hpp
/// @brief Minimal self-contained test harness (no gtest dependency — same
/// pattern as `mrs_trajopt/test/test_util.hpp`).
///
/// Each test is a `main` that calls CHECK/CHECK_NEAR and ends with
/// `return mapf_test::summary();`. A nonzero return fails the CTest case.

namespace mapf_test
{

inline int& failures()
{
    static int n = 0;
    return n;
}

inline void report(bool cond, const char* expr, const char* file, int line)
{
    if (!cond)
    {
        ++failures();
        std::printf("  FAIL %s:%d  %s\n", file, line, expr);
    }
}

inline void report_near(double a,
                        double b,
                        double tol,
                        const char* expr,
                        const char* file,
                        int line)
{
    if (!(std::fabs(a - b) <= tol))
    {
        ++failures();
        std::printf("  FAIL %s:%d  %s  (|%.9g - %.9g| = %.3g > %.3g)\n",
                    file,
                    line,
                    expr,
                    a,
                    b,
                    std::fabs(a - b),
                    tol);
    }
}

inline int summary()
{
    if (failures() == 0)
    {
        std::printf("  ok\n");
        return 0;
    }
    std::printf("  %d check(s) failed\n", failures());
    return 1;
}

}  // namespace mapf_test

#define CHECK(cond) ::mapf_test::report((cond), #cond, __FILE__, __LINE__)
#define CHECK_NEAR(a, b, tol) \
    ::mapf_test::report_near((a), (b), (tol), #a " ~= " #b, __FILE__, __LINE__)

#endif  // MRS_MAPF_TEST_TEST_UTIL_HPP
