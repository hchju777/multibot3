# TaskAssignment — L1(ta_service) → L2(pp_service) 작업 배정
#
# 계약 버전: 2.0.0  (1.0.0 → 2.0.0 — **major**. `view_scope` 필수 필드 추가. 이 메시지는 노드 id 를
#                    실으면서 유효 스코프(roadmap_version)를 갖지 않았다 — R-14 (a) C 요건 미충족.
#                    스코프 없는 v1.x 메시지는 수신 시 폐기되므로 wire 호환이 깨진다)
# 근거: D-02(분리형 lifelong TA), 완주 분해 조건 ④(무기아 — aging)
# 토픽: /task_assignment   QoS: reliable / volatile / depth 5

std_msgs/Header header
uint32 assignment_epoch  # 배정 세대 번호. 단조증가
uint64 task_id           # 작업 id. lifelong 전 구간 유일
uint16 robot_id          # 배정 로봇 id. 65535 = 회수(unassign)
# ── 뷰 스코프 (v2.0.0 신설) ────────────────────────────────────────────────────────────
#  아래 노드 id 2건이 유효한 뷰. **view_kind = VIEW_KIND_PHYSICAL 고정, view_id = 0.**
#  뷰 근거: 작업 엔드포인트는 창고 도면의 실제 지점이며 세분화 가상 노드가 아니다
#  (RoadmapNode.is_task_endpoint 는 물리 뷰 속성). L2 는 이를 균일 뷰로 변환해 계획한다 —
#  그 변환은 MapRegistry 의 단독 책임이며 L1 은 물리 뷰만 안다.
ViewScope view_scope

uint32 pickup_node       # 픽업 노드 id — **물리 roadmap(PhysicalNodeId)**. 균일 뷰 아님
uint32 delivery_node     # 딜리버리 노드 id — **물리 roadmap(PhysicalNodeId)**
float64 waiting_time_s   # 작업 도착 후 대기 시간 [s] — aging 우선순위 근거, 기아율 계측 원천
uint32 reassign_count    # 이 작업이 재할당된 누적 횟수 (D-02 상한·R5 승격 판정, M7 핑퐁 계측)
