#pragma once

/**
 * @file map_registry_node.hpp
 * @brief map_registry 서비스 노드 — GetRoadmap/GetUniformView/GetDependencySkeleton/
 * TransformNode/ValidateMap 5종 + `roadmap_version` latched 통지 (D-11, 계약 L-16).
 *
 * ## [0a] 범위 (architecture §7-1 + 통합 시 확장 1건)
 * [0a] tracer bullet 은 **소형 하드코딩 roadmap 1장**이면 된다. 실동작 서비스는 2종이다.
 *  - `GetRoadmap`     : 물리 뷰. 변환은 `mrs::convert::fill_response` 가 유일하게 수행한다.
 *  - `GetUniformView` : 균일 뷰. **§7 최소 목록에는 없었으나 통합 단계에서 추가했다** — 근거는
 *                       바로 아래 절. 세분화 알고리즘은 없다(픽스처가 이미 균일하다).
 *  - 나머지 3종       : **스텁 유지**. collapse·좌표 변환·검사기 3종은 전부 [1](D-11) 대상이며
 *                       이 노드는 그것들을 **호출하지 않는다**. 스텁 응답은 항상 비-OK 결과
 *                       코드와 `roadmap_version = 0`(계약이 런타임에 금지하는 값)으로 나가
 *                       수신자가 100% 폐기하게 한다 — "빈 성공"으로 위장하지 않는다.
 *
 * ## `GetUniformView` 를 [0a] 에 넣은 근거 (통합 판단 — 되돌릴 수 있게 기록한다)
 * architecture §7 "최소 토픽/서비스" 목록은 `map_registry/GetRoadmap` 만 적었다. 그 목록은
 * 2026-07-20 작성분이고, **그 다음 날 4차 재잠금(R-14)** 이 노드 id 를 뷰별 강타입으로 쪼개면서
 * 계약 §0.1 정본표가 `PlannedPaths`(L2 산출)와 `ExecutionWindow`(L3 산출)의 노드 id 를
 * **UNIFORM** 으로 고정했다. 그 결과 [0a] 배관에 다음 삼단논법이 생겼다:
 *  1. L2·L3 는 균일 뷰 스코프 없이는 계약을 지키며 아무것도 발행할 수 없다(전량 폐기 대상).
 *  2. 균일 뷰 스코프의 발급처는 계약이 `GetUniformView` 응답으로 못박았다(§0.1 정본표).
 *  3. 물리 뷰를 균일 뷰로 재해석하는 것은 **C4 위반**이라 소비자(L2·L3)가 스스로 메울 수 없다.
 * ⇒ 발급을 **3뷰의 단일 소유자인 이 노드**가 한다. 소비자 쪽에 우회로를 만들면 그것이 바로
 *   C4 가 막으려던 "각 프로세스가 자기 지도 사본을 갖는" 상태다.
 *
 * **이것이 D-11 의 선취가 아닌 이유**: 이 노드는 세분화·collapse 를 **구현하지 않는다**.
 * 픽스처가 전 엣지 길이 균일로 만들어져 있어 세분화가 항등일 뿐이며, 그 사실은 픽스처의
 * 성질이지 알고리즘이 아니다(`tracer_roadmap.hpp` 참조). 알맹이는 [1] 에 그대로 남아 있다.
 *
 * **계약 결손 1건(보고 대상)**: `mrs_msg_convert` 에 `fill_response(…, GetUniformView::Response&)`
 * 가 없다(`GetRoadmap`·`ValidateMap`·`ReassignRequest`·`ReorderPassing` 4종만 존재). 따라서 이
 * 노드가 **봉투 스칼라**(result·roadmap_version·view_id·unit_length_*)만 직접 채우고, **노드·엣지
 * id 는 전량 `nodes_to_msg`/`edges_to_msg`/`mappings_to_msg` 경유**로 유지했다 — V3 의 실제 구속
 * 대상(`uint32` ↔ 강타입 변환)은 convert 밖으로 새지 않는다. [1] 에서 `fill_response` 신설 요청.
 *
 * ## 콜백에 알고리즘 없음
 * 3뷰 변환 알고리즘은 이 패키지에 두지 않는다(architecture C4, CI 검사 §2.4-3). [0a] 의
 * 하드코딩 지도는 변환이 아니라 **픽스처 좌표 리터럴**이며 `tracer_roadmap.hpp` 에 격리돼 있다.
 *
 * ## 캐시 규약 (Q-6)
 * 응답의 `roadmap_version` 이 클라이언트 불변 캐시의 키다. 무효화는 latched 토픽
 * `/map_registry/roadmap_version`(reliable/transient_local/depth 1)의 값 변화로만 통지된다.
 * [0a] 는 지도가 바뀌지 않으므로 이 토픽을 **기동 시 1회** 발행한다.
 */

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/u_int64.hpp>

