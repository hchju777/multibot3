#include <stdexcept>

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

mrs::ReassignTrigger trigger_from_request(const mrs_interfaces::srv::ReassignRequest::Request & req)
{
  (void)req;
  throw std::logic_error("not implemented: mrs::convert::trigger_from_request — Phase 5 (ros-builder) 대상");
}

void fill_response(mrs::ReassignResult result, mrs_interfaces::srv::ReassignRequest::Response & resp)
{
  (void)result;
  (void)resp;
  throw std::logic_error("not implemented: mrs::convert::fill_response(ReassignResult) — Phase 5 (ros-builder) 대상");
}

void from_request(
  const mrs_interfaces::srv::PartialReplan::Request & req,
  std::vector<mrs::CommitState> & out_commit_states,
  std::vector<mrs::FrozenOrder> & out_frozen_orders)
{
  (void)req;
  (void)out_commit_states;
  (void)out_frozen_orders;
  throw std::logic_error("not implemented: mrs::convert::from_request(PartialReplan::Request) — Phase 5 (ros-builder) 대상");
}

mrs::ReorderRequest from_request(const mrs_interfaces::srv::ReorderPassing::Request & req)
{
  (void)req;
  throw std::logic_error("not implemented: mrs::convert::from_request(ReorderPassing::Request) — Phase 5 (ros-builder) 대상");
}

void fill_response(
  mrs::ReorderAck ack, std::uint64_t job_id, mrs_interfaces::srv::ReorderPassing::Response & resp)
{
  (void)ack;
  (void)job_id;
  (void)resp;
  throw std::logic_error("not implemented: mrs::convert::fill_response(ReorderAck) — Phase 5 (ros-builder) 대상");
}


} // namespace mrs::convert
