# generated from rosidl_generator_py/resource/_idl.py.em
# with input from mrs_msgs:msg/StopReason.idl
# generated code does not contain a copyright notice

# This is being done at the module level and not on the instance level to avoid looking
# for the same variable multiple times on each instance. This variable is not supposed to
# change during runtime so it makes sense to only look for it once.
from os import getenv

ros_python_check_fields = getenv('ROS_PYTHON_CHECK_FIELDS', default='')


# Import statements for member types

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_StopReason(type):
    """Metaclass of message 'StopReason'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
        'REASON_UNKNOWN': 0,
        'REASON_EXOGENOUS_BLOCK': 1,
        'REASON_CTRL_STOP_DECLARED': 2,
        'REASON_INFEASIBLE_SUBGOAL': 3,
        'REASON_ORIENTATION_FLIP': 4,
        'REASON_UNRESOLVABLE_LOCALLY': 5,
        'REASON_HORIZON_EXHAUSTING': 6,
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
                'mrs_msgs.msg.StopReason')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__stop_reason
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__stop_reason
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__stop_reason
            cls._TYPE_SUPPORT = module.type_support_msg__msg__stop_reason
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__stop_reason

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
            'REASON_UNKNOWN': cls.__constants['REASON_UNKNOWN'],
            'REASON_EXOGENOUS_BLOCK': cls.__constants['REASON_EXOGENOUS_BLOCK'],
            'REASON_CTRL_STOP_DECLARED': cls.__constants['REASON_CTRL_STOP_DECLARED'],
            'REASON_INFEASIBLE_SUBGOAL': cls.__constants['REASON_INFEASIBLE_SUBGOAL'],
            'REASON_ORIENTATION_FLIP': cls.__constants['REASON_ORIENTATION_FLIP'],
            'REASON_UNRESOLVABLE_LOCALLY': cls.__constants['REASON_UNRESOLVABLE_LOCALLY'],
            'REASON_HORIZON_EXHAUSTING': cls.__constants['REASON_HORIZON_EXHAUSTING'],
        }

    @property
    def REASON_UNKNOWN(self):
        """Message constant 'REASON_UNKNOWN'."""
        return Metaclass_StopReason.__constants['REASON_UNKNOWN']

    @property
    def REASON_EXOGENOUS_BLOCK(self):
        """Message constant 'REASON_EXOGENOUS_BLOCK'."""
        return Metaclass_StopReason.__constants['REASON_EXOGENOUS_BLOCK']

    @property
    def REASON_CTRL_STOP_DECLARED(self):
        """Message constant 'REASON_CTRL_STOP_DECLARED'."""
        return Metaclass_StopReason.__constants['REASON_CTRL_STOP_DECLARED']

    @property
    def REASON_INFEASIBLE_SUBGOAL(self):
        """Message constant 'REASON_INFEASIBLE_SUBGOAL'."""
        return Metaclass_StopReason.__constants['REASON_INFEASIBLE_SUBGOAL']

    @property
    def REASON_ORIENTATION_FLIP(self):
        """Message constant 'REASON_ORIENTATION_FLIP'."""
        return Metaclass_StopReason.__constants['REASON_ORIENTATION_FLIP']

    @property
    def REASON_UNRESOLVABLE_LOCALLY(self):
        """Message constant 'REASON_UNRESOLVABLE_LOCALLY'."""
        return Metaclass_StopReason.__constants['REASON_UNRESOLVABLE_LOCALLY']

    @property
    def REASON_HORIZON_EXHAUSTING(self):
        """Message constant 'REASON_HORIZON_EXHAUSTING'."""
        return Metaclass_StopReason.__constants['REASON_HORIZON_EXHAUSTING']


class StopReason(metaclass=Metaclass_StopReason):
    """
    Message class 'StopReason'.

    Constants:
      REASON_UNKNOWN
      REASON_EXOGENOUS_BLOCK
      REASON_CTRL_STOP_DECLARED
      REASON_INFEASIBLE_SUBGOAL
      REASON_ORIENTATION_FLIP
      REASON_UNRESOLVABLE_LOCALLY
      REASON_HORIZON_EXHAUSTING
    """

    __slots__ = [
        '_check_fields',
    ]

    _fields_and_field_types = {
    }

    # This attribute is used to store an rosidl_parser.definition variable
    # related to the data type of each of the components the message.
    SLOT_TYPES = (
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
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)
