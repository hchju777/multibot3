# TaskAssignment — L1(ta_service) → L2(pp_service) 작업 배정
#
# 계약 버전: 1.0.0
# 근거: D-02(분리형 lifelong TA), 완주 분해 조건 ④(무기아 — aging)
# 토픽: /task_assignment   QoS: reliable / volatile / depth 5

std_msgs/Header header
uint32 assignment_epoch  # 배정 세대 번호. 단조증가
uint64 task_id           # 작업 id. lifelong 전 구간 유일
uint16 robot_id          # 배정 로봇 id. 65535 = 회수(unassign)
uint32 pickup_node       # 픽업 노드 id (물리 roadmap 기준 — 균일 뷰 아님)
uint32 delivery_node     # 딜리버리 노드 id (물리 roadmap 기준)
float64 waiting_time_s   # 작업 도착 후 대기 시간 [s] — aging 우선순위 근거, 기아율 계측 원천
uint32 reassign_count    # 이 작업이 재할당된 누적 횟수 (D-02 상한·R5 승격 판정, M7 핑퐁 계측)
