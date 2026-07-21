# RobotState — sim_bridge → 전 구독자 (로봇 상태 스트림)
#
# 계약 버전: 1.0.0
# 근거: D-01, architecture §5.2(sim 누수 금지 — 관측 불확실성 필드 필수)
# 토픽: /robot_{i}/robot_state   QoS: best_effort / volatile / depth 1 / deadline 2*publish_period
# 주의: 이 메시지는 **시뮬 백엔드가 유일한 발행자**다. 코어 노드는 시뮬레이터를 모른다.

std_msgs/Header header    # stamp = 시뮬 시각(sim clock), frame_id = "map"
uint16 robot_id
Pose2D pose               # 몸체 기준점 자세 (map). NID 오프셋점 z 가 아니라 **몸체점 q** 다
float64 v_mps             # 몸체 선속도 [m/s]
float64 omega_rps         # 몸체 각속도 [rad/s]

uint32 occupied_node      # 현재 점유 중인 균일 뷰 노드 id. 4294967295 = 엣지 위
uint32 next_node          # 진행 방향 다음 노드 id. 4294967295 = 미상/정지
float64 edge_progress     # 현재 엣지 진행률 [0.0, 1.0]. occupied_node 유효 시 0.0

ObservationUncertainty observation  # sim 누수 방지 (§5.2)
