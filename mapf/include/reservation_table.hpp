#pragma once

#include "planning_types.hpp"

#include <string>
#include <unordered_map>
#include <unordered_set>

struct TimedVertexReservation {
    int vertex = -1;
    int timestep = 0;

    bool operator==(const TimedVertexReservation& other) const
    {
        return vertex == other.vertex && timestep == other.timestep;
    }
};

struct TimedVertexReservationHash {
    std::size_t operator()(const TimedVertexReservation& key) const
    {
        std::size_t h = 2166136261u;
        h ^= static_cast<std::size_t>(key.vertex);
        h *= 16777619u;
        h ^= static_cast<std::size_t>(key.timestep);
        h *= 16777619u;
        return h;
    }
};

struct TimedMoveReservation {
    int from = -1;
    int to = -1;
    int timestep = 0;

    bool operator==(const TimedMoveReservation& other) const
    {
        return from == other.from &&
               to == other.to &&
               timestep == other.timestep;
    }
};

struct TimedMoveReservationHash {
    std::size_t operator()(const TimedMoveReservation& key) const
    {
        std::size_t h = 2166136261u;
        h ^= static_cast<std::size_t>(key.from);
        h *= 16777619u;
        h ^= static_cast<std::size_t>(key.to);
        h *= 16777619u;
        h ^= static_cast<std::size_t>(key.timestep);
        h *= 16777619u;
        return h;
    }
};

class ReservationTable {
public:
    void clear();

    void reserve_vertex(int vertex, int timestep);
    void reserve_vertex_forever(int vertex, int from_timestep);
    void reserve_blocked_move(int from, int to, int timestep);

    void reserve_plan(const FrozenRobotPlan& frozen_plan, int base_timestep = 0);
    void reserve_hold(const HoldReservation& hold, int base_timestep = 0);
    void reserve_all(const FrozenPlanSet& frozen, int base_timestep = 0);

    bool empty() const;
    bool is_vertex_reserved(int vertex, int timestep) const;
    bool is_move_reserved(int from, int to, int timestep) const;
    bool is_move_allowed(int from, int to, int timestep) const;

    bool validate_plan(const AgentPlan& plan, std::string* error = nullptr) const;
    bool validate_solution(const Solution& solution, std::string* error = nullptr) const;

private:
    std::unordered_set<TimedVertexReservation, TimedVertexReservationHash> timed_vertices_;
    std::unordered_set<TimedMoveReservation, TimedMoveReservationHash> blocked_moves_;
    std::unordered_map<int, int> persistent_vertices_;
};
