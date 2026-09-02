// generated from rosidl_generator_py/resource/_idl_support.c.em
// with input from mrs_msgs:msg/DiscretePlan.idl
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
#include "mrs_msgs/msg/detail/discrete_plan__struct.h"
#include "mrs_msgs/msg/detail/discrete_plan__functions.h"

#include "rosidl_runtime_c/string.h"
#include "rosidl_runtime_c/string_functions.h"

#include "rosidl_runtime_c/primitives_sequence.h"
#include "rosidl_runtime_c/primitives_sequence_functions.h"

// Nested array functions includes
#include "mrs_msgs/msg/detail/robot_plan__functions.h"
#include "mrs_msgs/msg/detail/visit_order__functions.h"
// end nested array functions include
bool mrs_msgs__msg__robot_plan__convert_from_py(PyObject * _pymsg, void * _ros_message);
PyObject * mrs_msgs__msg__robot_plan__convert_to_py(void * raw_ros_message);
bool mrs_msgs__msg__visit_order__convert_from_py(PyObject * _pymsg, void * _ros_message);
PyObject * mrs_msgs__msg__visit_order__convert_to_py(void * raw_ros_message);

ROSIDL_GENERATOR_C_EXPORT
bool mrs_msgs__msg__discrete_plan__convert_from_py(PyObject * _pymsg, void * _ros_message)
{
  // check that the passed message is of the expected Python class
  {
    char full_classname_dest[41];
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
    assert(strncmp("mrs_msgs.msg._discrete_plan.DiscretePlan", full_classname_dest, 40) == 0);
  }
  mrs_msgs__msg__DiscretePlan * ros_message = _ros_message;
  {  // schema
    PyObject * field = PyObject_GetAttrString(_pymsg, "schema");
    if (!field) {
      return false;
    }
    assert(PyUnicode_Check(field));
    PyObject * encoded_field = PyUnicode_AsUTF8String(field);
    if (!encoded_field) {
      Py_DECREF(field);
      return false;
    }
    rosidl_runtime_c__String__assign(&ros_message->schema, PyBytes_AS_STRING(encoded_field));
    Py_DECREF(encoded_field);
    Py_DECREF(field);
  }
  {  // schema_version
    PyObject * field = PyObject_GetAttrString(_pymsg, "schema_version");
    if (!field) {
      return false;
    }
    assert(PyUnicode_Check(field));
    PyObject * encoded_field = PyUnicode_AsUTF8String(field);
    if (!encoded_field) {
      Py_DECREF(field);
      return false;
    }
    rosidl_runtime_c__String__assign(&ros_message->schema_version, PyBytes_AS_STRING(encoded_field));
    Py_DECREF(encoded_field);
    Py_DECREF(field);
  }
  {  // instance_id
    PyObject * field = PyObject_GetAttrString(_pymsg, "instance_id");
    if (!field) {
      return false;
    }
    assert(PyUnicode_Check(field));
    PyObject * encoded_field = PyUnicode_AsUTF8String(field);
    if (!encoded_field) {
      Py_DECREF(field);
      return false;
    }
    rosidl_runtime_c__String__assign(&ros_message->instance_id, PyBytes_AS_STRING(encoded_field));
    Py_DECREF(encoded_field);
    Py_DECREF(field);
  }
  {  // plan_revision
    PyObject * field = PyObject_GetAttrString(_pymsg, "plan_revision");
    if (!field) {
      return false;
    }
    assert(PyLong_Check(field));
    ros_message->plan_revision = PyLong_AsUnsignedLongLong(field);
    Py_DECREF(field);
  }
  {  // plans
    PyObject * field = PyObject_GetAttrString(_pymsg, "plans");
    if (!field) {
      return false;
    }
    PyObject * seq_field = PySequence_Fast(field, "expected a sequence in 'plans'");
    if (!seq_field) {
      Py_DECREF(field);
      return false;
    }
    Py_ssize_t size = PySequence_Size(field);
    if (-1 == size) {
      Py_DECREF(seq_field);
      Py_DECREF(field);
      return false;
    }
    if (!mrs_msgs__msg__RobotPlan__Sequence__init(&(ros_message->plans), size)) {
      PyErr_SetString(PyExc_RuntimeError, "unable to create mrs_msgs__msg__RobotPlan__Sequence ros_message");
      Py_DECREF(seq_field);
      Py_DECREF(field);
      return false;
    }
    mrs_msgs__msg__RobotPlan * dest = ros_message->plans.data;
    for (Py_ssize_t i = 0; i < size; ++i) {
      if (!mrs_msgs__msg__robot_plan__convert_from_py(PySequence_Fast_GET_ITEM(seq_field, i), &dest[i])) {
        Py_DECREF(seq_field);
        Py_DECREF(field);
        return false;
      }
    }
    Py_DECREF(seq_field);
    Py_DECREF(field);
  }
  {  // visit_order
    PyObject * field = PyObject_GetAttrString(_pymsg, "visit_order");
    if (!field) {
      return false;
    }
    PyObject * seq_field = PySequence_Fast(field, "expected a sequence in 'visit_order'");
    if (!seq_field) {
      Py_DECREF(field);
      return false;
    }
    Py_ssize_t size = PySequence_Size(field);
    if (-1 == size) {
      Py_DECREF(seq_field);
      Py_DECREF(field);
      return false;
    }
    if (!mrs_msgs__msg__VisitOrder__Sequence__init(&(ros_message->visit_order), size)) {
      PyErr_SetString(PyExc_RuntimeError, "unable to create mrs_msgs__msg__VisitOrder__Sequence ros_message");
      Py_DECREF(seq_field);
      Py_DECREF(field);
      return false;
    }
    mrs_msgs__msg__VisitOrder * dest = ros_message->visit_order.data;
    for (Py_ssize_t i = 0; i < size; ++i) {
      if (!mrs_msgs__msg__visit_order__convert_from_py(PySequence_Fast_GET_ITEM(seq_field, i), &dest[i])) {
        Py_DECREF(seq_field);
        Py_DECREF(field);
        return false;
      }
    }
    Py_DECREF(seq_field);
    Py_DECREF(field);
  }

  return true;
}

