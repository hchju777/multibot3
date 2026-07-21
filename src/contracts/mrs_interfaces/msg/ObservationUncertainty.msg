# ObservationUncertainty — 관측 불확실성 (sim 누수 방지 필드, architecture §5.2)
#
# 왜 지금 넣는가: pysim 은 완전관측 이상화가 가능하지만, 코어가 "즉시·무지연·완전관측"을
# 전제하면 isaac 부착 시 무너진다(sim-abstraction 규율 3 누수 금지). 이 필드가 없으면
# isaac 부착 시 RobotState 계약 자체가 깨진다. pysim 은 0/이상화 값을 채운다.

float64 pos_std_m         # 위치 추정 표준편차 [m]. pysim = 0.0
float64 theta_std_rad     # 자세 추정 표준편차 [rad]. pysim = 0.0
bool partial_observation  # true = 일부 상태가 관측되지 않음(값은 마지막 추정치). pysim = false
builtin_interfaces/Time observation_stamp  # 이 관측이 유효한 시뮬 시각. header.stamp 와 다를 수 있다
