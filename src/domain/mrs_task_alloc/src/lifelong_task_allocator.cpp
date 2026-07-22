#include <stdexcept>

#include "mrs_task_alloc/lifelong_task_allocator.hpp"

namespace mrs
{

bool LifelongTaskAllocator::assign_task(
  std::uint64_t task_id, PhysicalNodeId pickup_node, PhysicalNodeId delivery_node,
  RobotId & out_robot_id)
{
  (void)task_id;
  (void)pickup_node;
  (void)delivery_node;
  (void)out_robot_id;
  throw std::logic_error(
    "not implemented: LifelongTaskAllocator::assign_task — Phase 5 (coordination-builder) 대상"
    " (OR-tools 링크는 로드맵 [4] 이연)");
}

ReassignResult LifelongTaskAllocator::handle_reassign_request(
  RobotId robot_id, std::uint64_t task_id, ReassignTrigger trigger, std::uint32_t attempt_count)
{
  (void)robot_id;
  (void)task_id;
  (void)trigger;
  (void)attempt_count;
  throw std::logic_error("not implemented: LifelongTaskAllocator::handle_reassign_request — Phase "
                         "5 (coordination-builder) 대상");
}

void LifelongTaskAllocator::tick_aging(double now_s)
{
  (void)now_s;
  throw std::logic_error(
    "not implemented: LifelongTaskAllocator::tick_aging — Phase 5 (coordination-builder) 대상");
}

} // namespace mrs
