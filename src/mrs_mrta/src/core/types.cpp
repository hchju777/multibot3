// SPDX-License-Identifier: Apache-2.0
#include "mrs_mrta/core/types.hpp"

#include <algorithm>

namespace mrs_mrta::core
{

bool RoadmapView::has_node(const std::string& vertex) const
{
    return std::find(node_ids.begin(), node_ids.end(), vertex) != node_ids.end();
}

bool RoadmapView::is_endpoint(const std::string& vertex) const
{
    return std::find(endpoints.begin(), endpoints.end(), vertex) != endpoints.end();
}

AssignmentState::AssignmentState(std::vector<std::string> robot_names,
                                 std::size_t goal_queue_capacity_ta)
    : goal_queue_capacity_ta_(goal_queue_capacity_ta), robot_names_(std::move(robot_names))
{
    if (robot_names_.empty())
    {
        throw ContractViolation("AssignmentState: robot roster must not be empty");
    }
    if (!std::is_sorted(robot_names_.begin(), robot_names_.end()))
    {
        throw ContractViolation("AssignmentState: robot roster must be ascending lexicographic");
    }
    if (std::adjacent_find(robot_names_.begin(), robot_names_.end()) != robot_names_.end())
    {
        throw ContractViolation("AssignmentState: robot roster contains a duplicate");
    }

    const std::size_t n = robot_names_.size();
    const std::size_t physical = physical_capacity();
    ring_.assign(n, std::vector<GoalRecord>(physical));
    head_.assign(n, 0);
    count_.assign(n, 0);
    robot_vertex_.assign(n, std::string{});
}

RobotIndex AssignmentState::index_of(const std::string& robot) const noexcept
{
    const auto it = std::lower_bound(robot_names_.begin(), robot_names_.end(), robot);
    if (it == robot_names_.end() || *it != robot)
    {
        return kNoRobot;
    }
    return static_cast<RobotIndex>(std::distance(robot_names_.begin(), it));
}

std::size_t AssignmentState::residual_len(RobotIndex r) const
{
    return count_.at(r);
}

Status AssignmentState::push_tail(RobotIndex r, GoalRecord goal)
{
    if (count_.at(r) >= physical_capacity())
    {
        return Status::kError;
    }
    const std::size_t physical = physical_capacity();
    const std::size_t write_at = (head_[r] + count_[r]) % physical;
    ring_[r][write_at] = std::move(goal);
    ++count_[r];
    return Status::kOk;
}

std::optional<GoalRecord> AssignmentState::pop_head(RobotIndex r)
{
    if (count_.at(r) == 0)
    {
        return std::nullopt;
    }
    const std::size_t physical = physical_capacity();
    GoalRecord popped = std::move(ring_[r][head_[r]]);
    head_[r] = (head_[r] + 1) % physical;
    --count_[r];
    return popped;
}

const GoalRecord* AssignmentState::peek_head(RobotIndex r) const
{
    if (count_.at(r) == 0)
    {
        return nullptr;
    }
    return &ring_[r][head_[r]];
}

const GoalRecord* AssignmentState::peek_tail(RobotIndex r) const
{
    if (count_.at(r) == 0)
    {
        return nullptr;
    }
    const std::size_t physical = physical_capacity();
    const std::size_t tail_at = (head_[r] + count_[r] - 1) % physical;
    return &ring_[r][tail_at];
}

std::vector<GoalRecord> AssignmentState::residual_view(RobotIndex r) const
{
    std::vector<GoalRecord> out;
    const std::size_t n = count_.at(r);
    out.reserve(n);
    const std::size_t physical = physical_capacity();
    for (std::size_t i = 0; i < n; ++i)
    {
        out.push_back(ring_[r][(head_[r] + i) % physical]);
    }
    return out;
}

bool AssignmentState::location_in_use(const std::string& location) const
{
    for (RobotIndex r = 0; r < robot_names_.size(); ++r)
    {
        const std::size_t n = count_[r];
        const std::size_t physical = physical_capacity();
        for (std::size_t i = 0; i < n; ++i)
        {
            if (ring_[r][(head_[r] + i) % physical].location == location)
            {
                return true;
            }
        }
    }
    return false;
}

void PendingPool::push_back(PendingTask task)
{
    tasks_.push_back(std::move(task));
}

const PendingTask* PendingPool::front() const
{
    if (tasks_.empty())
    {
        return nullptr;
    }
    return &tasks_.front();
}

PendingTask PendingPool::pop_front()
{
    if (tasks_.empty())
    {
        throw ContractViolation("PendingPool::pop_front on an empty pool");
    }
    PendingTask front = std::move(tasks_.front());
    tasks_.erase(tasks_.begin());
    return front;
}

std::vector<std::string> PendingPool::task_ids() const
{
    std::vector<std::string> ids;
    ids.reserve(tasks_.size());
    for (const auto& t : tasks_)
    {
        ids.push_back(t.task);
    }
    return ids;
}

void AssignmentState::set_robot_vertex(RobotIndex r, std::string vertex)
{
    robot_vertex_.at(r) = std::move(vertex);
}

const std::string& AssignmentState::robot_vertex(RobotIndex r) const
{
    return robot_vertex_.at(r);
}

void AssignmentState::push_pending(PendingTask task)
{
    pool_.push_back(std::move(task));
}

bool AssignmentState::pending_empty() const noexcept
{
    return pool_.empty();
}

std::size_t AssignmentState::pending_size() const noexcept
{
    return pool_.size();
}

const PendingTask* AssignmentState::peek_pending_front() const
{
    return pool_.front();
}

PendingTask AssignmentState::pop_pending_front()
{
    return pool_.pop_front();
}

std::vector<std::string> AssignmentState::pending_task_ids() const
{
    return pool_.task_ids();
}

}  // namespace mrs_mrta::core