#include "mrs_interfaces/srv/get_dependency_skeleton.hpp"
#include "mrs_interfaces/srv/get_roadmap.hpp"
#include "mrs_interfaces/srv/get_uniform_view.hpp"
#include "mrs_interfaces/srv/transform_node.hpp"
#include "mrs_interfaces/srv/validate_map.hpp"
#include "mrs_map_registry/map_registry.hpp"
#include "mrs_msg_convert/convert_result.hpp"

namespace mrs
{

/**
 * @brief MapRegistry 조회 서비스 5종을 노출하는 노드 ([0a] 는 `GetRoadmap` 만 실동작).
 */
class MapRegistryNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 [0a] 픽스처 지도를 적재한 뒤 서비스 5종·버전 토픽을 구성한다.
   * @throws std::invalid_argument 파라미터가 계약 조건(지도 버전 >= 1, 격자 간격 > 0 등)을
   *         어겨 픽스처 지도를 만들 수 없을 때. 조용한 폴백 대신 **기동 실패**로 처리한다
   *         (nav2-reference §2-B1 — 정적 파라미터 정합은 기동 게이트에서 판정).
   */
  MapRegistryNode();
  ~MapRegistryNode() override = default;

private:
  /**
   * @brief `/map_registry/get_roadmap` 핸들러 — 물리 뷰(PHYSICAL)를 응답한다.
   *
   * 변환은 `mrs::convert::fill_response` 단독 소유다. 실패 시 **노드가** 사유별로 로그·카운터를
   * 남기고(R-15 (b)) 응답을 비워 `roadmap_version = 0` 으로 돌려준다 — 수신자는 그 값을 보고
   * 폐기한다. 서비스 응답 스키마에 실패 코드가 없으므로 이것이 유일한 대역 내 실패 신호다.
   *
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::GetRoadmap::Request`.
   *            `known_version` 이 현재 버전과 같으면 본문 없이 `RESULT_UNCHANGED` 로 응답한다.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::GetRoadmap::Response`.
   * @return void
   */
  void on_get_roadmap(
    const std::shared_ptr<mrs_interfaces::srv::GetRoadmap::Request> request,
    std::shared_ptr<mrs_interfaces::srv::GetRoadmap::Response> response);

  /**
   * @brief `/map_registry/get_uniform_view` 핸들러 — 균일 뷰(UNIFORM)를 응답한다.
   *
   * 요청의 `roadmap_version` 은 **기준 물리 지도 버전**이며 `0 = 최신`이다(`GetUniformView.srv`).
   * [0a] 는 지도가 1장뿐이므로 0 이거나 현재 버전과 같을 때만 응답하고, 다른 값이면
   * `RESULT_STALE_VERSION` 으로 거절한다 — 클라이언트가 모르는 지도를 기준으로 삼은 상태에서
   * 균일 뷰를 주면 그 해가 어느 지도 위의 것인지 알 수 없게 된다.
   *
   * 변환 실패 시 **노드가** 사유별 로그·카운터를 남기고(R-15 (b)) 응답을 비운 채
   * `roadmap_version = 0` 으로 돌려준다 — 수신자는 그 값을 보고 폐기한다.
   *
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::GetUniformView::Request`.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::GetUniformView::Response`.
   * @return void
   */
  void on_get_uniform_view(
    const std::shared_ptr<mrs_interfaces::srv::GetUniformView::Request> request,
    std::shared_ptr<mrs_interfaces::srv::GetUniformView::Response> response);

  /**
   * @brief 균일 뷰 데이터를 `GetUniformView` 응답에 싣는다 (콜백에서 분리 — 길이 상한 준수).
   *
   * 노드·엣지·대응표 id 는 전부 `mrs_msg_convert` 경유로 채운다(V3). 이 함수가 직접 대입하는
   * 것은 노드 id 가 아닌 봉투 스칼라뿐이다 — convert 에 이 응답용 `fill_response` 가 없기
   * 때문이며, 그 결손은 헤더 머리말에 보고돼 있다.
   *
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::GetUniformView::Response`.
   *             실패 시 **비워진 채**(기본 구성) 남는다 — 부분 채움을 만들지 않는다.
   * @return `bool` — 전량 변환에 성공하면 true. 실패 시 사유별 카운터·로그를 남기고 false.
   */
  [[nodiscard]] bool fill_uniform_view_response(
    mrs_interfaces::srv::GetUniformView::Response & response);

  /**
   * @brief `/map_registry/get_dependency_skeleton` 핸들러 — **[1](D-11) 대상 스텁**.
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::GetDependencySkeleton::Request`.
   *            사용하지 않는다.
   * @param[out] response 채울 응답.
   *             자료형 `mrs_interfaces::srv::GetDependencySkeleton::Response`.
   *             `RESULT_UNKNOWN_VIEW` + `roadmap_version = 0`.
   * @return void
   */
  void on_get_dependency_skeleton(
    const std::shared_ptr<mrs_interfaces::srv::GetDependencySkeleton::Request> request,
    std::shared_ptr<mrs_interfaces::srv::GetDependencySkeleton::Response> response);

