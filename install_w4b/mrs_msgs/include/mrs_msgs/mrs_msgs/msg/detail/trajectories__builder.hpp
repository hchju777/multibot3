// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/Trajectories.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/trajectories.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__TRAJECTORIES__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__TRAJECTORIES__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/trajectories__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_Trajectories_trajectories
{
public:
  explicit Init_Trajectories_trajectories(::mrs_msgs::msg::Trajectories & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::Trajectories trajectories(::mrs_msgs::msg::Trajectories::_trajectories_type arg)
  {
    msg_.trajectories = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::Trajectories msg_;
};

class Init_Trajectories_limits
{
public:
  explicit Init_Trajectories_limits(::mrs_msgs::msg::Trajectories & msg)
  : msg_(msg)
  {}
  Init_Trajectories_trajectories limits(::mrs_msgs::msg::Trajectories::_limits_type arg)
  {
    msg_.limits = std::move(arg);
    return Init_Trajectories_trajectories(msg_);
  }

private:
  ::mrs_msgs::msg::Trajectories msg_;
};

class Init_Trajectories_instance_id
{
public:
  explicit Init_Trajectories_instance_id(::mrs_msgs::msg::Trajectories & msg)
  : msg_(msg)
  {}
  Init_Trajectories_limits instance_id(::mrs_msgs::msg::Trajectories::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_Trajectories_limits(msg_);
  }

private:
  ::mrs_msgs::msg::Trajectories msg_;
};

class Init_Trajectories_schema_version
{
public:
  explicit Init_Trajectories_schema_version(::mrs_msgs::msg::Trajectories & msg)
  : msg_(msg)
  {}
  Init_Trajectories_instance_id schema_version(::mrs_msgs::msg::Trajectories::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_Trajectories_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::Trajectories msg_;
};

class Init_Trajectories_schema
{
public:
  Init_Trajectories_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Trajectories_schema_version schema(::mrs_msgs::msg::Trajectories::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_Trajectories_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::Trajectories msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::Trajectories>()
{
  return mrs_msgs::msg::builder::Init_Trajectories_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__TRAJECTORIES__BUILDER_HPP_
