#pragma once

#include "sadg_graph.hpp"

namespace sadg {

class SadgScheduler {
public:
    explicit SadgScheduler(SadgSchedulingPolicy policy = {});

    SadgSchedulerInitResult initialize(const SadgGraph& graph,
                                       const FleetSnapshot& snapshot) const;

    SadgFeedbackApplyResult apply_feedback(const SadgGraph& graph,
                                           SadgRuntimeState& state,
                                           std::vector<RobotSadgState>& robots,
                                           const ExecutionFeedback& feedback) const;

    SadgReadyResult recompute_ready(const SadgGraph& graph,
                                    SadgRuntimeState& state,
                                    std::vector<RobotSadgState>& robots) const;

    SadgDispatchResult dispatch_ready_actions(const SadgGraph& graph,
                                              SadgRuntimeState& state,
                                              std::vector<RobotSadgState>& robots,
                                              int64_t now_ms) const;

    void hold_robot(const SadgGraph& graph,
                    SadgRuntimeState& state,
                    std::vector<RobotSadgState>& robots,
                    RobotIndex robot_idx,
                    const std::string& reason) const;

    void enter_safe_stop(const SadgGraph& graph,
                         SadgRuntimeState& state,
                         std::vector<RobotSadgState>& robots,
                         const std::string& reason) const;

private:
    SadgSchedulingPolicy policy_;

    bool resources_available(const SadgGraph& graph,
                             const SadgRuntimeState& state,
                             const SadgActionNode& action) const;

    void sync_robot_states(const SadgGraph& graph,
                           const SadgRuntimeState& state,
                           std::vector<RobotSadgState>& robots) const;
};

}  // namespace sadg
