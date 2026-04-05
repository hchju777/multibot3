#include "sadg_graph.hpp"

#include <stdexcept>

namespace sadg {

void SadgGraph::ensure_robot_slots(RobotIndex robot_idx)
{
    if (robot_idx < 0) {
        throw std::runtime_error("negative robot index");
    }
    const std::size_t size = static_cast<std::size_t>(robot_idx + 1);
    if (robot_action_indices_.size() < size) {
        robot_action_indices_.resize(size);
    }
}

ActionIndex SadgGraph::add_action(SadgActionNode node)
{
    if (node.action_id.empty()) {
        throw std::runtime_error("action_id must not be empty");
    }
    if (action_id_to_index_.count(node.action_id) != 0) {
        throw std::runtime_error("duplicate action_id: " + node.action_id);
    }

    ensure_robot_slots(node.robot_index);

    const ActionIndex idx = static_cast<ActionIndex>(actions_.size());
    node.action_index = idx;
    actions_.push_back(std::move(node));
    action_id_to_index_[actions_.back().action_id] = idx;
    robot_action_indices_[actions_.back().robot_index].push_back(idx);
    in_edge_indices_.emplace_back();
    out_edge_indices_.emplace_back();
    return idx;
}

int SadgGraph::add_edge(SadgDependencyEdge edge)
{
    if (edge.edge_id.empty()) {
        throw std::runtime_error("edge_id must not be empty");
    }
    if (edge_id_to_index_.count(edge.edge_id) != 0) {
        throw std::runtime_error("duplicate edge_id: " + edge.edge_id);
    }
    if (edge.from_action < 0 || edge.to_action < 0 ||
        edge.from_action >= static_cast<ActionIndex>(actions_.size()) ||
        edge.to_action >= static_cast<ActionIndex>(actions_.size())) {
        throw std::runtime_error("edge action index out of range");
    }

    const int idx = static_cast<int>(edges_.size());
    edges_.push_back(std::move(edge));
    edge_id_to_index_[edges_.back().edge_id] = idx;
    out_edge_indices_[edges_.back().from_action].push_back(idx);
    in_edge_indices_[edges_.back().to_action].push_back(idx);
    return idx;
}

ResourceIndex SadgGraph::add_resource(SadgResource resource)
{
    if (resource.resource_id.empty()) {
        throw std::runtime_error("resource_id must not be empty");
    }
    if (resource_id_to_index_.count(resource.resource_id) != 0) {
        throw std::runtime_error("duplicate resource_id: " + resource.resource_id);
    }

    const ResourceIndex idx = static_cast<ResourceIndex>(resources_.size());
    resource.resource_index = idx;
    resources_.push_back(std::move(resource));
    resource_id_to_index_[resources_.back().resource_id] = idx;
    resource_action_indices_.emplace_back();
    return idx;
}

bool SadgGraph::has_action(const ActionId& id) const
{
    return action_id_to_index_.count(id) != 0;
}

bool SadgGraph::has_resource(const ResourceId& id) const
{
    return resource_id_to_index_.count(id) != 0;
}

ActionIndex SadgGraph::find_action_index(const ActionId& id) const
{
    const auto it = action_id_to_index_.find(id);
    if (it == action_id_to_index_.end()) {
        throw std::runtime_error("unknown action_id: " + id);
    }
    return it->second;
}

ResourceIndex SadgGraph::find_resource_index(const ResourceId& id) const
{
    const auto it = resource_id_to_index_.find(id);
    if (it == resource_id_to_index_.end()) {
        throw std::runtime_error("unknown resource_id: " + id);
    }
    return it->second;
}

SadgActionNode& SadgGraph::action(ActionIndex idx)
{
    return actions_.at(static_cast<std::size_t>(idx));
}

const SadgActionNode& SadgGraph::action(ActionIndex idx) const
{
    return actions_.at(static_cast<std::size_t>(idx));
}

SadgDependencyEdge& SadgGraph::edge(int idx)
{
    return edges_.at(static_cast<std::size_t>(idx));
}

const SadgDependencyEdge& SadgGraph::edge(int idx) const
{
    return edges_.at(static_cast<std::size_t>(idx));
}

SadgResource& SadgGraph::resource(ResourceIndex idx)
{
    return resources_.at(static_cast<std::size_t>(idx));
}

const SadgResource& SadgGraph::resource(ResourceIndex idx) const
{
    return resources_.at(static_cast<std::size_t>(idx));
}

const std::vector<ActionIndex>& SadgGraph::robot_actions(RobotIndex robot_idx) const
{
    return robot_action_indices_.at(static_cast<std::size_t>(robot_idx));
}

const std::vector<int>& SadgGraph::in_edges(ActionIndex action_idx) const
{
    return in_edge_indices_.at(static_cast<std::size_t>(action_idx));
}

const std::vector<int>& SadgGraph::out_edges(ActionIndex action_idx) const
{
    return out_edge_indices_.at(static_cast<std::size_t>(action_idx));
}

const std::vector<ActionIndex>& SadgGraph::resource_actions(ResourceIndex resource_idx) const
{
    return resource_action_indices_.at(static_cast<std::size_t>(resource_idx));
}

std::vector<ActionIndex> SadgGraph::predecessor_actions(ActionIndex idx) const
{
    std::vector<ActionIndex> result;
    for (int edge_idx : in_edges(idx)) {
        result.push_back(edges_[static_cast<std::size_t>(edge_idx)].from_action);
    }
    return result;
}

std::vector<ActionIndex> SadgGraph::successor_actions(ActionIndex idx) const
{
    std::vector<ActionIndex> result;
    for (int edge_idx : out_edges(idx)) {
        result.push_back(edges_[static_cast<std::size_t>(edge_idx)].to_action);
    }
    return result;
}

std::optional<ActionIndex> SadgGraph::next_robot_action(RobotIndex robot_idx,
                                                        ActionIndex current_idx) const
{
    const auto& actions = robot_actions(robot_idx);
    for (std::size_t i = 0; i + 1 < actions.size(); ++i) {
        if (actions[i] == current_idx) return actions[i + 1];
    }
    return std::nullopt;
}

std::optional<ActionIndex> SadgGraph::previous_robot_action(RobotIndex robot_idx,
                                                            ActionIndex current_idx) const
{
    const auto& actions = robot_actions(robot_idx);
    for (std::size_t i = 1; i < actions.size(); ++i) {
        if (actions[i] == current_idx) return actions[i - 1];
    }
    return std::nullopt;
}

ResourceIndex SadgGraph::ensure_vertex_resource(int vertex)
{
    const auto it = vertex_resource_index_.find(vertex);
    if (it != vertex_resource_index_.end()) return it->second;

    SadgResource resource;
    resource.resource_id = "v:" + std::to_string(vertex);
    resource.kind = ResourceKind::VERTEX;
    resource.backing_vertices.push_back(vertex);
    const ResourceIndex idx = add_resource(std::move(resource));
    vertex_resource_index_[vertex] = idx;
    return idx;
}

ResourceIndex SadgGraph::ensure_directed_edge_resource(int from_vertex, int to_vertex)
{
    const auto key = std::make_pair(from_vertex, to_vertex);
    const auto it = directed_edge_resource_index_.find(key);
    if (it != directed_edge_resource_index_.end()) return it->second;

    SadgResource resource;
    resource.resource_id =
        "e:" + std::to_string(from_vertex) + "->" + std::to_string(to_vertex);
    resource.kind = ResourceKind::DIRECTED_EDGE;
    resource.directed_edge = key;
    const ResourceIndex idx = add_resource(std::move(resource));
    directed_edge_resource_index_[key] = idx;
    return idx;
}

void SadgGraph::connect_action_resource(ActionIndex action_idx, ResourceIndex resource_idx)
{
    auto& action_ref = action(action_idx);
    action_ref.resources.push_back(resource_idx);
    resource_action_indices_.at(static_cast<std::size_t>(resource_idx)).push_back(action_idx);
}

}  // namespace sadg
