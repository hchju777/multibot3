#include <stdexcept>

#include "mrs_msg_convert/msg_convert.hpp"

namespace mrs::convert
{

mrs_interfaces::msg::RungEvent to_msg(const mrs::RungEventRecord & record)
{
  (void)record;
  throw std::logic_error("not implemented: mrs::convert::to_msg(RungEventRecord) — Phase 5 (ros-builder) 대상");
}


} // namespace mrs::convert
