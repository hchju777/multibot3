// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from mrs_msgs:msg/RoadmapEdge.idl
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
#include "mrs_msgs/msg/detail/roadmap_edge__struct.h"
#include "mrs_msgs/msg/detail/roadmap_edge__functions.h"

#include "rosidl_runtime_c/string.h"
#include "rosidl_runtime_c/string_functions.h"


ROSIDL_GENERATOR_C_EXPORT
bool mrs_msgs__msg__roadmap_edge__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[39];
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
    assert(strncmp("mrs_msgs.msg._roadmap_edge.RoadmapEdge", full_classname_dest, 38) == 0);
  }
  mrs_msgs__msg__RoadmapEdge * ros_message = _ros_message;
  {  // from_id
    PyObject * field = PyObject_GetAttrString(_pymsg, "from_id");
    if (!field) {
      return false;
    }
    assert(PyUnicode_Check(field));
    PyObject * encoded_field = PyUnicode_AsUTF8String(field);
    if (!encoded_field) {
      Py_DECREF(field);
      return false;
    }
    rosidl_runtime_c__String__assign(&ros_message->from_id, PyBytes_AS_STRING(encoded_field));
    Py_DECREF(encoded_field);
    Py_DECREF(field);
  }
  {  // to_id
    PyObject * field = PyObject_GetAttrString(_pymsg, "to_id");
    if (!field) {
      return false;
    }
    assert(PyUnicode_Check(field));
    PyObject * encoded_field = PyUnicode_AsUTF8String(field);
    if (!encoded_field) {
      Py_DECREF(field);
      return false;
    }
    rosidl_runtime_c__String__assign(&ros_message->to_id, PyBytes_AS_STRING(encoded_field));
    Py_DECREF(encoded_field);
    Py_DECREF(field);
  }
  {  // traversal
    PyObject * field = PyObject_GetAttrString(_pymsg, "traversal");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->traversal = (uint8_t)PyLong_AsUnsignedLong(field);
    Py_DECREF(field);
  }
  {  // length_m
    PyObject * field = PyObject_GetAttrString(_pymsg, "length_m");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->length_m = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // width_m
    PyObject * field = PyObject_GetAttrString(_pymsg, "width_m");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->width_m = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // capacity_robots
    PyObject * field = PyObject_GetAttrString(_pymsg, "capacity_robots");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->capacity_robots = (uint8_t)PyLong_AsUnsignedLong(field);
    Py_DECREF(field);
  }
  {  // corridor
    PyObject * field = PyObject_GetAttrString(_pymsg, "corridor");
    if (!field) {
      return false;
    }
    assert(PyUnicode_Check(field));
    PyObject * encoded_field = PyUnicode_AsUTF8String(field);
    if (!encoded_field) {
      Py_DECREF(field);
      return false;
    }
    rosidl_runtime_c__String__assign(&ros_message->corridor, PyBytes_AS_STRING(encoded_field));
    Py_DECREF(encoded_field);
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * mrs_msgs__msg__roadmap_edge__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of RoadmapEdge */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("mrs_msgs.msg._roadmap_edge");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "RoadmapEdge");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  mrs_msgs__msg__RoadmapEdge * ros_message = (mrs_msgs__msg__RoadmapEdge *)raw_ros_message;
  {  // from_id
    PyObject * field = NULL;
    field = PyUnicode_DecodeUTF8(
      ros_message->from_id.data,
      strlen(ros_message->from_id.data),
      "replace");
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "from_id", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // to_id
    PyObject * field = NULL;
    field = PyUnicode_DecodeUTF8(
      ros_message->to_id.data,
      strlen(ros_message->to_id.data),
      "replace");
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "to_id", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // traversal
    PyObject * field = NULL;
    field = PyLong_FromUnsignedLong(ros_message->traversal);
    {
      int rc = PyObject_SetAttrString(_pymessage, "traversal", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // length_m
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->length_m);
    {
      int rc = PyObject_SetAttrString(_pymessage, "length_m", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // width_m
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->width_m);
    {
      int rc = PyObject_SetAttrString(_pymessage, "width_m", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // capacity_robots
    PyObject * field = NULL;
    field = PyLong_FromUnsignedLong(ros_message->capacity_robots);
    {
      int rc = PyObject_SetAttrString(_pymessage, "capacity_robots", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // corridor
    PyObject * field = NULL;
    field = PyUnicode_DecodeUTF8(
      ros_message->corridor.data,
      strlen(ros_message->corridor.data),
      "replace");
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "corridor", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}
