# RobotState — sim_bridge → 전 구독자 (로봇 상태 스트림)
#
# 계약 버전: 2.0.0  (1.0.0 → 2.0.0 — **major**. `view_scope` 필수 필드 추가 + 노드 id 의 뷰 확정.
#                    스코프 없는 v1.x 메시지는 수신 시 폐기되므로 wire 호환이 깨진다. R-14 (a))
# 근거: D-01, architecture §5.2(sim 누수 금지 — 관측 불확실성 필드 필수)
# 토픽: /robot_{i}/robot_state   QoS: best_effort / volatile / depth 1 / deadline 2*publish_period
# 주의: 이 메시지는 **시뮬 백엔드가 유일한 발행자**다. 코어 노드는 시뮬레이터를 모른다.

std_msgs/Header header    # stamp = 시뮬 시각(sim clock), frame_id = "map"
uint16 robot_id
Pose2D pose               # 몸체 기준점 자세 (map). NID 오프셋점 z 가 아니라 **몸체점 q** 다
float64 v_mps             # 몸체 선속도 [m/s]
float64 omega_rps         # 몸체 각속도 [rad/s]

# ── 뷰 스코프 (v2.0.0 신설) ────────────────────────────────────────────────────────────
#  아래 노드 id 2건이 유효한 뷰. **view_kind = VIEW_KIND_UNIFORM 고정.**
#  뷰 근거: 이 값들은 L3 진행 추적과 L4 창 소비의 입력이고, 두 경로가 쓰는 좌표계는
#  ExecutionWindow.segments(균일 뷰)다. 물리 노드를 실으면 진행 판정이 세그먼트와 어긋난다.
#  시뮬 백엔드는 연속 좌표를 갖고 있을 뿐이므로, 노드 부착은 MapRegistry 균일 뷰 기준으로
#  수행하고 그때 사용한 (roadmap_version, view_id)를 여기에 싣는다.
ViewScope view_scope

uint32 occupied_node      # 현재 점유 중인 노드 id — **균일 뷰(UniformNodeId)**. 4294967295 = 엣지 위
uint32 next_node          # 진행 방향 다음 노드 id — **균일 뷰**. 4294967295 = 미상/정지
float64 edge_progress     # 현재 엣지 진행률 [0.0, 1.0]. occupied_node 유효 시 0.0

ObservationUncertainty observation  # sim 누수 방지 (§5.2)