ROSIDL_GENERATOR_C_EXPORT
PyObject * mrs_msgs__msg__discrete_plan__convert_to_py(void * raw_ros_message)
{
  /* NOTE(esteve): Call constructor of DiscretePlan */
  PyObject * _pymessage = NULL;
  {
    PyObject * pymessage_module = PyImport_ImportModule("mrs_msgs.msg._discrete_plan");
    assert(pymessage_module);
    PyObject * pymessage_class = PyObject_GetAttrString(pymessage_module, "DiscretePlan");
    assert(pymessage_class);
    Py_DECREF(pymessage_module);
    _pymessage = PyObject_CallObject(pymessage_class, NULL);
    Py_DECREF(pymessage_class);
    if (!_pymessage) {
      return NULL;
    }
  }
  mrs_msgs__msg__DiscretePlan * ros_message = (mrs_msgs__msg__DiscretePlan *)raw_ros_message;
  {  // schema
    PyObject * field = NULL;
    field = PyUnicode_DecodeUTF8(
      ros_message->schema.data,
      strlen(ros_message->schema.data),
      "replace");
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "schema", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // schema_version
    PyObject * field = NULL;
    field = PyUnicode_DecodeUTF8(
      ros_message->schema_version.data,
      strlen(ros_message->schema_version.data),
      "replace");
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "schema_version", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // instance_id
    PyObject * field = NULL;
    field = PyUnicode_DecodeUTF8(
      ros_message->instance_id.data,
      strlen(ros_message->instance_id.data),
      "replace");
    if (!field) {
      return NULL;
    }
    {
      int rc = PyObject_SetAttrString(_pymessage, "instance_id", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // plan_revision
    PyObject * field = NULL;
    field = PyLong_FromUnsignedLongLong(ros_message->plan_revision);
    {
      int rc = PyObject_SetAttrString(_pymessage, "plan_revision", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // plans
    PyObject * field = NULL;
    size_t size = ros_message->plans.size;
    field = PyList_New(size);
    if (!field) {
      return NULL;
    }
    mrs_msgs__msg__RobotPlan * item;
    for (size_t i = 0; i < size; ++i) {
      item = &(ros_message->plans.data[i]);
      PyObject * pyitem = mrs_msgs__msg__robot_plan__convert_to_py(item);
      if (!pyitem) {
        Py_DECREF(field);
        return NULL;
      }
      int rc = PyList_SetItem(field, i, pyitem);
      (void)rc;
      assert(rc == 0);
    }
    assert(PySequence_Check(field));
    {
      int rc = PyObject_SetAttrString(_pymessage, "plans", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }
  {  // visit_order
    PyObject * field = NULL;
    size_t size = ros_message->visit_order.size;
    field = PyList_New(size);
    if (!field) {
      return NULL;
    }
    mrs_msgs__msg__VisitOrder * item;
    for (size_t i = 0; i < size; ++i) {
      item = &(ros_message->visit_order.data[i]);
      PyObject * pyitem = mrs_msgs__msg__visit_order__convert_to_py(item);
      if (!pyitem) {
        Py_DECREF(field);
        return NULL;
      }
      int rc = PyList_SetItem(field, i, pyitem);
      (void)rc;
      assert(rc == 0);
    }
    assert(PySequence_Check(field));
    {
      int rc = PyObject_SetAttrString(_pymessage, "visit_order", field);
      Py_DECREF(field);
      if (rc) {
        return NULL;
      }
    }
  }

  // ownership of _pymessage is transferred to the caller
  return _pymessage;
}
