# EscalationReport — L4(l4_traj_node) → 서버(ladder_orchestrator) 상향 에스컬레이션
#
# 계약 버전: 1.1.0  (1.0.0 → 1.1.0: REASON_SYNC_LOST 추가 — 오케스트레이터 R-06 승인)
# 근거: D-01(상향 경로), D-08(이원 트리거 — 하드/소프트), D-02(HARD 트리거 = '주행 불가'),
#       T3-R3(event_id 상관 키), R-06/architecture §1.7-E(동기 결손 상향 보고처)
# 토픽: /robot_{i}/escalation_report   QoS: reliable / volatile / depth 20
#
# ── 사유 enum (Q-4 답: 아래 5종이 **확정 집합**, v1.1.0) ──────────────────────────────
#  BLOCKED           : 정지선/장애로 진행 불가가 지속 — D-02 HARD 트리거 원천
#  WINDOW_EXPIRING   : 창 소진 임박(남은 세그먼트·시간 부족) — 재릴리스 요청, SOFT
#  QP_INFEASIBLE     : QP 솔버 실패/예산 초과로 **초기궤적 폴백**이 발동 (T1 §6.7).
#                      ※ 정리 T1.2 따름정리에 의해 "정지 유지"는 항상 실행가능하므로 이 값은
#                        이론적 infeasibility 가 아니라 **솔버 실패**를 뜻한다. 오독 금지.
#  LOCALIZATION_LOST : 자기 위치 추정 상실 — 안전 정지 + HARD
#  SYNC_LOST         : **동기 재계획 틱 결손**(v1.1.0 신설). /plan_tick 연속 결번이 허용 상한 k 회를
#                      초과 — 정리 T1 의 전제 N5(동기 재계획)가 성립하지 않는 상태.
# enum 확장은 계약 변경 게이트를 거친다(D-02 분기표·D-08 라우팅표가 직접 매핑되므로
# 조용한 추가는 사다리 라우팅을 무효화한다).
#
# ── SYNC_LOST 의 의미론 (R-06 구속 — 라우팅 오발동 방지) ──────────────────────────────
#  severity : HARD (다른 HARD 3종과 동일 취급)
#  ⛔ **그러나 R3(부분 재계획)·R4(재할당)로 라우팅하지 말 것.** 동기 결손은 계획 문제가 아니라
#     시간 동기·통신 문제이며, 해법은 재계획이 아니라 **안전 정지 + 동기 복구**다. BLOCKED 로
#     보내 R3/R4 를 오발동시키면 F2 의 R2 발동률(D-04 신규성 성립 조건)과 F6 의 스래싱
#     카운터(D-08 수렴 판정)가 오염된다 — 이것이 이 값을 신설한 이유다.
#     D-08 라우팅표는 SYNC_LOST 를 **안전 정지·동기 복구 경로**로 보내고, 복구 실패가 지속될
#     때에만 R5(운영자 알림)로 승격한다.
#  발행 조건 : /plan_tick 연속 결번(tick_seq 건너뜀)이 k 회를 초과. **k 는 ROS2 param 이며
#     값은 미정**이다 — Δt_h·DDS 실측 지터 종속이라 [0a] 실측 전에 정할 근거가 없다
#     (레지스트리 미결 U-11 / architecture Q-16). 값을 지어내지 말 것.
#  발행 전 거동 : 틱 1회 미도착·지터 초과는 그 스텝만 초기궤적 폴백(CommitStatus.qp_status =
#     QP_TICK_JITTER)으로 처리하고 **에스컬레이션하지 않는다**. k 회 초과 시에만 이 값을 발행하고
#     정지 유지로 수렴한다. **틱 외삽 금지**(자체 시계로 틱을 지어내면 N5 를 스스로 깬다).

std_msgs/Header header
uint64 event_id          # 교란 발생부터 해소까지 관통하는 상관 키(T3-R3). 0 = 미할당(금지)
uint16 robot_id

uint8 REASON_BLOCKED=0
uint8 REASON_WINDOW_EXPIRING=1
uint8 REASON_QP_INFEASIBLE=2
uint8 REASON_LOCALIZATION_LOST=3
uint8 REASON_SYNC_LOST=4   # v1.1.0 신설. 라우팅은 R3/R4 가 아니라 안전 정지·동기 복구 (위 주석)
uint8 reason

uint8 SEVERITY_HARD=0    # 즉시 사다리 상향 — BLOCKED / QP_INFEASIBLE / LOCALIZATION_LOST / SYNC_LOST
uint8 SEVERITY_SOFT=1    # 효율 저하 계열 — WINDOW_EXPIRING
uint8 severity

uint32 window_seq            # 보고 시점에 수용 중이던 창의 seq
uint32 current_segment_index # 진행 중 세그먼트 인덱스 (segments 배열 기준)
uint32 blocking_node_id      # 진행을 막는 노드 id. 4294967295 = 해당 없음
uint16 blocking_robot_id     # 정지선을 소유한 선행 로봇 id. 65535 = 미상/해당 없음
float64 stalled_duration_s   # 진행 정체 지속 시간 [s]. 0 이상

Pose2D pose                  # 보고 시점 관측 자세 (map 프레임)
float64 v_mps                # 몸체 선속도 [m/s]
float64 omega_rps            # 몸체 각속도 [rad/s]
