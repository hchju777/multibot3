# generated from rosidl_generator_py/resource/_idl.py.em
# with input from mrs_msgs:msg/RobotSpec.idl
# generated code does not contain a copyright notice

# This is being done at the module level and not on the instance level to avoid looking
# for the same variable multiple times on each instance. This variable is not supposed to
# change during runtime so it makes sense to only look for it once.
from os import getenv

ros_python_check_fields = getenv('ROS_PYTHON_CHECK_FIELDS', default='')


# Import statements for member types

import builtins  # noqa: E402, I100

import math  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_RobotSpec(type):
    """Metaclass of message 'RobotSpec'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
    }

    @classmethod
    def __import_type_support__(cls):
        try:
            from rosidl_generator_py import import_type_support
            module = import_type_support('mrs_msgs')
        except ImportError:
            import logging
            import traceback
            logger = logging.getLogger(
                'mrs_msgs.msg.RobotSpec')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__robot_spec
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__robot_spec
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__robot_spec
            cls._TYPE_SUPPORT = module.type_support_msg__msg__robot_spec
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__robot_spec

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class RobotSpec(metaclass=Metaclass_RobotSpec):
    """Message class 'RobotSpec'."""

    __slots__ = [
        '_robot',
        '_avg_traversal_speed_mps',
        '_v_max',
        '_brake_decel_min_mps2',
        '_curvature_max_invm',
        '_reverse_motion_allowed',
        '_circumradius_m',
        '_check_fields',
    ]

    _fields_and_field_types = {
        'robot': 'string',
        'avg_traversal_speed_mps': 'double',
        'v_max': 'double',
        'brake_decel_min_mps2': 'double',
        'curvature_max_invm': 'double',
        'reverse_motion_allowed': 'boolean',
        'circumradius_m': 'double',
    }

    # This attribute is used to store an rosidl_parser.definition variable
    # related to the data type of each of the components the message.
    SLOT_TYPES = (
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
    )

    def __init__(self, **kwargs):
        if 'check_fields' in kwargs:
            self._check_fields = kwargs['check_fields']
        else:
            self._check_fields = ros_python_check_fields == '1'
        if self._check_fields:
            assert all('_' + key in self.__slots__ for key in kwargs.keys()), \
                'Invalid arguments passed to constructor: %s' % \
                ', '.join(sorted(k for k in kwargs.keys() if '_' + k not in self.__slots__))
        self.robot = kwargs.get('robot', str())
        self.avg_traversal_speed_mps = kwargs.get('avg_traversal_speed_mps', float())
        self.v_max = kwargs.get('v_max', float())
        self.brake_decel_min_mps2 = kwargs.get('brake_decel_min_mps2', float())
        self.curvature_max_invm = kwargs.get('curvature_max_invm', float())
        self.reverse_motion_allowed = kwargs.get('reverse_motion_allowed', bool())
        self.circumradius_m = kwargs.get('circumradius_m', float())

    def __repr__(self):
        typename = self.__class__.__module__.split('.')
        typename.pop()
        typename.append(self.__class__.__name__)
        args = []
        for s, t in zip(self.get_fields_and_field_types().keys(), self.SLOT_TYPES):
            field = getattr(self, s)
            fieldstr = repr(field)
            # We use Python array type for fields that can be directly stored
            # in them, and "normal" sequences for everything else.  If it is
            # a type that we store in an array, strip off the 'array' portion.
            if (
                isinstance(t, rosidl_parser.definition.AbstractSequence) and
                isinstance(t.value_type, rosidl_parser.definition.BasicType) and
                t.value_type.typename in ['float', 'double', 'int8', 'uint8', 'int16', 'uint16', 'int32', 'uint32', 'int64', 'uint64']
            ):
                if len(field) == 0:
                    fieldstr = '[]'
                else:
                    if self._check_fields:
                        assert fieldstr.startswith('array(')
                    prefix = "array('X', "
                    suffix = ')'
                    fieldstr = fieldstr[len(prefix):-len(suffix)]
            args.append(s + '=' + fieldstr)
        return '%s(%s)' % ('.'.join(typename), ', '.join(args))

    def __eq__(self, other):
        if not isinstance(other, self.__class__):
            return False
        if self.robot != other.robot:
            return False
        if self.avg_traversal_speed_mps != other.avg_traversal_speed_mps:
            return False
        if self.v_max != other.v_max:
            return False
        if self.brake_decel_min_mps2 != other.brake_decel_min_mps2:
            return False
        if self.curvature_max_invm != other.curvature_max_invm:
            return False
        if self.reverse_motion_allowed != other.reverse_motion_allowed:
            return False
        if self.circumradius_m != other.circumradius_m:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def robot(self):
        """Message field 'robot'."""
        return self._robot

    @robot.setter
    def robot(self, value):
        if self._check_fields:
            assert \
                isinstance(value, str), \
                "The 'robot' field must be of type 'str'"
        self._robot = value

    @builtins.property
    def avg_traversal_speed_mps(self):
        """Message field 'avg_traversal_speed_mps'."""
        return self._avg_traversal_speed_mps

    @avg_traversal_speed_mps.setter
    def avg_traversal_speed_mps(self, value):
        if self._check_fields:
            assert \
                isinstance(value, float), \
                "The 'avg_traversal_speed_mps' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'avg_traversal_speed_mps' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._avg_traversal_speed_mps = value

    @builtins.property
    def v_max(self):
        """Message field 'v_max'."""
        return self._v_max

    @v_max.setter
    def v_max(self, value):
        if self._check_fields:
            assert \
                isinstance(value, float), \
                "The 'v_max' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'v_max' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._v_max = value

    @builtins.property
    def brake_decel_min_mps2(self):
        """Message field 'brake_decel_min_mps2'."""
        return self._brake_decel_min_mps2

    @brake_decel_min_mps2.setter
    def brake_decel_min_mps2(self, value):
        if self._check_fields:
            assert \
                isinstance(value, float), \
                "The 'brake_decel_min_mps2' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'brake_decel_min_mps2' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._brake_decel_min_mps2 = value

    @builtins.property
    def curvature_max_invm(self):
        """Message field 'curvature_max_invm'."""
        return self._curvature_max_invm

    @curvature_max_invm.setter
    def curvature_max_invm(self, value):
        if self._check_fields:
            assert \
                isinstance(value, float), \
                "The 'curvature_max_invm' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'curvature_max_invm' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._curvature_max_invm = value

    @builtins.property
    def reverse_motion_allowed(self):
        """Message field 'reverse_motion_allowed'."""
        return self._reverse_motion_allowed

    @reverse_motion_allowed.setter
    def reverse_motion_allowed(self, value):
        if self._check_fields:
            assert \
                isinstance(value, bool), \
                "The 'reverse_motion_allowed' field must be of type 'bool'"
        self._reverse_motion_allowed = value

    @builtins.property
    def circumradius_m(self):
        """Message field 'circumradius_m'."""
        return self._circumradius_m

    @circumradius_m.setter
    def circumradius_m(self, value):
        if self._check_fields:
            assert \
                isinstance(value, float), \
                "The 'circumradius_m' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'circumradius_m' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._circumradius_m = value
