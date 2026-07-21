#pragma once

/**
 * @file sim_bridge_node.hpp
 * @brief sim_bridge — ISimBackend(seam c) 뒤에 실제 백엔드(pysim|isaac|fake)를 숨기는 유일 통로.
 *
 * ## 이 노드가 소유하는 것 두 가지
 * 1. **시간** (architecture §1.7-B, 계약 §3 정본표) — 시뮬 티어의 `/clock` **유일 발행자**다.
 *    ⚠ `/clock` 발행 타이머와 자동 스텝 타이머는 **반드시 벽시계**(`create_wall_timer`)로 돈다
 *    (nav2-reference §2 C2 채택). `use_sim_time` 로 돌리면 자기가 발행하지 않은 시계를 기다리며
 *    **부팅 데드락**에 빠진다 — 시계의 소유자가 자기 시계를 소비할 수는 없다.
 *    시뮬 시각의 정본은 `ISimBackend::step()` 의 out 파라미터이지 `Node::now()` 가 아니다.
 * 2. **시뮬 경계** (architecture §5.1) — 코어(도메인·노드)는 시뮬레이터를 모른다. 알고 있는 것은
 *    이 노드가 발행하는 `RobotState`/`SimMetricSample` 과 자신이 발행하는 `cmd_vel` 뿐이다.
 *    actuate/sense/inject/metrics-tap 전량이 이 노드를 거친다.
 *
 * ## [0a] tracer bullet 범위
 * 백엔드는 **`FakeSimBackend`(C++ 인프로세스, 결정론)** 로 시작한다 — 이미 구현·검증됐다.
 * pysim 프로세스 백엔드(`mrs_sim_pysim`)는 프로세스 **형태**만 고정돼 있고 C++ 어댑터가 아직
 * 없으므로, `sim_backend:=pysim` 으로 기동하면 이 노드는 **조용히 fake 로 대체하지 않고 기동을
 * 거부**한다(@ref SimBridgeNode::backend_ready). 조용한 대체는 kill-gate 정직성을 깬다 —
 * `SimCapabilities.backend_name` 이 실제와 달라지면 sim-runner 의 `OPEN(pending-isaac)` 자동
 * 태깅이 근거를 잃는다.
 *
 * ## pending_isaac 전파 (architecture §5.3)
 * 백엔드가 신고한 `SimMetricSample.pending_isaac` 을 **그대로** 실어 보낸다. 여기서 떨어뜨리면
 * 충실도 부족 지표가 연구 검증으로 승격된다. 이 노드가 스스로 만드는 실측 지표
 * (`realtime_factor`·`step_wall_duration_s`)에도 `physics_fidelity < FULL_PHYSICS` 이면
 * 같은 플래그를 세운다.
 *
 * ## 규율
 * - **콜백 안에 알고리즘 없음.** 운동학·충돌·지표 산출은 전부 백엔드(도메인) 몫이다.
 * - 모든 콜백은 try/catch 로 감싼다 — 노드가 죽지 않는다(CLAUDE.md 규율 2).
 * - 메시지↔도메인 변환은 **전부 `mrs_msg_convert`** 를 거친다. 변환 실패의 **판정은 convert,
 *   보고(로그 THROTTLE · 사유별 폐기 카운터 · 안전 폴백)는 이 노드**다(R-15 (b)).
 * - 시각 변환은 **반드시** `mrs::convert::seconds_to_time` 경유(R-18 구속). `1e9` 직접 나눗셈 금지.
 */

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <geometry_msgs/msg/twist.hpp>
#include <rclcpp/rclcpp.hpp>
#include <rosgraph_msgs/msg/clock.hpp>

#include "mrs/i_sim_backend.hpp"
#include "mrs/view_ids.hpp"
#include "mrs_interfaces/msg/robot_state.hpp"
#include "mrs_interfaces/msg/sim_metric_sample.hpp"
#include "mrs_interfaces/srv/sim_inject.hpp"
#include "mrs_interfaces/srv/sim_query_capabilities.hpp"
#include "mrs_interfaces/srv/sim_step.hpp"
#include "mrs_msg_convert/convert_result.hpp"
#include "mrs_sim_abstraction/fake_sim_backend.hpp"

