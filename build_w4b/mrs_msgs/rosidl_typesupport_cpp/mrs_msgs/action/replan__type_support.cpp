// generated from rosidl_typesupport_cpp/resource/idl__type_support.cpp.em
// with input from mrs_msgs:action/Replan.idl
// generated code does not contain a copyright notice

#include "cstddef"
#include "rosidl_runtime_c/message_type_support_struct.h"
#include "mrs_msgs/action/detail/replan__functions.h"
#include "mrs_msgs/action/detail/replan__struct.hpp"
#include "rosidl_typesupport_cpp/identifier.hpp"
#include "rosidl_typesupport_cpp/message_type_support.hpp"
#include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
#include "rosidl_typesupport_cpp/visibility_control.h"
#include "rosidl_typesupport_interface/macros.h"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _Replan_Goal_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _Replan_Goal_type_support_ids_t;

static const _Replan_Goal_type_support_ids_t _Replan_Goal_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _Replan_Goal_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _Replan_Goal_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _Replan_Goal_type_support_symbol_names_t _Replan_Goal_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mrs_msgs, action, Replan_Goal)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, action, Replan_Goal)),
  }
};

typedef struct _Replan_Goal_type_support_data_t
{
  void * data[2];
} _Replan_Goal_type_support_data_t;

static _Replan_Goal_type_support_data_t _Replan_Goal_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _Replan_Goal_message_typesupport_map = {
  2,
  "mrs_msgs",
  &_Replan_Goal_message_typesupport_ids.typesupport_identifier[0],
  &_Replan_Goal_message_typesupport_symbol_names.symbol_name[0],
  &_Replan_Goal_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t Replan_Goal_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_Replan_Goal_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_Goal__get_type_hash,
  &mrs_msgs__action__Replan_Goal__get_type_description,
  &mrs_msgs__action__Replan_Goal__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::action::Replan_Goal>()
{
  return &::mrs_msgs::action::rosidl_typesupport_cpp::Replan_Goal_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan_Goal)() {
  return get_message_type_support_handle<mrs_msgs::action::Replan_Goal>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _Replan_Result_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _Replan_Result_type_support_ids_t;

static const _Replan_Result_type_support_ids_t _Replan_Result_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _Replan_Result_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _Replan_Result_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _Replan_Result_type_support_symbol_names_t _Replan_Result_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mrs_msgs, action, Replan_Result)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, action, Replan_Result)),
  }
};

typedef struct _Replan_Result_type_support_data_t
{
  void * data[2];
} _Replan_Result_type_support_data_t;

static _Replan_Result_type_support_data_t _Replan_Result_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _Replan_Result_message_typesupport_map = {
  2,
  "mrs_msgs",
  &_Replan_Result_message_typesupport_ids.typesupport_identifier[0],
  &_Replan_Result_message_typesupport_symbol_names.symbol_name[0],
  &_Replan_Result_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t Replan_Result_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_Replan_Result_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_Result__get_type_hash,
  &mrs_msgs__action__Replan_Result__get_type_description,
  &mrs_msgs__action__Replan_Result__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::action::Replan_Result>()
{
  return &::mrs_msgs::action::rosidl_typesupport_cpp::Replan_Result_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan_Result)() {
  return get_message_type_support_handle<mrs_msgs::action::Replan_Result>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _Replan_Feedback_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _Replan_Feedback_type_support_ids_t;

static const _Replan_Feedback_type_support_ids_t _Replan_Feedback_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _Replan_Feedback_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _Replan_Feedback_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _Replan_Feedback_type_support_symbol_names_t _Replan_Feedback_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mrs_msgs, action, Replan_Feedback)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, action, Replan_Feedback)),
  }
};

typedef struct _Replan_Feedback_type_support_data_t
{
  void * data[2];
} _Replan_Feedback_type_support_data_t;

