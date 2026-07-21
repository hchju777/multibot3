#pragma once

/**
 * @file fake_sim_backend.hpp
 * @brief 결정론적 인프로세스 시뮬 백엔드 — unit 티어의 기반.
 *
 * ## 무엇을 위한 것인가
 * `unit` 티어는 **ROS 도, 별도 시뮬 프로세스도 없이** 순수 로직을 결정론적으로 검증한다.
 * 그러려면 `ISimBackend` 구현체 하나가 프로세스 안에 있어야 하며, 이것이 그 구현체다.
 * pysim·isaac 과 **같은 인터페이스 뒤에** 서므로 코어·시나리오·지표가 백엔드 교체로 바뀌지 않는다.
 *
 * ## 무엇이 아닌가 — 충실도 정직성 (architecture §5.3)
 * 물리 충실도는 @ref PhysicsFidelity::NONE (기하만) 이다. 접촉·동역학·물리 비결정성이 없다.
 * 따라서 **QP feasibility·livelock·실제 diff-drive 물리는 이 백엔드로 판정할 수 없다.**
 * 그 사실을 숨기지 않기 위해 이 백엔드가 내는 **모든 metrics 표본은 `pending_isaac = true`** 다 —
 * sim-runner 가 `OPEN(pending-isaac)` 태깅을 자동화할 수 있게 하는 근거이며,
 * 여기서 나온 수치를 연구 검증으로 승격하는 것을 구조적으로 막는다.
 *
 * ## 운동학
 * 유니사이클 전방 오일러 적분: `x += v·cos(θ)·dt`, `y += v·sin(θ)·dt`, `θ += ω·dt`.
 * 몸체점 q 기준이며 NID 오프셋점 z 는 L4 내부 표현이라 여기 나타나지 않는다.
 * 충돌은 기하 중첩(반지름 합)으로만 판정하고 접촉력은 모형화하지 않는다.
 *
 * ## 노드 부착
 * 포트는 `occupied_node`/`next_node` 를 **균일 뷰** 노드로 요구하지만, 이 패키지는
 * `mrs_core_types` 에만 의존하므로 MapRegistry 를 호출할 수 없다(아키텍처 §2.2).
 * 그래서 노드 위치 테이블을 @ref FakeSimBackend::configure 로 **주입**받아 스스로 부착한다.
 * 테이블이 주어지지 않으면 계약대로 `NODE_ID_NONE` 을 보고한다 — 추측해서 붙이지 않는다.
 *
 * ## 결정론
 * 같은 시드·같은 지령열이면 부동소수 수준까지 같은 궤적을 낸다. 난수는 시드된 엔진 하나만
 * 쓰고 전역 상태를 읽지 않는다. `actuate_to_state_latency_s > 0` 인 구성에서도 결정론은 유지된다.
 */

#include <cstdint>
#include <deque>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

#include "mrs/i_sim_backend.hpp"

namespace mrs
{

/**
 * @brief 노드 부착용 위치 표의 원소 — 균일 뷰 노드 하나의 map 프레임 좌표.
 */
struct FakeNodeAnchor
{
  UniformNodeId node_id{}; ///< 균일 뷰 노드 id
  double x_m{0.0};         ///< 노드 위치 x [m], map 프레임
  double y_m{0.0};         ///< 노드 위치 y [m], map 프레임
};

/**
 * @brief 로봇 1대의 초기 배치.
 */
struct FakeRobotSpawn
{
  RobotId robot_id{ROBOT_ID_NONE}; ///< 로봇 id
  Pose2D initial_pose;             ///< 초기 몸체 자세 q [m, rad], map
};

/**
 * @brief `FakeSimBackend` 구성 — `reset()` 이전에 한 번 주입한다.
 */
struct FakeSimConfig
{
  /** @brief 시뮬 스텝 [s]. 계약 R-A2 상 Δt_h 는 이 값의 정수배여야 한다. */
  double step_dt_s{0.05};
  /** @brief 로봇 반지름 [m] — 기하 충돌 판정에만 쓴다. */
  double robot_radius_m{0.3};
  /** @brief 노드 부착 반경 [m]. 이보다 가까우면 그 노드를 점유 중으로 본다. */
  double node_attach_radius_m{0.25};
  /** @brief `actuate` → 상태 반영 지연 [s]. 0 이면 즉시 반영. */
  double actuate_to_state_latency_s{0.0};
  /** @brief 로봇 초기 배치. */
  std::vector<FakeRobotSpawn> robots;
  /** @brief 노드 부착 표. 비어 있으면 노드를 부착하지 않고 `NODE_ID_NONE` 을 보고한다. */
  std::vector<FakeNodeAnchor> node_anchors;
  /** @brief 관측이 실어 보낼 뷰 스코프. `view_kind` 는 UNIFORM 이어야 한다. */
  ViewScope view_scope;
};

/**
 * @brief 기하 전용 결정론 시뮬 백엔드.
 *
 * @note 이 클래스는 `noexcept` 포트를 구현하므로 **어떤 메서드도 예외를 밖으로 내보내지 않는다.**
 *       내부 실패는 `false` 반환으로 보고한다(CLAUDE.md 규율 2).
 */
class FakeSimBackend final : public ISimBackend
{
public:
  /**
   * @brief 구성 없이 만든다. @ref configure 와 @ref reset 을 호출하기 전에는 동작하지 않는다.
   */
  FakeSimBackend() = default;