  /**
   * @brief `/map_registry/transform_node` 핸들러 — **[1](D-11) 대상 스텁**.
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::TransformNode::Request`.
   *            사용하지 않는다.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::TransformNode::Response`.
   *             `RESULT_NOT_FOUND` — 부분 결과를 반환하지 않는다는 계약 문언과 일치한다.
   * @return void
   */
  void on_transform_node(
    const std::shared_ptr<mrs_interfaces::srv::TransformNode::Request> request,
    std::shared_ptr<mrs_interfaces::srv::TransformNode::Response> response);

  /**
   * @brief `/map_registry/validate_map` 핸들러 — **[1] 대상 스텁. 검사기를 호출하지 않는다.**
   *
   * 검사기 3종(well-formed·biconnected·통로폭)은 [1] 대상이므로 실행하지 않는다. 미실행을
   * `RESULT_PASS`(기본값 0)로 내보내면 **기본값이 곧 오탐**이 되므로 `RESULT_FAIL` 로 닫는다
   * (fail-closed). 4개 bool 은 전부 false 다 — "검사했고 통과했다"가 아니라 "검사하지 않았다".
   *
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::ValidateMap::Request`.
   *            사용하지 않는다.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::ValidateMap::Response`.
   * @return void
   */
  void on_validate_map(
    const std::shared_ptr<mrs_interfaces::srv::ValidateMap::Request> request,
    std::shared_ptr<mrs_interfaces::srv::ValidateMap::Response> response);

  /**
   * @brief 변환 실패를 사유별로 적립한다 (계약 §0.2 — 사유를 뭉개지 않는다).
   * @param[in] status 변환 실패 사유. 자료형 `mrs::convert::ConvertStatus`.
   * @return void
   */
  void count_convert_failure(mrs::convert::ConvertStatus status);

  /**
   * @brief 균일 뷰 배열 변환 결과를 판정하고, 실패면 사유별 카운터·로그를 남긴다.
   *
   * 판정은 `mrs_msg_convert` 가 했고 보고만 여기서 한다(R-15 (b)).
   *
   * @param[in] field 실패를 귀속시킬 응답 필드 이름. 자료형 `const char *`. 널이 아니어야 한다.
   * @param[in] result 변환 결과. 자료형 `const mrs::convert::ConvertResult &`.
   * @return `bool` — **실패했으면 true**(호출자는 즉시 중단한다), 성공이면 false.
   */
  [[nodiscard]] bool uniform_convert_failed(
    const char * field, const mrs::convert::ConvertResult & result);

  /**
   * @brief 미구현 서비스가 호출됐음을 스로틀 로그로 남긴다.
   * @param[in] service_name 호출된 서비스 이름. 자료형 `const char *`. 널이 아니어야 한다.
   * @return void
   */
  void log_unimplemented(const char * service_name);

  /** @brief `ConvertStatus` 열거자 개수 (계약 §0.2 정본표 7행). */
  static constexpr std::size_t CONVERT_STATUS_COUNT = 7U;
  /** @brief 반복 경고의 스로틀 주기 [ms] (계약 §0.2 THROTTLE). */
  static constexpr std::uint32_t LOG_THROTTLE_MS = 5000U;

  /**
   * @brief [0a] 픽스처 물리 뷰. 기동 시 1회 만들어지고 이후 **불변**이다.
   * @note [1](D-11)에서 `mrs::MapRegistry` 로 이관된다 — 그때 이 멤버는 사라지고 3뷰 정합의
   *       단일 소유자(C4)가 이 자리를 대신한다. [0a] 는 그 로더가 아직 스텁이라 픽스처를 쓴다.
   */
  RoadmapViewData physical_view_;

  /**
   * @brief [0a] 픽스처 균일 뷰. 기동 시 1회 만들어지고 이후 **불변**이다.
   * @note 이 픽스처에 한해 노드·엣지가 @ref physical_view_ 와 같다 — 세분화가 항등이기
   *       때문이며 알고리즘의 산물이 아니다(`tracer_roadmap.hpp` 참조). `view_id` 만 다르다.
   */
  RoadmapViewData uniform_view_;

  rclcpp::CallbackGroup::SharedPtr service_callback_group_; ///< MutuallyExclusive (nav2 §2-A1)

  rclcpp::Service<mrs_interfaces::srv::GetRoadmap>::SharedPtr get_roadmap_srv_;
  rclcpp::Service<mrs_interfaces::srv::GetUniformView>::SharedPtr get_uniform_view_srv_;
  rclcpp::Service<mrs_interfaces::srv::GetDependencySkeleton>::SharedPtr
    get_dependency_skeleton_srv_;
  rclcpp::Service<mrs_interfaces::srv::TransformNode>::SharedPtr transform_node_srv_;
  rclcpp::Service<mrs_interfaces::srv::ValidateMap>::SharedPtr validate_map_srv_;
  rclcpp::Publisher<std_msgs::msg::UInt64>::SharedPtr roadmap_version_pub_; ///< transient_local

  /** @brief `ConvertStatus` 사유별 변환 실패 누적 카운터 (인덱스 = 열거값). */
  std::array<std::uint64_t, CONVERT_STATUS_COUNT> convert_failure_counts_{};

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
