#pragma once

/**
 * @file pp_service_node.hpp
 * @brief pp_service — **[0a] tracer bullet 더미** L2 경로계획 노드 (D-03, architecture §7-2).
 *
 * ## 이 노드가 [0a] 에서 하는 일 / 하지 않는 일
 *  - 한다: MapRegistry 에서 균일 뷰를 받아 **고정 왕복 라우트**를 만들고(@ref
 * mrs::ros_pp::CannedSolver), 계약 표준형(`PlannedPaths` = 시각 부여 node-visit 시퀀스)으로
 * `/planned_paths` 에 발행한다.
 *  - **하지 않는다**: 탐색·충돌회피·수신지평 운용·부분 재계획 접합. 전부 로드맵 [1]~[2] 의 일이다.
 *    이 노드는 배관(메시지 왕복 지연·계약 정합)을 재기 위한 얇은 I/O 어댑터일 뿐이다.
 *
 * ## 콜백 안에 알고리즘이 없다 (ros2-conventions)
 * 경로 생성은 `IPathSolver` 구현체(seam a)가 하고, 도메인↔메시지 변환은 `mrs_msg_convert` 가
 * 단독으로 한다. 이 클래스에 남는 것은 구독·발행·서비스 응답·실패 보고뿐이다.
 *
 * ## 뷰 규약 (계약 §0.1)
 * 출력 `PlannedPaths` 의 노드 id 는 전부 **UNIFORM** 이고, 인스턴스 스코프는 `roadmap_version` +
 * `view_id` 평면 쌍이다. 두 값의 유일한 출처는 **MapRegistry 의 `GetUniformView` 응답**이며,
 * 값을 파라미터로 지어내지 않는다 — 지도 버전 0 은 계약이 런타임에 금지하고, 틀린 버전은
 * 수신자(L3/L4)에서 `VIEW_SCOPE_MISMATCH` 로 전량 폐기된다.
 * ⚠ [0a] 는 세분화가 없어 물리 roadmap 과 균일 뷰가 1:1 이지만, **그렇다고 물리 노드 id 를
 * 균일 뷰 id 로 재해석하지 않는다**(C4 — 뷰 변환은 MapRegistry 단독 소유).
 */

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <rclcpp/rclcpp.hpp>

#include "mrs/view_ids.hpp"
#include "mrs_msg_convert/convert_result.hpp"
#include "mrs_planning/windowed_path_planner.hpp"
#include "mrs_ros_pp/canned_solver.hpp"
#include "mrs_ros_pp/convert_diagnostics.hpp"

#include "mrs_interfaces/msg/planned_paths.hpp"
#include "mrs_interfaces/msg/roadmap_edge.hpp"
#include "mrs_interfaces/msg/task_assignment.hpp"
#include "mrs_interfaces/srv/get_uniform_view.hpp"
#include "mrs_interfaces/srv/partial_replan.hpp"
#include "mrs_interfaces/srv/plan_paths.hpp"

namespace mrs
{

/**
 * @brief L2 경로계획 서비스 노드의 [0a] 더미 구현.
 */
class PpServiceNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 파라미터·구독·발행·서비스 2종·MapRegistry 조회 배선을 구성한다.
   *
   * 생성자는 **블로킹하지 않는다** — MapRegistry 조회는 비동기 클라이언트 + 재시도 타이머로
   * 처리한다. 콜백 컨텍스트에서 `spin_until_future_complete` 를 부르면 자기 자신을
   * 데드락시키기 때문이다(nav2-reference §A2).
   */
  PpServiceNode();
  ~PpServiceNode() override = default;

private:
  /**
   * @brief `/task_assignment` 수신 콜백 — [0a] 는 **트리거로만** 쓴다.
   *
   * `TaskAssignment` → 도메인 변환 함수가 `mrs_msg_convert` 에 없으므로(발행 방향
   * `make_task_assignment` 만 존재) 필드를 도메인 값으로 해석하지 않는다. 노드가 메시지 필드를
   * 도메인에 직접 대입하는 것은 계약 §0.1 V3 위반이다. 따라서 수신 사실만 기록하고 고정 계획을
   * 재발행한다.
   *
   * @param[in] msg 수신한 작업 배정. 자료형 `mrs_interfaces::msg::TaskAssignment::SharedPtr`.
   * @return void
   */
  void on_task_assignment(const mrs_interfaces::msg::TaskAssignment::SharedPtr msg);

