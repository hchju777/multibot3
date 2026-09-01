# DependencyType vocabulary (350 §3-2 V2~V6).
uint8 DEP_UNKNOWN=0
uint8 DEP_SEQUENTIAL=1
uint8 DEP_ORDERING=2

# 🔴 from_id/to_id here are SEGMENT identifiers ({robot}#{index}), NOT roadmap
# node ids (350 §8 D6 item 1). Do NOT nest an EdgeRef into this message — the
# name space differs from RoadmapEdge/Segment's from_id/to_id even though the
# JSON source field names ("from"/"to") are identical.
string from_id
string to_id
uint8 type