static _Replan_Feedback_type_support_data_t _Replan_Feedback_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _Replan_Feedback_message_typesupport_map = {
  2,
  "mrs_msgs",
  &_Replan_Feedback_message_typesupport_ids.typesupport_identifier[0],
  &_Replan_Feedback_message_typesupport_symbol_names.symbol_name[0],
  &_Replan_Feedback_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t Replan_Feedback_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_Replan_Feedback_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_Feedback__get_type_hash,
  &mrs_msgs__action__Replan_Feedback__get_type_description,
  &mrs_msgs__action__Replan_Feedback__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::action::Replan_Feedback>()
{
  return &::mrs_msgs::action::rosidl_typesupport_cpp::Replan_Feedback_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan_Feedback)() {
  return get_message_type_support_handle<mrs_msgs::action::Replan_Feedback>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _Replan_SendGoal_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _Replan_SendGoal_Request_type_support_ids_t;

static const _Replan_SendGoal_Request_type_support_ids_t _Replan_SendGoal_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _Replan_SendGoal_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _Replan_SendGoal_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _Replan_SendGoal_Request_type_support_symbol_names_t _Replan_SendGoal_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mrs_msgs, action, Replan_SendGoal_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, action, Replan_SendGoal_Request)),
  }
};

typedef struct _Replan_SendGoal_Request_type_support_data_t
{
  void * data[2];
} _Replan_SendGoal_Request_type_support_data_t;

static _Replan_SendGoal_Request_type_support_data_t _Replan_SendGoal_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _Replan_SendGoal_Request_message_typesupport_map = {
  2,
  "mrs_msgs",
  &_Replan_SendGoal_Request_message_typesupport_ids.typesupport_identifier[0],
  &_Replan_SendGoal_Request_message_typesupport_symbol_names.symbol_name[0],
  &_Replan_SendGoal_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t Replan_SendGoal_Request_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_Replan_SendGoal_Request_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_SendGoal_Request__get_type_hash,
  &mrs_msgs__action__Replan_SendGoal_Request__get_type_description,
  &mrs_msgs__action__Replan_SendGoal_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::action::Replan_SendGoal_Request>()
{
  return &::mrs_msgs::action::rosidl_typesupport_cpp::Replan_SendGoal_Request_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan_SendGoal_Request)() {
  return get_message_type_support_handle<mrs_msgs::action::Replan_SendGoal_Request>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _Replan_SendGoal_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _Replan_SendGoal_Response_type_support_ids_t;

static const _Replan_SendGoal_Response_type_support_ids_t _Replan_SendGoal_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _Replan_SendGoal_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _Replan_SendGoal_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _Replan_SendGoal_Response_type_support_symbol_names_t _Replan_SendGoal_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mrs_msgs, action, Replan_SendGoal_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, action, Replan_SendGoal_Response)),
  }
};

typedef struct _Replan_SendGoal_Response_type_support_data_t
{
  void * data[2];
} _Replan_SendGoal_Response_type_support_data_t;

static _Replan_SendGoal_Response_type_support_data_t _Replan_SendGoal_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _Replan_SendGoal_Response_message_typesupport_map = {
  2,
  "mrs_msgs",
  &_Replan_SendGoal_Response_message_typesupport_ids.typesupport_identifier[0],
  &_Replan_SendGoal_Response_message_typesupport_symbol_names.symbol_name[0],
  &_Replan_SendGoal_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t Replan_SendGoal_Response_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_Replan_SendGoal_Response_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_SendGoal_Response__get_type_hash,
  &mrs_msgs__action__Replan_SendGoal_Response__get_type_description,
  &mrs_msgs__action__Replan_SendGoal_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::action::Replan_SendGoal_Response>()
{
  return &::mrs_msgs::action::rosidl_typesupport_cpp::Replan_SendGoal_Response_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan_SendGoal_Response)() {
  return get_message_type_support_handle<mrs_msgs::action::Replan_SendGoal_Response>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _Replan_SendGoal_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _Replan_SendGoal_Event_type_support_ids_t;

static const _Replan_SendGoal_Event_type_support_ids_t _Replan_SendGoal_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _Replan_SendGoal_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _Replan_SendGoal_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _Replan_SendGoal_Event_type_support_symbol_names_t _Replan_SendGoal_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mrs_msgs, action, Replan_SendGoal_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, action, Replan_SendGoal_Event)),
  }
};

typedef struct _Replan_SendGoal_Event_type_support_data_t
{
  void * data[2];
} _Replan_SendGoal_Event_type_support_data_t;