  /**
   * @brief `/pp_service/plan_paths` 핸들러 (계약 L-08).
   * @param[in] request 계획 요청. 자료형 `mrs_interfaces::srv::PlanPaths::Request` 공유 포인터.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::PlanPaths::Response` 공유 포인터.
   * @return void
   */
  void on_plan_paths(
    const std::shared_ptr<mrs_interfaces::srv::PlanPaths::Request> request,
    std::shared_ptr<mrs_interfaces::srv::PlanPaths::Response> response);

  /**
   * @brief `/pp_service/partial_replan` 핸들러 (계약 L-06, R3).
   *
   * [0a] 는 접합(splice)을 구현하지 않는다 — 응답은 "구계획 유지"로 수렴시킨다.
   *
   * @param[in] request 부분 재계획 요청. 자료형 `mrs_interfaces::srv::PartialReplan::Request` 공유
   * 포인터.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::PartialReplan::Response` 공유
   * 포인터.
   * @return void
   */
  void on_partial_replan(
    const std::shared_ptr<mrs_interfaces::srv::PartialReplan::Request> request,
    std::shared_ptr<mrs_interfaces::srv::PartialReplan::Response> response);

  /**
   * @brief MapRegistry 균일 뷰 조회 재시도 타이머 콜백.
   *
   * 스코프를 이미 확보했으면 타이머를 멈춘다. 서비스가 아직 안 떠 있으면 다음 주기로 미룬다.
   *
   * @return void
   */
  void on_map_query_timer();

  /**
   * @brief 응답 없는 조회 요청을 시한 초과 시 회수한다 (nav2-reference §A2 pending 누수 방지).
   * @return `bool` — 새 요청을 보내도 되면 true, 아직 응답을 기다리는 중이면 false.
   */
  bool reclaim_stale_map_query();

  /**
   * @brief `GetUniformView` 요청을 비동기로 보낸다 (블로킹하지 않는다).
   * @return void
   */
  void send_uniform_view_request();

  /**
   * @brief 균일 뷰 응답을 채택한다 — 스코프 유효성 검사 + 고정 라우트 생성 + 상태 확정.
   * @param[in] response 균일 뷰 응답. 자료형 `mrs_interfaces::srv::GetUniformView::Response`.
   * @return `bool` — 채택했으면 true. 스코프가 사용 불가이거나 라우트 생성에 실패하면 false
   *         (사유는 이 함수가 로그로 남긴다).
   */
  bool adopt_uniform_view(const mrs_interfaces::srv::GetUniformView::Response & response);

  /**
   * @brief 엣지의 양 끝 노드 id 를 강타입으로 랩해 라우트에 채운다 (규칙 V3 — convert 경유).
   * @param[in] edge 균일 뷰 엣지. 자료형 `mrs_interfaces::msg::RoadmapEdge`.
   * @param[out] route 채울 라우트. 자료형 `mrs::ros_pp::CannedRoute`.
   *             실패 시 내용은 정의되지 않는다.
   * @return `bool` — 두 끝점을 모두 랩했으면 true, 센티넬 등으로 실패하면 false.
   */
  bool wrap_edge_nodes(
    const mrs_interfaces::msg::RoadmapEdge & edge, mrs::ros_pp::CannedRoute & route);

  /**
   * @brief 고정 라우트로 솔버 입력을 채운다.
   * @param[out] input 채울 솔버 입력. 자료형 `mrs::PathSolverInput`.
   * @return void
   */
  void fill_plan_input(PathSolverInput & input) const;

  /**
   * @brief `GetUniformView` 비동기 응답 콜백 — 뷰 스코프 확정 + 고정 라우트 생성 + 초기 계획 발행.
   * @param[in] future 응답 future. 자료형
   *            `rclcpp::Client<mrs_interfaces::srv::GetUniformView>::SharedFuture`.
   * @return void
   */
  void on_uniform_view_response(
    rclcpp::Client<mrs_interfaces::srv::GetUniformView>::SharedFuture future);

  /**
   * @brief 균일 뷰 응답의 엣지로 로봇별 고정 왕복 라우트를 만든다.
   *
   * **엣지에서만** 만드는 이유: L3 가 이 경로를 세그먼트로 자르고, 계약 불변식 W2 가 세그먼트
   * 양 끝의 균일 뷰 인접을 요구한다. 임의의 노드 쌍을 쓰면 [0a] 배관이 계약 위반 창을 낸다.
   * 노드 id 랩(`uint32` → 강타입)은 `mrs_msg_convert` 헬퍼를 거친다(규칙 V3).
   *
   * @param[in] response 균일 뷰 응답. 자료형 `mrs_interfaces::srv::GetUniformView::Response`.
   * @param[in] scope 이 응답이 정의하는 뷰 스코프. 자료형 `mrs::ViewScope`.
   * @return `bool` — 라우트를 하나 이상 만들었으면 true, 엣지가 없거나 노드 id 랩에 실패하면 false.
   */
  bool build_canned_routes(
    const mrs_interfaces::srv::GetUniformView::Response & response, const ViewScope & scope);

