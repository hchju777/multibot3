// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from mrs_msgs:msg/Limits.idl
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
#include "mrs_msgs/msg/detail/limits__struct.h"
#include "mrs_msgs/msg/detail/limits__functions.h"


ROSIDL_GENERATOR_C_EXPORT
bool mrs_msgs__msg__limits__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[28];
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
    assert(strncmp("mrs_msgs.msg._limits.Limits", full_classname_dest, 27) == 0);
  }
  mrs_msgs__msg__Limits * ros_message = _ros_message;
  {  // a_max
    PyObject * field = PyObject_GetAttrString(_pymsg, "a_max");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->a_max = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // j_max
    PyObject * field = PyObject_GetAttrString(_pymsg, "j_max");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->j_max = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // min_separation_m
    PyObject * field = PyObject_GetAttrString(_pymsg, "min_separation_m");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->min_separation_m = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // omega_max_radps
    PyObject * field = PyObject_GetAttrString(_pymsg, "omega_max_radps");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->omega_max_radps = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }
  {  // yaw_accel_max_radps2
    PyObject * field = PyObject_GetAttrString(_pymsg, "yaw_accel_max_radps2");
    if (!field) {
      return false;
    }
    assert(PyFloat_Check(field));
    ros_message->yaw_accel_max_radps2 = PyFloat_AS_DOUBLE(field);
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * mrs_msgs__msg__limits__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of Limits */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("mrs_msgs.msg._limits");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "Limits");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  mrs_msgs__msg__Limits * ros_message = (mrs_msgs__msg__Limits *)raw_ros_message;
  {  // a_max
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->a_max);
    {
      int rc = PyObject_SetAttrString(_pymessage, "a_max", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // j_max
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->j_max);
    {
      int rc = PyObject_SetAttrString(_pymessage, "j_max", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // min_separation_m
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->min_separation_m);
    {
      int rc = PyObject_SetAttrString(_pymessage, "min_separation_m", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // omega_max_radps
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->omega_max_radps);
    {
      int rc = PyObject_SetAttrString(_pymessage, "omega_max_radps", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // yaw_accel_max_radps2
    PyObject * field = NULL;
    field = PyFloat_FromDouble(ros_message->yaw_accel_max_radps2);
    {
      int rc = PyObject_SetAttrString(_pymessage, "yaw_accel_max_radps2", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}