static _Replan_SendGoal_Event_type_support_data_t _Replan_SendGoal_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _Replan_SendGoal_Event_message_typesupport_map = {
  2,
  "mrs_msgs",
  &_Replan_SendGoal_Event_message_typesupport_ids.typesupport_identifier[0],
  &_Replan_SendGoal_Event_message_typesupport_symbol_names.symbol_name[0],
  &_Replan_SendGoal_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t Replan_SendGoal_Event_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_Replan_SendGoal_Event_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_SendGoal_Event__get_type_hash,
  &mrs_msgs__action__Replan_SendGoal_Event__get_type_description,
  &mrs_msgs__action__Replan_SendGoal_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::action::Replan_SendGoal_Event>()
{
  return &::mrs_msgs::action::rosidl_typesupport_cpp::Replan_SendGoal_Event_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan_SendGoal_Event)() {
  return get_message_type_support_handle<mrs_msgs::action::Replan_SendGoal_Event>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
#include "rosidl_runtime_c/service_type_support_struct.h"
#include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
#include "rosidl_typesupport_cpp/service_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _Replan_SendGoal_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _Replan_SendGoal_type_support_ids_t;

static const _Replan_SendGoal_type_support_ids_t _Replan_SendGoal_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _Replan_SendGoal_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _Replan_SendGoal_type_support_symbol_names_t;
#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _Replan_SendGoal_type_support_symbol_names_t _Replan_SendGoal_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mrs_msgs, action, Replan_SendGoal)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, action, Replan_SendGoal)),
  }
};

typedef struct _Replan_SendGoal_type_support_data_t
{
  void * data[2];
} _Replan_SendGoal_type_support_data_t;

static _Replan_SendGoal_type_support_data_t _Replan_SendGoal_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _Replan_SendGoal_service_typesupport_map = {
  2,
  "mrs_msgs",
  &_Replan_SendGoal_service_typesupport_ids.typesupport_identifier[0],
  &_Replan_SendGoal_service_typesupport_symbol_names.symbol_name[0],
  &_Replan_SendGoal_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t Replan_SendGoal_service_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_Replan_SendGoal_service_typesupport_map),
  ::rosidl_typesupport_cpp::get_service_typesupport_handle_function,
  ::rosidl_typesupport_cpp::get_message_type_support_handle<mrs_msgs::action::Replan_SendGoal_Request>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<mrs_msgs::action::Replan_SendGoal_Response>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<mrs_msgs::action::Replan_SendGoal_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<mrs_msgs::action::Replan_SendGoal>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<mrs_msgs::action::Replan_SendGoal>,
  &mrs_msgs__action__Replan_SendGoal__get_type_hash,
  &mrs_msgs__action__Replan_SendGoal__get_type_description,
  &mrs_msgs__action__Replan_SendGoal__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<mrs_msgs::action::Replan_SendGoal>()
{
  return &::mrs_msgs::action::rosidl_typesupport_cpp::Replan_SendGoal_service_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan_SendGoal)() {
  return ::rosidl_typesupport_cpp::get_service_type_support_handle<mrs_msgs::action::Replan_SendGoal>();
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _Replan_GetResult_Request_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _Replan_GetResult_Request_type_support_ids_t;

static const _Replan_GetResult_Request_type_support_ids_t _Replan_GetResult_Request_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _Replan_GetResult_Request_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _Replan_GetResult_Request_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _Replan_GetResult_Request_type_support_symbol_names_t _Replan_GetResult_Request_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mrs_msgs, action, Replan_GetResult_Request)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, action, Replan_GetResult_Request)),
  }
};

typedef struct _Replan_GetResult_Request_type_support_data_t
{
  void * data[2];
} _Replan_GetResult_Request_type_support_data_t;

