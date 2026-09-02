// generated from rosidl_generator_cpp/resource/idl__struct.hpp.em
// with input from mrs_msgs:action/Replan.idl
// generated code does not contain a copyright notice

// IWYU pragma: private, include "mrs_msgs/action/replan.hpp"


#ifndef MRS_MSGS__ACTION__DETAIL__REPLAN__STRUCT_HPP_
#define MRS_MSGS__ACTION__DETAIL__REPLAN__STRUCT_HPP_

#include <algorithm>
#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "rosidl_runtime_cpp/bounded_vector.hpp"
#include "rosidl_runtime_cpp/message_initialization.hpp"


// Include directives for member types
// Member 'blocked'
#include "mrs_msgs/msg/detail/edge_ref__struct.hpp"
// Member 'ordering_constraints'
#include "mrs_msgs/msg/detail/visit_order__struct.hpp"
// Member 'baseline'
#include "mrs_msgs/msg/detail/robot_baseline__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__action__Replan_Goal __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__action__Replan_Goal __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace action
{

// message struct
template<class ContainerAllocator>
struct Replan_Goal_
{
  using Type = Replan_Goal_<ContainerAllocator>;

  explicit Replan_Goal_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->schema = "";
      this->schema_version = "";
      this->instance_id = "";
      this->reason = 0;
      this->blocked_hash = "";
      this->plan_revision = 0ull;
    }
  }

  explicit Replan_Goal_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : schema(_alloc),
    schema_version(_alloc),
    instance_id(_alloc),
    blocked_hash(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->schema = "";
      this->schema_version = "";
      this->instance_id = "";
      this->reason = 0;
      this->blocked_hash = "";
      this->plan_revision = 0ull;
    }
  }

  // field types and members
  using _schema_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _schema_type schema;
  using _schema_version_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _schema_version_type schema_version;
  using _instance_id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _instance_id_type instance_id;
  using _reason_type =
    uint8_t;
  _reason_type reason;
  using _affected_robots_type =
    std::vector<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>>>;
  _affected_robots_type affected_robots;
  using _blocked_type =
    std::vector<mrs_msgs::msg::EdgeRef_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::EdgeRef_<ContainerAllocator>>>;
  _blocked_type blocked;
  using _blocked_hash_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _blocked_hash_type blocked_hash;
  using _ordering_constraints_type =
    std::vector<mrs_msgs::msg::VisitOrder_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::VisitOrder_<ContainerAllocator>>>;
  _ordering_constraints_type ordering_constraints;
  using _baseline_type =
    std::vector<mrs_msgs::msg::RobotBaseline_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::RobotBaseline_<ContainerAllocator>>>;
  _baseline_type baseline;
  using _plan_revision_type =
    uint64_t;
  _plan_revision_type plan_revision;

  // setters for named parameter idiom
  Type & set__schema(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->schema = _arg;
    return *this;
  }
  Type & set__schema_version(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->schema_version = _arg;
    return *this;
  }
  Type & set__instance_id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->instance_id = _arg;
    return *this;
  }
  Type & set__reason(
    const uint8_t & _arg)
  {
    this->reason = _arg;
    return *this;
  }
  Type & set__affected_robots(
    const std::vector<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>>> & _arg)
  {
    this->affected_robots = _arg;
    return *this;
  }
  Type & set__blocked(
    const std::vector<mrs_msgs::msg::EdgeRef_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::EdgeRef_<ContainerAllocator>>> & _arg)
  {
    this->blocked = _arg;
    return *this;
  }
  Type & set__blocked_hash(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->blocked_hash = _arg;
    return *this;
  }
  Type & set__ordering_constraints(
    const std::vector<mrs_msgs::msg::VisitOrder_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::VisitOrder_<ContainerAllocator>>> & _arg)
  {
    this->ordering_constraints = _arg;
    return *this;
  }
  Type & set__baseline(
    const std::vector<mrs_msgs::msg::RobotBaseline_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::RobotBaseline_<ContainerAllocator>>> & _arg)
  {
    this->baseline = _arg;
    return *this;
  }
  Type & set__plan_revision(
    const uint64_t & _arg)
  {
    this->plan_revision = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::action::Replan_Goal_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::action::Replan_Goal_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_Goal_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_Goal_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_Goal_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_Goal_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_Goal_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_Goal_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_Goal_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_Goal_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__action__Replan_Goal
    std::shared_ptr<mrs_msgs::action::Replan_Goal_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__action__Replan_Goal
    std::shared_ptr<mrs_msgs::action::Replan_Goal_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Replan_Goal_ & other) const
  {
    if (this->schema != other.schema) {
      return false;
    }
    if (this->schema_version != other.schema_version) {
      return false;
    }
    if (this->instance_id != other.instance_id) {
      return false;
    }
    if (this->reason != other.reason) {
      return false;
    }
    if (this->affected_robots != other.affected_robots) {
      return false;
    }
    if (this->blocked != other.blocked) {
      return false;
    }
    if (this->blocked_hash != other.blocked_hash) {
      return false;
    }
    if (this->ordering_constraints != other.ordering_constraints) {
      return false;
    }
    if (this->baseline != other.baseline) {
      return false;
    }
    if (this->plan_revision != other.plan_revision) {
      return false;
    }
    return true;
  }
  bool operator!=(const Replan_Goal_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Replan_Goal_

// alias to use template instance with default allocator
using Replan_Goal =
  mrs_msgs::action::Replan_Goal_<std::allocator<void>>;

// constant definitions

}  // namespace action

}  // namespace mrs_msgs


