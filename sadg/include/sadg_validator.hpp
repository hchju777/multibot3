#pragma once

#include "sadg_graph.hpp"

namespace sadg {

class SadgValidator {
public:
    SadgHealthReport evaluate(const SadgGraph& graph,
                              const SadgRuntimeState& state,
                              const std::vector<RobotSadgState>& robots,
                              int64_t now_ms,
                              const SadgTimingPolicy& timing_policy,
                              const SadgReplanTriggerPolicy& trigger_policy) const;
};

}  // namespace sadg
