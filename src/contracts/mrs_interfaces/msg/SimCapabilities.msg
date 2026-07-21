# SimCapabilities — 시뮬 백엔드 자기신고 (Q-8 답: **포함한다**)
#
# 계약 버전: 1.0.0
# 근거: architecture §5.2·§5.3(kill-gate 정직성), 오케스트레이터 R-01(백엔드가 스스로 능력을
#       신고해 sim-runner 의 OPEN(pending-isaac) 태깅을 자동화할 것), sim-abstraction 규율
#
# sim-runner 규약: physics_fidelity < FIDELITY_FULL_PHYSICS 인 백엔드에서 얻은
#   QP feasibility rate · livelock rate · 물리 재현성 지표는 **자동으로 OPEN(pending-isaac)**
#   으로 태깅한다. 이 판정을 사람이 손으로 뒤집는 것은 금지다.

string backend_name              # "fake" | "pysim" | "isaac" (자유문자열 아님 — 이 3종)
bool deterministic_with_seed     # 시드 고정 시 완전 재현 가능한가. pysim=true, isaac=false 예상
uint64 seed                      # 현재 적용된 시드. deterministic_with_seed=false 면 참고값

uint8 FIDELITY_NONE=0            # 기하만 (FakeSimBackend)
uint8 FIDELITY_KINEMATIC=1       # 이산 운동학 (pysim)
uint8 FIDELITY_FULL_PHYSICS=2    # 접촉·동역학 (isaac)
uint8 physics_fidelity

bool supports_partial_observation  # ObservationUncertainty 를 실제로 채우는가
bool supports_contact              # 충돌 접촉을 물리적으로 판정하는가
bool supports_fault_injection      # SimInject 훅 지원 여부
float64 nominal_realtime_factor    # 공칭 배속 (1.0 = 실시간). tracer bullet [0]의 실측 대상
float64 actuate_to_state_latency_s # actuate → 상태 반영 지연 [s]. pysim 에서 0 이상으로 스윕 가능해야
                                   # 한다(§5.2 누수 조기 발견). 코어는 "즉시 반영"을 전제하지 말 것
