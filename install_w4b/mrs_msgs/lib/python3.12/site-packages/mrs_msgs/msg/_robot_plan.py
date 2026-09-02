# generated from rosidl_generator_py/resource/_idl.py.em
# with input from mrs_msgs:msg/RobotPlan.idl
# generated code does not contain a copyright notice

# This is being done at the module level and not on the instance level to avoid looking
# for the same variable multiple times on each instance. This variable is not supposed to
# change during runtime so it makes sense to only look for it once.
from os import getenv

ros_python_check_fields = getenv('ROS_PYTHON_CHECK_FIELDS', default='')


# Import statements for member types

import builtins  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_RobotPlan(type):
    """Metaclass of message 'RobotPlan'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
        'TERMINAL_UNKNOWN': 0,
        'TERMINAL_GOAL_REACHED': 1,
        'TERMINAL_PLAN_TRUNCATED': 2,
        'TERMINAL_NO_PROGRESS': 3,
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
                'mrs_msgs.msg.RobotPlan')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__robot_plan
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__robot_plan
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__robot_plan
            cls._TYPE_SUPPORT = module.type_support_msg__msg__robot_plan
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__robot_plan

            from mrs_msgs.msg import PlanStep
            if PlanStep.__class__._TYPE_SUPPORT is None:
                PlanStep.__class__.__import_type_support__()

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
            'TERMINAL_UNKNOWN': cls.__constants['TERMINAL_UNKNOWN'],
            'TERMINAL_GOAL_REACHED': cls.__constants['TERMINAL_GOAL_REACHED'],
            'TERMINAL_PLAN_TRUNCATED': cls.__constants['TERMINAL_PLAN_TRUNCATED'],
            'TERMINAL_NO_PROGRESS': cls.__constants['TERMINAL_NO_PROGRESS'],
        }

    @property
    def TERMINAL_UNKNOWN(self):
        """Message constant 'TERMINAL_UNKNOWN'."""
        return Metaclass_RobotPlan.__constants['TERMINAL_UNKNOWN']

    @property
    def TERMINAL_GOAL_REACHED(self):
        """Message constant 'TERMINAL_GOAL_REACHED'."""
        return Metaclass_RobotPlan.__constants['TERMINAL_GOAL_REACHED']

    @property
    def TERMINAL_PLAN_TRUNCATED(self):
        """Message constant 'TERMINAL_PLAN_TRUNCATED'."""
        return Metaclass_RobotPlan.__constants['TERMINAL_PLAN_TRUNCATED']

    @property
    def TERMINAL_NO_PROGRESS(self):
        """Message constant 'TERMINAL_NO_PROGRESS'."""
        return Metaclass_RobotPlan.__constants['TERMINAL_NO_PROGRESS']


class RobotPlan(metaclass=Metaclass_RobotPlan):
    """
    Message class 'RobotPlan'.

    Constants:
      TERMINAL_UNKNOWN
      TERMINAL_GOAL_REACHED
      TERMINAL_PLAN_TRUNCATED
      TERMINAL_NO_PROGRESS
    """

    __slots__ = [
        '_robot',
        '_terminal',
        '_steps',
        '_check_fields',
    ]

    _fields_and_field_types = {
        'robot': 'string',
        'terminal': 'uint8',
        'steps': 'sequence<mrs_msgs/PlanStep>',
    }

    # This attribute is used to store an rosidl_parser.definition variable
    # related to the data type of each of the components the message.
    SLOT_TYPES = (
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.BasicType('uint8'),  # noqa: E501
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.NamespacedType(['mrs_msgs', 'msg'], 'PlanStep')),  # noqa: E501
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
        self.terminal = kwargs.get('terminal', int())
        self.steps = kwargs.get('steps', [])

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
        if self.terminal != other.terminal:
            return False
        if self.steps != other.steps:
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
    def terminal(self):
        """Message field 'terminal'."""
        return self._terminal

    @terminal.setter
    def terminal(self, value):
        if self._check_fields:
            assert \
                isinstance(value, int), \
                "The 'terminal' field must be of type 'int'"
            assert value >= 0 and value < 256, \
                "The 'terminal' field must be an unsigned integer in [0, 255]"
        self._terminal = value

    @builtins.property
    def steps(self):
        """Message field 'steps'."""
        return self._steps

    @steps.setter
    def steps(self, value):
        if self._check_fields:
            from mrs_msgs.msg import PlanStep
            from collections.abc import Sequence
            from collections.abc import Set
            from collections import UserList
            from collections import UserString
            assert \
                ((isinstance(value, Sequence) or
                  isinstance(value, Set) or
                  isinstance(value, UserList)) and
                 not isinstance(value, str) and
                 not isinstance(value, UserString) and
                 all(isinstance(v, PlanStep) for v in value) and
                 True), \
                "The 'steps' field must be a set or sequence and each value of type 'PlanStep'"
        self._steps = value
