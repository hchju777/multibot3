# NodeMapping — 뷰 간 노드 대응 1건 (D-11 왕복 무손실의 표현체)
#
# ⚠ 이 메시지는 **한 원소 안에 서로 다른 뷰의 id 가 공존**하는 유일한 계약이다. 뷰 종류는
#   필드별·용례별로 **계약이 고정**하며, 방향을 뒤집어 담는 것은 계약 위반이다(§0 규칙 V4).
# 용례 1 (세분화, GetUniformView.subdivision_map):
#     from_node_id = **균일 뷰** 가상 노드(UniformNodeId) → to_edge_id = **물리** 엣지 + s
#     index/denominator = 물리 엣지 위 k/n 서수 (k = 0..n, n = 그 물리 엣지의 분할 수)
# 용례 2 (collapse 대응, GetDependencySkeleton.collapse_map):
#     from_node_id = **균일 뷰** 노드(UniformNodeId) → to_node_id = **골격** 노드(SkeletonNodeId)
# 용례 3 (collapse 체인 내부 노드, 재잠금 8차 (c) 신설):
#     from_node_id = 삼켜진 **균일 뷰** 노드(UniformNodeId) → to_edge_id = 그 노드를 삼킨
#     **골격** 엣지(SkeletonEdgeId), index/denominator = 그 골격 엣지 체인 내 서수(순서/체인 길이).
#     이 용례가 skeleton → uniform 체인 전개(왕복 무손실)의 표현체다 — 용례 2 만으로는 체인
#     내부 노드를 담을 자리가 없어 왕복이 원리적으로 불가했다(계약 자기모순, GATE Q4 해소).
#
# ── 서수(index/denominator) vs 기하 위치(s) — 섞지 말 것 ──────────────────────────────
#  index/denominator 는 **정수 서수**다. 왕복 무손실의 순서 복원은 이 정수쌍으로만 한다 —
#  부동소수 s 로 순서를 정렬하면 같은 s 를 갖는 두 노드가 생길 여지가 있고 경계에서 오차가
#  새어 "오차 0"이 프로세스 밖에서 무너진다(GATE Q4). s 는 **용례 1 의 기하 위치 전용**이며,
#  용례 3 에서는 사용하지 않는다(좌표 보간 금지 — 체인 순서는 기하가 아니라 위상 정보다).

uint32 from_node_id   # 항상 **균일 뷰(UniformNodeId)**
uint32 to_node_id     # 용례 2 에서 **골격 뷰(SkeletonNodeId)**. 4294967295 = 해당 없음
uint32 to_edge_id     # 용례 1 에서 **물리 뷰** 엣지 id / 용례 3 에서 **골격 뷰** 엣지 id
                      # (SkeletonEdgeId). 4294967295 = 해당 없음
float64 s             # **용례 1 전용** — to_edge 위의 정규화 기하 위치 [0.0, 1.0] (node_a → node_b
                      # 방향). 용례 3 에서는 사용하지 않는다(순서는 index/denominator 로 담는다)
uint32 index          # 서수 (정수). 용례 1 = 물리 엣지 위 k(0..n) / 용례 3 = 골격 엣지 체인 내 순서.
                      # 왕복 무손실의 순서 복원은 이 값으로만 한다(부동소수 s 정렬 금지)
uint32 denominator    # 서수의 분모 (정수). 용례 1 = 물리 엣지의 분할 수 n / 용례 3 = 골격 엣지
                      # 체인 길이. index/denominator 가 서수 비율이며, denominator = 0 은 서수 미사용
                      # (용례 2 = 노드↔노드 대응, 서수 없음)
