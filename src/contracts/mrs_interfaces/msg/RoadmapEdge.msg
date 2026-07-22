# RoadmapEdge — MapRegistry 엣지
#
# 근거: D-11, theory T1 §4.2(통로 폭·속도 디레이팅), T1-R6(순수기하 3단 검사기)
#
# ── 기하 전제 (재잠금 8차 (g)) ────────────────────────────────────────────────────────
#  **물리 뷰 엣지는 직선이다** — 로더가 |length_m − ‖p_a−p_b‖| <= tol 로 강제한다(그 없이는
#  세분화 가상 노드가 통로 밖에 놓여 좌표 변환이 조용히 틀린다). 골격 뷰 엣지는 정의상 여러
#  물리 엣지를 삼킨 **폴리라인**이라 이 전제가 적용되지 않는다.
#  ⚠ 골격 엣지의 **균일 스텝 수**는 별도 필드로 싣지 않는다((f), 신규 필드 기각) — 같은 정보를
#     두 곳에 두면 드리프트한다. collapse_map(NodeMapping 용례 3)의 denominator 로 복원한다.

uint32 edge_id   # 뷰 종류·엔티티는 담은 응답이 고정(PhysicalEdgeId / UniformEdgeId / SkeletonEdgeId)
uint32 node_a    # 같은 뷰의 노드 id
uint32 node_b    # 같은 뷰의 노드 id
float64 length_m            # 엣지 길이 [m]. 물리 뷰는 직선이라 ‖p_a−p_b‖ 와 tol 이내 일치
float64 corridor_width_m    # 이 엣지 구간의 최소 통로 폭 W_min [m] — 순수기하 3단 검사기 입력.
                            # 3단 임계: 통과 2ρ / 90°점턴 2ρ+L* / U턴 2(ρ+L*), 교행(검사기4) 4ρ
float64 speed_limit_mps     # 이 구간 허용 최대 몸체 속도 [m/s].
                            # W_min 미달 시 디레이팅 v'_max = (W_min/2 − r)*omega_max 가 반영된 값
                            # (v_max 의존 — pending-[0b]. 순수기하 3단은 이 값과 무관)
