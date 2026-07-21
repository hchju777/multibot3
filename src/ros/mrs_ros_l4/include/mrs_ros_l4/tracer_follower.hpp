#pragma once

/**
 * @file tracer_follower.hpp
 * @brief [0a] tracer bullet 전용 창 소비·지령 판정 (순수 함수, ROS 무의존).
 *
 * ⛔ **이것은 D-07 의 국소 궤적 계획이 아니다.** QP·LSC·시프트 연산자·이웃 고려가 하나도 없고,
 * 그것들은 `mrs_local_traj`(`ILocalPlanner`)의 소관이며 로드맵 [2] 에서 붙는다. 여기 있는 것은
 * "창을 순서대로 소비하고 그동안 전진 지령을 낸다"는 **배관 구동용 최소 규칙**뿐이다.
 *
 * ## 왜 기하 추종(목표 노드 방향 비례 제어)이 아닌가 — 근거를 남긴다
 * 비례 제어를 하려면 균일 뷰 노드 id → map 프레임 좌표의 표가 필요하다. 그 표의 유일한 출처는
 * MapRegistry 이고(아키텍처 C4 — 3뷰 변환의 **단일 소유자**), 계약 Q-6 은 L4 가
 * `GetRoadmap`/`GetUniformView` 응답을 캐시해 인프로세스로 변환하라고 규정한다. 그런데
 * `mrs_msg_convert` 에는 그 응답의 **클라이언트측(수신) 변환 함수가 존재하지 않는다** —
 * `fill_response()`(서버측 발행)만 있다. 노드가 응답 메시지 필드를 도메인 값에 직접 대입하면
 * 계약 §0.1 V3(변환 단일 소유)를 어기고, 좌표표를 ROS2 param 으로 따로 실으면 지도의 정본이
 * 둘이 되어 C4 를 어긴다. **둘 다 [0a] 의 배관 실측을 위해 치를 값이 아니다.**
 * ⇒ [0a] 는 세그먼트를 **시간·노드 도달**로 소비하고, 기하 추종은 [1](D-11 MapRegistry)에서
 *   클라이언트측 변환이 생긴 뒤 `mrs_local_traj` 가 가져간다.
 */

#include <cstddef>
#include <cstdint>

#include "mrs/contract_types.hpp"
#include "mrs/view_ids.hpp"

