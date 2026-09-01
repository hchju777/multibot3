// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_TEST_TEST_UTIL_HPP
#define MRS_SADG_TEST_TEST_UTIL_HPP

#include <cstdio>

/// @file test_util.hpp
/// @brief Minimal self-contained test harness (no gtest — same pattern as
/// mrs_mapf/mrs_trajopt). Each test is a `main` that ends with
/// `return sadg_test::summary();`. A nonzero return fails the CTest case.

namespace sadg_test
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

}  // namespace sadg_test

#define CHECK(cond) ::sadg_test::report((cond), #cond, __FILE__, __LINE__)

#endif  // MRS_SADG_TEST_TEST_UTIL_HPP
