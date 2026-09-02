#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mrs_core::mrs_core_msgs" for configuration ""
set_property(TARGET mrs_core::mrs_core_msgs APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(mrs_core::mrs_core_msgs PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libmrs_core_msgs.a"
  )

list(APPEND _cmake_import_check_targets mrs_core::mrs_core_msgs )
list(APPEND _cmake_import_check_files_for_mrs_core::mrs_core_msgs "${_IMPORT_PREFIX}/lib/libmrs_core_msgs.a" )

# Import target "mrs_core::startup_checker" for configuration ""
set_property(TARGET mrs_core::startup_checker APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(mrs_core::startup_checker PROPERTIES
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/mrs_core/startup_checker"
  )

list(APPEND _cmake_import_check_targets mrs_core::startup_checker )
list(APPEND _cmake_import_check_files_for_mrs_core::startup_checker "${_IMPORT_PREFIX}/lib/mrs_core/startup_checker" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
