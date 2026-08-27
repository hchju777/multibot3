// SPDX-License-Identifier: Apache-2.0
//
// 측정 스파이크 실행기 (산출물 c). 🔴 판정 어휘 금지 — 수와 여유율 산술만 낸다.
// 판정은 research-auditor(증거)·boundary-verifier(정합)의 몫이다.
//
// 무엇을 재는가:
//   T-S  = ITrajectorySearch::solve            (궤적 틱마다)
//   T-VP = VelocityProfiler::parameterize      (궤적 틱마다)
//   T-H  = SafetyMonitor::remaining_stoppable  (제어 틱마다)
//   worst-tick = 서브골 틱의 (T-S + T-VP + T-H) 합 (세 시계 동시 참)
// 무엇을 [값 부재]로 남기는가 (구현 스파이크 밖):
//   T-P/T-R/T-G(라운드 루프·채널·커밋 게이트) — 라운드 루프가 단일 로봇 경로에 미배선(20d 한계).
//
// 시드 규약: reproducibility.md §3 (조건당 25 시드 {0..24}, 조건 간 동일).
// 시간: p50/p95/p99/max. 로봇 수 스케일 {4, 10, 20}.

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <random>
#include <string>
#include <vector>

#include "mrs_trajopt/core/safety_monitor.hpp"
#include "mrs_trajopt/core/types.hpp"
#include "mrs_trajopt/core/velocity_profiler.hpp"
#include "mrs_trajopt/plugins/search_hybrid_astar.hpp"

using namespace mrs_trajopt::core;
using namespace mrs_trajopt::plugins;

namespace
{

double percentile(std::vector<double> v, double p)
{
    if (v.empty())
    {
        return 0.0;
    }
    std::sort(v.begin(), v.end());
    const double idx = p / 100.0 * static_cast<double>(v.size() - 1);
    const std::size_t lo = static_cast<std::size_t>(std::floor(idx));
    const std::size_t hi = static_cast<std::size_t>(std::ceil(idx));
    if (lo == hi)
    {
        return v[lo];
    }
    const double frac = idx - static_cast<double>(lo);
    return v[lo] * (1.0 - frac) + v[hi] * frac;
}

RobotLimits make_lim()
{
    RobotLimits l;
    l.robot = "r";
    l.v_max = 1.0;
    l.brake_decel_min_mps2 = 1.0;
    l.curvature_max_invm = 2.0;
    l.reverse_motion_allowed = false;
    l.avg_traversal_speed_mps = 0.8;
    l.circumradius_m = 0.25;
    return l;
}

FleetLimits make_fleet()
{
    FleetLimits f;
    f.a_max = 1.0;
    f.j_max = 5.0;
    f.omega_max_radps = 0.5;
    f.yaw_accel_max_radps2 = 1.0;
    f.min_separation_m = 0.6;
    return f;
}

// A crossing-corridor subgoal chain of `count` windows with seeded jitter.
std::vector<PassWindow> make_subgoals(int count, std::mt19937& rng)
{
    std::uniform_real_distribution<double> jitter(-0.05, 0.05);
    std::vector<PassWindow> sgs;
    for (int i = 1; i <= count; ++i)
    {
        PassWindow w;
        w.center.x = static_cast<double>(i) * 0.5;
        w.center.y = jitter(rng);
        w.center.theta = 0.0;
        w.radius_win_m = 0.2;
        sgs.push_back(w);
    }
    return sgs;
}

}  // namespace