  /**
   * @brief 구성을 주입한다. `reset()` 이전에 호출해야 한다.
   *
   * 유효성 검사에 실패하면 구성을 받아들이지 않는다 — 잘못된 구성으로 조용히 도는 것보다
   * 기동을 막는 편이 낫다(기동 거부 검사와 같은 취지).
   *
   * @param[in] config 시뮬 구성. 자료형 `FakeSimConfig`.
   * @return `bool` — 구성이 유효하면 true. 다음이면 false: `step_dt_s <= 0`,
   *         `robot_radius_m < 0`, `node_attach_radius_m < 0`, `actuate_to_state_latency_s < 0`,
   *         로봇 id 중복, `view_scope.view_kind != UNIFORM`, 노드 표에 중복 id.
   */
  [[nodiscard]] bool configure(const FakeSimConfig & config) noexcept;

  /**
   * @brief 백엔드 능력을 신고한다.
   * @return `SimCapabilities` — `backend_name = "fake"`, `deterministic_with_seed = true`,
   *         `physics_fidelity = NONE`, `supports_contact = false`,
   *         `supports_partial_observation = false`, `supports_fault_injection = true`
   *         (단 지원 결함은 `ROBOT_STALL`·`ROBOT_FAULT` 뿐이며 나머지는 `inject()` 가 거부한다).
   */
  [[nodiscard]] SimCapabilities capabilities() const noexcept override;

  /**
   * @brief 시뮬을 초기 상태로 되돌리고 시드를 고정한다.
   * @param[in] seed 난수 시드. 자료형 `std::uint64_t`. 같은 시드면 같은 궤적이 재현된다.
   * @return `bool` — 성공하면 true. @ref configure 가 성공한 적이 없으면 false.
   */
  [[nodiscard]] bool reset(std::uint64_t seed) noexcept override;

  /**
   * @brief 시뮬 시간을 명시적으로 전진시킨다.
   * @param[in] steps 전진할 스텝 수. 자료형 `std::uint32_t`. 0 은 1 로 취급한다.
   * @param[out] sim_time_s 전진 후 시뮬 시각 [s]. 자료형 `double`.
   * @return `bool` — 성공하면 true. `reset()` 전이면 false.
   */
  [[nodiscard]] bool step(std::uint32_t steps, double & sim_time_s) noexcept override;

  /**
   * @brief 현재 로봇 상태를 관측한다.
   * @param[out] observations 로봇별 관측 결과. 자료형 `std::vector<RobotObservation>`.
   *             로봇 id 오름차순으로 채운다(결정론적 순서).
   * @return `bool` — 성공하면 true. `reset()` 전이면 false이며 out 은 건드리지 않는다.
   */
  [[nodiscard]] bool sense(std::vector<RobotObservation> & observations) noexcept override;

  /**
   * @brief 구동 지령을 적용한다.
   *
   * `actuate_to_state_latency_s > 0` 이면 지령은 즉시 반영되지 않고 그 지연만큼 큐에 머문다 —
   * 코어가 "지금 곧바로 반영됨"을 전제하지 않게 하는 누수 방지 장치다(architecture §5.2).
   *
   * @param[in] commands 로봇별 지령. 자료형 `std::vector<ActuationCommand>`.
   *            구성에 없는 로봇 id 가 섞이면 **전체를 거부**한다(부분 적용 금지).
   * @return `bool` — 성공하면 true. `reset()` 전이거나 미지 로봇 id 가 있으면 false.
   */
  [[nodiscard]] bool actuate(const std::vector<ActuationCommand> & commands) noexcept override;

  /**
   * @brief 결함을 주입한다.
   * @param[in] fault 결함 요청. 자료형 `FaultInjection`.
   * @param[out] injection_id 해제·감사용 식별자. 자료형 `std::uint64_t`. 1 부터 증가한다.
   * @return `bool` — 접수하면 true. **미지원 결함이면 false** — 이 백엔드는 기하 전용이라
   *         `ROBOT_STALL`·`ROBOT_FAULT` 만 지원한다. 통신 결함은 `comms_emul` 소관이고,
   *         통로 개폐는 지도를 아는 계층의 일이라 여기서 흉내내지 않는다.
   */
  [[nodiscard]] bool inject(
    const FaultInjection & fault, std::uint64_t & injection_id) noexcept override;