namespace mrs
{

/**
 * @brief 시뮬 백엔드 경계를 감싸는 sim_bridge 노드 (`/clock` 유일 발행자 겸 sim 추상화 통로).
 */
class SimBridgeNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 파라미터 조회 → 백엔드 구성 → 토픽·서비스·타이머 배선을 수행한다.
   *
   * 백엔드 구성에 실패하면 **타이머를 만들지 않는다** — 구성되지 않은 백엔드를 두드리며 도는
   * 것보다 아무 일도 하지 않는 편이 안전하다. 실패 사실은 @ref backend_ready 로 노출되며
   * `main()` 이 그 값으로 기동 거부를 결정한다.
   */
  SimBridgeNode();

  /**
   * @brief 소멸자 — 사유별 폐기 카운터 요약을 남긴다(사후 감사용).
   */
  ~SimBridgeNode() override;

  /**
   * @brief 백엔드가 실제로 구성·초기화되어 시뮬을 전진시킬 수 있는 상태인지 알려준다.
   * @return `bool` — true 면 정상 기동. false 면 **기동 거부 대상**이며 `main()` 이 0 이 아닌
   *         종료코드로 빠져나가야 한다(미지원 백엔드·구성 오류·`reset()` 실패).
   */
  [[nodiscard]] bool backend_ready() const noexcept
  {
    return backend_ready_;
  }

private:
  /** @brief @ref mrs::convert::ConvertStatus 값의 개수(0..6). 사유별 카운터 배열 길이다. */
  static constexpr std::size_t CONVERT_STATUS_COUNT = 7U;

  /** @brief 사유별 폐기 카운터 — 인덱스는 `ConvertStatus` 의 정수값이다. */
  using DiscardCounters = std::array<std::uint64_t, CONVERT_STATUS_COUNT>;

  /**
   * @brief 로봇 1대의 최신 구동 지령과 그 도달 시각(워치독 입력).
   */
  struct PendingActuation
  {
    double v_mps{0.0};               ///< 최신 선속도 지령 [m/s]
    double omega_rps{0.0};           ///< 최신 각속도 지령 [rad/s]
    double received_sim_time_s{0.0}; ///< 이 지령이 도달한 시뮬 시각 [s]
    bool ever_received{false};       ///< 한 번이라도 유효한 지령을 받았는가
  };

  // ── 기동 ───────────────────────────────────────────────────────────────────
  /**
   * @brief ROS2 파라미터를 전부 선언·조회해 멤버에 채운다(아래 4개 헬퍼의 호출 순서 고정).
   * @return `void`
   */
  void declare_and_read_parameters();

  /**
   * @brief 백엔드 선택·규모·결정론 관련 파라미터를 선언·조회한다.
   * @return `void`
   */
  void declare_backend_parameters();

  /**
   * @brief 시간 관련 파라미터(스텝 dt·벽시계 주기·워치독·보고 주기)를 선언·조회한다.
   * @return `void`
   */
  void declare_timing_parameters();

  /**
   * @brief 시나리오 구성 파라미터(반지름·초기 배치·뷰 스코프 원천)를 선언·조회한다.
   * @return `void`
   */
  void declare_scenario_parameters();

  /**
   * @brief 조회된 지도 버전·뷰 id 로 관측용(UNIFORM)·주입용(PHYSICAL) 스코프를 조립한다.
   * @return `void`
   */
  void build_view_scopes();

  /**
   * @brief `sim_backend` 파라미터가 이 빌드에서 실제로 만들 수 있는 백엔드인지 판정한다.
   *
   * `"pysim"`·`"isaac"` 은 C++ 프로세스 어댑터가 없어 **거부**한다. fake 로 조용히 대체하지
   * 않는 이유는 `SimCapabilities.backend_name` 이 실제와 달라지는 순간 sim-runner 의
   * `OPEN(pending-isaac)` 자동 태깅이 근거를 잃기 때문이다(architecture §5.3).
   *
   * @return `bool` — 만들 수 있으면 true. 미지원·미지의 값·`robot_count <= 0` 이면 false.
   */
  [[nodiscard]] bool is_backend_selection_supported() const;

