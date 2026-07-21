#pragma once

/**
 * @file i_sim_backend.hpp
 * @brief 시뮬레이터 백엔드 포트 (seam c, sim-abstraction). 계약 v2.0.0.
 *
 * v1.0.0 → v2.0.0 (**major**, 사용자 결정 R-14 (a)): `RobotObservation` 의 노드 id 2건이
 * `NodeId`(맨 uint32) → **`UniformNodeId`** 강타입으로 바뀌고, 인스턴스 스코프(`ViewScope`)를
 * 동반한다. 기존 호출부는 컴파일되지 않는다 — 그것이 이 변경의 목적이다.
 *
 * 목적(사용자 확정 제약): 최종 목표는 Isaac Sim 이지만 지금은 pysim 으로 간다. **아이작이
 * 나중에 붙어도 코어 로직·시나리오·지표 파서가 재작성되지 않도록** 실행·센싱·구동을 전부
 * 이 포트 뒤에 둔다. 이 인터페이스가 흔들리면 진화 비용이 폭증한다.
 *
 * 구현체: FakeSimBackend(C++ 인프로세스, unit 티어 결정론) / pysim(별도 프로세스) /
 *         isaac(스텁 — NotImplemented, 계약 주석은 지금 확정).
 *
 * 누수 금지 규칙(architecture §5.2 — 구조적 강제):
 *  1. sense() 반환형은 pysim 이 완전관측이어도 **관측 불확실성 필드를 갖는다**. 지금 넣지
 *     않으면 isaac 부착 시 계약이 깨진다.
 *  2. step() 은 **명시적 시간 전진**이다. 코어가 "지금 곧바로 반영됨"을 전제하지 않도록
 *     actuate → 상태 반영 지연을 capabilities().actuate_to_state_latency_s 로 노출한다.
 *  3. 결정론은 **백엔드 속성으로 질의 가능**해야 한다(Q-8) — sim-runner 가
 *     OPEN(pending-isaac) 태깅을 자동화하는 근거다.
 */

#include <cstdint>
#include <string>
#include <vector>

#include "mrs/contract_types.hpp"

namespace mrs
{

/**
 * @brief 물리 충실도 등급 (SimCapabilities.msg 와 값 일치).
 */
enum class PhysicsFidelity : std::uint8_t
{
  NONE = 0,        ///< 기하만 (FakeSimBackend)
  KINEMATIC = 1,   ///< 이산 운동학 (pysim)
  FULL_PHYSICS = 2 ///< 접촉·동역학 (isaac)
};

/**
 * @brief 백엔드 자기신고 능력 (Q-8).
 */
struct SimCapabilities
{
  std::string backend_name{"fake"};                     ///< "fake" | "pysim" | "isaac"
  bool deterministic_with_seed{false};                  ///< 시드 고정 시 완전 재현 가능 여부
  std::uint64_t seed{0};                                ///< 현재 시드
  PhysicsFidelity physics_fidelity{PhysicsFidelity::NONE}; ///< 물리 충실도
  bool supports_partial_observation{false};             ///< 부분관측/노이즈를 실제로 채우는가
  bool supports_contact{false};                         ///< 접촉 충돌을 물리적으로 판정하는가
  bool supports_fault_injection{false};                 ///< inject() 지원 여부
  double nominal_realtime_factor{1.0};                  ///< 공칭 배속 (1.0 = 실시간)
  double actuate_to_state_latency_s{0.0};               ///< actuate → 상태 반영 지연 [s]
};

/**
 * @brief 관측 불확실성 (sim 누수 방지 — architecture §5.2).
 */
struct ObservationUncertainty
{
  double pos_std_m{0.0};          ///< 위치 추정 표준편차 [m]. pysim = 0
  double theta_std_rad{0.0};      ///< 자세 추정 표준편차 [rad]. pysim = 0
  bool partial_observation{false};///< true = 일부 상태 미관측(마지막 추정치 반환)
  double observation_time_s{0.0}; ///< 이 관측이 유효한 시뮬 시각 [s]
};

/**
 * @brief 로봇 관측 상태.
 * @note 노드 부착은 **균일 뷰** 기준이다(v2.0.0). 이 값들은 L3 진행 추적과 L4 창 소비의
 *       입력이고 두 경로의 좌표계가 `ExecutionWindow.segments`(균일 뷰)이기 때문이다.
 *       백엔드는 연속 좌표만 갖고 있으므로, MapRegistry 균일 뷰로 부착하고 그때 사용한
 *       스코프를 `view_scope` 에 싣는다. 물리 노드를 실으면 진행 판정이 세그먼트와 어긋난다.
 */
struct RobotObservation
{
  RobotId robot_id{ROBOT_ID_NONE};    ///< 로봇 id
  Pose2D body_pose;                   ///< 몸체 자세 q [m, rad], map (오프셋점 z 아님)
  double v_mps{0.0};                  ///< 몸체 선속도 [m/s]
  double omega_rps{0.0};              ///< 몸체 각속도 [rad/s]
  ViewScope view_scope;               ///< 아래 노드 id 2건의 뷰. view_kind = UNIFORM 고정
  UniformNodeId occupied_node{NODE_ID_NONE}; ///< 점유 중 노드(**균일 뷰**). 엣지 위면 NODE_ID_NONE
  UniformNodeId next_node{NODE_ID_NONE};     ///< 진행 방향 다음 노드(**균일 뷰**)
  double edge_progress{0.0};          ///< 엣지 진행률 [0,1]
  ObservationUncertainty uncertainty; ///< 관측 불확실성 (isaac 에서 실채워짐)
};

/**
 * @brief 구동 지령.
 */
struct ActuationCommand
{
  RobotId robot_id{ROBOT_ID_NONE}; ///< 로봇 id
  double v_mps{0.0};               ///< 몸체 선속도 지령 [m/s]
  double omega_rps{0.0};           ///< 몸체 각속도 지령 [rad/s]
};

/**
 * @brief 결함 종류 (SimInject.srv 와 값 일치).
 */
enum class FaultKind : std::uint8_t
{
  COMM_DELAY = 0,     ///< 통신 지연 [magnitude = s]
  COMM_DROP = 1,      ///< 패킷 손실 [magnitude = 손실률 0..1]
  CORRIDOR_CLOSE = 2, ///< 통로 폐쇄
  CORRIDOR_OPEN = 3,  ///< 통로 재개방
  ROBOT_STALL = 4,    ///< 일시 정지
  ROBOT_FAULT = 5     ///< 영구 고장
};

/**
 * @brief 결함 주입 요청.
 */
struct FaultInjection
{
  FaultKind kind{FaultKind::COMM_DELAY};   ///< 결함 종류
  RobotId target_robot_id{ROBOT_ID_NONE};  ///< 대상 로봇
  EdgeId target_edge_id{NODE_ID_NONE};     ///< 대상 **물리 뷰** 엣지 (통로 개폐). 통로 개폐는
                                           ///< 물리 지도의 사건이므로 균일 뷰 엣지가 아니다.
                                           ///< (센티넬은 EDGE_ID_NONE 로 정정 예정 — 값 동일)
  double magnitude{0.0};                   ///< 강도 (단위는 kind 별)
  double duration_s{0.0};                  ///< 지속 시간 [s]. 0 = 해제 전까지
};

/**
 * @brief metrics tap 표본 — RobotObservation 으로부터 유도 불가한 백엔드 고유 진실값.
 */
struct SimMetricSample
{
  std::string key;                 ///< 지표 키 ("collision_count", "min_separation_m" …)
  double value{0.0};               ///< 값 (단위는 key 접미사)
  RobotId robot_id{ROBOT_ID_NONE}; ///< 로봇별 지표면 로봇 id, 전역이면 ROBOT_ID_NONE
  bool pending_isaac{false};       ///< true = 충실도 부족, 연구 검증 승격 금지 (§5.3)
};

/**
 * @brief 시뮬레이터 백엔드 포트.
 */
class ISimBackend
{
public:
  virtual ~ISimBackend() = default;

