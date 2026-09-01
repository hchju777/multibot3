# Directed pair of roadmap-node identifiers reused across three JSON shapes
# (350 §6 "재사용 결정 둘"): roadmap.corridors[].edges (2-element tuple),
# stop_declaration.blocked_edge (object {from,to}), replan_request.blocked[]
# (object {from,to}). The io/ adapter absorbs both JSON shapes into this one
# type (350 §8 D6 item 8) — that adapter code is outside this round's scope.
string from_id
string to_id
