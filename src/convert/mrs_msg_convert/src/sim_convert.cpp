#include <stdexcept>

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

mrs_interfaces::msg::RobotState to_msg(const mrs::RobotObservation & observation)
{
  (void)observation;
  throw std::logic_error("not implemented: mrs::convert::to_msg(RobotObservation) — Phase 5 (ros-builder) 대상");
}

mrs::RobotObservation from_msg(const mrs_interfaces::msg::RobotState & msg)
{
  (void)msg;
  throw std::logic_error("not implemented: mrs::convert::from_msg(RobotState) — Phase 5 (ros-builder) 대상");
}

mrs_interfaces::msg::SimCapabilities to_msg(const mrs::SimCapabilities & caps)
{
  (void)caps;
  throw std::logic_error("not implemented: mrs::convert::to_msg(SimCapabilities) — Phase 5 (ros-builder) 대상");
}

mrs_interfaces::msg::SimMetricSample to_msg(const mrs::SimMetricSample & sample)
{
  (void)sample;
  throw std::logic_error("not implemented: mrs::convert::to_msg(SimMetricSample) — Phase 5 (ros-builder) 대상");
}

mrs::FaultInjection from_request(const mrs_interfaces::srv::SimInject::Request & req)
{
  (void)req;
  throw std::logic_error("not implemented: mrs::convert::from_request(SimInject::Request) — Phase 5 (ros-builder) 대상");
}


} // namespace mrs::convert
