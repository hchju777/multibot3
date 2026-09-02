# generated from rosidl_generator_py/resource/_idl.py.em
# with input from mrs_msgs:msg/RoadmapEdge.idl
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


class Metaclass_RoadmapEdge(type):
    """Metaclass of message 'RoadmapEdge'."""

    _CREATE_ROS_MESSAGE = None
    _CONVERT_FROM_PY = None
    _CONVERT_TO_PY = None
    _DESTROY_ROS_MESSAGE = None
    _TYPE_SUPPORT = None

    __constants = {
        'TRAVERSAL_UNKNOWN': 0,
        'TRAVERSAL_BIDIRECTIONAL': 1,
        'TRAVERSAL_FORWARD_ONLY': 2,
        'TRAVERSAL_BACKWARD_ONLY': 3,
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
                'mrs_msgs.msg.RoadmapEdge')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__roadmap_edge
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__roadmap_edge
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__roadmap_edge
            cls._TYPE_SUPPORT = module.type_support_msg__msg__roadmap_edge
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__roadmap_edge

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
            'TRAVERSAL_UNKNOWN': cls.__constants['TRAVERSAL_UNKNOWN'],
            'TRAVERSAL_BIDIRECTIONAL': cls.__constants['TRAVERSAL_BIDIRECTIONAL'],
            'TRAVERSAL_FORWARD_ONLY': cls.__constants['TRAVERSAL_FORWARD_ONLY'],
            'TRAVERSAL_BACKWARD_ONLY': cls.__constants['TRAVERSAL_BACKWARD_ONLY'],
        }

    @property
    def TRAVERSAL_UNKNOWN(self):
        """Message constant 'TRAVERSAL_UNKNOWN'."""
        return Metaclass_RoadmapEdge.__constants['TRAVERSAL_UNKNOWN']

    @property
    def TRAVERSAL_BIDIRECTIONAL(self):
        """Message constant 'TRAVERSAL_BIDIRECTIONAL'."""
        return Metaclass_RoadmapEdge.__constants['TRAVERSAL_BIDIRECTIONAL']

    @property
    def TRAVERSAL_FORWARD_ONLY(self):
        """Message constant 'TRAVERSAL_FORWARD_ONLY'."""
        return Metaclass_RoadmapEdge.__constants['TRAVERSAL_FORWARD_ONLY']

    @property
    def TRAVERSAL_BACKWARD_ONLY(self):
        """Message constant 'TRAVERSAL_BACKWARD_ONLY'."""
        return Metaclass_RoadmapEdge.__constants['TRAVERSAL_BACKWARD_ONLY']


class RoadmapEdge(metaclass=Metaclass_RoadmapEdge):
    """
    Message class 'RoadmapEdge'.

    Constants:
      TRAVERSAL_UNKNOWN
      TRAVERSAL_BIDIRECTIONAL
      TRAVERSAL_FORWARD_ONLY
      TRAVERSAL_BACKWARD_ONLY
    """

    __slots__ = [
        '_from_id',
        '_to_id',
        '_traversal',
        '_length_m',
        '_width_m',
        '_capacity_robots',
        '_corridor',
        '_check_fields',
    ]

    _fields_and_field_types = {
        'from_id': 'string',
        'to_id': 'string',
        'traversal': 'uint8',
        'length_m': 'double',
        'width_m': 'double',
        'capacity_robots': 'uint8',
        'corridor': 'string',
    }

    # This attribute is used to store an rosidl_parser.definition variable
    # related to the data type of each of the components the message.
    SLOT_TYPES = (
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.BasicType('uint8'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('double'),  # noqa: E501
        rosidl_parser.definition.BasicType('uint8'),  # noqa: E501
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
        self.from_id = kwargs.get('from_id', str())
        self.to_id = kwargs.get('to_id', str())
        self.traversal = kwargs.get('traversal', int())
        self.length_m = kwargs.get('length_m', float())
        self.width_m = kwargs.get('width_m', float())
        self.capacity_robots = kwargs.get('capacity_robots', int())
        self.corridor = kwargs.get('corridor', str())

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
        if self.from_id != other.from_id:
            return False
        if self.to_id != other.to_id:
            return False
        if self.traversal != other.traversal:
            return False
        if self.length_m != other.length_m:
            return False
        if self.width_m != other.width_m:
            return False
        if self.capacity_robots != other.capacity_robots:
            return False
        if self.corridor != other.corridor:
            return False
        return True

    @classmethod
    def get_fields_and_field_types(cls):
        from copy import copy
        return copy(cls._fields_and_field_types)

    @builtins.property
    def from_id(self):
        """Message field 'from_id'."""
        return self._from_id

    @from_id.setter
    def from_id(self, value):
        if self._check_fields:
            assert \
                isinstance(value, str), \
                "The 'from_id' field must be of type 'str'"
        self._from_id = value

    @builtins.property
    def to_id(self):
        """Message field 'to_id'."""
        return self._to_id

    @to_id.setter
    def to_id(self, value):
        if self._check_fields:
            assert \
                isinstance(value, str), \
                "The 'to_id' field must be of type 'str'"
        self._to_id = value

    @builtins.property
    def traversal(self):
        """Message field 'traversal'."""
        return self._traversal

    @traversal.setter
    def traversal(self, value):
        if self._check_fields:
            assert \
                isinstance(value, int), \
                "The 'traversal' field must be of type 'int'"
            assert value >= 0 and value < 256, \
                "The 'traversal' field must be an unsigned integer in [0, 255]"
        self._traversal = value

    @builtins.property
    def length_m(self):
        """Message field 'length_m'."""
        return self._length_m

    @length_m.setter
    def length_m(self, value):
        if self._check_fields:
            assert \
                isinstance(value, float), \
                "The 'length_m' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'length_m' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._length_m = value

    @builtins.property
    def width_m(self):
        """Message field 'width_m'."""
        return self._width_m

    @width_m.setter
    def width_m(self, value):
        if self._check_fields:
            assert \
                isinstance(value, float), \
                "The 'width_m' field must be of type 'float'"
            assert not (value < -1.7976931348623157e+308 or value > 1.7976931348623157e+308) or math.isinf(value), \
                "The 'width_m' field must be a double in [-1.7976931348623157e+308, 1.7976931348623157e+308]"
        self._width_m = value

    @builtins.property
    def capacity_robots(self):
        """Message field 'capacity_robots'."""
        return self._capacity_robots

    @capacity_robots.setter
    def capacity_robots(self, value):
        if self._check_fields:
            assert \
                isinstance(value, int), \
                "The 'capacity_robots' field must be of type 'int'"
            assert value >= 0 and value < 256, \
                "The 'capacity_robots' field must be an unsigned integer in [0, 255]"
        self._capacity_robots = value

    @builtins.property
    def corridor(self):
        """Message field 'corridor'."""
        return self._corridor

    @corridor.setter
    def corridor(self, value):
        if self._check_fields:
            assert \
                isinstance(value, str), \
                "The 'corridor' field must be of type 'str'"
        self._corridor = value