  /**
   * @brief 고정 계획을 산출해 `/planned_paths` 에 1회 발행한다.
   *
   * 스코프 미확보·솔버 실패·변환 실패는 전부 **발행 없음 + 로그 + 카운터**로 폴백한다.
   * 절반만 채운 계획을 내보내면 L3 의 의존성 그래프가 조용히 틀린 상태가 된다.
   *
   * @param[in] trigger 발행을 유발한 사건 이름(로그용). 자료형 `const char *`. 널이 아니어야 한다.
   * @return `bool` — 실제로 발행했으면 true.
   */
  bool publish_canned_plan(const char * trigger);

  /**
   * @brief 도메인 경로를 `PlannedPaths.msg` 로 변환해 발행하고 계획 세대를 전진시킨다.
   * @param[in] paths 도메인 경로 목록. 자료형 `std::vector<mrs::RobotPath>`.
   * @param[in] now_s 발행 시각 [s], 시뮬 시계 절대시각. 자료형 `double`.
   * @param[in] trigger 발행을 유발한 사건 이름(로그용). 자료형 `const char *`.
   * @return `bool` — 발행했으면 true. 변환 실패면 false이며 계획 세대는 전진하지 않는다.
   */
  bool publish_plan_message(
    const std::vector<RobotPath> & paths, double now_s, const char * trigger);

  /**
   * @brief `PlanPaths.srv` 요청을 변환 실패 사유에 따라 거부 응답으로 채운다.
   *
   * ⚠ 이 srv 에는 **형식 위반 전용 응답 코드가 없다** — 계약 결손이며, 구현 선택의 근거는
   * 정의부 주석에 있다(요약: `RESULT_INFEASIBLE` 뭉갬은 계약이 금지하므로 회복이 무해한
   * `STALE_VERSION` 으로 응답한다).
   *
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::PlanPaths::Response`.
   * @param[in] result 변환 실패 결과. 자료형 `mrs::convert::ConvertResult`.
   * @return void
   */
  void reject_plan_paths(
    mrs_interfaces::srv::PlanPaths::Response & response,
    const mrs::convert::ConvertResult & result);

  /**
   * @brief `PartialReplan.srv` 요청을 변환 실패 사유에 따라 거부 응답으로 채운다.
   *
   * 이 srv 는 `STALE_VERSION` 과 `REJECTED_BAD_REQUEST` 를 모두 갖고 있어 계약 §0.2 매핑을
   * 그대로 따를 수 있다.
   *
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::PartialReplan::Response`.
   * @param[in] result 변환 실패 결과. 자료형 `mrs::convert::ConvertResult`.
   * @return void
   */
  void reject_partial_replan(
    mrs_interfaces::srv::PartialReplan::Response & response,
    const mrs::convert::ConvertResult & result);

  /**
   * @brief 변환 실패 1건을 사유별로 적립하고 스로틀 로그를 남긴다 (R-15 (b) = b3).
   * @param[in] context 실패 지점 이름(로그용). 자료형 `const char *`. 예: "to_msg(PlannedPaths)".
   * @param[in] result 변환 결과. 자료형 `mrs::convert::ConvertResult`. `ok == true` 이면 아무 일도
   * 하지 않는다.
   * @return void
   */
  void report_convert_failure(const char * context, const mrs::convert::ConvertResult & result);

  /**
   * @brief 현재 시각을 시뮬 시계 기준 절대 초로 얻는다.
   *
   * `use_sim_time: true` 전제(계약 §0 시각 규약)에서 노드 시계는 `/clock` 을 따른다. 벽시계
   * (`WallTimer`·`steady_clock`)를 쓰지 않는 이유가 그것이다(nav2-reference §C1).
   *
   * @return `double` — 시뮬 시계 기준 절대시각 [s].
   */
  [[nodiscard]] double now_seconds() const;

  // ── seam a (IPathSolver) ────────────────────────────────────────────────
  std::shared_ptr<mrs::ros_pp::CannedSolver> canned_solver_; ///< [0a] 더미 솔버 ([1] 에서 교체)
  mrs::WindowedPathPlanner planner_; ///< seam a 소비자 — 운용 로직은 Phase 5
  std::vector<mrs::ros_pp::CannedRoute> canned_routes_; ///< 솔버에 주입한 라우트의 노드 측 사본

