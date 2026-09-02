# reason uses the SHARED StopReason vocabulary (mrs_msgs/msg/StopReason.msg,
# 350 §3-2 V1). This channel's legal subset is {1,3,5} — NON-contiguous.
# 🔴 Do not validate with a range check (350 §3-2, §4). Validate by set
# membership against the {1,3,5} subset.
uint64 seq
string robot
EdgeRef blocked_edge
bool declared
uint8 reason