  /**
   * @brief 초기 배치 배열 길이와 관측 뷰 스코프의 유효성을 검사한다.
   * @return `bool` — 통과하면 true. 배열 길이가 `robot_count` 와 다르거나 스코프가 사용
   *         불가(지도 버전 0 등)이면 false — 자세를 추측해서 채우지 않는다.
   */
  [[nodiscard]] bool validate_spawn_parameters() const;

  /**
   * @brief `sim_backend` 파라미터가 지시한 백엔드를 만들어 구성·리셋한다.
   *
   * [0a] 는 `"fake"` 만 실제로 만들 수 있다. `"pysim"`·`"isaac"` 은 프로세스 어댑터가 아직
   * 없으므로 **fake 로 대체하지 않고 실패를 반환**한다(조용한 대체 금지 — §5.3 정직성).
   *
   * @return `bool` — 백엔드가 구성·리셋까지 성공하면 true, 그 밖에는 false(기동 거부).
   */
  [[nodiscard]] bool create_and_configure_backend();

  /**
   * @brief 파라미터로 받은 초기 배치·뷰 스코프로 `FakeSimConfig` 를 조립한다.
   * @param[out] config 채울 구성. 자료형 `mrs::FakeSimConfig`. 실패 시 내용은 정의되지 않는다.
   * @return `bool` — 배열 길이 정합·뷰 스코프 유효성까지 통과하면 true.
   */
  [[nodiscard]] bool build_fake_config(FakeSimConfig & config) const;

  // ── 주기 구동 (전부 벽시계 타이머) ──────────────────────────────────────────
  /**
   * @brief `/clock` 발행 타이머 콜백 — 현재 시뮬 시각을 재발행한다(벽시계 구동).
   *
   * 시각을 **전진시키지 않는다**. 전진은 오직 @ref advance_sim 이 한다. 이 타이머의 목적은
   * 늦게 뜬 `use_sim_time` 구독자가 즉시 시각을 얻게 하는 것이다.
   *
   * @return `void`
   */
  void on_clock_timer();

  /**
   * @brief 자동 스텝 타이머 콜백 — 시뮬을 1 스텝 전진시킨다(벽시계 구동).
   *
   * `auto_step: false` 이면 이 타이머를 만들지 않으며, 세계는 `/sim/step` 서비스로만 전진한다
   * (bench_runner 의 lockstep 구동 경로).
   *
   * @return `void`
   */
  void on_step_timer();

  // ── 서비스·구독 콜백 ────────────────────────────────────────────────────────
  /**
   * @brief `/sim/step` 서비스 핸들러 — 시뮬 시간을 명시적으로 전진시킨다.
   * @param[in] request `steps` 필드를 담은 요청. 자료형 `SimStep::Request`. 0 은 1 로 취급된다.
   * @param[out] response 결과 코드·전진 후 시뮬 시각·누적 스텝 수·사유 문자열.
   * @return `void`
   */
  void on_step(
    const std::shared_ptr<mrs_interfaces::srv::SimStep::Request> request,
    std::shared_ptr<mrs_interfaces::srv::SimStep::Response> response);

  /**
   * @brief `/sim/step` 응답의 결과 코드·시각·누적 스텝 수를 채운다.
   *
   * 시각 변환은 **반드시** `mrs::convert::seconds_to_time` 을 거친다(R-18 구속) — 가드 위반이면
   * 전진에 성공했더라도 `RESULT_BACKEND_ERROR` 로 내린다. 틀린 시각을 성공으로 보고하면
   * 호출자가 그 값으로 배리어를 맞추게 된다.
   *
   * @param[in] advanced 백엔드 전진에 성공했는가. 자료형 `bool`.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::SimStep::Response`.
   * @return `void`
   */
  void fill_step_response(bool advanced, mrs_interfaces::srv::SimStep::Response & response);

