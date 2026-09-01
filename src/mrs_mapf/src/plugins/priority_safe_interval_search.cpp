// SPDX-License-Identifier: Apache-2.0
#include "mrs_mapf/plugins/priority_safe_interval_search.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <tuple>
#include <utility>

#include "mrs_mapf/core/reservation.hpp"

namespace mrs_mapf::plugins
{
using namespace mrs_mapf::core;  // NOLINT

namespace
{

// 🔴 Implementation-level floating tolerance for "arrival is instantaneous
// pass-through" comparisons — NOT the "로버스트 여유"(robustness margin) canon
// fixed at 0 (`255`§255-2-3). This constant never widens a reservation; it only
// avoids double==double flakiness in interval comparisons.
constexpr double kEps = 1e-6;
// Sentinel "occupied until the end of any plausible planning horizon" marker for
// a parked robot's final vertex (RB-1's conservative direction — §255-5-4(다)).
constexpr double kParkedHorizon = 1e18;
// Defensive iteration cap for the fixed-point "earliest safe departure" search
// (the true number of iterations is bounded by the finite reservation count on
// the touched edges/vertex — this is a correctness guard against a latent bug,
// not a tuned search-node budget; §255-11 value #4 remains [값 부재] and is not
// approximated here).
constexpr int kDepartureFixedPointCap = 4096;

struct Interval
{
    double lo = 0.0;
    double hi = 0.0;
};

bool overlaps(const Interval& a, const Interval& b)
{
    return a.lo < b.hi - kEps && b.lo < a.hi - kEps;
}

/// @brief Directed-edge + vertex reservation table (continuous time — no grid).
class ReservationTable
{
public:
    void reserve_edge(const std::string& from, const std::string& to, double lo, double hi)
    {
        edges_[{from, to}].push_back({lo, hi});
    }

    void reserve_vertex(const std::string& v, double lo, double hi)
    {
        vertices_[v].push_back({lo, hi});
    }

    /// @brief The smallest hi >= @p window.lo among intervals on (from,to) or
    /// (to,from) [swap] overlapping @p window, or NaN if none block it.
    double next_edge_clear_time(const std::string& from,
                                const std::string& to,
                                Interval window) const
    {
        double best = std::numeric_limits<double>::quiet_NaN();
        const std::pair<std::string, std::string> forward_key(from, to);
        const std::pair<std::string, std::string> reverse_key(to,
                                                              from);  // swap-conflict direction.
        for (const auto& key : {forward_key, reverse_key})
        {
            auto it = edges_.find(key);
            if (it == edges_.end())
            {
                continue;
            }
            for (const auto& iv : it->second)
            {
                if (overlaps(iv, window) && (std::isnan(best) || iv.hi > best))
                {
                    best = iv.hi;
                }
            }
        }
        return best;
    }