static _Replan_GetResult_Request_type_support_data_t _Replan_GetResult_Request_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _Replan_GetResult_Request_message_typesupport_map = {
  2,
  "mrs_msgs",
  &_Replan_GetResult_Request_message_typesupport_ids.typesupport_identifier[0],
  &_Replan_GetResult_Request_message_typesupport_symbol_names.symbol_name[0],
  &_Replan_GetResult_Request_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t Replan_GetResult_Request_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_Replan_GetResult_Request_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_GetResult_Request__get_type_hash,
  &mrs_msgs__action__Replan_GetResult_Request__get_type_description,
  &mrs_msgs__action__Replan_GetResult_Request__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::action::Replan_GetResult_Request>()
{
  return &::mrs_msgs::action::rosidl_typesupport_cpp::Replan_GetResult_Request_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan_GetResult_Request)() {
  return get_message_type_support_handle<mrs_msgs::action::Replan_GetResult_Request>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _Replan_GetResult_Response_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _Replan_GetResult_Response_type_support_ids_t;

static const _Replan_GetResult_Response_type_support_ids_t _Replan_GetResult_Response_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _Replan_GetResult_Response_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _Replan_GetResult_Response_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _Replan_GetResult_Response_type_support_symbol_names_t _Replan_GetResult_Response_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mrs_msgs, action, Replan_GetResult_Response)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, action, Replan_GetResult_Response)),
  }
};

typedef struct _Replan_GetResult_Response_type_support_data_t
{
  void * data[2];
} _Replan_GetResult_Response_type_support_data_t;

static _Replan_GetResult_Response_type_support_data_t _Replan_GetResult_Response_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _Replan_GetResult_Response_message_typesupport_map = {
  2,
  "mrs_msgs",
  &_Replan_GetResult_Response_message_typesupport_ids.typesupport_identifier[0],
  &_Replan_GetResult_Response_message_typesupport_symbol_names.symbol_name[0],
  &_Replan_GetResult_Response_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t Replan_GetResult_Response_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_Replan_GetResult_Response_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_GetResult_Response__get_type_hash,
  &mrs_msgs__action__Replan_GetResult_Response__get_type_description,
  &mrs_msgs__action__Replan_GetResult_Response__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::action::Replan_GetResult_Response>()
{
  return &::mrs_msgs::action::rosidl_typesupport_cpp::Replan_GetResult_Response_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan_GetResult_Response)() {
  return get_message_type_support_handle<mrs_msgs::action::Replan_GetResult_Response>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _Replan_GetResult_Event_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _Replan_GetResult_Event_type_support_ids_t;

static const _Replan_GetResult_Event_type_support_ids_t _Replan_GetResult_Event_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _Replan_GetResult_Event_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _Replan_GetResult_Event_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _Replan_GetResult_Event_type_support_symbol_names_t _Replan_GetResult_Event_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mrs_msgs, action, Replan_GetResult_Event)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, action, Replan_GetResult_Event)),
  }
};

typedef struct _Replan_GetResult_Event_type_support_data_t
{
  void * data[2];
} _Replan_GetResult_Event_type_support_data_t;

static _Replan_GetResult_Event_type_support_data_t _Replan_GetResult_Event_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _Replan_GetResult_Event_message_typesupport_map = {
  2,
  "mrs_msgs",
  &_Replan_GetResult_Event_message_typesupport_ids.typesupport_identifier[0],
  &_Replan_GetResult_Event_message_typesupport_symbol_names.symbol_name[0],
  &_Replan_GetResult_Event_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t Replan_GetResult_Event_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_Replan_GetResult_Event_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_GetResult_Event__get_type_hash,
  &mrs_msgs__action__Replan_GetResult_Event__get_type_description,
  &mrs_msgs__action__Replan_GetResult_Event__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::action::Replan_GetResult_Event>()
{
  return &::mrs_msgs::action::rosidl_typesupport_cpp::Replan_GetResult_Event_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan_GetResult_Event)() {
  return get_message_type_support_handle<mrs_msgs::action::Replan_GetResult_Event>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/service_type_support_struct.h"
// already included above
// #include "rosidl_typesupport_cpp/service_type_support.hpp"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/service_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _Replan_GetResult_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _Replan_GetResult_type_support_ids_t;

static const _Replan_GetResult_type_support_ids_t _Replan_GetResult_service_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _Replan_GetResult_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _Replan_GetResult_type_support_symbol_names_t;
#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _Replan_GetResult_type_support_symbol_names_t _Replan_GetResult_service_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mrs_msgs, action, Replan_GetResult)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, action, Replan_GetResult)),
  }
};

