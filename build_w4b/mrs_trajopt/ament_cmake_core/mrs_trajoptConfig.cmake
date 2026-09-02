# generated from ament/cmake/core/templates/nameConfig.cmake.in

# prevent multiple inclusion
if(_mrs_trajopt_CONFIG_INCLUDED)
  # ensure to keep the found flag the same
  if(NOT DEFINED mrs_trajopt_FOUND)
    # explicitly set it to FALSE, otherwise CMake will set it to TRUE
    set(mrs_trajopt_FOUND FALSE)
  elseif(NOT mrs_trajopt_FOUND)
    # use separate condition to avoid uninitialized variable warning
    set(mrs_trajopt_FOUND FALSE)
  endif()
  return()
endif()
set(_mrs_trajopt_CONFIG_INCLUDED TRUE)

# output package information
if(NOT mrs_trajopt_FIND_QUIETLY)
  message(STATUS "Found mrs_trajopt: 0.1.0 (${mrs_trajopt_DIR})")
endif()

# warn when using a deprecated package
if(NOT "" STREQUAL "")
  set(_msg "Package 'mrs_trajopt' is deprecated")
  # append custom deprecation text if available
  if(NOT "" STREQUAL "TRUE")
    set(_msg "${_msg} ()")
  endif()
  # optionally quiet the deprecation message
  if(NOT mrs_trajopt_DEPRECATED_QUIET)
    message(DEPRECATION "${_msg}")
  endif()
endif()

# flag package as ament-based to distinguish it after being find_package()-ed
set(mrs_trajopt_FOUND_AMENT_PACKAGE TRUE)

# include all config extra files
set(_extras "")
foreach(_extra ${_extras})
  include("${mrs_trajopt_DIR}/${_extra}")
endforeach()
