# CommitState — 접합 시작 상태 = theory 정의 T2.1 의 커밋 상태 s_hat_i (Q-5 답)
#
# 계약 버전: 1.0.0
# 근거: theory T2 정의 T2.1 — 커밋 상태 = (beta_i, t_hat(beta_i), theta_hat(beta_i)).
#       T2-R1: **theta 필수**(diff-drive 회전 비용. 자세 없이 접합하면 L2 가 낸 순서가
#       물리적으로 값비싼 회전을 강요한다 — adg-tradeoffs-icaps2026 회전 27~33%).
#
# Q-5 표현형 결정: **연속 pose 가 아니라 (노드 + 예측 도착시각 + 예측 자세)** 다.
#  - L2 의 정의역은 roadmap 노드이므로 연속 pose 를 넘기면 L2 가 쓸 수 없다.
#  - 그러나 theta 는 노드로 환원되지 않는 자유도라 별도 필수 필드로 싣는다.
#  - 커밋 볼록포 자체는 여기 싣지 않는다 — L3 가 CommitStatus 로 이미 보유하며,
#    L2 는 볼록포를 쓰지 않는다(계약 최소성).
# ⚠ theta_hat 은 **예측값**이며 오차 전파의 유계화는 OPEN(theory O-T2-3).

uint16 robot_id
uint32 boundary_node                       # 커밋 경계 beta_i 의 균일 뷰 노드 id
builtin_interfaces/Time predicted_arrival  # t_hat(beta_i) — 예측 도착 시각 (sim clock)
float64 predicted_theta_rad                # theta_hat(beta_i) [rad], map, [-pi, pi]
uint32 window_seq                          # 커밋 경계를 산출한 창의 seq (정합 감사용)
uint32 committed_through_segment_index     # 그 창에서의 커밋 접두 끝 인덱스 (A1 감사용)
