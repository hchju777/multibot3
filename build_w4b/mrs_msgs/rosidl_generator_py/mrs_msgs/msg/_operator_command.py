# generated from rosidl_generator_py/resource/_idl.py.em
# with input from mrs_msgs:msg/OperatorCommand.idl
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


class Metaclass_OperatorCommand(type):
    """Metaclass of message 'OperatorCommand'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
        'VERB_UNKNOWN': 0,
        'VERB_HOLD': 1,
        'VERB_SPEED_CAP': 2,
        'VERB_GOAL_OVERRIDE': 3,
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
                'mrs_msgs.msg.OperatorCommand')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__operator_command
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__operator_command
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__operator_command
            cls._TYPE_SUPPORT = module.type_support_msg__msg__operator_command
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__operator_command

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
            'VERB_UNKNOWN': cls.__constants['VERB_UNKNOWN'],
            'VERB_HOLD': cls.__constants['VERB_HOLD'],
            'VERB_SPEED_CAP': cls.__constants['VERB_SPEED_CAP'],
            'VERB_GOAL_OVERRIDE': cls.__constants['VERB_GOAL_OVERRIDE'],
        }

    @property
    def VERB_UNKNOWN(self):
        """Message constant 'VERB_UNKNOWN'."""
        return Metaclass_OperatorCommand.__constants['VERB_UNKNOWN']

    @property
    def VERB_HOLD(self):
        """Message constant 'VERB_HOLD'."""
        return Metaclass_OperatorCommand.__constants['VERB_HOLD']

    @property
    def VERB_SPEED_CAP(self):
        """Message constant 'VERB_SPEED_CAP'."""
        return Metaclass_OperatorCommand.__constants['VERB_SPEED_CAP']

    @property
    def VERB_GOAL_OVERRIDE(self):
        """Message constant 'VERB_GOAL_OVERRIDE'."""
        return Metaclass_OperatorCommand.__constants['VERB_GOAL_OVERRIDE']


class OperatorCommand(metaclass=Metaclass_OperatorCommand):
    """
    Message class 'OperatorCommand'.

    Constants:
      VERB_UNKNOWN
      VERB_HOLD
      VERB_SPEED_CAP
      VERB_GOAL_OVERRIDE
    """

    __slots__ = [
        '_seq',
        '_robot',
        '_verb',
        '_active',
        '_speed_cap_mps',
        '_has_speed_cap_mps',
        '_goal_location',
        '_check_fields',
    ]

    _fields_and_field_types = {
        'seq': 'uint64',
        'robot': 'string',
        'verb': 'uint8',
        'active': 'boolean',
        'speed_cap_mps': 'double',
        'has_speed_cap_mps': 'boolean',
        'goal_location': 'string',
    }

    # This attribute is used to store an rosidl_parser.definition variable
    # related to the data type of each of the components the message.
    SLOT_TYPES = (
        rosidl_parser.definition.BasicType('uint64'),  # noqa: E501
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.BasicType('uint8'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
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
        self.seq = kwargs.get('seq', int())
        self.robot = kwargs.get('robot', str())
        self.verb = kwargs.get('verb', int())
        self.active = kwargs.get('active', bool())
        self.speed_cap_mps = kwargs.get('speed_cap_mps', float())
        self.has_speed_cap_mps = kwargs.get('has_speed_cap_mps', bool())
        self.goal_location = kwargs.get('goal_location', str())

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
        if self.seq != other.seq:
            return False
        if self.robot != other.robot:
            return False
        if self.verb != other.verb:
            return False
        if self.active != other.active:
            return False
        if self.speed_cap_mps != other.speed_cap_mps:
            return False
        if self.has_speed_cap_mps != other.has_speed_cap_mps:
            return False
        if self.goal_location != other.goal_location:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def seq(self):
        """Message field 'seq'."""
        return self._seq

    @seq.setter
    def seq(self, value):
        if self._check_fields:
            assert \
                isinstance(value, int), \
                "The 'seq' field must be of type 'int'"
            assert value >= 0 and value < 18446744073709551616, \
                "The 'seq' field must be an unsigned integer in [0, 18446744073709551615]"
        self._seq = value

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
    def verb(self):
        """Message field 'verb'."""
        return self._verb

    @verb.setter
    def verb(self, value):
        if self._check_fields:
            assert \
                isinstance(value, int), \
                "The 'verb' field must be of type 'int'"
            assert value >= 0 and value < 256, \
                "The 'verb' field must be an unsigned integer in [0, 255]"
        self._verb = value

    @builtins.property
    def active(self):
        """Message field 'active'."""
        return self._active

    @active.setter
    def active(self, value):
        if self._check_fields:
            assert \
                isinstance(value, bool), \
                "The 'active' field must be of type 'bool'"
        self._active = value

    @builtins.property
    def speed_cap_mps(self):
        """Message field 'speed_cap_mps'."""
        return self._speed_cap_mps

    @speed_cap_mps.setter
    def speed_cap_mps(self, value):
        if self._check_fields:
            assert \
                isinstance(value, float), \
                "The 'speed_cap_mps' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'speed_cap_mps' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._speed_cap_mps = value

    @builtins.property
    def has_speed_cap_mps(self):
        """Message field 'has_speed_cap_mps'."""
        return self._has_speed_cap_mps

    @has_speed_cap_mps.setter
    def has_speed_cap_mps(self, value):
        if self._check_fields:
            assert \
                isinstance(value, bool), \
                "The 'has_speed_cap_mps' field must be of type 'bool'"
        self._has_speed_cap_mps = value

    @builtins.property
    def goal_location(self):
        """Message field 'goal_location'."""
        return self._goal_location

    @goal_location.setter
    def goal_location(self, value):
        if self._check_fields:
            assert \
                isinstance(value, str), \
                "The 'goal_location' field must be of type 'str'"
        self._goal_location = value