  // ── 뷰 스코프 (MapRegistry 가 유일 출처) ────────────────────────────────
  ViewScope view_scope_{}; ///< 균일 뷰 스코프. `roadmap_version == 0` 이면 미확보 상태
  bool scope_ready_{false}; ///< 스코프·라우트 확보 여부. false 면 아무것도 발행하지 않는다

  // ── ROS 배선 ────────────────────────────────────────────────────────────
  rclcpp::Subscription<mrs_interfaces::msg::TaskAssignment>::SharedPtr task_assignment_sub_;
  rclcpp::Publisher<mrs_interfaces::msg::PlannedPaths>::SharedPtr planned_paths_pub_;
  rclcpp::Service<mrs_interfaces::srv::PlanPaths>::SharedPtr plan_paths_srv_;
  rclcpp::Service<mrs_interfaces::srv::PartialReplan>::SharedPtr partial_replan_srv_;
  rclcpp::Client<mrs_interfaces::srv::GetUniformView>::SharedPtr uniform_view_client_;
  rclcpp::TimerBase::SharedPtr map_query_timer_; ///< MapRegistry 조회 재시도 (스코프 확보 후 정지)
  rclcpp::TimerBase::SharedPtr auto_plan_timer_; ///< 주기 재발행 (기본 비활성 — 지연 표본 채집용)

  // ── 파라미터 (값의 근거는 생성자 주석 참조) ─────────────────────────────
  int robot_count_{2};             ///< 로봇 수. [0a] = 2대 (architecture §7)
  double segment_duration_s_{0.5}; ///< 방문 간 계획 시각 간격 [s] (문헌치 Q-11 Δt)
  int canned_lap_count_{4};   ///< 고정 경로 왕복 횟수 — 세그먼트 수 = 2 × 이 값
  double plan_budget_s_{1.0}; ///< 계획 예산 [s] (문헌치 — 더미는 소비하지 않는다)
  double map_query_retry_period_s_{1.0}; ///< MapRegistry 조회 재시도 주기 [s] (배관 상수)
  double map_query_timeout_s_{2.0}; ///< 조회 응답 대기 시한 [s]. 초과 시 pending 요청을 회수한다
  double auto_plan_period_s_{0.0}; ///< 계획 주기 재발행 [s]. 0 = 비활성(기동 시 1회만)

  // ── MapRegistry 조회 진행 상태 ──────────────────────────────────────────
  bool map_query_pending_{false};        ///< 응답 대기 중 여부 (중복 요청 방지)
  std::int64_t map_query_request_id_{0}; ///< 대기 중 요청 id (`remove_pending_request` 인자)
  double map_query_sent_at_s_{0.0};      ///< 요청 발신 시각 [s], 시뮬 시계 절대시각

  // ── 상태·계측 ───────────────────────────────────────────────────────────
  std::uint32_t plan_epoch_{0};           ///< 계획 세대 번호. **단조증가**(계약 L-09)
  std::uint64_t published_plan_count_{0}; ///< 발행 성공 누적 건수 (로그·[0a] 실측 보조)
  mrs::ros_pp::ConvertDiagnostics convert_diag_; ///< 변환 실패 사유별 폐기 카운터

  /**
   * @brief 로그 스로틀 전용 steady 시계. 노드 시계(시뮬 시계)를 쓰지 않는다.
   *
   * `RCLCPP_*_THROTTLE` 은 마지막 기록 시각을 0 으로 초기화하고 `now >= last + interval` 일 때만
   * 통과시킨다. 시뮬 시계도 0 에서 시작하므로 **기동 직후 첫 스로틀 창이 통째로 삼켜진다** —
   * [0a] 실측에서 배관 결손이 처음에 완전히 조용했던 원인이다(0a 보고서 §4-④). 배속을 올리면
   * 스로틀 주기가 함께 빨라지고 시뮬이 멈추면 스로틀이 영원히 굳는 문제도 같은 뿌리다.
   *
   * @note 계약 §0 의 "시각은 시뮬 시계, 벽시계 혼용 금지"는 **메시지에 실리는 시각**의 규약이다.
   *       이 시계 값은 와이어에도 도메인에도 들어가지 않는다 — 진단 출력의 속도 제한 전용이며,
   *       `sim_bridge` 는 같은 이유로 이미 steady 시계를 쓰고 있었고 그 노드만 §4-④ 를 피했다.
   */
  rclcpp::Clock log_throttle_clock_{RCL_STEADY_TIME};
};

} // namespace mrs
