#pragma once

#include "sadg_types.hpp"

#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

namespace sadg {

class SadgGraph {
public:
    ActionIndex add_action(SadgActionNode node);
    int add_edge(SadgDependencyEdge edge);
    ResourceIndex add_resource(SadgResource resource);

    bool has_action(const ActionId& id) const;
    bool has_resource(const ResourceId& id) const;

    ActionIndex find_action_index(const ActionId& id) const;
    ResourceIndex find_resource_index(const ResourceId& id) const;

    SadgActionNode& action(ActionIndex idx);
    const SadgActionNode& action(ActionIndex idx) const;

    SadgDependencyEdge& edge(int idx);
    const SadgDependencyEdge& edge(int idx) const;

    SadgResource& resource(ResourceIndex idx);
    const SadgResource& resource(ResourceIndex idx) const;

    const std::vector<ActionIndex>& robot_actions(RobotIndex robot_idx) const;
    const std::vector<int>& in_edges(ActionIndex action_idx) const;
    const std::vector<int>& out_edges(ActionIndex action_idx) const;
    const std::vector<ActionIndex>& resource_actions(ResourceIndex resource_idx) const;

    std::vector<ActionIndex> predecessor_actions(ActionIndex idx) const;
    std::vector<ActionIndex> successor_actions(ActionIndex idx) const;

    std::optional<ActionIndex> next_robot_action(RobotIndex robot_idx,
                                                 ActionIndex current_idx) const;
    std::optional<ActionIndex> previous_robot_action(RobotIndex robot_idx,
                                                     ActionIndex current_idx) const;

    ResourceIndex ensure_vertex_resource(int vertex);
    ResourceIndex ensure_directed_edge_resource(int from_vertex, int to_vertex);
    void connect_action_resource(ActionIndex action_idx, ResourceIndex resource_idx);

    std::size_t action_count() const { return actions_.size(); }
    std::size_t edge_count() const { return edges_.size(); }
    std::size_t resource_count() const { return resources_.size(); }

private:
    std::vector<SadgActionNode> actions_;
    std::vector<SadgDependencyEdge> edges_;
    std::vector<SadgResource> resources_;

    std::unordered_map<ActionId, ActionIndex> action_id_to_index_;
    std::unordered_map<EdgeId, int> edge_id_to_index_;
    std::unordered_map<ResourceId, ResourceIndex> resource_id_to_index_;

    std::vector<std::vector<ActionIndex>> robot_action_indices_;
    std::vector<std::vector<int>> in_edge_indices_;
    std::vector<std::vector<int>> out_edge_indices_;
    std::vector<std::vector<ActionIndex>> resource_action_indices_;

    std::unordered_map<int, ResourceIndex> vertex_resource_index_;
    std::map<std::pair<int, int>, ResourceIndex> directed_edge_resource_index_;

    void ensure_robot_slots(RobotIndex robot_idx);
};

}  // namespace sadg
