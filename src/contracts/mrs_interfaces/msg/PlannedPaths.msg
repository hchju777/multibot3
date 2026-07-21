# PlannedPaths — L2(pp_service) → L3(sadg_service) 계획 산출물
#
# 계약 버전: 1.0.0
# 근거: D-03(솔버 불가지론 표준형), D-11 v2(뷰 정합 — view_id 로 collapse 정합 보장, Q-7)
# 토픽: /planned_paths   QoS: reliable / volatile / depth 5

std_msgs/Header header
uint64 event_id        # 이 계획을 유발한 사건의 상관 키. 정기 계획이면 0
uint32 plan_epoch      # 계획 세대 번호. **단조증가**. 구계획 창 식별(ExecutionWindow.plan_epoch)
uint64 roadmap_version # 계획이 사용한 지도 버전
uint32 view_id         # 계획이 사용한 균일 뷰 id (입도가 다른 뷰 혼용 금지 — D-11 무손실 요건)

bool is_partial        # true = R3 부분 재계획 결과(영향 집합만 포함), false = 전역/신규 계획
RobotPath[] paths      # is_partial 이면 영향 집합 A 의 로봇만 실린다
