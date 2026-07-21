# NodeMapping — 뷰 간 노드 대응 1건 (D-11 왕복 무손실의 표현체)
#
# ⚠ 이 메시지는 **한 원소 안에 서로 다른 뷰의 id 가 공존**하는 유일한 계약이다. 뷰 종류는
#   필드별·용례별로 **계약이 고정**하며, 방향을 뒤집어 담는 것은 계약 위반이다(§0 규칙 V4).
# 용례 1 (세분화, GetUniformView.subdivision_map):
#     from_node_id = **균일 뷰** 가상 노드(UniformNodeId) → to_edge_id = **물리** 엣지 + s
# 용례 2 (collapse, GetDependencySkeleton.collapse_map):
#     from_node_id = **균일 뷰** 노드(UniformNodeId) → to_node_id = **골격** 노드(SkeletonNodeId)

uint32 from_node_id   # 항상 **균일 뷰(UniformNodeId)**
uint32 to_node_id     # 용례 2 에서 **골격 뷰(SkeletonNodeId)**. 4294967295 = 해당 없음
uint32 to_edge_id     # 용례 1 에서 **물리 뷰** 엣지 id. 4294967295 = 해당 없음
float64 s             # to_edge 위의 정규화 위치 [0.0, 1.0] (node_a → node_b 방향). to_edge 유효 시만
