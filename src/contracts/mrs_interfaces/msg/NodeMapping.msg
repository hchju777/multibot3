# NodeMapping — 뷰 간 노드 대응 1건 (D-11 왕복 무손실의 표현체)
#
# 용례 1 (세분화): 균일 뷰 가상 노드 → 물리 엣지 위의 위치.  from=가상노드, to_edge 유효, s 유효
# 용례 2 (collapse): 균일 뷰 노드 → 의존성 골격 노드.        from=뷰노드, to_node 유효

uint32 from_node_id
uint32 to_node_id     # 대응 노드 id. 4294967295 = 해당 없음
uint32 to_edge_id     # 대응 물리 엣지 id. 4294967295 = 해당 없음
float64 s             # to_edge 위의 정규화 위치 [0.0, 1.0] (node_a → node_b 방향). to_edge 유효 시만