  /**
   * @brief 변환 실패 사유를 `SimInject.srv` 응답 코드로 옮긴다.
   *
   * 계약 주석(`msg_convert`)이 요구한 구별이다 — `ENUM_OUT_OF_RANGE`(지원하지 않는 종류)는
   * `RESULT_UNSUPPORTED`, 그 밖(대상 부적합)은 `RESULT_BAD_TARGET`. 뭉개면 "이 백엔드가
   * 지원하지 않는 결함"과 "요청이 틀렸다"를 사후에 구별할 수 없다.
   *
   * @param[in] reason 변환 실패 사유. 자료형 `mrs::convert::ConvertStatus`.
   * @return `std::uint8_t` — `SimInject::Response::RESULT_UNSUPPORTED` 또는 `RESULT_BAD_TARGET`.
   */
  [[nodiscard]] std::uint8_t inject_result_for(convert::ConvertStatus reason) const;

  /**
   * @brief `/sim/inject` 서비스 핸들러 — 결함을 주입한다.
   * @param[in] request 결함 종류·대상·강도·지속시간. 자료형 `SimInject::Request`.
   * @param[out] response 접수 결과·주입 id·사유 문자열.
   * @return `void`
   */
  void on_inject(
    const std::shared_ptr<mrs_interfaces::srv::SimInject::Request> request,
    std::shared_ptr<mrs_interfaces::srv::SimInject::Response> response);

  /**
   * @brief 결함 주입 요청을 변환·전달하고 응답을 채운다 (@ref on_inject 의 본체).
   *
   * `on_inject` 는 try/catch 와 널 검사만 갖는 얇은 껍데기이고, 실제 처리는 여기 있다 —
   * 예외 경계와 처리 로직을 한 함수에 섞으면 어느 쪽도 읽기 어려워진다.
   *
   * @param[in] request 결함 요청. 자료형 `mrs_interfaces::srv::SimInject::Request`.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::SimInject::Response`.
   * @return `void`
   */
  void handle_inject(
    const mrs_interfaces::srv::SimInject::Request & request,
    mrs_interfaces::srv::SimInject::Response & response);

  /**
   * @brief `/sim/query_capabilities` 서비스 핸들러 — 백엔드 능력을 신고한다 (Q-8).
   * @param[in] request 빈 요청. 자료형 `SimQueryCapabilities::Request`.
   * @param[out] response 백엔드가 스스로 신고한 `SimCapabilities`.
   * @return `void`
   */
  void on_query_capabilities(
    const std::shared_ptr<mrs_interfaces::srv::SimQueryCapabilities::Request> request,
    std::shared_ptr<mrs_interfaces::srv::SimQueryCapabilities::Response> response);

  /**
   * @brief 로봇 i 의 `cmd_vel` 수신 콜백 — 최신 지령을 보관한다(적용은 스텝 시점).
   *
   * 여기서 곧바로 `actuate()` 하지 않는 이유: `step()` 이 **명시적 시간 전진**이므로 지령 적용
   * 시점을 스텝 경계에 맞춰야 백엔드의 결정론이 메시지 도착 순서에 흔들리지 않는다.
   *
   * @param[in] robot_index 0-base 로봇 인덱스. 자료형 `std::size_t`.
   * @param[in] msg 몸체 선속도(`linear.x`)·각속도(`angular.z`) 지령. 자료형 `Twist::SharedPtr`.
   * @return `void`
   */
  void on_cmd_vel(std::size_t robot_index, const geometry_msgs::msg::Twist::SharedPtr msg);

  // ── 시뮬 전진 파이프라인 ────────────────────────────────────────────────────
  /**
   * @brief 시뮬을 @p steps 만큼 전진시키고 그 결과를 전부 발행한다.
   *
   * 순서: 워치독·구동 지령 적용 → `step()` → `/clock` 발행 → `sense()` → `robot_state` 발행 →
   * `poll_metrics()` → `metrics_tap` 발행 → 자체 실측 지표 발행.
   * `/clock` 을 상태 발행보다 **먼저** 내보내는 이유는, 수신자가 새 상태를 옛 시각으로 해석하지
   * 않게 하기 위함이다.
   *
   * @param[in] steps 전진할 스텝 수. 자료형 `std::uint32_t`. 0 은 1 로 취급한다.
   * @return `bool` — 백엔드 전진에 성공하면 true. false 면 호출자는 현상유지로 폴백한다.
   */
  [[nodiscard]] bool advance_sim(std::uint32_t steps);

