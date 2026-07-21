# JudgeVerdict — judge_module(L3) → ladder_orchestrator 승격 판정 (D-06)
#
# 계약 버전: 1.0.0
# 근거: D-06(R2↔R3 **단일 경계** 한정 — 확장은 검증할 가설), holistic-adg-monitoring-2025
#       Algorithm 1 재구현, theory T3 §4(G-eps 가드의 Phi_hat)
# 토픽: /ladder/judge_verdict   QoS: reliable / volatile / depth 50
#
# 경계 규율: 이 메시지는 **R2↔R3 경계 판정만** 나른다. R1→R2 게이팅(D-04)은 btpg_module
# 내부 카운터로만 결정되며 judge 를 참조하지 않는다(v2 C3 독립성 — F2·F3 교락 방지).
# 따라서 이 메시지에 R1 관련 필드를 추가하는 것은 **계약 위반**이다.

std_msgs/Header header
uint64 event_id
bool promote_r3                 # true = R3 부분 재계획 승격, false = R2 에 머문다
uint16[] direct_affected_robots # 직접 영향 집합 D (slack 전파 결과). 축약 확장 A 는 별도 모듈 산출(T2 §4.3)
float64 completion_excess_s     # (예상 완료시각 − 계획 완료시각) 최대값 [s] — 판정량
float64 threshold_s             # 적용된 소프트 트리거 임계 [s]. ROS2 param, 초기값 2.0 (D-06 v2)
float64 phi_hat_s               # Phi_hat = sum_{i in D} t_hat_completion [s] — G-eps 가드 입력(T3 §4)
