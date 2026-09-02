# generated from rosidl_cmake/cmake/rosidl_cmake_aggregate_target-extras.cmake.in

# Create a convenience aggregate target mrs_msgs::mrs_msgs
# that links all generated interface targets, so downstream packages can use
# a single modern CMake target name instead of ${mrs_msgs_TARGETS}.
if(mrs_msgs_TARGETS AND NOT TARGET mrs_msgs::mrs_msgs)
  add_library(mrs_msgs::mrs_msgs INTERFACE IMPORTED)
  set_target_properties(mrs_msgs::mrs_msgs PROPERTIES
    INTERFACE_LINK_LIBRARIES "${mrs_msgs_TARGETS}")
endif()