  /**
   * @brief 로봇별 최신 지령을 워치독과 함께 백엔드에 적용한다.
   *
   * `cmd_vel` 은 best_effort 스트림이라 유실될 수 있다. 마지막 지령이 `cmd_vel_timeout_s` 보다
   * 오래됐거나 한 번도 오지 않았으면 **정지 지령(0, 0)** 으로 대체한다 — 계약 §1.5 가 규정한
   * "cmd_vel 유실 시 안전 폴백은 sim_bridge 워치독 책임".
   *
   * @return `void`
   */
  void apply_actuation();

  /**
   * @brief 현재 시뮬 시각으로 `/clock` 을 1회 발행한다.
   * @return `void`
   */
  void publish_clock();

  /**
   * @brief `sense()` 결과를 로봇별 `/robot_{i}/robot_state` 로 발행한다.
   * @return `void`
   */
  void publish_robot_states();

  /**
   * @brief 백엔드 metrics tap 을 회수해 `/sim/metrics_tap` 으로 발행한다(`pending_isaac` 전파).
   * @return `void`
   */
  void publish_backend_metrics();

  /**
   * @brief 이 노드가 직접 잰 배속·스텝 벽시계 비용을 `/sim/metrics_tap` 으로 발행한다.
   *
   * [0a] 의 측정 항목("시뮬 배속")이 여기서 나온다. **측정값이며 창작값이 아니다.**
   *
   * @param[in] step_wall_duration_s 이번 전진에 걸린 벽시계 시간 [s]. 자료형 `double`.
   * @return `void`
   */
  void publish_bridge_metrics(double step_wall_duration_s);

  /**
   * @brief 지표 표본 하나를 변환·발행한다(실패 시 폐기 + 카운터).
   * @param[in] sample 도메인 표본. 자료형 `mrs::SimMetricSample`.
   * @param[in] site 폐기 카운터·로그에 남길 호출 지점 이름. 자료형 `const char *`.
   * @return `void`
   */
  void publish_metric_sample(const SimMetricSample & sample, const char * site);

  // ── 보고 (R-15 (b): 판정은 convert, 보고는 노드) ─────────────────────────────
  /**
   * @brief 변환 실패를 사유별로 적립하고 스로틀 로그를 남긴다.
   * @param[in] site 실패가 난 호출 지점 이름. 자료형 `const char *`. 카운터 키가 된다.
   * @param[in] reason 변환 실패 사유. 자료형 `mrs::convert::ConvertStatus`.
   * @return `void`
   */
  void record_convert_failure(const char * site, convert::ConvertStatus reason);

  /**
   * @brief 지금까지 적립된 사유별 폐기 카운터를 한 줄씩 로그로 남긴다.
   * @return `void`
   */
  void log_discard_summary() const;

  // ── 백엔드·상태 ─────────────────────────────────────────────────────────────
  std::shared_ptr<ISimBackend> backend_; ///< 활성 백엔드 (seam c). 미구성이면 nullptr
  bool backend_ready_{false}; ///< 구성·리셋까지 성공했는가 (기동 거부 판정)
  double sim_time_s_{0.0};    ///< 시뮬 시각 정본 [s]. `step()` out 파라미터만이 갱신
  std::uint64_t step_count_{0}; ///< `reset()` 이후 누적 스텝 수 (결정론 재현 키)
  double last_realtime_report_sim_s_{0.0};  ///< 직전 배속 보고 시점의 시뮬 시각 [s]
  double last_realtime_report_wall_s_{0.0}; ///< 직전 배속 보고 시점의 벽시계 시각 [s]

