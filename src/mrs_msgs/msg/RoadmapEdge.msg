# EdgeTraversal vocabulary (350 §3-2 V2~V6). 0 is reserved *_UNKNOWN across all
# vocabularies (350 D1-a) — do not hand it to the first legal value.
uint8 TRAVERSAL_UNKNOWN=0
uint8 TRAVERSAL_BIDIRECTIONAL=1
uint8 TRAVERSAL_FORWARD_ONLY=2
uint8 TRAVERSAL_BACKWARD_ONLY=3

string from_id
string to_id
uint8 traversal
float64 length_m
float64 width_m
# 350 §3-2 V7: uint8 but NOT a vocabulary — no constants, value carried as-is.
uint8 capacity_robots
# "" sentinel = no corridor reference (roadmap.schema.json:88, 350 §4-3).
string corridor
