#include "solution_validation.hpp"

#include <algorithm>
#include <sstream>
#include <vector>

namespace {

bool has_edge(const Graph& G, int from_id, int to_id)
{
    if (from_id == to_id) return true;
    for (Vertex* nb : G.V[from_id]->neighbors) {
        if (nb->id == to_id) return true;
    }
    return false;
}

void set_error(std::string* error, const std::string& message)
{
    if (error != nullptr) *error = message;
}

}  // namespace

bool validate_physical_solution(const Instance& ins,
                                const Solution& sol,
                                std::string* error)
{
    const int n = ins.num_agents();
    if (static_cast<int>(sol.size()) != n) {
        set_error(error, "solution agent count mismatch");
        return false;
    }

    std::vector<std::vector<int>> paths(n);
    int makespan = 0;

    for (int i = 0; i < n; ++i) {
        if (sol[i].empty()) {
            set_error(error, "robot " + std::to_string(i) + " has empty plan");
            return false;
        }
        if (sol[i].front().first != ins.starts[i]->id) {
            set_error(error, "robot " + std::to_string(i) +
                                 " does not start at start vertex");
            return false;
        }
        if (sol[i].back().first != ins.goals[i]->id) {
            set_error(error, "robot " + std::to_string(i) +
                                 " does not end at goal vertex");
            return false;
        }

        paths[i].reserve(sol[i].size());
        for (int t = 0; t < static_cast<int>(sol[i].size()); ++t) {
            const auto [vid, plan_t] = sol[i][t];
            if (plan_t != t) {
                set_error(error, "robot " + std::to_string(i) +
                                     " has non-contiguous timestep");
                return false;
            }
            paths[i].push_back(vid);

            if (t == 0) continue;
            if (!has_edge(ins.G, paths[i][t - 1], paths[i][t])) {
                std::ostringstream oss;
                oss << "robot " << i << " uses invalid edge "
                    << paths[i][t - 1] << " -> " << paths[i][t]
                    << " at t=" << (t - 1) << "->" << t;
                set_error(error, oss.str());
                return false;
            }
        }

        makespan = std::max(makespan, static_cast<int>(paths[i].size()));
    }

    for (auto& path : paths) {
        while (static_cast<int>(path.size()) < makespan) {
            path.push_back(path.back());
        }
    }

    for (int t = 0; t < makespan; ++t) {
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (paths[i][t] == paths[j][t]) {
                    std::ostringstream oss;
                    oss << "vertex collision at t=" << t
                        << " between robot " << i
                        << " and robot " << j
                        << " on v" << paths[i][t];
                    set_error(error, oss.str());
                    return false;
                }
            }
        }
    }

    for (int t = 0; t + 1 < makespan; ++t) {
        for (int i = 0; i < n; ++i) {
            for (int j = i + 1; j < n; ++j) {
                if (paths[i][t] == paths[j][t + 1] &&
                    paths[j][t] == paths[i][t + 1] &&
                    paths[i][t] != paths[i][t + 1]) {
                    std::ostringstream oss;
                    oss << "swap collision at t=" << t << "->" << (t + 1)
                        << " between robot " << i
                        << " and robot " << j;
                    set_error(error, oss.str());
                    return false;
                }
            }
        }
    }

    for (int t = 0; t + 2 < makespan; ++t) {
        for (int i = 0; i < n; ++i) {
            const int a0 = paths[i][t];
            const int a1 = paths[i][t + 1];
            if (a0 == a1) continue;

            for (int j = 0; j < n; ++j) {
                if (i == j) continue;
                const int b0 = paths[j][t + 1];
                const int b1 = paths[j][t + 2];
                if (b0 == b1) continue;

                if (a0 == b1 && a1 == b0) {
                    std::ostringstream oss;
                    oss << "non-passing collision between t=" << t
                        << " and t=" << (t + 2)
                        << " for robots " << i
                        << " and " << j;
                    set_error(error, oss.str());
                    return false;
                }
            }
        }
    }

    if (error != nullptr) error->clear();
    return true;
}