int main()
{
    const std::vector<int> robot_counts = {4, 10, 20};
    const int seeds = 25;  // {0..24}, 조건 간 동일.

    VelocityProfiler prof;
    auto pred = [](const StateSample&, double)
    {
        return true;
    };
    SafetyMonitor safety(pred);
    const RobotLimits lim = make_lim();
    const FleetLimits fleet = make_fleet();

    std::printf("# mrs_trajopt measurement spike — 실측 (판정 어휘 없음)\n");
    std::printf("# seeds/condition = %d {0..24}, control period = 50.000 ms\n", seeds);
    std::printf("# stage timings in ms: p50/p95/p99/max\n\n");

    std::printf("%-6s %-8s %10s %10s %10s %10s\n", "N", "stage", "p50", "p95", "p99", "max");
    for (int N : robot_counts)
    {
        // path length grows with N (denser corridor) to exercise scaling.
        const int subgoal_count = N * 4;
        std::vector<double> ts, tvp, th, worst;
        for (int s = 0; s < seeds; ++s)
        {
            std::mt19937 rng(static_cast<unsigned>(s));  // seed injected (CN-18).
            auto sgs = make_subgoals(subgoal_count, rng);
            SearchHybridAstar search(0.1);  // small lateral yield budget.
            SearchContext ctx;
            ctx.start.x = 0.0;
            ctx.subgoals = sgs;
            ctx.limits = lim;

            SearchOutput out;
            auto t0 = std::chrono::steady_clock::now();
            const bool found = search.solve(ctx, out);
            auto t1 = std::chrono::steady_clock::now();

            ProfileResult pr;
            auto t2 = std::chrono::steady_clock::now();
            if (found)
            {
                pr = prof.parameterize(out.chain, lim, fleet, 0.0);
            }
            auto t3 = std::chrono::steady_clock::now();

            auto t4 = std::chrono::steady_clock::now();
            if (pr.ok)
            {
                safety.remaining_stoppable(pr.chain, lim.brake_decel_min_mps2);
            }
            auto t5 = std::chrono::steady_clock::now();

            const double ms_s = std::chrono::duration<double, std::milli>(t1 - t0).count();
            const double ms_vp = std::chrono::duration<double, std::milli>(t3 - t2).count();
            const double ms_h = std::chrono::duration<double, std::milli>(t5 - t4).count();
            ts.push_back(ms_s);
            tvp.push_back(ms_vp);
            th.push_back(ms_h);
            worst.push_back(ms_s + ms_vp + ms_h);
        }

        auto row = [&](const char* name, std::vector<double>& v)
        {
            std::printf("%-6d %-8s %10.4f %10.4f %10.4f %10.4f\n",
                        N,
                        name,
                        percentile(v, 50),
                        percentile(v, 95),
                        percentile(v, 99),
                        *std::max_element(v.begin(), v.end()));
        };
        row("T-S", ts);
        row("T-VP", tvp);
        row("T-H", th);
        row("worst", worst);
        // headroom vs 50 ms budget (worst-tick p99). 산술만.
        const double p99_worst = percentile(worst, 99);
        std::printf("%-6d %-8s p99=%.4f ms of 50.000 ms  headroom=%.4f ms (%.3f%%)\n",
                    N,
                    "budget",
                    p99_worst,
                    50.0 - p99_worst,
                    100.0 * (50.0 - p99_worst) / 50.0);
        std::printf("\n");
    }

    // Distance derivation from robot_specs (경계 신설 0 — 온보드 유도).
    std::printf("# three distances derived from robot_specs (191 B16, 온보드 유도)\n");
    const double cir = lim.circumradius_m;
    const double d_occ = 2.0 * cir;  // occupancy distance = f(circumradius).
    const double d_perm =
        d_occ + lim.v_max * 0.05;  // permission > occupancy (제동+비트나이 여유 예시).
    const double d_req = d_perm + lim.v_max * 0.05;  // request > permission.
    std::printf(
        "circumradius_m=%.4f -> d_occ=%.4f (=2*cir) < d_perm=%.4f < d_req=%.4f [실측: robot_specs "
        "유도]\n",
        cir,
        d_occ,
        d_perm,
        d_req);
    std::printf(
        "# 🔴 d_perm/d_req의 여유항 계수는 [값 부재] — 위는 유도식의 형태이고 계수는 제안. 확정은 "
        "191/사용자.\n");

    return 0;
}
