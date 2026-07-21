# ExecutionWindow — L3(sadg_service) → L4(l4_traj_node) 실행 창 릴리스 (★ load-bearing seam)
#
# 계약 버전: 1.0.0 / schema_version = 1
# 근거: D-05(창 릴리스 규약: 순서 hard + 구간 내 자유), D-11(뷰 정합), D-03(접합),
#       theory T1 §7(정지선 인코딩·정리 T1.2), T1-R6 = T2-R3(세그먼트 단위 무효화),
#       T2 §5(창 폐기 P5·P6·P7)
# 토픽: /robot_{i}/execution_window   QoS: reliable / volatile / depth 5 (deadline 없음 — 이벤트 구동)
#
# ── 두 개의 무효화 경로 (v3 NF7: 한 필드가 겸할 수 없다) ───────────────────────────────
#  (a) staleness repair  : window_valid_until 만료 → 창 **전체** 실효 (좀비 창 차단)
#  (b) 접합 시 부분 폐기 : revision_kind = REVISION_TRUNCATE + valid_through_segment_index
#                          → 커밋 경계 **바깥**만 폐기. 전체 폐기는 T1 I-QP 전제를 깨
#                          feasibility 귀납(정리 T1.1)을 무너뜨린다.
#
# ── 수신자(L4) 거동 ────────────────────────────────────────────────────────────────
#  * window_seq 가 현재 수용 seq 보다 작으면 **폐기**(순서 역전 방지, T2 P5).
#  * REVISION_TRUNCATE 의 valid_through_segment_index 가 자신의 커밋 접두(이미 QP 를 푼
#    구간)를 자르면 **거부**하고 커밋 접두 실행을 계속하며 CommitStatus.last_rejected_window_seq
#    로 보고한다(안전 폴백 — CLAUDE.md 규율 2). 예외를 던져 노드를 죽이지 않는다.
#  * window_valid_until 이 지나면 새 창이 올 때까지 감속 정지(T2 P6: 만료는 "멈출 수 없는 정지"를
#    요구해서는 안 되며 tau_i 하한이 존재한다 — A8).

std_msgs/Header header          # stamp = 릴리스 시각(sim clock), frame_id = "map"
uint16 schema_version           # 이 스키마의 버전. 현재 1. 불일치 시 수신자는 폐기 + 로그

uint16 robot_id                 # 창 소유 로봇 id
uint32 window_seq               # 로봇별 **단조증가** 창 시퀀스 (Q-1 답: 시간이 아닌 seq 로 수용 판정)
uint32 plan_epoch               # 이 창이 유래한 PlannedPaths.plan_epoch (구계획 창 식별용)
uint64 roadmap_version          # MapRegistry 지도 버전 (Q-6 캐시 무효화 키)
uint32 view_id                  # L2 균일 뷰 id (Q-7 — 세분화 입도가 다른 뷰의 노드 id 혼용 금지)

WindowSegment[] segments        # 실행 순서대로. REVISION_TRUNCATE 일 때는 비어 있어야 한다
PredecessorConstraint[] predecessor_constraints  # **hard**. 발행 시점 미충족분만

builtin_interfaces/Time window_valid_until  # 창 전체 만료 시각(sim clock). (a) staleness repair 전용

# 개정 종류
uint8 REVISION_NEW=0            # 신규/대체 창. segments 유효, valid_through_segment_index = -1
uint8 REVISION_TRUNCATE=1       # 기존 window_seq 의 부분 폐기 통지. segments 는 비움
uint8 revision_kind

int32 valid_through_segment_index  # REVISION_NEW: -1 (전 구간 유효)
                                   # REVISION_TRUNCATE: 인덱스 k 이하 세그먼트만 유효, k+1 이후 폐기.
                                   #   k 는 발행자(L3)가 A1(commit_nesting)·A9(monotone_stopline)를
                                   #   만족함을 확인한 값이어야 한다
