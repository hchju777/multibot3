#pragma once

#include "sadg_graph.hpp"

namespace sadg {

struct SadgBuildRequest {
    SadgPlanInput plan_input;
    SadgTimingPolicy timing_policy;
};

struct SadgBuildResult {
    SadgGraph graph;
    std::string warning;
};

class SadgBuilder {
public:
    SadgBuildResult build(const SadgBuildRequest& request) const;
};

}  // namespace sadg