    double next_vertex_clear_time(const std::string& v, Interval window) const
    {
        auto it = vertices_.find(v);
        if (it == vertices_.end())
        {
            return std::numeric_limits<double>::quiet_NaN();
        }
        double best = std::numeric_limits<double>::quiet_NaN();
        for (const auto& iv : it->second)
        {
            if (overlaps(iv, window) && (std::isnan(best) || iv.hi > best))
            {
                best = iv.hi;
            }
        }
        return best;
    }

private:
    std::map<std::pair<std::string, std::string>, std::vector<Interval>> edges_;
    std::map<std::string, std::vector<Interval>> vertices_;
};

/// @brief Seed the table with every OUT-OF-SCOPE robot's reconstructed
/// occupancy (RB-1, §255-8-3's "밖 로봇 지선" — tick absent => unquantized,
/// §255-11's [값 부재]).
void seed_out_of_scope(ReservationTable& table,
                       const PlanningRequest& request,
                       const std::set<std::string>& scope_set)
{
    if (!request.previous_plan.has_value())
    {
        return;
    }
    std::map<std::string, double> speed_of;
    for (const auto& rs : request.robot_specs)
    {
        speed_of[rs.robot] = rs.avg_traversal_speed_mps;
    }
    for (const auto& plan : request.previous_plan->plans)
    {
        if (scope_set.count(plan.robot) != 0)
        {
            continue;  // will be replanned.
        }
        if (plan.steps.empty())
        {
            continue;
        }
        const double speed = speed_of.count(plan.robot) != 0 ? speed_of.at(plan.robot) : 0.0;
        if (plan.steps.size() < 2 || speed <= 0.0)
        {
            // Parked already, or speed unknown — conservative: occupy its (only
            // known) vertex indefinitely (RB-1's conservative direction).
            table.reserve_vertex(plan.steps.front().location, 0.0, kParkedHorizon);
            continue;
        }
        const auto arrival = exact_arrival_schedule(plan.steps, request.roadmap, speed);
        if (arrival.size() != plan.steps.size())
        {
            table.reserve_vertex(plan.steps.front().location, 0.0, kParkedHorizon);
            continue;
        }
        for (std::size_t i = 0; i + 1 < plan.steps.size(); ++i)
        {
            table.reserve_edge(plan.steps[i].location,
                               plan.steps[i + 1].location,
                               arrival[i],
                               arrival[i + 1]);
        }
        table.reserve_vertex(plan.steps.back().location, arrival.back(), kParkedHorizon);
    }
}

/// @brief Reverse-Dijkstra geometric distance from every vertex to @p goal,
/// following the roadmap's directed arcs forward (i.e. distance FROM v TO
/// goal). Used both as the A* heuristic and as tie-break level ② (§255-2-3).
std::map<std::string, double> distance_to_goal(const Roadmap& roadmap, const std::string& goal)
{
    std::map<std::string, std::vector<std::pair<std::string, double>>> reverse_adj;
    for (const auto& [u, arcs] : roadmap.out_arcs)
    {
        for (const auto& arc : arcs)
        {
            reverse_adj[arc.to].push_back({u, arc.length_m});
        }
    }
    std::map<std::string, double> dist;
    for (const auto& id : roadmap.node_ids)
    {
        dist[id] = std::numeric_limits<double>::infinity();
    }
    if (dist.find(goal) == dist.end())
    {
        return dist;
    }
    dist[goal] = 0.0;
    std::set<std::pair<double, std::string>> open;
    open.insert({0.0, goal});
    while (!open.empty())
    {
        auto [d, u] = *open.begin();
        open.erase(open.begin());
        if (d > dist[u] + kEps)
        {
            continue;
        }
        auto it = reverse_adj.find(u);
        if (it == reverse_adj.end())
        {
            continue;
        }
        for (const auto& [v, len] : it->second)
        {
            const double nd = d + len;
            if (nd < dist[v] - kEps)
            {
                dist[v] = nd;
                open.insert({nd, v});
            }
        }
    }
    return dist;
}

/// @brief Earliest t0 >= @p earliest such that traversing (u,v) departing at t0
/// (arriving t0+duration) is free of every conflict this plugin checks: same
/// direction/opposite-direction (swap) edge occupancy, destination-vertex park
/// occupancy, and origin-vertex park occupancy during the wait window.
/// @return the earliest safe departure time, or +inf if the fixed point did not
///   converge within the defensive cap (treated as "no path" by the caller).
double earliest_safe_departure(const ReservationTable& table,
                               const std::string& u,
                               const std::string& v,
                               double earliest,
                               double duration)
{
    double t0 = earliest;
    for (int iter = 0; iter < kDepartureFixedPointCap; ++iter)
    {
        bool advanced = false;

        const double edge_block = table.next_edge_clear_time(u, v, {t0, t0 + duration});
        if (!std::isnan(edge_block) && edge_block > t0)
        {
            t0 = edge_block;
            advanced = true;
        }

        const double dst_block =
            table.next_vertex_clear_time(v, {t0 + duration, t0 + duration + kEps});
        if (!std::isnan(dst_block) && dst_block > t0 + duration)
        {
            t0 = dst_block - duration;
            advanced = true;
        }

        const double src_block = table.next_vertex_clear_time(u, {earliest, t0 + kEps});
        if (!std::isnan(src_block) && src_block > t0)
        {
            t0 = src_block;
            advanced = true;
        }

        if (!advanced)
        {
            return t0;
        }
    }
    return std::numeric_limits<double>::infinity();
}

struct LegResult
{
    bool found = false;
    std::vector<std::pair<std::string, double>> path;  ///< (vertex, arrival_time), start excluded.
};

/// @brief Single-robot, single-leg (one start -> one goal) continuous-time
/// safe-interval search. Label-correcting earliest-arrival A* (heuristic =
/// geometric distance / speed, admissible since reservations only delay).
/// Family guarantee: feasible-only — no completeness claim (`255`§255-2-2 row
/// "셋째").
LegResult search_leg(const Roadmap& roadmap,
                     const ReservationTable& table,
                     const std::string& start,
                     double start_t,
                     const std::string& goal,
                     double speed)
{
    if (start == goal)
    {
        return {true, {}};
    }
    const auto dist = distance_to_goal(roadmap, goal);
    auto h_time = [&](const std::string& v) -> double
    {
        auto it = dist.find(v);
        if (it == dist.end() || !std::isfinite(it->second))
        {
            return std::numeric_limits<double>::infinity();
        }
        return it->second / speed;
    };
    if (!std::isfinite(h_time(start)))
    {
        return {false, {}};  // goal structurally unreachable from start.
    }

    std::map<std::string, double> best;
    std::map<std::string, std::string> parent;
    best[start] = start_t;

    // (f, h, vertex, insertion) — matches §255-2-3 tie-break levels ①②④⑤
    // (level ③ "robot index" does not apply inside one robot's own search).
    std::set<std::tuple<double, double, std::string, std::size_t>> open;
    std::size_t insertion = 0;
    open.insert({start_t + h_time(start), h_time(start), start, insertion++});

    while (!open.empty())
    {
        auto it = open.begin();
        const auto [f, h, u, ins] = *it;
        open.erase(it);
        (void)ins;
        const double t = f - h;
        if (t > best[u] + kEps)
        {
            continue;  // stale entry.
        }
        if (u == goal)
        {
            std::vector<std::pair<std::string, double>> path;
            std::string cur = u;
            while (cur != start)
            {
                path.push_back({cur, best[cur]});
                cur = parent[cur];
            }
            std::reverse(path.begin(), path.end());
            return {true, path};
        }
        auto arcs_it = roadmap.out_arcs.find(u);
        if (arcs_it == roadmap.out_arcs.end())
        {
            continue;
        }
        for (const auto& arc : arcs_it->second)
        {
            const double duration = arc.length_m / speed;
            const double departure = earliest_safe_departure(table, u, arc.to, t, duration);
            if (!std::isfinite(departure))
            {
                continue;
            }
            const double arrival = departure + duration;
            const double hv = h_time(arc.to);
            if (!std::isfinite(hv))
            {
                continue;
            }
            auto bit = best.find(arc.to);
            if (bit == best.end() || arrival < bit->second - kEps)
            {
                best[arc.to] = arrival;
                parent[arc.to] = u;
                open.insert({arrival + hv, hv, arc.to, insertion++});
            }
        }
    }
    return {false, {}};
}

}  // namespace

Result<std::map<std::string, TimedRobotPlan>, PlanFailure> PrioritySafeIntervalSearch::solve(
    const PlanningRequest& request,
    const std::vector<std::string>& scope)
{
    std::set<std::string> scope_set(scope.begin(), scope.end());

    // R04 (§255-1-1): blocked edges are excluded from traversal for this call
    // (this plugin filters a copy rather than bitmasking the source roadmap —
    // documented simplification, `20b_impl_mapf.md` known limitations).
    Roadmap filtered = request.roadmap;
    if (!request.blocked.empty())
    {
        std::set<std::pair<std::string, std::string>> blocked_set;
        for (const auto& b : request.blocked)
        {
            blocked_set.insert({b.from, b.to});
        }
        for (auto& [from, arcs] : filtered.out_arcs)
        {
            arcs.erase(std::remove_if(arcs.begin(),
                                      arcs.end(),
                                      [&](const RoadmapArc& a)
                                      {
                                          return blocked_set.count({from, a.to}) != 0;
                                      }),
                       arcs.end());
        }
    }

    ReservationTable table;
    seed_out_of_scope(table, request, scope_set);

    std::map<std::string, double> speed_of;
    std::map<std::string, const AssignmentEntry*> assignment_of;
    for (const auto& rs : request.robot_specs)
    {
        speed_of[rs.robot] = rs.avg_traversal_speed_mps;
    }
    for (const auto& a : request.assignments)
    {
        assignment_of[a.robot] = &a;
    }

    // Deterministic priority order — ascending robot identifier (a
    // simplification of the literature's dynamic priority schemes; documented
    // known limitation).
    std::vector<std::string> ordered_scope(scope.begin(), scope.end());
    std::sort(ordered_scope.begin(), ordered_scope.end());

    std::map<std::string, TimedRobotPlan> result;

    for (const auto& robot : ordered_scope)
    {
        auto assign_it = assignment_of.find(robot);
        if (assign_it == assignment_of.end())
        {
            return Result<std::map<std::string, TimedRobotPlan>, PlanFailure>::err(
                PlanFailure{robot, "요청 로스터에 이 로봇의 assignment가 없다"});
        }
        const AssignmentEntry& a = *assign_it->second;
        const double speed = speed_of.count(robot) != 0 ? speed_of.at(robot) : 0.0;
        if (speed <= 0.0)
        {
            return Result<std::map<std::string, TimedRobotPlan>, PlanFailure>::err(
                PlanFailure{robot,
                            "robot_specs 에 이 로봇의 avg_traversal_speed_mps 가 없거나 0 이하다"});
        }

        RobotPlan plan;
        plan.robot = robot;
        std::vector<double> arrival = {0.0};
        std::vector<std::string> locations = {a.start};
        std::string current = a.start;
        double current_t = 0.0;
        bool ok_all_goals = true;

        for (const auto& goal_loc : a.goal_locations)
        {
            if (goal_loc == current)
            {
                continue;  // consecutive-duplicate fold (§1-2(1)) — no move needed.
            }
            LegResult leg = search_leg(filtered, table, current, current_t, goal_loc, speed);
            if (!leg.found)
            {
                ok_all_goals = false;
                break;
            }
            for (const auto& [v, t] : leg.path)
            {
                locations.push_back(v);
                arrival.push_back(t);
            }
            if (!leg.path.empty())
            {
                current = leg.path.back().first;
                current_t = leg.path.back().second;
            }
        }

        if (!ok_all_goals && locations.size() <= 1)
        {
            return Result<std::map<std::string, TimedRobotPlan>, PlanFailure>::err(
                PlanFailure{robot, "첫 목표부터 도달 가능한 안전 구간 경로가 없다"});
        }

        for (std::size_t i = 0; i < locations.size(); ++i)
        {
            StepEntry se;
            se.index = static_cast<std::uint32_t>(i);
            se.location = locations[i];
            plan.steps.push_back(se);
        }
        plan.terminal = ok_all_goals ? Terminal::kGoalReached : Terminal::kPlanTruncated;

        // Reserve this robot's own segments so lower-priority (later, larger id)
        // in-scope robots treat it as a moving obstacle too.
        for (std::size_t i = 0; i + 1 < locations.size(); ++i)
        {
            table.reserve_edge(locations[i], locations[i + 1], arrival[i], arrival[i + 1]);
        }
        table.reserve_vertex(locations.back(), arrival.back(), kParkedHorizon);

        TimedRobotPlan timed;
        timed.plan = plan;
        timed.arrival_s = arrival;
        result[robot] = timed;
    }

    return Result<std::map<std::string, TimedRobotPlan>, PlanFailure>::ok_value(result);
}

}  // namespace mrs_mapf::plugins
