# RoadmapNode — MapRegistry 노드 (물리 roadmap / 균일 뷰 / 의존성 골격 공통 표현)
#
# 근거: D-11(3뷰 정합·왕복 무손실), T1 §7.2·T1-R6(정지선 claim region 반경 r_clear)

uint32 node_id              # 뷰 종류는 **이 메시지를 담은 응답이 고정**한다:
                            #   GetRoadmap → 물리(PhysicalNodeId) / GetUniformView → 균일(UniformNodeId)
                            #   GetDependencySkeleton → 골격(SkeletonNodeId)
                            # 인스턴스 스코프는 같은 응답의 roadmap_version·view_id 다
float64 x_m                 # 노드 위치 x [m], map
float64 y_m                 # 노드 위치 y [m], map
float64 claim_radius_m      # r_clear [m] — 이 노드의 점유 주장 반경. 정지선 오프셋 계산에 사용.
                            # ⚠ 뷰별 규약(재잠금 8차 (j), §0.1 정본표): 균일 뷰 = **그 노드의 실제
                            #   인접 간격 최솟값 / 2**(요청 unit/2 가 아님 — 짧은 인접 간격에서 원판이
                            #   겹쳐 T1 §7.2 정지선이 무의미해지는 것을 막는다). 하드 하한: r_clear >= r
                            #   (T1-R6 C4, ValidateMap.r_clear_ok). 산정 정밀화는 OPEN(T1 O6).
bool is_endpoint            # well-formed 검사의 엔드포인트(대기 가능 지점) 여부
bool is_task_endpoint       # 픽업/딜리버리 등 작업 엔드포인트 여부 (비작업 엔드포인트 = endpoint ∧ ¬task)
bool is_virtual             # 균일 뷰 세분화로 생성된 가상 노드(D-11 v2). 물리 뷰에서는 항상 false
