# generated from rosidl_generator_py/resource/_idl.py.em
# with input from mrs_msgs:msg/SegmentRelease.idl
# generated code does not contain a copyright notice

# This is being done at the module level and not on the instance level to avoid looking
# for the same variable multiple times on each instance. This variable is not supposed to
# change during runtime so it makes sense to only look for it once.
from os import getenv

ros_python_check_fields = getenv('ROS_PYTHON_CHECK_FIELDS', default='')


# Import statements for member types

import builtins  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_SegmentRelease(type):
    """Metaclass of message 'SegmentRelease'."""

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
                'mrs_msgs.msg.SegmentRelease')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__segment_release
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__segment_release
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__segment_release
            cls._TYPE_SUPPORT = module.type_support_msg__msg__segment_release
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__segment_release

            from mrs_msgs.msg import SegmentReleaseItem
            if SegmentReleaseItem.__class__._TYPE_SUPPORT is None:
                SegmentReleaseItem.__class__.__import_type_support__()

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class SegmentRelease(metaclass=Metaclass_SegmentRelease):
    """Message class 'SegmentRelease'."""

    __slots__ = [
        '_schema',
        '_schema_version',
        '_instance_id',
        '_robot',
        '_release_seq',
        '_commit_seq',
        '_entry_seq_ack',
        '_segments',
        '_check_fields',
    ]

    _fields_and_field_types = {
        'schema': 'string',
        'schema_version': 'string',
        'instance_id': 'string',
        'robot': 'string',
        'release_seq': 'uint64',
        'commit_seq': 'uint64',
        'entry_seq_ack': 'uint64',
        'segments': 'sequence<mrs_msgs/SegmentReleaseItem>',
    }

    # This attribute is used to store an rosidl_parser.definition variable
    # related to the data type of each of the components the message.
    SLOT_TYPES = (
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.BasicType('uint64'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint64'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint64'),  # noqa: E501
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.NamespacedType(['mrs_msgs', 'msg'], 'SegmentReleaseItem')),  # noqa: E501
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
        self.schema = kwargs.get('schema', str())
        self.schema_version = kwargs.get('schema_version', str())
        self.instance_id = kwargs.get('instance_id', str())
        self.robot = kwargs.get('robot', str())
        self.release_seq = kwargs.get('release_seq', int())
        self.commit_seq = kwargs.get('commit_seq', int())
        self.entry_seq_ack = kwargs.get('entry_seq_ack', int())
        self.segments = kwargs.get('segments', [])

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
        if self.schema != other.schema:
            return False
        if self.schema_version != other.schema_version:
            return False
        if self.instance_id != other.instance_id:
            return False
        if self.robot != other.robot:
            return False
        if self.release_seq != other.release_seq:
            return False
        if self.commit_seq != other.commit_seq:
            return False
        if self.entry_seq_ack != other.entry_seq_ack:
            return False
        if self.segments != other.segments:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def schema(self):
        """Message field 'schema'."""
        return self._schema

    @schema.setter
    def schema(self, value):
        if self._check_fields:
            assert \
                isinstance(value, str), \
                "The 'schema' field must be of type 'str'"
        self._schema = value

    @builtins.property
    def schema_version(self):
        """Message field 'schema_version'."""
        return self._schema_version

    @schema_version.setter
    def schema_version(self, value):
        if self._check_fields:
            assert \
                isinstance(value, str), \
                "The 'schema_version' field must be of type 'str'"
        self._schema_version = value

    @builtins.property
    def instance_id(self):
        """Message field 'instance_id'."""
        return self._instance_id

    @instance_id.setter
    def instance_id(self, value):
        if self._check_fields:
            assert \
                isinstance(value, str), \
                "The 'instance_id' field must be of type 'str'"
        self._instance_id = value

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
    def release_seq(self):
        """Message field 'release_seq'."""
        return self._release_seq

    @release_seq.setter
    def release_seq(self, value):
        if self._check_fields:
            assert \
                isinstance(value, int), \
                "The 'release_seq' field must be of type 'int'"
            assert value >= 0 and value < 18446744073709551616, \
                "The 'release_seq' field must be an unsigned integer in [0, 18446744073709551615]"
        self._release_seq = value

    @builtins.property
    def commit_seq(self):
        """Message field 'commit_seq'."""
        return self._commit_seq

    @commit_seq.setter
    def commit_seq(self, value):
        if self._check_fields:
            assert \
                isinstance(value, int), \
                "The 'commit_seq' field must be of type 'int'"
            assert value >= 0 and value < 18446744073709551616, \
                "The 'commit_seq' field must be an unsigned integer in [0, 18446744073709551615]"
        self._commit_seq = value

    @builtins.property
    def entry_seq_ack(self):
        """Message field 'entry_seq_ack'."""
        return self._entry_seq_ack

    @entry_seq_ack.setter
    def entry_seq_ack(self, value):
        if self._check_fields:
            assert \
                isinstance(value, int), \
                "The 'entry_seq_ack' field must be of type 'int'"
            assert value >= 0 and value < 18446744073709551616, \
                "The 'entry_seq_ack' field must be an unsigned integer in [0, 18446744073709551615]"
        self._entry_seq_ack = value

    @builtins.property
    def segments(self):
        """Message field 'segments'."""
        return self._segments

    @segments.setter
    def segments(self, value):
        if self._check_fields:
            from mrs_msgs.msg import SegmentReleaseItem
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
                 all(isinstance(v, SegmentReleaseItem) for v in value) and
                 True), \
                "The 'segments' field must be a set or sequence and each value of type 'SegmentReleaseItem'"
        self._segments = value
