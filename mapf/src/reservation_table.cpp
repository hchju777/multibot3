#include "reservation_table.hpp"

#include <algorithm>
#include <sstream>

void ReservationTable::clear()
{
    timed_vertices_.clear();
    blocked_moves_.clear();
    persistent_vertices_.clear();
}

void ReservationTable::reserve_vertex(int vertex, int timestep)
{
    timed_vertices_.insert({vertex, timestep});
}

void ReservationTable::reserve_vertex_forever(int vertex, int from_timestep)
{
    const auto it = persistent_vertices_.find(vertex);
    if (it == persistent_vertices_.end()) {
        persistent_vertices_.emplace(vertex, from_timestep);
        return;
    }
    if (from_timestep < it->second) {
        it->second = from_timestep;
    }
}

void ReservationTable::reserve_blocked_move(int from, int to, int timestep)
{
    blocked_moves_.insert({from, to, timestep});
}

void ReservationTable::reserve_plan(const FrozenRobotPlan& frozen_plan, int base_timestep)
{
    if (frozen_plan.plan.empty()) return;

    for (const auto& [vertex, timestep] : frozen_plan.plan) {
        if (timestep < base_timestep) continue;
        reserve_vertex(vertex, timestep - base_timestep);
    }

    for (std::size_t i = 1; i < frozen_plan.plan.size(); ++i) {
        const auto& [from_vertex, _from_timestep] = frozen_plan.plan[i - 1];
        const auto& [to_vertex, to_timestep] = frozen_plan.plan[i];
        (void)_from_timestep;
        if (to_timestep < base_timestep) continue;

        const int relative_timestep = to_timestep - base_timestep;

        // 같은 timestep swap 과 다음 timestep delayed reverse-edge 를 모두 막는다.
        reserve_blocked_move(to_vertex, from_vertex, relative_timestep);
        reserve_blocked_move(to_vertex, from_vertex, relative_timestep + 1);
    }

    if (frozen_plan.reserve_goal_forever) {
        reserve_vertex_forever(
            frozen_plan.plan.back().first,
            std::max(0, frozen_plan.plan.back().second - base_timestep));
    }
}

void ReservationTable::reserve_hold(const HoldReservation& hold, int base_timestep)
{
    reserve_vertex_forever(hold.vertex, std::max(0, hold.from_timestep - base_timestep));
    if (hold.reverse_edge_to.has_value()) {
        const int relative_timestep =
            std::max(0, hold.from_timestep - base_timestep) + 1;
        reserve_blocked_move(hold.vertex, *hold.reverse_edge_to, relative_timestep);
    }
}

void ReservationTable::reserve_all(const FrozenPlanSet& frozen, int base_timestep)
{
    for (const auto& plan : frozen.plan_suffixes) {
        reserve_plan(plan, base_timestep);
    }
    for (const auto& hold : frozen.holds) {
        reserve_hold(hold, base_timestep);
    }
}

bool ReservationTable::empty() const
{
    return timed_vertices_.empty() &&
           blocked_moves_.empty() &&
           persistent_vertices_.empty();
}

bool ReservationTable::is_vertex_reserved(int vertex, int timestep) const
{
    if (timed_vertices_.count({vertex, timestep}) != 0) {
        return true;
    }

    const auto it = persistent_vertices_.find(vertex);
    return it != persistent_vertices_.end() && timestep >= it->second;
}

bool ReservationTable::is_move_reserved(int from, int to, int timestep) const
{
    return blocked_moves_.count({from, to, timestep}) != 0;
}

bool ReservationTable::is_move_allowed(int from, int to, int timestep) const
{
    return !is_vertex_reserved(to, timestep) &&
           !is_move_reserved(from, to, timestep);
}

bool ReservationTable::validate_plan(const AgentPlan& plan, std::string* error) const
{
    for (std::size_t i = 0; i < plan.size(); ++i) {
        const auto& [vertex, timestep] = plan[i];
        if (is_vertex_reserved(vertex, timestep)) {
            if (error != nullptr) {
                std::ostringstream oss;
                oss << "reservation conflict at vertex " << vertex
                    << " timestep " << timestep;
                *error = oss.str();
            }
            return false;
        }

        if (i == 0) continue;

        const auto& [prev_vertex, prev_timestep] = plan[i - 1];
        (void)prev_timestep;
        if (is_move_reserved(prev_vertex, vertex, timestep)) {
            if (error != nullptr) {
                std::ostringstream oss;
                oss << "reservation conflict on move " << prev_vertex
                    << "->" << vertex << " timestep " << timestep;
                *error = oss.str();
            }
            return false;
        }
    }

    return true;
}

bool ReservationTable::validate_solution(const Solution& solution, std::string* error) const
{
    for (const auto& plan : solution) {
        if (!validate_plan(plan, error)) return false;
    }
    return true;
}
