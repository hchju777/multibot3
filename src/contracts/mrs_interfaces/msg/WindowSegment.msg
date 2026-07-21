# WindowSegment — 실행 창의 단위 구간 (node-to-node)
#
# 근거: D-05 (ExecutionWindow 스키마 = segments:(node_from,node_to)[])
# 노드 id 는 **L2 균일 뷰(D-11 v2) 기준**이며, 해당 뷰의 식별자는 ExecutionWindow.view_id 와
# ExecutionWindow.roadmap_version 이 지정한다. 길이·통로폭·속도상한 같은 기하 속성은
# 여기에 중복 싣지 않는다 — 수신자는 MapRegistry(roadmap_version 키) 캐시에서 조회한다(Q-6).

uint32 node_from   # 출발 노드 id (균일 뷰). MapRegistry GetUniformView 로 조회 가능해야 한다
uint32 node_to     # 도착 노드 id (균일 뷰). node_from 과 균일 뷰에서 인접해야 한다 (불변식 W2)
