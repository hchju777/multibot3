// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/RobotSpecs.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/robot_specs.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROBOT_SPECS__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ROBOT_SPECS__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/robot_specs__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_RobotSpecs_robots
{
public:
  explicit Init_RobotSpecs_robots(::mrs_msgs::msg::RobotSpecs & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::RobotSpecs robots(::mrs_msgs::msg::RobotSpecs::_robots_type arg)
  {
    msg_.robots = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::RobotSpecs msg_;
};

class Init_RobotSpecs_instance_id
{
public:
  explicit Init_RobotSpecs_instance_id(::mrs_msgs::msg::RobotSpecs & msg)
  : msg_(msg)
  {}
  Init_RobotSpecs_robots instance_id(::mrs_msgs::msg::RobotSpecs::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_RobotSpecs_robots(msg_);
  }

private:
  ::mrs_msgs::msg::RobotSpecs msg_;
};

class Init_RobotSpecs_schema_version
{
public:
  explicit Init_RobotSpecs_schema_version(::mrs_msgs::msg::RobotSpecs & msg)
  : msg_(msg)
  {}
  Init_RobotSpecs_instance_id schema_version(::mrs_msgs::msg::RobotSpecs::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_RobotSpecs_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::RobotSpecs msg_;
};

class Init_RobotSpecs_schema
{
public:
  Init_RobotSpecs_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_RobotSpecs_schema_version schema(::mrs_msgs::msg::RobotSpecs::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_RobotSpecs_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::RobotSpecs msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::RobotSpecs>()
{
  return mrs_msgs::msg::builder::Init_RobotSpecs_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROBOT_SPECS__BUILDER_HPP_
