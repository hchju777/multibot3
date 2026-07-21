# RungEvent — 사다리 가로대 발동·전이 계측 이벤트 (Q-10 답: 계약 레지스트리에 **등록한다**)
#
# 계약 버전: 1.0.0 / schema_version = 1
# 근거: 06_implementation-requirements §4.2(가로대 발동/왕복 카운터), D-04 F2(R2 발동률 =
#       신규성 성립 조건), D-08 F6(스래싱), theory T3 §5 계측 M1·M2·M5·M6·M8·M9
# 토픽: /ladder/rung_event   QoS: reliable / volatile / depth 50
# 왜 계약인가: F2 의 신규성 판정과 F6 의 반례 실증(M6)이 이 스키마에 걸려 있어
#              **스키마 드리프트 = 실험 무효**다. 계측 토픽이지만 계약으로 잠근다.

std_msgs/Header header
uint16 schema_version    # 현재 1
uint64 event_id          # 교란 상관 키(T3-R3). M1(사건당 에스컬레이션 횟수)·M5(왕복)의 그룹 키

uint8 RUNG_R0=0          # 창 내 국소 흡수 (L4)
uint8 RUNG_R1=1          # BTPG 순서 유연화
uint8 RUNG_R2=2          # SADG MILP 재조정
uint8 RUNG_R3=3          # 부분 경로 재계획
uint8 RUNG_R4=4          # 작업 재할당
uint8 RUNG_R5=5          # 운영자 알림(사람)
uint8 rung

uint8 TRANSITION_ENTER=0        # 해당 가로대 진입(시도 시작)
uint8 TRANSITION_ABSORBED=1     # 해당 가로대에서 흡수 성공(상향 종료)
uint8 TRANSITION_ESCALATE=2     # 흡수 실패 → 상위 가로대로 상향
uint8 TRANSITION_GUARD_REJECT=3 # G-eps 수용 가드가 R3 릴리스를 거부(M8)
uint8 TRANSITION_ESCAPE_HATCH=4 # G-eps 탈출구 발동(도달불가 또는 연속 거부 N_reject 초과, M8)
uint8 transition

uint8 TRIGGER_HARD=0     # EscalationReport 계열(feasibility)
uint8 TRIGGER_SOFT=1     # judge slack 계열(efficiency)
uint8 trigger_kind

uint16[] affected_robots        # 영향 집합 A (M6 삼중쌍 요소 1, M9 churn 분모)
uint64 edge_set_fingerprint     # 사용 roadmap 엣지 집합의 순서무관 해시 (M6 삼중쌍 요소 3).
                                # 동일 (affected_robots, rung, fingerprint) 재방문 = 순환 증인
float64 phi_hat_before_s        # Phi_hat = sum_{i in A} t_hat_completion [s], 판정 직전 (M2)
float64 phi_hat_after_s         # 판정/재계획 직후 [s]. 미산출 시 NaN
uint32 attempt_count            # 해당 event_id 에 대한 이 가로대 시도 횟수 (D-04 게이팅 카운터)
float64 duration_s              # 이 가로대 처리 소요 시간 [s]
string detail                   # 사람이 읽을 부가 정보. 기계 판정에 쓰지 말 것
