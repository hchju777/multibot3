# RoadmapNode — MapRegistry 노드 (물리 roadmap / 균일 뷰 / 의존성 골격 공통 표현)
#
# 근거: D-11(3뷰 정합·왕복 무손실), T1 §7.2(정지선의 claim region 반경 r_clear)

uint32 node_id
float64 x_m                 # 노드 위치 x [m], map
float64 y_m                 # 노드 위치 y [m], map
float64 claim_radius_m      # r_clear [m] — 이 노드의 점유 주장 반경. 정지선 오프셋 계산에 사용.
                            # ⚠ 산정식은 OPEN(theory T1 O6). 보수적 초기값 = unit_length_m / 2
bool is_endpoint            # well-formed 검사의 엔드포인트(대기 가능 지점) 여부
bool is_task_endpoint       # 픽업/딜리버리 등 작업 엔드포인트 여부 (비작업 엔드포인트 = endpoint ∧ ¬task)
bool is_virtual             # 균일 뷰 세분화로 생성된 가상 노드(D-11 v2). 물리 뷰에서는 항상 false