  /**
   * @brief 마지막 회수 이후 누적된 metrics 표본을 가져간다(회수 후 내부 버퍼는 비워진다).
   *
   * 내는 지표: `collision_count`(누적, 전역) · `min_separation_m`(이번 구간 최소, 전역).
   * **모든 표본은 `pending_isaac = true`** 다 — 물리 충실도가 NONE 이므로 이 값들을
   * 연구 검증으로 승격해서는 안 된다(§5.3).
   *
   * @param[out] samples 표본 목록. 자료형 `std::vector<SimMetricSample>`.
   * @return `bool` — 성공하면 true. `reset()` 전이면 false이며 out 은 건드리지 않는다.
   */
  [[nodiscard]] bool poll_metrics(std::vector<SimMetricSample> & samples) noexcept override;

private:
  /** @brief 로봇 1대의 내부 상태. */
  struct RobotState
  {
    Pose2D pose;               ///< 몸체 자세 q
    double v_mps{0.0};         ///< 현재 적용 중인 선속도 [m/s]
    double omega_rps{0.0};     ///< 현재 적용 중인 각속도 [rad/s]
    bool stalled{false};       ///< ROBOT_STALL 로 일시 정지 중
    double stall_until_s{0.0}; ///< 정지 해제 시각 [s]. 0 이면 해제 전까지
    bool faulted{false};       ///< ROBOT_FAULT 로 영구 고장
  };

  /** @brief 지연 큐의 원소 — 반영 예정 시각과 지령. */
  struct PendingCommand
  {
    double apply_at_s{0.0}; ///< 반영 예정 시뮬 시각 [s]
    std::uint64_t sequence{0}; ///< 접수 순번 — 같은 시각에 대한 결정론적 순서 보장
    ActuationCommand command; ///< 지령
  };

  /**
   * @brief 스텝 1회를 적분한다 (지연 큐 반영 → 자세 적분 → 충돌·최소거리 갱신).
   */
  void advance_one_step() noexcept;

  /**
   * @brief 자세에 가장 가까운 부착 노드를 찾는다.
   * @param[in] pose 몸체 자세. 자료형 `Pose2D`.
   * @param[out] out 부착된 노드 id. 자료형 `UniformNodeId`.
   * @return `bool` — 부착 반경 안에 노드가 있으면 true. 없거나 표가 비었으면 false.
   */
  [[nodiscard]] bool attach_node(const Pose2D & pose, UniformNodeId & out) const noexcept;

  FakeSimConfig config_{};
  bool configured_{false};
  bool ready_{false};

  /**
   * @brief 시뮬 시각의 **정본은 정수 나노초**다. `double` 로 누산하지 않는다.
   *
   * `sim_time_s_ += dt` 로 누산하면 0.05 처럼 이진수로 정확히 표현되지 않는 스텝에서 오차가
   * 쌓여 시각이 격자에서 미끄러진다. 그러면 `/clock` 을 소비하는
   * `tick_seq = floor((t - t0)/Δt_h)` 가 격자 경계에서 **직전 틱 번호를 재계산**하고,
   * 중복이 억제되면서 그 번호가 **영구 결번**이 된다.
   * [0a] 실측에서 결번 20.8% 가 결정론적으로 재현됐고 근인이 정확히 이것이었다.
   *
   * ⚠ R-A2(Δt_h 가 스텝 dt 의 정수배)는 이 상황에서도 통과한다 — **필요조건일 뿐
   * 충분조건이 아니다.** 격자 정합은 누산 방식으로 별도 보장해야 한다.
   */
  std::uint64_t sim_time_ns_{0};
  /** @brief 스텝 길이의 정수 나노초 표현. `configure()` 에서 한 번 계산한다. */
  std::uint64_t step_dt_ns_{0};
  /** @brief `sim_time_ns_` 에서 유도한 초 단위 시각 — 외부 보고용 파생값이다. */
  double sim_time_s_{0.0};
  std::uint64_t seed_{0};
  std::mt19937_64 rng_{0};
  std::uint64_t next_injection_id_{1};
  std::uint64_t next_command_sequence_{0};
  std::vector<RobotId> robot_order_;
  std::unordered_map<RobotId, RobotState> robots_;
  std::deque<PendingCommand> pending_commands_;
  std::uint64_t collision_count_{0};
  double min_separation_m_{0.0};
  bool separation_seen_{false};
};

} // namespace mrs
