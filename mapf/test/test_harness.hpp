#pragma once

#include <spdlog/spdlog.h>

#include <exception>
#include <functional>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace mapf_test {

struct TestContext {
    int failures = 0;
};

using TestCase = std::pair<std::string, std::function<void(TestContext&)>>;

inline void fail(TestContext& ctx,
                 const char* file,
                 int line,
                 const std::string& message)
{
    ++ctx.failures;
    spdlog::error("{}:{} {}", file, line, message);
}

template <typename T>
std::string repr(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

inline void expect_true(TestContext& ctx,
                        bool condition,
                        const char* expr,
                        const char* file,
                        int line)
{
    if (!condition) {
        fail(ctx, file, line, std::string("EXPECT_TRUE failed: ") + expr);
    }
}

inline void expect_false(TestContext& ctx,
                         bool condition,
                         const char* expr,
                         const char* file,
                         int line)
{
    if (condition) {
        fail(ctx, file, line, std::string("EXPECT_FALSE failed: ") + expr);
    }
}

template <typename L, typename R>
void expect_eq(TestContext& ctx,
               const L& lhs,
               const R& rhs,
               const char* lhs_expr,
               const char* rhs_expr,
               const char* file,
               int line)
{
    if (!(lhs == rhs)) {
        fail(ctx,
             file,
             line,
             "EXPECT_EQ failed: " + std::string(lhs_expr) + "=" + repr(lhs) +
                 ", " + rhs_expr + "=" + repr(rhs));
    }
}

inline void expect_contains(TestContext& ctx,
                            const std::string& haystack,
                            const std::string& needle,
                            const char* haystack_expr,
                            const char* needle_expr,
                            const char* file,
                            int line)
{
    if (haystack.find(needle) == std::string::npos) {
        fail(ctx,
             file,
             line,
             "EXPECT_CONTAINS failed: " + std::string(haystack_expr) +
                 " does not contain " + needle_expr +
                 " (value=\"" + haystack + "\", needle=\"" + needle + "\")");
    }
}

inline int run_all(const std::vector<TestCase>& tests)
{
    spdlog::set_pattern("[%H:%M:%S] [%^%l%$] %v");
    spdlog::set_level(spdlog::level::info);

    TestContext ctx;
    int failed_tests = 0;

    for (const auto& [name, fn] : tests) {
        spdlog::info("[ RUN      ] {}", name);
        const int failures_before = ctx.failures;
        try {
            fn(ctx);
        } catch (const std::exception& e) {
            fail(ctx, __FILE__, __LINE__, "unexpected exception: " + std::string(e.what()));
        } catch (...) {
            fail(ctx, __FILE__, __LINE__, "unexpected non-standard exception");
        }

        if (ctx.failures == failures_before) {
            spdlog::info("[       OK ] {}", name);
        } else {
            ++failed_tests;
            spdlog::error("[  FAILED  ] {}", name);
        }
    }

    if (failed_tests == 0) {
        spdlog::info("all {} tests passed", tests.size());
        return 0;
    }

    spdlog::error("{} of {} tests failed ({} assertion failures)",
                  failed_tests,
                  tests.size(),
                  ctx.failures);
    return 1;
}

}  // namespace mapf_test

#define MAPF_EXPECT_TRUE(ctx, expr) \
    ::mapf_test::expect_true((ctx), static_cast<bool>(expr), #expr, __FILE__, __LINE__)

#define MAPF_EXPECT_FALSE(ctx, expr) \
    ::mapf_test::expect_false((ctx), static_cast<bool>(expr), #expr, __FILE__, __LINE__)

#define MAPF_EXPECT_EQ(ctx, lhs, rhs) \
    ::mapf_test::expect_eq((ctx), (lhs), (rhs), #lhs, #rhs, __FILE__, __LINE__)

#define MAPF_EXPECT_CONTAINS(ctx, haystack, needle) \
    ::mapf_test::expect_contains((ctx), (haystack), (needle), #haystack, #needle, __FILE__, __LINE__)