// Include directives for member types
// Member 'components'
#include "mrs_msgs/msg/detail/robot_group__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__action__Replan_Result __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__action__Replan_Result __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace action
{

// message struct
template<class ContainerAllocator>
struct Replan_Result_
{
  using Type = Replan_Result_<ContainerAllocator>;

  explicit Replan_Result_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->schema = "";
      this->schema_version = "";
      this->instance_id = "";
      this->outcome = 0;
      this->plan_revision = 0ull;
      this->blocked_hash = "";
    }
  }

  explicit Replan_Result_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : schema(_alloc),
    schema_version(_alloc),
    instance_id(_alloc),
    blocked_hash(_alloc)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->schema = "";
      this->schema_version = "";
      this->instance_id = "";
      this->outcome = 0;
      this->plan_revision = 0ull;
      this->blocked_hash = "";
    }
  }

  // field types and members
  using _schema_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _schema_type schema;
  using _schema_version_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _schema_version_type schema_version;
  using _instance_id_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _instance_id_type instance_id;
  using _outcome_type =
    uint8_t;
  _outcome_type outcome;
  using _plan_revision_type =
    uint64_t;
  _plan_revision_type plan_revision;
  using _blocked_hash_type =
    std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>;
  _blocked_hash_type blocked_hash;
  using _unreachable_robots_type =
    std::vector<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>>>;
  _unreachable_robots_type unreachable_robots;
  using _components_type =
    std::vector<mrs_msgs::msg::RobotGroup_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::RobotGroup_<ContainerAllocator>>>;
  _components_type components;

  // setters for named parameter idiom
  Type & set__schema(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->schema = _arg;
    return *this;
  }
  Type & set__schema_version(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->schema_version = _arg;
    return *this;
  }
  Type & set__instance_id(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->instance_id = _arg;
    return *this;
  }
  Type & set__outcome(
    const uint8_t & _arg)
  {
    this->outcome = _arg;
    return *this;
  }
  Type & set__plan_revision(
    const uint64_t & _arg)
  {
    this->plan_revision = _arg;
    return *this;
  }
  Type & set__blocked_hash(
    const std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>> & _arg)
  {
    this->blocked_hash = _arg;
    return *this;
  }
  Type & set__unreachable_robots(
    const std::vector<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<std::basic_string<char, std::char_traits<char>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<char>>>> & _arg)
  {
    this->unreachable_robots = _arg;
    return *this;
  }
  Type & set__components(
    const std::vector<mrs_msgs::msg::RobotGroup_<ContainerAllocator>, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::msg::RobotGroup_<ContainerAllocator>>> & _arg)
  {
    this->components = _arg;
    return *this;
  }

  // constant declarations
  static constexpr uint8_t OUTCOME_UNKNOWN =
    0u;
  static constexpr uint8_t OUTCOME_PLANNED =
    1u;
  static constexpr uint8_t OUTCOME_FALLBACK =
    2u;
  static constexpr uint8_t OUTCOME_PREEMPTED =
    3u;
  static constexpr uint8_t OUTCOME_REJECTED =
    4u;

  // pointer types
  using RawPtr =
    mrs_msgs::action::Replan_Result_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::action::Replan_Result_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_Result_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_Result_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_Result_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_Result_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_Result_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_Result_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_Result_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_Result_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__action__Replan_Result
    std::shared_ptr<mrs_msgs::action::Replan_Result_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__action__Replan_Result
    std::shared_ptr<mrs_msgs::action::Replan_Result_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Replan_Result_ & other) const
  {
    if (this->schema != other.schema) {
      return false;
    }
    if (this->schema_version != other.schema_version) {
      return false;
    }
    if (this->instance_id != other.instance_id) {
      return false;
    }
    if (this->outcome != other.outcome) {
      return false;
    }
    if (this->plan_revision != other.plan_revision) {
      return false;
    }
    if (this->blocked_hash != other.blocked_hash) {
      return false;
    }
    if (this->unreachable_robots != other.unreachable_robots) {
      return false;
    }
    if (this->components != other.components) {
      return false;
    }
    return true;
  }
  bool operator!=(const Replan_Result_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Replan_Result_

// alias to use template instance with default allocator
using Replan_Result =
  mrs_msgs::action::Replan_Result_<std::allocator<void>>;

// constant definitions
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t Replan_Result_<ContainerAllocator>::OUTCOME_UNKNOWN;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t Replan_Result_<ContainerAllocator>::OUTCOME_PLANNED;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t Replan_Result_<ContainerAllocator>::OUTCOME_FALLBACK;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t Replan_Result_<ContainerAllocator>::OUTCOME_PREEMPTED;
#endif  // __cplusplus < 201703L
#if __cplusplus < 201703L
// static constexpr member variable definitions are only needed in C++14 and below, deprecated in C++17
template<typename ContainerAllocator>
constexpr uint8_t Replan_Result_<ContainerAllocator>::OUTCOME_REJECTED;
#endif  // __cplusplus < 201703L

}  // namespace action

}  // namespace mrs_msgs


