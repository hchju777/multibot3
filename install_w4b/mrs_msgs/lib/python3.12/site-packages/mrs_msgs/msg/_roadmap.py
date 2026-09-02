# generated from rosidl_generator_py/resource/_idl.py.em
# with input from mrs_msgs:msg/Roadmap.idl
# generated code does not contain a copyright notice

# This is being done at the module level and not on the instance level to avoid looking
# for the same variable multiple times on each instance. This variable is not supposed to
# change during runtime so it makes sense to only look for it once.
from os import getenv

ros_python_check_fields = getenv('ROS_PYTHON_CHECK_FIELDS', default='')


# Import statements for member types

import builtins  # noqa: E402, I100

import rosidl_parser.definition  # noqa: E402, I100


class Metaclass_Roadmap(type):
    """Metaclass of message 'Roadmap'."""

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
                'mrs_msgs.msg.Roadmap')
            logger.debug(
                'Failed to import needed modules for type support:\n' +
                traceback.format_exc())
        else:
            cls._CREATE_ROS_MESSAGE = module.create_ros_message_msg__msg__roadmap
            cls._CONVERT_FROM_PY = module.convert_from_py_msg__msg__roadmap
            cls._CONVERT_TO_PY = module.convert_to_py_msg__msg__roadmap
            cls._TYPE_SUPPORT = module.type_support_msg__msg__roadmap
            cls._DESTROY_ROS_MESSAGE = module.destroy_ros_message_msg__msg__roadmap

            from mrs_msgs.msg import Corridor
            if Corridor.__class__._TYPE_SUPPORT is None:
                Corridor.__class__.__import_type_support__()

            from mrs_msgs.msg import RoadmapEdge
            if RoadmapEdge.__class__._TYPE_SUPPORT is None:
                RoadmapEdge.__class__.__import_type_support__()

            from mrs_msgs.msg import RoadmapNode
            if RoadmapNode.__class__._TYPE_SUPPORT is None:
                RoadmapNode.__class__.__import_type_support__()

            from mrs_msgs.msg import RoadmapProvenance
            if RoadmapProvenance.__class__._TYPE_SUPPORT is None:
                RoadmapProvenance.__class__.__import_type_support__()

    @classmethod
    def __prepare__(cls, name, bases, **kwargs):
        # list constant names here so that they appear in the help text of
        # the message class under "Data and other attributes defined here:"
        # as well as populate each message instance
        return {
        }


class Roadmap(metaclass=Metaclass_Roadmap):
    """Message class 'Roadmap'."""

    __slots__ = [
        '_schema',
        '_schema_version',
        '_instance_id',
        '_nodes',
        '_edges',
        '_endpoints',
        '_corridors',
        '_provenance',
        '_check_fields',
    ]

    _fields_and_field_types = {
        'schema': 'string',
        'schema_version': 'string',
        'instance_id': 'string',
        'nodes': 'sequence<mrs_msgs/RoadmapNode>',
        'edges': 'sequence<mrs_msgs/RoadmapEdge>',
        'endpoints': 'sequence<string>',
        'corridors': 'sequence<mrs_msgs/Corridor>',
        'provenance': 'mrs_msgs/RoadmapProvenance',
    }

    # This attribute is used to store an rosidl_parser.definition variable
    # related to the data type of each of the components the message.
    SLOT_TYPES = (
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.UnboundedString(),  # noqa: E501
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.NamespacedType(['mrs_msgs', 'msg'], 'RoadmapNode')),  # noqa: E501
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.NamespacedType(['mrs_msgs', 'msg'], 'RoadmapEdge')),  # noqa: E501
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.UnboundedString()),  # noqa: E501
        rosidl_parser.definition.UnboundedSequence(rosidl_parser.definition.NamespacedType(['mrs_msgs', 'msg'], 'Corridor')),  # noqa: E501
        rosidl_parser.definition.NamespacedType(['mrs_msgs', 'msg'], 'RoadmapProvenance'),  # noqa: E501
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
        self.nodes = kwargs.get('nodes', [])
        self.edges = kwargs.get('edges', [])
        self.endpoints = kwargs.get('endpoints', [])
        self.corridors = kwargs.get('corridors', [])
        from mrs_msgs.msg import RoadmapProvenance
        self.provenance = kwargs.get('provenance', RoadmapProvenance())

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
        if self.nodes != other.nodes:
            return False
        if self.edges != other.edges:
            return False
        if self.endpoints != other.endpoints:
            return False
        if self.corridors != other.corridors:
            return False
        if self.provenance != other.provenance:
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
    def nodes(self):
        """Message field 'nodes'."""
        return self._nodes

    @nodes.setter
    def nodes(self, value):
        if self._check_fields:
            from mrs_msgs.msg import RoadmapNode
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
                 all(isinstance(v, RoadmapNode) for v in value) and
                 True), \
                "The 'nodes' field must be a set or sequence and each value of type 'RoadmapNode'"
        self._nodes = value

    @builtins.property
    def edges(self):
        """Message field 'edges'."""
        return self._edges

    @edges.setter
    def edges(self, value):
        if self._check_fields:
            from mrs_msgs.msg import RoadmapEdge
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
                 all(isinstance(v, RoadmapEdge) for v in value) and
                 True), \
                "The 'edges' field must be a set or sequence and each value of type 'RoadmapEdge'"
        self._edges = value

    @builtins.property
    def endpoints(self):
        """Message field 'endpoints'."""
        return self._endpoints

    @endpoints.setter
    def endpoints(self, value):
        if self._check_fields:
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
                 all(isinstance(v, str) for v in value) and
                 True), \
                "The 'endpoints' field must be a set or sequence and each value of type 'str'"
        self._endpoints = value

    @builtins.property
    def corridors(self):
        """Message field 'corridors'."""
        return self._corridors

    @corridors.setter
    def corridors(self, value):
        if self._check_fields:
            from mrs_msgs.msg import Corridor
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
                 all(isinstance(v, Corridor) for v in value) and
                 True), \
                "The 'corridors' field must be a set or sequence and each value of type 'Corridor'"
        self._corridors = value

    @builtins.property
    def provenance(self):
        """Message field 'provenance'."""
        return self._provenance

    @provenance.setter
    def provenance(self, value):
        if self._check_fields:
            from mrs_msgs.msg import RoadmapProvenance
            assert \
                isinstance(value, RoadmapProvenance), \
                "The 'provenance' field must be a sub message of type 'RoadmapProvenance'"
        self._provenance = value