  std::vector<PendingActuation> pending_actuations_; ///< 로봇별 최신 지령 (인덱스 = 로봇 인덱스)
  std::unordered_map<std::string, DiscardCounters> discard_counts_; ///< 지점별·사유별 폐기 카운터
  std::uint64_t watchdog_stop_count_{0}; ///< 워치독이 정지 지령으로 대체한 횟수(누적)
  std::uint64_t rejected_cmd_vel_count_{0}; ///< 비유한 지령으로 폐기한 `cmd_vel` 수(누적)

  /** @brief 로그 스로틀·벽시계 계측 전용 시계. 시뮬 시계가 멈춰도 스로틀이 굳지 않게 한다. */
  rclcpp::Clock steady_clock_{RCL_STEADY_TIME};

  // ── 파라미터 ────────────────────────────────────────────────────────────────
  int robot_count_{2};              ///< 로봇 수 ([0a] = 2)
  std::string sim_backend_{"fake"}; ///< 백엔드 선택 문자열 ("fake" | "pysim" | "isaac")
  double sim_step_s_{0.05};         ///< 시뮬 스텝 dt [s] — **[0a] 실측 대상 placeholder**
  double step_wall_period_s_{0.05}; ///< 자동 스텝의 벽시계 주기 [s] — **[0a] 실측 대상**
  double clock_publish_wall_period_s_{0.01}; ///< `/clock` 재발행 벽시계 주기 [s]
  bool auto_step_{true};                     ///< true = 벽시계 타이머가 세계를 굴린다
  double cmd_vel_timeout_s_{0.5}; ///< 워치독 상한 [s] — **[0a] 실측 대상 placeholder**
  double realtime_report_period_s_{1.0}; ///< 배속 지표 보고 주기(시뮬 시각 기준) [s]
  std::int64_t sim_seed_{0};             ///< 백엔드 시드(결정론 재현 키)
  double robot_radius_m_{0.3};           ///< 로봇 반지름 [m] (기하 충돌 판정 전용)
  double node_attach_radius_m_{0.25};    ///< 노드 부착 반경 [m]
  double actuate_to_state_latency_s_{
    0.0}; ///< actuate → 상태 반영 지연 [s] (누수 조기 발견용 스윕축)
  std::vector<double> initial_x_m_;       ///< 로봇별 초기 x [m] (길이 = robot_count)
  std::vector<double> initial_y_m_;       ///< 로봇별 초기 y [m]
  std::vector<double> initial_theta_rad_; ///< 로봇별 초기 방위각 [rad], `[-pi, pi]`
  std::int64_t view_roadmap_version_{1}; ///< 관측이 실을 지도 버전 (0 = 런타임 금지값)
  std::int64_t view_uniform_view_id_{0}; ///< 관측이 실을 균일 뷰 id

  ViewScope observation_scope_; ///< 관측·주입에 쓰는 뷰 스코프(종류 UNIFORM)
  ViewScope physical_scope_;    ///< `SimInject.target_edge_id` 랩용 스코프(종류 PHYSICAL)

  // ── ROS 배선 ────────────────────────────────────────────────────────────────
  rclcpp::CallbackGroup::SharedPtr backend_group_; ///< 백엔드 접근을 직렬화하는 상호배타 그룹

  rclcpp::Publisher<rosgraph_msgs::msg::Clock>::SharedPtr clock_pub_;
  std::vector<rclcpp::Publisher<mrs_interfaces::msg::RobotState>::SharedPtr> robot_state_pubs_;
  rclcpp::Publisher<mrs_interfaces::msg::SimMetricSample>::SharedPtr metrics_pub_;
  std::vector<rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr> cmd_vel_subs_;

  rclcpp::Service<mrs_interfaces::srv::SimStep>::SharedPtr step_srv_;
  rclcpp::Service<mrs_interfaces::srv::SimInject>::SharedPtr inject_srv_;
  rclcpp::Service<mrs_interfaces::srv::SimQueryCapabilities>::SharedPtr capabilities_srv_;

  rclcpp::TimerBase::SharedPtr clock_timer_; ///< 벽시계 구동 (C2 — 부팅 데드락 회피)
  rclcpp::TimerBase::SharedPtr step_timer_;  ///< 벽시계 구동 (`auto_step` 일 때만 생성)
};

} // namespace mrs