#ifndef _WIN32
# define DEPRECATED__mrs_msgs__action__Replan_Feedback __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__action__Replan_Feedback __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace action
{

// message struct
template<class ContainerAllocator>
struct Replan_Feedback_
{
  using Type = Replan_Feedback_<ContainerAllocator>;

  explicit Replan_Feedback_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->structure_needs_at_least_one_member = 0;
    }
  }

  explicit Replan_Feedback_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  {
    (void)_alloc;
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->structure_needs_at_least_one_member = 0;
    }
  }

  // field types and members
  using _structure_needs_at_least_one_member_type =
    uint8_t;
  _structure_needs_at_least_one_member_type structure_needs_at_least_one_member;


  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::action::Replan_Feedback_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::action::Replan_Feedback_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_Feedback_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_Feedback_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_Feedback_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_Feedback_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_Feedback_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_Feedback_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_Feedback_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_Feedback_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__action__Replan_Feedback
    std::shared_ptr<mrs_msgs::action::Replan_Feedback_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__action__Replan_Feedback
    std::shared_ptr<mrs_msgs::action::Replan_Feedback_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Replan_Feedback_ & other) const
  {
    if (this->structure_needs_at_least_one_member != other.structure_needs_at_least_one_member) {
      return false;
    }
    return true;
  }
  bool operator!=(const Replan_Feedback_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Replan_Feedback_

// alias to use template instance with default allocator
using Replan_Feedback =
  mrs_msgs::action::Replan_Feedback_<std::allocator<void>>;

// constant definitions

}  // namespace action

}  // namespace mrs_msgs