  /**
   * @brief 백엔드 능력을 신고한다 (Q-8 — sim-runner 의 OPEN(pending-isaac) 자동 태깅 근거).
   * @return SimCapabilities 결정론 여부·물리 충실도·지원 기능·지연.
   */
  virtual SimCapabilities capabilities() const noexcept = 0;

  /**
   * @brief 시뮬을 초기 상태로 되돌리고 시드를 고정한다.
   * @param[in] seed 난수 시드. 재현성 실험의 키.
   * @return bool 성공 여부. 실패 시 호출자는 시나리오를 시작하지 않는다.
   */
  virtual bool reset(std::uint64_t seed) noexcept = 0;

  /**
   * @brief 시뮬 시간을 명시적으로 전진시킨다.
   * @param[in] steps 전진할 스텝 수 (0 은 1 로 취급).
   * @param[out] sim_time_s 전진 후 시뮬 시각 [s].
   * @return bool 성공 여부.
   */
  virtual bool step(std::uint32_t steps, double & sim_time_s) noexcept = 0;

  /**
   * @brief 현재 로봇 상태를 관측한다.
   * @param[out] observations 로봇별 관측 결과 (관측 불확실성 포함).
   * @return bool 성공 여부. false 면 호출자는 마지막 관측을 유지하고 안전 폴백한다.
   */
  virtual bool sense(std::vector<RobotObservation> & observations) noexcept = 0;

  /**
   * @brief 구동 지령을 적용한다. 반영은 즉시가 아닐 수 있다(actuate_to_state_latency_s).
   * @param[in] commands 로봇별 지령.
   * @return bool 성공 여부.
   */
  virtual bool actuate(const std::vector<ActuationCommand> & commands) noexcept = 0;

  /**
   * @brief 결함을 주입한다.
   * @param[in] fault 결함 요청.
   * @param[out] injection_id 해제·감사용 식별자.
   * @return bool 지원·접수 여부. 미지원 결함이면 false (capabilities 로 사전 확인할 것).
   */
  virtual bool inject(const FaultInjection & fault, std::uint64_t & injection_id) noexcept = 0;

  /**
   * @brief 마지막 step 이후 누적된 metrics tap 표본을 회수한다(회수 후 내부 버퍼는 비워진다).
   * @param[out] samples 표본 목록. 충실도 부족 지표는 pending_isaac = true 로 표시된다.
   * @return bool 성공 여부.
   */
  virtual bool poll_metrics(std::vector<SimMetricSample> & samples) noexcept = 0;
};

} // namespace mrs
