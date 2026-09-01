# Shared closed vocabulary for mrs.stop_declaration.declarations[].reason
# (legal subset {1,3,5}) and mrs.replan_request.reason (legal subset
# {1,2,3,4,5,6}) — 350 §3-2 V1, D1-c. ONE numbering, not two.
# 🔴 [추정] whether rosidl accepts a constants-only, zero-field .msg is
# UNCONFIRMED before the first colcon build (350 §3-4). If rejected, the
# documented fallback is to declare these seven constants inside
# StopDeclaration.msg instead (350 §3-4) — not applied here pre-emptively.
uint8 REASON_UNKNOWN=0
uint8 REASON_EXOGENOUS_BLOCK=1
uint8 REASON_CTRL_STOP_DECLARED=2
uint8 REASON_INFEASIBLE_SUBGOAL=3
uint8 REASON_ORIENTATION_FLIP=4
uint8 REASON_UNRESOLVABLE_LOCALLY=5
uint8 REASON_HORIZON_EXHAUSTING=6
