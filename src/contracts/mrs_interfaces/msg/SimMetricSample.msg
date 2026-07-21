# SimMetricSample — 시뮬 백엔드 metrics tap 표본 (sim-abstraction 계약)
#
# 계약 버전: 1.0.0
# 근거: sim-abstraction(metrics tap 은 백엔드 공통 포트), architecture §5.3(충실도 태깅)
# 토픽: /sim/metrics_tap   QoS: reliable / volatile / depth 50
# 용도: RobotState 로부터 유도할 수 없는 **백엔드 고유 진실값**만 싣는다
#       (예: 접촉 충돌 횟수, 최소 이격거리, 실제 배속). 파생 지표는 mrs_metrics 가 계산한다.

std_msgs/Header header
string key                # 지표 키. 예: "collision_count", "min_separation_m", "realtime_factor"
float64 value             # 값. 단위는 key 접미사로 표기(_m, _s, _mps). 무단위는 접미사 없음
uint16 robot_id           # 로봇별 지표면 로봇 id, 전역 지표면 65535

uint8 TAG_VALID=0             # 이 백엔드 충실도에서 유효한 값
uint8 TAG_OPEN_PENDING_ISAAC=1 # 충실도 부족 — 연구 검증으로 승격 금지 (architecture §5.3)
uint8 fidelity_tag        # SimCapabilities.physics_fidelity 로부터 백엔드가 자동 부여