typedef struct _Replan_GetResult_type_support_data_t
{
  void * data[2];
} _Replan_GetResult_type_support_data_t;

static _Replan_GetResult_type_support_data_t _Replan_GetResult_service_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _Replan_GetResult_service_typesupport_map = {
  2,
  "mrs_msgs",
  &_Replan_GetResult_service_typesupport_ids.typesupport_identifier[0],
  &_Replan_GetResult_service_typesupport_symbol_names.symbol_name[0],
  &_Replan_GetResult_service_typesupport_data.data[0],
};

static const rosidl_service_type_support_t Replan_GetResult_service_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_Replan_GetResult_service_typesupport_map),
  ::rosidl_typesupport_cpp::get_service_typesupport_handle_function,
  ::rosidl_typesupport_cpp::get_message_type_support_handle<mrs_msgs::action::Replan_GetResult_Request>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<mrs_msgs::action::Replan_GetResult_Response>(),
  ::rosidl_typesupport_cpp::get_message_type_support_handle<mrs_msgs::action::Replan_GetResult_Event>(),
  &::rosidl_typesupport_cpp::service_create_event_message<mrs_msgs::action::Replan_GetResult>,
  &::rosidl_typesupport_cpp::service_destroy_event_message<mrs_msgs::action::Replan_GetResult>,
  &mrs_msgs__action__Replan_GetResult__get_type_hash,
  &mrs_msgs__action__Replan_GetResult__get_type_description,
  &mrs_msgs__action__Replan_GetResult__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
get_service_type_support_handle<mrs_msgs::action::Replan_GetResult>()
{
  return &::mrs_msgs::action::rosidl_typesupport_cpp::Replan_GetResult_service_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_service_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__SERVICE_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan_GetResult)() {
  return ::rosidl_typesupport_cpp::get_service_type_support_handle<mrs_msgs::action::Replan_GetResult>();
}

#ifdef __cplusplus
}
#endif

// already included above
// #include "cstddef"
// already included above
// #include "rosidl_runtime_c/message_type_support_struct.h"
// already included above
// #include "mrs_msgs/action/detail/replan__functions.h"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/identifier.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_c/type_support_map.h"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support_dispatch.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
// already included above
// #include "rosidl_typesupport_interface/macros.h"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

typedef struct _Replan_FeedbackMessage_type_support_ids_t
{
  const char * typesupport_identifier[2];
} _Replan_FeedbackMessage_type_support_ids_t;

static const _Replan_FeedbackMessage_type_support_ids_t _Replan_FeedbackMessage_message_typesupport_ids = {
  {
    "rosidl_typesupport_fastrtps_cpp",  // ::rosidl_typesupport_fastrtps_cpp::typesupport_identifier,
    "rosidl_typesupport_introspection_cpp",  // ::rosidl_typesupport_introspection_cpp::typesupport_identifier,
  }
};

typedef struct _Replan_FeedbackMessage_type_support_symbol_names_t
{
  const char * symbol_name[2];
} _Replan_FeedbackMessage_type_support_symbol_names_t;

#define STRINGIFY_(s) #s
#define STRINGIFY(s) STRINGIFY_(s)

static const _Replan_FeedbackMessage_type_support_symbol_names_t _Replan_FeedbackMessage_message_typesupport_symbol_names = {
  {
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_fastrtps_cpp, mrs_msgs, action, Replan_FeedbackMessage)),
    STRINGIFY(ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_introspection_cpp, mrs_msgs, action, Replan_FeedbackMessage)),
  }
};

typedef struct _Replan_FeedbackMessage_type_support_data_t
{
  void * data[2];
} _Replan_FeedbackMessage_type_support_data_t;

static _Replan_FeedbackMessage_type_support_data_t _Replan_FeedbackMessage_message_typesupport_data = {
  {
    0,  // will store the shared library later
    0,  // will store the shared library later
  }
};

