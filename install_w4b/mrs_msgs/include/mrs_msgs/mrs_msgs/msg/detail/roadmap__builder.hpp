// generated from rosidl_generator_cpp/resource/idl__builder.hpp.em
// with input from mrs_msgs:msg/Roadmap.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/msg/roadmap.hpp"


#ifndef MRS_MSGS__MSG__DETAIL__ROADMAP__BUILDER_HPP_
#define MRS_MSGS__MSG__DETAIL__ROADMAP__BUILDER_HPP_

#include <algorithm>
#include <utility>

#include "mrs_msgs/msg/detail/roadmap__struct.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


namespace mrs_msgs
{

namespace msg
{

namespace builder
{

class Init_Roadmap_provenance
{
public:
  explicit Init_Roadmap_provenance(::mrs_msgs::msg::Roadmap & msg)
  : msg_(msg)
  {}
  ::mrs_msgs::msg::Roadmap provenance(::mrs_msgs::msg::Roadmap::_provenance_type arg)
  {
    msg_.provenance = std::move(arg);
    return std::move(msg_);
  }

private:
  ::mrs_msgs::msg::Roadmap msg_;
};

class Init_Roadmap_corridors
{
public:
  explicit Init_Roadmap_corridors(::mrs_msgs::msg::Roadmap & msg)
  : msg_(msg)
  {}
  Init_Roadmap_provenance corridors(::mrs_msgs::msg::Roadmap::_corridors_type arg)
  {
    msg_.corridors = std::move(arg);
    return Init_Roadmap_provenance(msg_);
  }

private:
  ::mrs_msgs::msg::Roadmap msg_;
};

class Init_Roadmap_endpoints
{
public:
  explicit Init_Roadmap_endpoints(::mrs_msgs::msg::Roadmap & msg)
  : msg_(msg)
  {}
  Init_Roadmap_corridors endpoints(::mrs_msgs::msg::Roadmap::_endpoints_type arg)
  {
    msg_.endpoints = std::move(arg);
    return Init_Roadmap_corridors(msg_);
  }

private:
  ::mrs_msgs::msg::Roadmap msg_;
};

class Init_Roadmap_edges
{
public:
  explicit Init_Roadmap_edges(::mrs_msgs::msg::Roadmap & msg)
  : msg_(msg)
  {}
  Init_Roadmap_endpoints edges(::mrs_msgs::msg::Roadmap::_edges_type arg)
  {
    msg_.edges = std::move(arg);
    return Init_Roadmap_endpoints(msg_);
  }

private:
  ::mrs_msgs::msg::Roadmap msg_;
};

class Init_Roadmap_nodes
{
public:
  explicit Init_Roadmap_nodes(::mrs_msgs::msg::Roadmap & msg)
  : msg_(msg)
  {}
  Init_Roadmap_edges nodes(::mrs_msgs::msg::Roadmap::_nodes_type arg)
  {
    msg_.nodes = std::move(arg);
    return Init_Roadmap_edges(msg_);
  }

private:
  ::mrs_msgs::msg::Roadmap msg_;
};

class Init_Roadmap_instance_id
{
public:
  explicit Init_Roadmap_instance_id(::mrs_msgs::msg::Roadmap & msg)
  : msg_(msg)
  {}
  Init_Roadmap_nodes instance_id(::mrs_msgs::msg::Roadmap::_instance_id_type arg)
  {
    msg_.instance_id = std::move(arg);
    return Init_Roadmap_nodes(msg_);
  }

private:
  ::mrs_msgs::msg::Roadmap msg_;
};

class Init_Roadmap_schema_version
{
public:
  explicit Init_Roadmap_schema_version(::mrs_msgs::msg::Roadmap & msg)
  : msg_(msg)
  {}
  Init_Roadmap_instance_id schema_version(::mrs_msgs::msg::Roadmap::_schema_version_type arg)
  {
    msg_.schema_version = std::move(arg);
    return Init_Roadmap_instance_id(msg_);
  }

private:
  ::mrs_msgs::msg::Roadmap msg_;
};

class Init_Roadmap_schema
{
public:
  Init_Roadmap_schema()
  : msg_(::rosidl_runtime_cpp::MessageInitialization::SKIP)
  {}
  Init_Roadmap_schema_version schema(::mrs_msgs::msg::Roadmap::_schema_type arg)
  {
    msg_.schema = std::move(arg);
    return Init_Roadmap_schema_version(msg_);
  }

private:
  ::mrs_msgs::msg::Roadmap msg_;
};

}  // namespace builder

}  // namespace msg

template<typename MessageType>
auto build();

template<>
inline
auto build<::mrs_msgs::msg::Roadmap>()
{
  return mrs_msgs::msg::builder::Init_Roadmap_schema();
}

}  // namespace mrs_msgs

#endif  // MRS_MSGS__MSG__DETAIL__ROADMAP__BUILDER_HPP_
