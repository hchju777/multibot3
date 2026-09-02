# OperatorVerb vocabulary (350 §3-2 V2~V6).
uint8 VERB_UNKNOWN=0
uint8 VERB_HOLD=1
uint8 VERB_SPEED_CAP=2
uint8 VERB_GOAL_OVERRIDE=3

uint64 seq
string robot
uint8 verb
bool active
# Optional numeric field: has_* companion (350 §4-3). Required only when
# verb==VERB_SPEED_CAP and active==true (layer-2 condition, not layer-1).
float64 speed_cap_mps
bool has_speed_cap_mps
# Optional STRING field: "" sentinel, NOT a has_* flag (350 §4-3 rule — this
# message is the one place both conventions coexist). Required only when
# verb==VERB_GOAL_OVERRIDE and active==true.
string goal_location
