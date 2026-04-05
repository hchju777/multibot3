#pragma once

#include "planner.hpp"

#include <string>

// Validates that a solution is physically executable on the MAPF graph.
// If error is non-null, it is populated with a human-readable failure reason.
bool validate_physical_solution(const Instance& ins,
                                const Solution& sol,
                                std::string* error = nullptr);