// Include directives for member types
// Member 'goal_id'
#include "unique_identifier_msgs/msg/detail/uuid__struct.hpp"
// Member 'goal'
#include "mrs_msgs/action/detail/replan__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__action__Replan_SendGoal_Request __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__action__Replan_SendGoal_Request __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace action
{

// message struct
template<class ContainerAllocator>
struct Replan_SendGoal_Request_
{
  using Type = Replan_SendGoal_Request_<ContainerAllocator>;

  explicit Replan_SendGoal_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : goal_id(_init),
    goal(_init)
  {
    (void)_init;
  }

  explicit Replan_SendGoal_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : goal_id(_alloc, _init),
    goal(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _goal_id_type =
    unique_identifier_msgs::msg::UUID_<ContainerAllocator>;
  _goal_id_type goal_id;
  using _goal_type =
    mrs_msgs::action::Replan_Goal_<ContainerAllocator>;
  _goal_type goal;

  // setters for named parameter idiom
  Type & set__goal_id(
    const unique_identifier_msgs::msg::UUID_<ContainerAllocator> & _arg)
  {
    this->goal_id = _arg;
    return *this;
  }
  Type & set__goal(
    const mrs_msgs::action::Replan_Goal_<ContainerAllocator> & _arg)
  {
    this->goal = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__action__Replan_SendGoal_Request
    std::shared_ptr<mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__action__Replan_SendGoal_Request
    std::shared_ptr<mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Replan_SendGoal_Request_ & other) const
  {
    if (this->goal_id != other.goal_id) {
      return false;
    }
    if (this->goal != other.goal) {
      return false;
    }
    return true;
  }
  bool operator!=(const Replan_SendGoal_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Replan_SendGoal_Request_

// alias to use template instance with default allocator
using Replan_SendGoal_Request =
  mrs_msgs::action::Replan_SendGoal_Request_<std::allocator<void>>;

// constant definitions

}  // namespace action

}  // namespace mrs_msgs


// Include directives for member types
// Member 'stamp'
#include "builtin_interfaces/msg/detail/time__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__action__Replan_SendGoal_Response __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__action__Replan_SendGoal_Response __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace action
{

// message struct
template<class ContainerAllocator>
struct Replan_SendGoal_Response_
{
  using Type = Replan_SendGoal_Response_<ContainerAllocator>;

  explicit Replan_SendGoal_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : stamp(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->accepted = false;
    }
  }

  explicit Replan_SendGoal_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : stamp(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->accepted = false;
    }
  }

  // field types and members
  using _accepted_type =
    bool;
  _accepted_type accepted;
  using _stamp_type =
    builtin_interfaces::msg::Time_<ContainerAllocator>;
  _stamp_type stamp;

  // setters for named parameter idiom
  Type & set__accepted(
    const bool & _arg)
  {
    this->accepted = _arg;
    return *this;
  }
  Type & set__stamp(
    const builtin_interfaces::msg::Time_<ContainerAllocator> & _arg)
  {
    this->stamp = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__action__Replan_SendGoal_Response
    std::shared_ptr<mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__action__Replan_SendGoal_Response
    std::shared_ptr<mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Replan_SendGoal_Response_ & other) const
  {
    if (this->accepted != other.accepted) {
      return false;
    }
    if (this->stamp != other.stamp) {
      return false;
    }
    return true;
  }
  bool operator!=(const Replan_SendGoal_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Replan_SendGoal_Response_

// alias to use template instance with default allocator
using Replan_SendGoal_Response =
  mrs_msgs::action::Replan_SendGoal_Response_<std::allocator<void>>;

// constant definitions

}  // namespace action

}  // namespace mrs_msgs


// Include directives for member types
// Member 'info'
#include "service_msgs/msg/detail/service_event_info__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__action__Replan_SendGoal_Event __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__action__Replan_SendGoal_Event __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace action
{

// message struct
template<class ContainerAllocator>
struct Replan_SendGoal_Event_
{
  using Type = Replan_SendGoal_Event_<ContainerAllocator>;

  explicit Replan_SendGoal_Event_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_init)
  {
    (void)_init;
  }

  explicit Replan_SendGoal_Event_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _info_type =
    service_msgs::msg::ServiceEventInfo_<ContainerAllocator>;
  _info_type info;
  using _request_type =
    rosidl_runtime_cpp::BoundedVector<mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator>>>;
  _request_type request;
  using _response_type =
    rosidl_runtime_cpp::BoundedVector<mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator>>>;
  _response_type response;

  // setters for named parameter idiom
  Type & set__info(
    const service_msgs::msg::ServiceEventInfo_<ContainerAllocator> & _arg)
  {
    this->info = _arg;
    return *this;
  }
  Type & set__request(
    const rosidl_runtime_cpp::BoundedVector<mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::action::Replan_SendGoal_Request_<ContainerAllocator>>> & _arg)
  {
    this->request = _arg;
    return *this;
  }
  Type & set__response(
    const rosidl_runtime_cpp::BoundedVector<mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::action::Replan_SendGoal_Response_<ContainerAllocator>>> & _arg)
  {
    this->response = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::action::Replan_SendGoal_Event_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::action::Replan_SendGoal_Event_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_SendGoal_Event_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_SendGoal_Event_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_SendGoal_Event_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_SendGoal_Event_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_SendGoal_Event_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_SendGoal_Event_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_SendGoal_Event_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_SendGoal_Event_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__action__Replan_SendGoal_Event
    std::shared_ptr<mrs_msgs::action::Replan_SendGoal_Event_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__action__Replan_SendGoal_Event
    std::shared_ptr<mrs_msgs::action::Replan_SendGoal_Event_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Replan_SendGoal_Event_ & other) const
  {
    if (this->info != other.info) {
      return false;
    }
    if (this->request != other.request) {
      return false;
    }
    if (this->response != other.response) {
      return false;
    }
    return true;
  }
  bool operator!=(const Replan_SendGoal_Event_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Replan_SendGoal_Event_

// alias to use template instance with default allocator
using Replan_SendGoal_Event =
  mrs_msgs::action::Replan_SendGoal_Event_<std::allocator<void>>;

// constant definitions

}  // namespace action

}  // namespace mrs_msgs

namespace mrs_msgs
{

namespace action
{

struct Replan_SendGoal
{
  using Request = mrs_msgs::action::Replan_SendGoal_Request;
  using Response = mrs_msgs::action::Replan_SendGoal_Response;
  using Event = mrs_msgs::action::Replan_SendGoal_Event;
};

}  // namespace action

}  // namespace mrs_msgs


// Include directives for member types
// Member 'goal_id'
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__action__Replan_GetResult_Request __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__action__Replan_GetResult_Request __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace action
{

// message struct
template<class ContainerAllocator>
struct Replan_GetResult_Request_
{
  using Type = Replan_GetResult_Request_<ContainerAllocator>;

  explicit Replan_GetResult_Request_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : goal_id(_init)
  {
    (void)_init;
  }

  explicit Replan_GetResult_Request_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : goal_id(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _goal_id_type =
    unique_identifier_msgs::msg::UUID_<ContainerAllocator>;
  _goal_id_type goal_id;

  // setters for named parameter idiom
  Type & set__goal_id(
    const unique_identifier_msgs::msg::UUID_<ContainerAllocator> & _arg)
  {
    this->goal_id = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__action__Replan_GetResult_Request
    std::shared_ptr<mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__action__Replan_GetResult_Request
    std::shared_ptr<mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Replan_GetResult_Request_ & other) const
  {
    if (this->goal_id != other.goal_id) {
      return false;
    }
    return true;
  }
  bool operator!=(const Replan_GetResult_Request_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Replan_GetResult_Request_

// alias to use template instance with default allocator
using Replan_GetResult_Request =
  mrs_msgs::action::Replan_GetResult_Request_<std::allocator<void>>;

// constant definitions

}  // namespace action

}  // namespace mrs_msgs


// Include directives for member types
// Member 'result'
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__action__Replan_GetResult_Response __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__action__Replan_GetResult_Response __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace action
{

// message struct
template<class ContainerAllocator>
struct Replan_GetResult_Response_
{
  using Type = Replan_GetResult_Response_<ContainerAllocator>;

  explicit Replan_GetResult_Response_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : result(_init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->status = 0;
    }
  }

  explicit Replan_GetResult_Response_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : result(_alloc, _init)
  {
    if (rosidl_runtime_cpp::MessageInitialization::ALL == _init ||
      rosidl_runtime_cpp::MessageInitialization::ZERO == _init)
    {
      this->status = 0;
    }
  }

  // field types and members
  using _status_type =
    int8_t;
  _status_type status;
  using _result_type =
    mrs_msgs::action::Replan_Result_<ContainerAllocator>;
  _result_type result;

  // setters for named parameter idiom
  Type & set__status(
    const int8_t & _arg)
  {
    this->status = _arg;
    return *this;
  }
  Type & set__result(
    const mrs_msgs::action::Replan_Result_<ContainerAllocator> & _arg)
  {
    this->result = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__action__Replan_GetResult_Response
    std::shared_ptr<mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__action__Replan_GetResult_Response
    std::shared_ptr<mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Replan_GetResult_Response_ & other) const
  {
    if (this->status != other.status) {
      return false;
    }
    if (this->result != other.result) {
      return false;
    }
    return true;
  }
  bool operator!=(const Replan_GetResult_Response_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Replan_GetResult_Response_

// alias to use template instance with default allocator
using Replan_GetResult_Response =
  mrs_msgs::action::Replan_GetResult_Response_<std::allocator<void>>;

// constant definitions

}  // namespace action

}  // namespace mrs_msgs


// Include directives for member types
// Member 'info'
// already included above
// #include "service_msgs/msg/detail/service_event_info__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__action__Replan_GetResult_Event __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__action__Replan_GetResult_Event __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace action
{

// message struct
template<class ContainerAllocator>
struct Replan_GetResult_Event_
{
  using Type = Replan_GetResult_Event_<ContainerAllocator>;

  explicit Replan_GetResult_Event_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_init)
  {
    (void)_init;
  }

  explicit Replan_GetResult_Event_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : info(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _info_type =
    service_msgs::msg::ServiceEventInfo_<ContainerAllocator>;
  _info_type info;
  using _request_type =
    rosidl_runtime_cpp::BoundedVector<mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator>>>;
  _request_type request;
  using _response_type =
    rosidl_runtime_cpp::BoundedVector<mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator>>>;
  _response_type response;

  // setters for named parameter idiom
  Type & set__info(
    const service_msgs::msg::ServiceEventInfo_<ContainerAllocator> & _arg)
  {
    this->info = _arg;
    return *this;
  }
  Type & set__request(
    const rosidl_runtime_cpp::BoundedVector<mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::action::Replan_GetResult_Request_<ContainerAllocator>>> & _arg)
  {
    this->request = _arg;
    return *this;
  }
  Type & set__response(
    const rosidl_runtime_cpp::BoundedVector<mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator>, 1, typename std::allocator_traits<ContainerAllocator>::template rebind_alloc<mrs_msgs::action::Replan_GetResult_Response_<ContainerAllocator>>> & _arg)
  {
    this->response = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::action::Replan_GetResult_Event_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::action::Replan_GetResult_Event_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_GetResult_Event_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_GetResult_Event_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_GetResult_Event_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_GetResult_Event_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_GetResult_Event_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_GetResult_Event_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_GetResult_Event_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_GetResult_Event_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__action__Replan_GetResult_Event
    std::shared_ptr<mrs_msgs::action::Replan_GetResult_Event_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__action__Replan_GetResult_Event
    std::shared_ptr<mrs_msgs::action::Replan_GetResult_Event_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Replan_GetResult_Event_ & other) const
  {
    if (this->info != other.info) {
      return false;
    }
    if (this->request != other.request) {
      return false;
    }
    if (this->response != other.response) {
      return false;
    }
    return true;
  }
  bool operator!=(const Replan_GetResult_Event_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Replan_GetResult_Event_

// alias to use template instance with default allocator
using Replan_GetResult_Event =
  mrs_msgs::action::Replan_GetResult_Event_<std::allocator<void>>;

// constant definitions

}  // namespace action

}  // namespace mrs_msgs

namespace mrs_msgs
{

namespace action
{

struct Replan_GetResult
{
  using Request = mrs_msgs::action::Replan_GetResult_Request;
  using Response = mrs_msgs::action::Replan_GetResult_Response;
  using Event = mrs_msgs::action::Replan_GetResult_Event;
};

}  // namespace action

}  // namespace mrs_msgs


// Include directives for member types
// Member 'goal_id'
// already included above
// #include "unique_identifier_msgs/msg/detail/uuid__struct.hpp"
// Member 'feedback'
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"

#ifndef _WIN32
# define DEPRECATED__mrs_msgs__action__Replan_FeedbackMessage __attribute__((deprecated))
#else
# define DEPRECATED__mrs_msgs__action__Replan_FeedbackMessage __declspec(deprecated)
#endif

namespace mrs_msgs
{

namespace action
{

// message struct
template<class ContainerAllocator>
struct Replan_FeedbackMessage_
{
  using Type = Replan_FeedbackMessage_<ContainerAllocator>;

  explicit Replan_FeedbackMessage_(rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : goal_id(_init),
    feedback(_init)
  {
    (void)_init;
  }

  explicit Replan_FeedbackMessage_(const ContainerAllocator & _alloc, rosidl_runtime_cpp::MessageInitialization _init = rosidl_runtime_cpp::MessageInitialization::ALL)
  : goal_id(_alloc, _init),
    feedback(_alloc, _init)
  {
    (void)_init;
  }

  // field types and members
  using _goal_id_type =
    unique_identifier_msgs::msg::UUID_<ContainerAllocator>;
  _goal_id_type goal_id;
  using _feedback_type =
    mrs_msgs::action::Replan_Feedback_<ContainerAllocator>;
  _feedback_type feedback;

  // setters for named parameter idiom
  Type & set__goal_id(
    const unique_identifier_msgs::msg::UUID_<ContainerAllocator> & _arg)
  {
    this->goal_id = _arg;
    return *this;
  }
  Type & set__feedback(
    const mrs_msgs::action::Replan_Feedback_<ContainerAllocator> & _arg)
  {
    this->feedback = _arg;
    return *this;
  }

  // constant declarations

  // pointer types
  using RawPtr =
    mrs_msgs::action::Replan_FeedbackMessage_<ContainerAllocator> *;
  using ConstRawPtr =
    const mrs_msgs::action::Replan_FeedbackMessage_<ContainerAllocator> *;
  using SharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_FeedbackMessage_<ContainerAllocator>>;
  using ConstSharedPtr =
    std::shared_ptr<mrs_msgs::action::Replan_FeedbackMessage_<ContainerAllocator> const>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_FeedbackMessage_<ContainerAllocator>>>
  using UniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_FeedbackMessage_<ContainerAllocator>, Deleter>;

  using UniquePtr = UniquePtrWithDeleter<>;

  template<typename Deleter = std::default_delete<
      mrs_msgs::action::Replan_FeedbackMessage_<ContainerAllocator>>>
  using ConstUniquePtrWithDeleter =
    std::unique_ptr<mrs_msgs::action::Replan_FeedbackMessage_<ContainerAllocator> const, Deleter>;
  using ConstUniquePtr = ConstUniquePtrWithDeleter<>;

  using WeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_FeedbackMessage_<ContainerAllocator>>;
  using ConstWeakPtr =
    std::weak_ptr<mrs_msgs::action::Replan_FeedbackMessage_<ContainerAllocator> const>;

  // pointer types similar to ROS 1, use SharedPtr / ConstSharedPtr instead
  // NOTE: Can't use 'using' here because GNU C++ can't parse attributes properly
  typedef DEPRECATED__mrs_msgs__action__Replan_FeedbackMessage
    std::shared_ptr<mrs_msgs::action::Replan_FeedbackMessage_<ContainerAllocator>>
    Ptr;
  typedef DEPRECATED__mrs_msgs__action__Replan_FeedbackMessage
    std::shared_ptr<mrs_msgs::action::Replan_FeedbackMessage_<ContainerAllocator> const>
    ConstPtr;

  // comparison operators
  bool operator==(const Replan_FeedbackMessage_ & other) const
  {
    if (this->goal_id != other.goal_id) {
      return false;
    }
    if (this->feedback != other.feedback) {
      return false;
    }
    return true;
  }
  bool operator!=(const Replan_FeedbackMessage_ & other) const
  {
    return !this->operator==(other);
  }
};  // struct Replan_FeedbackMessage_

// alias to use template instance with default allocator
using Replan_FeedbackMessage =
  mrs_msgs::action::Replan_FeedbackMessage_<std::allocator<void>>;

// constant definitions

}  // namespace action

}  // namespace mrs_msgs

#include "action_msgs/srv/cancel_goal.hpp"
#include "action_msgs/msg/goal_info.hpp"
#include "action_msgs/msg/goal_status_array.hpp"

namespace mrs_msgs
{

namespace action
{

struct Replan
{
  /// The goal message defined in the action definition.
  using Goal = mrs_msgs::action::Replan_Goal;
  /// The result message defined in the action definition.
  using Result = mrs_msgs::action::Replan_Result;
  /// The feedback message defined in the action definition.
  using Feedback = mrs_msgs::action::Replan_Feedback;

  struct Impl
  {
    /// The send_goal service using a wrapped version of the goal message as a request.
    using SendGoalService = mrs_msgs::action::Replan_SendGoal;
    /// The get_result service using a wrapped version of the result message as a response.
    using GetResultService = mrs_msgs::action::Replan_GetResult;
    /// The feedback message with generic fields which wraps the feedback message.
    using FeedbackMessage = mrs_msgs::action::Replan_FeedbackMessage;

    /// The generic service to cancel a goal.
    using CancelGoalService = action_msgs::srv::CancelGoal;
    /// The generic message for the status of a goal.
    using GoalStatusMessage = action_msgs::msg::GoalStatusArray;
  };
};

typedef struct Replan Replan;

}  // namespace action

}  // namespace mrs_msgs

#endif  // MRS_MSGS__ACTION__DETAIL__REPLAN__STRUCT_HPP_
