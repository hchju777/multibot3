// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from mrs_msgs:msg/RobotSpec.idl
// generated code does not contain a copyright notice
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include <Python.h>
#include <stdbool.h>
#ifndef _WIN32
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wunused-function"
#endif
#include "numpy/ndarrayobject.h"
#ifndef _WIN32
# pragma GCC diagnostic pop
#endif
#include "rosidl_runtime_c/visibility_control.h"
#include "mrs_msgs/msg/detail/robot_spec__struct.h"
#include "mrs_msgs/msg/detail/robot_spec__functions.h"

#include "rosidl_runtime_c/string.h"
#include "rosidl_runtime_c/string_functions.h"


ROSIDL_GENERATOR_C_EXPORT
bool mrs_msgs__msg__robot_spec__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[35];
    {
      char * class_name = NULL;
      char * module_name = NULL;
      {
        PyObject * class_attr = PyObject_GetAttrString(_pymsg, "__class__");
        if (class_attr) {
          PyObject * name_attr = PyObject_GetAttrString(class_attr, "__name__");
          if (name_attr) {
            class_name = (char *)PyUnicode_1BYTE_DATA(name_attr);
            Py_DECREF(name_attr);
          }
          PyObject * module_attr = PyObject_GetAttrString(class_attr, "__module__");
          if (module_attr) {
            module_name = (char *)PyUnicode_1BYTE_DATA(module_attr);
            Py_DECREF(module_attr);
          }
          Py_DECREF(class_attr);
        }
      }
      if (!class_name || !module_name) {
        return false;
      }
      snprintf(full_classname_dest, sizeof(full_classname_dest), "%s.%s", module_name, class_name);
    }
    assert(strncmp("mrs_msgs.msg._robot_spec.RobotSpec", full_classname_dest, 34) == 0);
  }
  mrs_msgs__msg__RobotSpec * ros_message = _ros_message;
  {  // robot
    PyObject * field = PyObject_GetAttrString(_pymsg, "robot");
    if (!field) {
      return false;
    }
    assert(PyUnicode_Check(field));
    PyObject * encoded_field = PyUnicode_AsUTF8String(field);
    if (!encoded_field) {
      Py_DECREF(field);
      return false;
    }
    rosidl_runtime_c__String__assign(&ros_message->robot, PyBytes_AS_STRING(encoded_field));
    Py_DECREF(encoded_field);
    Py_DECREF(field);
  }
  {  // avg_traversal_speed_mps
    PyObject * field = PyObject_GetAttrString(_pymsg, "avg_traversal_speed_mps");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->avg_traversal_speed_mps = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // v_max
    PyObject * field = PyObject_GetAttrString(_pymsg, "v_max");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->v_max = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // brake_decel_min_mps2
    PyObject * field = PyObject_GetAttrString(_pymsg, "brake_decel_min_mps2");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->brake_decel_min_mps2 = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // curvature_max_invm
    PyObject * field = PyObject_GetAttrString(_pymsg, "curvature_max_invm");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->curvature_max_invm = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // reverse_motion_allowed
    PyObject * field = PyObject_GetAttrString(_pymsg, "reverse_motion_allowed");
    if (!field) {
      return false;
    }
    assert(PyBool_Check(field));
    ros_message->reverse_motion_allowed = (Py_True == field);
    Py_DECREF(field);
  }
  {  // circumradius_m
    PyObject * field = PyObject_GetAttrString(_pymsg, "circumradius_m");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->circumradius_m = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * mrs_msgs__msg__robot_spec__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of RobotSpec */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("mrs_msgs.msg._robot_spec");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "RobotSpec");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  mrs_msgs__msg__RobotSpec * ros_message = (mrs_msgs__msg__RobotSpec *)raw_ros_message;
  {  // robot
    PyObject * field = NULL;
    field = PyUnicode_DecodeUTF8(
      ros_message->robot.data,
      strlen(ros_message->robot.data),
      "replace");
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "robot", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // avg_traversal_speed_mps
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->avg_traversal_speed_mps);
    {
      int rc = PyObject_SetAttrString(_pymessage, "avg_traversal_speed_mps", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // v_max
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->v_max);
    {
      int rc = PyObject_SetAttrString(_pymessage, "v_max", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // brake_decel_min_mps2
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->brake_decel_min_mps2);
    {
      int rc = PyObject_SetAttrString(_pymessage, "brake_decel_min_mps2", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // curvature_max_invm
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->curvature_max_invm);
    {
      int rc = PyObject_SetAttrString(_pymessage, "curvature_max_invm", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // reverse_motion_allowed
    PyObject * field = NULL;
    field = PyBool_FromLong(ros_message->reverse_motion_allowed ? 1 : 0);
    {
      int rc = PyObject_SetAttrString(_pymessage, "reverse_motion_allowed", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // circumradius_m
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->circumradius_m);
    {
      int rc = PyObject_SetAttrString(_pymessage, "circumradius_m", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}
