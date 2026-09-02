# generated from rosidl_generator_py/resource/_idl.py.em
# with input from mrs_msgs:msg/StopDeclaration.idl
# generated code does not contain a copyright notice

# This is being done at the module level and not on the instance level to avoid looking
# for the same variable multiple times on each instance. This variable is not supposed to
# change during runtime so it makes sense to only look for it once.
from os import getenv

ros_python_check_fields = getenv('ROS_PYTHON_CHECK_FIELDS', default='')


# Import statements for member types

import builtins  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_StopDeclaration(type):
    """Metaclass of message 'StopDeclaration'."""

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
                'mrs_msgs.msg.StopDeclaration')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__stop_declaration
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__stop_declaration
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__stop_declaration
            cls._TYPE_SUPPORT = module.type_support_msg__msg__stop_declaration
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__stop_declaration

            from mrs_msgs.msg import EdgeRef
            if EdgeRef.__class__._TYPE_SUPPORT is None:
                EdgeRef.__class__.__import_type_support__()

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class StopDeclaration(metaclass=Metaclass_StopDeclaration):
    """Message class 'StopDeclaration'."""

    __slots__ = [
        '_seq',
        '_robot',
        '_blocked_edge',
        '_declared',
        '_reason',
        '_check_fields',
    ]

    _fields_and_field_types = {
        'seq': 'uint64',
        'robot': 'string',
        'blocked_edge': 'mrs_msgs/EdgeRef',
        'declared': 'boolean',
        'reason': 'uint8',
    }

    # This attribute is used to store an rosidl_parser.definition variable
    # related to the data type of each of the components the message.
    SLOT_TYPES = (
        rosidl_parser.definition.BasicType('uint64'),  # noqa: E501
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['mrs_msgs', 'msg'], 'EdgeRef'),  # noqa: E501
        rosidl_parser.definition.BasicType('boolean'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint8'),  # noqa: E501
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
        from mrs_msgs.msg import EdgeRef
        self.blocked_edge = kwargs.get('blocked_edge', EdgeRef())
        self.declared = kwargs.get('declared', bool())
        self.reason = kwargs.get('reason', int())

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
        if self.blocked_edge != other.blocked_edge:
            return False
        if self.declared != other.declared:
            return False
        if self.reason != other.reason:
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
    def blocked_edge(self):
        """Message field 'blocked_edge'."""
        return self._blocked_edge

    @blocked_edge.setter
    def blocked_edge(self, value):
        if self._check_fields:
            from mrs_msgs.msg import EdgeRef
            assert \
                isinstance(value, EdgeRef), \
                "The 'blocked_edge' field must be a sub message of type 'EdgeRef'"
        self._blocked_edge = value

    @builtins.property
    def declared(self):
        """Message field 'declared'."""
        return self._declared

    @declared.setter
    def declared(self, value):
        if self._check_fields:
            assert \
                isinstance(value, bool), \
                "The 'declared' field must be of type 'bool'"
        self._declared = value

    @builtins.property
    def reason(self):
        """Message field 'reason'."""
        return self._reason

    @reason.setter
    def reason(self, value):
        if self._check_fields:
            assert \
                isinstance(value, int), \
                "The 'reason' field must be of type 'int'"
            assert value >= 0 and value < 256, \
                "The 'reason' field must be an unsigned integer in [0, 255]"
        self._reason = value