static const type_support_map_t _Replan_FeedbackMessage_message_typesupport_map = {
  2,
  "mrs_msgs",
  &_Replan_FeedbackMessage_message_typesupport_ids.typesupport_identifier[0],
  &_Replan_FeedbackMessage_message_typesupport_symbol_names.symbol_name[0],
  &_Replan_FeedbackMessage_message_typesupport_data.data[0],
};

static const rosidl_message_type_support_t Replan_FeedbackMessage_message_type_support_handle = {
  ::rosidl_typesupport_cpp::typesupport_identifier,
  reinterpret_cast<const type_support_map_t *>(&_Replan_FeedbackMessage_message_typesupport_map),
  ::rosidl_typesupport_cpp::get_message_typesupport_handle_function,
  &mrs_msgs__action__Replan_FeedbackMessage__get_type_hash,
  &mrs_msgs__action__Replan_FeedbackMessage__get_type_description,
  &mrs_msgs__action__Replan_FeedbackMessage__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
get_message_type_support_handle<mrs_msgs::action::Replan_FeedbackMessage>()
{
  return &::mrs_msgs::action::rosidl_typesupport_cpp::Replan_FeedbackMessage_message_type_support_handle;
}

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_message_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__MESSAGE_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan_FeedbackMessage)() {
  return get_message_type_support_handle<mrs_msgs::action::Replan_FeedbackMessage>();
}

#ifdef __cplusplus
}
#endif
}  // namespace rosidl_typesupport_cpp

#include "action_msgs/msg/goal_status_array.hpp"
#include "action_msgs/srv/cancel_goal.hpp"
// already included above
// #include "mrs_msgs/action/detail/replan__struct.hpp"
// already included above
// #include "rosidl_typesupport_cpp/visibility_control.h"
#include "rosidl_runtime_c/action_type_support_struct.h"
#include "rosidl_typesupport_cpp/action_type_support.hpp"
// already included above
// #include "rosidl_typesupport_cpp/message_type_support.hpp"
// already included above
// #include "rosidl_typesupport_cpp/service_type_support.hpp"

namespace mrs_msgs
{

namespace action
{

namespace rosidl_typesupport_cpp
{

static rosidl_action_type_support_t Replan_action_type_support_handle = {
  NULL, NULL, NULL, NULL, NULL,
  &mrs_msgs__action__Replan__get_type_hash,
  &mrs_msgs__action__Replan__get_type_description,
  &mrs_msgs__action__Replan__get_type_description_sources,
};

}  // namespace rosidl_typesupport_cpp

}  // namespace action

}  // namespace mrs_msgs

namespace rosidl_typesupport_cpp
{

template<>
ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_action_type_support_t *
get_action_type_support_handle<mrs_msgs::action::Replan>()
{
  using ::mrs_msgs::action::rosidl_typesupport_cpp::Replan_action_type_support_handle;
  // Thread-safe by always writing the same values to the static struct
  Replan_action_type_support_handle.goal_service_type_support = get_service_type_support_handle<::mrs_msgs::action::Replan::Impl::SendGoalService>();
  Replan_action_type_support_handle.result_service_type_support = get_service_type_support_handle<::mrs_msgs::action::Replan::Impl::GetResultService>();
  Replan_action_type_support_handle.cancel_service_type_support = get_service_type_support_handle<::mrs_msgs::action::Replan::Impl::CancelGoalService>();
  Replan_action_type_support_handle.feedback_message_type_support = get_message_type_support_handle<::mrs_msgs::action::Replan::Impl::FeedbackMessage>();
  Replan_action_type_support_handle.status_message_type_support = get_message_type_support_handle<::mrs_msgs::action::Replan::Impl::GoalStatusMessage>();
  return &Replan_action_type_support_handle;
}

}  // namespace rosidl_typesupport_cpp

#ifdef __cplusplus
extern "C"
{
#endif

ROSIDL_TYPESUPPORT_CPP_PUBLIC
const rosidl_action_type_support_t *
ROSIDL_TYPESUPPORT_INTERFACE__ACTION_SYMBOL_NAME(rosidl_typesupport_cpp, mrs_msgs, action, Replan)() {
  return ::rosidl_typesupport_cpp::get_action_type_support_handle<mrs_msgs::action::Replan>();
}

#ifdef __cplusplus
}
#endif