namespace mrs::tracer
{

/**
 * @brief 현재 세그먼트를 소비(다음 세그먼트로 전진)할 사유.
 */
enum class SegmentAdvance : std::uint8_t
{
  /** @brief 아직 소비 조건이 성립하지 않았다. */
  NOT_DUE = 0,
  /** @brief 관측된 점유 노드가 세그먼트 종점과 일치한다 — 정상 소비. */
  NODE_REACHED = 1,
  /**
   * @brief 체류 시간이 상한을 넘었다 — **관측 없이** 소비한다.
   * @note [0a] 관측 대상. 이 사유의 발생률이 높으면 sim 백엔드가 창을 따라가지 못한다는 뜻이며,
   *       그것은 배관이 아니라 시나리오·백엔드 문제다. 사유를 뭉개면 그 구별을 잃는다.
   */
  TIMEOUT = 2
};

/**
 * @brief [0a] 가 내보내는 속도 지령 (몸체점 q 기준).
 */
struct FollowCommand
{
  double v_mps{0.0}; ///< 몸체 선속도 지령 [m/s]
  double omega_rps{0.0}; ///< 몸체 각속도 지령 [rad/s]. [0a] 는 항상 0 (조향 근거가 없다)
};

/**
 * @brief 현재 세그먼트를 소비할지 판정한다 ([0a] 배관 규칙).
 *
 * 우선순위는 **노드 도달 > 타임아웃**이다. 관측이 유효하고 점유 노드가 세그먼트 종점과 같으면
 * 정상 소비이며, 그 조건이 성립하지 않는 동안 체류 시간이 @p segment_timeout_s 를 넘으면
 * 타임아웃 소비다. 두 사유를 구별해 반환하는 이유는 호출자가 사유별 카운터를 적립해야 하기
 * 때문이다 — 뭉개면 "창이 정상 소비됐다"와 "sim 이 못 따라와 강제로 넘겼다"가 같은 수치가 된다.
 *
 * @param[in] observation_valid 직전 `RobotState` 관측을 보유하고 있는지. 자료형 `bool`.
 *            false 이면 노드 도달 판정을 하지 않는다(미관측을 도달로 오인하지 않기 위함).
 * @param[in] occupied_node 관측된 점유 노드. 자료형 `mrs::UniformNodeId`(균일 뷰 강타입).
 *            센티넬(엣지 위)이면 도달로 보지 않는다.
 * @param[in] target_node 현재 세그먼트의 종점 노드. 자료형 `mrs::UniformNodeId`.
 * @param[in] elapsed_s 현재 세그먼트 진입 후 경과 시간 [s]. 자료형 `double`. 시뮬 시계 기준.
 * @param[in] segment_timeout_s 세그먼트 체류 상한 [s]. 자료형 `double`.
 *            0 이하이면 타임아웃 소비를 하지 않는다(기능 비활성).
 * @return `SegmentAdvance` — NOT_DUE(유지) / NODE_REACHED(관측 도달) / TIMEOUT(시간 초과).
 */
[[nodiscard]] SegmentAdvance evaluate_segment_advance(
  bool observation_valid, UniformNodeId occupied_node, UniformNodeId target_node, double elapsed_s,
  double segment_timeout_s) noexcept;

/**
 * @brief 창이 지금 실행 가능한 상태인지 판정한다.
 *
 * 실행 가능 조건은 세 가지다: (i) 소비할 세그먼트가 남아 있고, (ii) 창이 만료되지 않았으며
 * (`window_valid_until_s`, 계약 L-01 불변식 W5 의 수신 측 대응), (iii) 개정 종류가
 * `RevisionKind::NEW` 다. `TRUNCATE` 창은 **부분 폐기 통지**이지 실행 대상이 아니다
 * (`segments` 가 비어 있어야 하는 계약이므로 그 자체로 실행할 것이 없다).
 *
 * @param[in] window 현재 수용 중인 실행 창. 자료형 `mrs::ExecutionWindow`.
 * @param[in] segment_index 다음에 소비할 세그먼트 인덱스. 자료형 `std::size_t`.
 * @param[in] now_s 현재 시각 [s]. 자료형 `double`. **시뮬 시계 기준 절대시각**.
 * @return `bool` — 실행 가능하면 true, 소진·만료·TRUNCATE 이면 false(호출자는 정지 지령).
 */
[[nodiscard]] bool is_window_live(
  const ExecutionWindow & window, std::size_t segment_index, double now_s) noexcept;

/**
 * @brief 창 소진이 임박했는지 판정한다 — `EscalationReport(WINDOW_EXPIRING)` 의 발행 조건.
 *
 * 두 축의 **논리합**이다: 남은 세그먼트 수가 임계 이하이거나, 만료까지 남은 시간이 임계 이하다.
 * 둘을 합친 이유는 [0a] 가 두 소진 경로(공간·시간)를 모두 배선 실측 대상으로 삼기 때문이며,
 * 어느 축이 먼저 발화하는지는 관측 결과이지 지금 정할 값이 아니다.
 *
 * @param[in] window 현재 수용 중인 실행 창. 자료형 `mrs::ExecutionWindow`.
 * @param[in] segment_index 다음에 소비할 세그먼트 인덱스. 자료형 `std::size_t`.
 * @param[in] now_s 현재 시각 [s]. 자료형 `double`. 시뮬 시계 기준 절대시각.
 * @param[in] remaining_segment_threshold 남은 세그먼트 수 임계. 자료형 `std::size_t`.
 *            남은 수가 이 값 **이하**면 임박으로 본다.
 * @param[in] lead_time_s 만료 선행 시간 임계 [s]. 자료형 `double`.
 *            0 이하이면 시간 축 판정을 하지 않는다.
 * @return `bool` — 임박이면 true. 창이 이미 소진·만료됐으면 true(그 역시 보고 대상이다).
 */
[[nodiscard]] bool is_window_exhaustion_imminent(
  const ExecutionWindow & window, std::size_t segment_index, double now_s,
  std::size_t remaining_segment_threshold, double lead_time_s) noexcept;

/**
 * @brief 창 상태로부터 속도 지령을 만든다 ([0a] 최소 규칙).
 *
 * 창이 살아 있으면 상수 전진, 아니면 정지다. 각속도는 항상 0 이다 — [0a] 는 노드 좌표를 갖지
 * 않으므로 조향의 근거가 없고, 근거 없는 조향값을 내보내는 것은 수치 창작이다(파일 머리말 참조).
 *
 * @param[in] window_live 창이 실행 가능한 상태인지. 자료형 `bool`.
 *            @ref is_window_live 의 결과를 그대로 넘긴다.
 * @param[in] follow_speed_mps 창 실행 중 내보낼 상수 선속도 [m/s]. 자료형 `double`.
 *            음수·비유한이면 0 으로 취급한다(구동기에 쓰레기를 보내지 않는다).
 * @return `FollowCommand` — `v_mps`(전진 또는 0)와 `omega_rps`(항상 0).
 */
[[nodiscard]] FollowCommand make_follow_command(bool window_live, double follow_speed_mps) noexcept;

} // namespace mrs::tracer
