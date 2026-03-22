#include "solution.hpp"

#include <cassert>

// ============================================================
//  to_solution
//  RawPaths → SADG 입력 형식 변환
//
//  raw[i][t] = Vertex*  →  sol[i][t] = (vertex_id, timestep)
// ============================================================
Solution to_solution(const RawPaths& raw)
{
    Solution sol;
    sol.reserve(raw.size());

    for (const RawPath& path : raw) {
        AgentPlan plan;
        plan.reserve(path.size());
        for (int t = 0; t < static_cast<int>(path.size()); ++t) {
            assert(path[t] != nullptr);
            plan.emplace_back(path[t]->id, t);
        }
        sol.push_back(std::move(plan));
    }

    return sol;
}
