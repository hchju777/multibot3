# CommitStatus — L4 → L3 상향 커밋 경계 보고 (T2-R6)
#
# 계약 버전: 2.0.0  (1.1.0 → 2.0.0 — **major**. `view_scope` 필수 필드 추가(V2 위반 시정).
#                    스코프 없는 v1.x 메시지는 수신 시 폐기되므로 wire 호환이 깨진다.
#                    승인: 오케스트레이터, 2026-07-21 / 레지스트리 §0.1.1·§0.1.2)
# (1.0.0 → 1.1.0 — qp_status 값 추가 + NF2' 철회 전파. theory T4 / 확정서 v4)
# 근거: theory T2 정의 T2.1(커밋 접두·커밋 경계·커밋 상태), 불변식 P0, 술어 A1(commit_nesting),
#       T1-R9(폴백 발동률 계측), T4 §8-(b) B5(폴백 사유별 분해 계측)
# 토픽: /robot_{i}/commit_status   QoS: reliable / volatile / depth 5 / deadline 2*Δt_h (주기 발행)
# 발행 주기: 재계획 틱마다(기본 Δt_h = 0.1 s, Q-11).
#
# 왜 EscalationReport 확장이 아니라 별도 토픽인가: 커밋 볼록포는 **평시에도** L3 가 알아야
# 접합 시 커밋 경계(정의 T2.1)를 계산할 수 있다. 에스컬레이션은 예외 사건이므로 겸할 수 없다.
# 수신 실패 시 L3 폴백: 커밋 경계를 **보수적으로 창 끝**으로 잡는다(접합이 늦어질 뿐 T1 은 안 깨짐).

std_msgs/Header header
uint16 robot_id
uint32 window_seq                        # 현재 수용 중인 창 seq
uint32 committed_through_segment_index   # 커밋 접두 Pi_i 의 끝 세그먼트 인덱스
                                         # (= ExecutionWindow.segments 의 인덱스)

# ── 뷰 스코프 (v2.0.0 신설) ────────────────────────────────────────────────────────────
#  아래 commit_boundary_node 가 유효한 뷰. **view_kind = VIEW_KIND_UNIFORM 고정**이며,
#  값은 보고 대상 창(window_seq)의 ExecutionWindow.roadmap_version/view_id 를 그대로 복사한다
#  (EscalationReport 와 동일 규약).
#  ⚠ view_kind 가 UNIFORM 이 아니거나 스코프가 수신자 기대와 다르면 메시지를 폐기한다
#    (ConvertStatus VIEW_KIND_MISMATCH / VIEW_SCOPE_MISMATCH — 레지스트리 §0.2).
#  왜 window_seq 로 암묵 해석하지 않는가: (i) 메시지에 스코프가 없으면 mrs_msg_convert 가
#  대조할 대상이 없어 판정이 노드로 넘어가 "판정은 convert, 보고는 노드" 경계가 무너지고,
#  (ii) L3 재시작·창 이력 정리 후에는 복원 자체가 불가능하다.
ViewScope view_scope

uint32 commit_boundary_node              # 커밋 경계 beta_i = last(Pi_i) — **균일 뷰(UniformNodeId)**.
                                         # 뷰 근거(순환 논증 아님, 레지스트리 §0.1.1):
                                         #  ① beta_i 는 committed_through_segment_index 가 가리키는
                                         #     ExecutionWindow.segments 원소의 종점 노드다.
                                         #  ② WindowSegment.node_from/node_to 는 균일 뷰로 고정돼 있다
                                         #     (근거: ExecutionWindow.view_id).
                                         #  ⇒ beta_i 가 물리·골격 노드가 될 수 있는 경로가 없다.
builtin_interfaces/Time predicted_arrival # beta_i 예측 도착 시각 t_hat(beta_i)
float64 predicted_theta_rad              # beta_i 예측 도착 자세 [rad], map, [-pi, pi] (T2-R1 근거)

Pose2D[] commit_hull_vertices  # 커밋 볼록포 C_i 의 볼록 정점열 [m], map, CCW.
                               # L3 의 A1(C_i ⊆ span(Pi_i) ⊆ span(W_i)) 검사 입력.
                               # 비어 있으면 "커밋 없음"(정지 상태)으로 해석한다
float64 brake_distance_m       # d_brake = u_max^2/(2 a_max) [m] — A7 선행거리 검사 입력

# 스텝 h 의 **채택 계획 종류**와 폴백 사유 (v1.1.0 — theory T4 §6.1 solve_ok 술어의 3항 분해).
# 값 0·1 은 v1.0.0 과 동일 의미이며, 3~5 는 폴백 사유 분해로 **추가**됐다.
uint8 QP_OK=0                  # solve_ok(h) 참 — QP 해 채택
uint8 QP_FALLBACK=1            # 인증 폴백 채택 (사유 미분류/일반). 정의 T4.1
uint8 QP_TICK_JITTER=2         # ⚠ **deprecated (v1.1.0)** — 지터·결번은 더 이상 폴백 사유가 아니다
                               #   (T4 §6.1: 폴백 여부는 solve_ok(h) 가 결정). **발행 금지**.
                               #   계측용 결번·지터 카운터는 별도 metrics 로 보고할 것
uint8 FALLBACK_NO_INDEX=3      # know_h 거짓 — 스텝 지수 h 를 어디서도 복원하지 못함
uint8 FALLBACK_NEIGHBOR_STALE=4 # fresh_j(h-1) 거짓 — 이웃 채택 계획 미수신. **QP 금지**(정리 T4.3)
uint8 FALLBACK_SOLVER=5        # solver_ok 거짓 — OSQP 미수렴/예산 초과
uint8 qp_status
float64 qp_solve_time_s        # 마지막 QP 해결 시간 [s]

float64 neighbor_traj_max_age_s  # max_j Delta_j — 이웃 궤적 최대 나이 [s].
                                 # ⚠ **NF2' 철회 전파(T4-R5)**: 이 값은 순수 **계측량**이다.
                                 # T1 §4.4 의 도달집합 팽창 폴백과 허용 상한 Delta_j^max 는
                                 # **철회됐다** — 일방 팽창은 거울 성질 n^ij = -n^ji 를 깨
                                 # Lemma 4 의 전제를 파괴한다(T4 §7.3, 확정서 v4 D-10).
                                 # **이 값을 안전 근거로 쓰지 말 것.** 손실 하 안전의 유일한
                                 # 건전 경로는 자기 인증 폴백(정리 T4.3)이며, 그 트리거는
                                 # 나이 임계가 아니라 fresh_j(h-1) 술어다.
uint32 last_rejected_window_seq  # 커밋 접두를 자르려 해서 거부한 창의 seq. 0 = 없음
