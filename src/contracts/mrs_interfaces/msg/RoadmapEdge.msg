# RoadmapEdge — MapRegistry 엣지
#
# 근거: D-11, theory T1 §4.2(N2 통로 폭 W_min > 2(r + v_max/omega_max), 속도 디레이팅)

uint32 edge_id   # 뷰 종류는 RoadmapNode.node_id 와 동일 규칙(담은 응답이 고정)
uint32 node_a    # 같은 뷰의 노드 id
uint32 node_b    # 같은 뷰의 노드 id
float64 length_m            # 엣지 길이 [m]
float64 corridor_width_m    # 이 엣지 구간의 최소 통로 폭 W_min [m] — 검사기 3번 입력
float64 speed_limit_mps     # 이 구간 허용 최대 몸체 속도 [m/s].
                            # W_min 미달 시 디레이팅 v'_max = (W_min/2 − r)*omega_max 가 반영된 값
