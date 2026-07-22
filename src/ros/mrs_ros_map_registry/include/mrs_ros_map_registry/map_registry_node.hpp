#pragma once

/**
 * @file map_registry_node.hpp
 * @brief map_registry 서비스 노드 — 실 `mrs::MapRegistry` 배선 ([1] D-11, 계약 L-16).
 *        GetRoadmap/GetUniformView/GetDependencySkeleton/TransformNode/ValidateMap 5종 실동작 +
 *        `roadmap_version` latched 통지.
 *
 * ## 실 배선 ([1] — tracer 픽스처 완전 제거)
 * 노드는 기동 시 `roadmap_file`(YAML) 을 로드하고 균일 뷰·골격 뷰를 발급한다(실 알고리즘). 이후
 * 5 서비스는 전부 `mrs::MapRegistry`(3뷰 정합 단일 소유자, C4)를 경유한다. 변환은
 * `mrs_msg_convert` 가 단독 수행하고(V3), 이 노드는 결과를 응답에 실어 보내며 실패를 사유별로
 * **보고**할 뿐이다(R-15 (b): 판정은 convert, 보고는 노드). 콜백에 3뷰 알고리즘은 없다(C4).
 *
 * ## D-j 경계 변환 — 골격 wire view_id (violations.md D-j)
 * MapRegistry 는 골격 뷰에 **내부 view_id**(균일=1 → 골격=2)를 발급한다(transform 이 bare id 로
 * 균일/골격을 구별해야 하기 때문). 그러나 잠긴 계약(`ViewScope`·`GetDependencySkeleton.srv`·
 * `TransformNode.srv`)은 **골격 wire view_id = 유래 균일 뷰 값(1), 종류만 SKELETON** 으로 못박는다.
 * 이 노드가 경계에서 변환한다:
 *  - `kind → 내부 view_id` : PHYSICAL→0, UNIFORM→@ref uniform_view_id_, SKELETON→@ref
 *    skeleton_internal_view_id_.
 *  - **wire view_id** : 골격 응답·스코프는 @ref uniform_view_id_ 를 에코한다(내부 골격 id 를
 *    노출하지 않는다).
 *
 * ## 캐시 규약 (Q-6)
 * 응답의 `roadmap_version` 이 클라이언트 불변 캐시의 키다. 무효화는 latched 토픽
 * `/map_registry/roadmap_version`(reliable/transient_local/depth 1)의 값 변화로만 통지된다.
 * 지도가 기동 시 1회 로드되므로 이 토픽을 기동 시 1회 발행한다.
 */

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/u_int64.hpp>

#include "mrs/view_ids.hpp" // ViewKind, ViewScope
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
 * @brief MapRegistry 조회·검증 서비스 5종을 노출하는 노드 (실 `mrs::MapRegistry` 배선).
 */
class MapRegistryNode : public rclcpp::Node
{
public:
  /**
   * @brief 노드를 생성하고 `roadmap_file`(YAML) 을 로드해 균일·골격 뷰를 발급한 뒤 서비스 5종·버전
   *        토픽을 구성한다.
   * @throws std::invalid_argument 파라미터 위반(지도 버전 파생 불가·기하 상수 비유한) 또는 지도
   *         로드·뷰 발급 실패 시. 조용한 폴백 대신 **기동 실패**로 처리한다(nav2-reference §2-B1).
   */
  MapRegistryNode();
  ~MapRegistryNode() override = default;

private:
  /**
   * @brief `/map_registry/get_roadmap` 핸들러 — 물리 뷰(PHYSICAL, wire view_id = 0)를 응답한다.
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::GetRoadmap::Request`.
   *            `known_version` 이 현재 버전과 같으면 본문 없이 `RESULT_UNCHANGED`.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::GetRoadmap::Response`.
   * @return void
   */
  void on_get_roadmap(
    const std::shared_ptr<mrs_interfaces::srv::GetRoadmap::Request> request,
    std::shared_ptr<mrs_interfaces::srv::GetRoadmap::Response> response);

  /**
   * @brief `/map_registry/get_uniform_view` 핸들러 — 균일 뷰(UNIFORM)를 응답한다.
   *
   * 요청 `roadmap_version` 은 기준 물리 지도 버전(0 = 최신). 현재 버전과 다르면
   * `RESULT_STALE_VERSION`. 변환은 `fill_response(GetUniformView)` 단독.
   *
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::GetUniformView::Request`.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::GetUniformView::Response`.
   * @return void
   */
  void on_get_uniform_view(
    const std::shared_ptr<mrs_interfaces::srv::GetUniformView::Request> request,
    std::shared_ptr<mrs_interfaces::srv::GetUniformView::Response> response);

  /**
   * @brief `/map_registry/get_dependency_skeleton` 핸들러 — 골격 뷰(SKELETON)를 응답한다.
   *
   * 요청 `(roadmap_version, view_id)` 는 유래 균일 뷰 스코프다. 버전·뷰 id 가 어긋나면
   * `RESULT_STALE_VERSION`/`RESULT_UNKNOWN_VIEW`. **wire view_id 는 유래 균일 뷰 값(D-j)** 으로
   * 에코한다 — 내부 골격 id 를 노출하지 않는다.
   *
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::GetDependencySkeleton::Request`.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::GetDependencySkeleton::Response`.
   * @return void
   */
  void on_get_dependency_skeleton(
    const std::shared_ptr<mrs_interfaces::srv::GetDependencySkeleton::Request> request,
    std::shared_ptr<mrs_interfaces::srv::GetDependencySkeleton::Response> response);

  /**
   * @brief `/map_registry/transform_node` 핸들러 — 노드 ↔ 좌표(node↔pose)를 변환한다.
   *
   * `view_scope`(kind + version + view_id)를 내부 view_id 로 해석(D-j)해
   * `MapRegistry::node_to_pose`
   * /`pose_to_node` 를 원소별 호출한다. 스코프 버전 불일치 `RESULT_STALE_VERSION`, 미지원 종류
   * `RESULT_UNSUPPORTED_VIEW`, 하나라도 대응 없음 `RESULT_NOT_FOUND`(부분 결과 없음).
   *
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::TransformNode::Request`.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::TransformNode::Response`.
   * @return void
   */
  void on_transform_node(
    const std::shared_ptr<mrs_interfaces::srv::TransformNode::Request> request,
    std::shared_ptr<mrs_interfaces::srv::TransformNode::Response> response);

  /**
   * @brief `/map_registry/validate_map` 핸들러 — 정적 검사기 1·3·4 를 실행한다(2 는 U-23 보류).
   *
   * 요청의 기하 상수(r·ρ·L*·v_max·omega_max)로 `MapRegistry::validate_map` 을 돌리고
   * `fill_response(ValidateMap)` 로 응답을 채운다. 통과 여부는 도메인 `result_pass()` 가 정본.
   *
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::ValidateMap::Request`.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::ValidateMap::Response`.
   * @return void
   */
  void on_validate_map(
    const std::shared_ptr<mrs_interfaces::srv::ValidateMap::Request> request,
    std::shared_ptr<mrs_interfaces::srv::ValidateMap::Response> response);

  /**
   * @brief 요청 `view_scope`(kind + wire view_id)를 내부 view_id 로 해석한다 (D-j 경계 변환).
   * @param[in] kind 뷰 종류. 자료형 `mrs::ViewKind`.
   * @param[in] wire_view_id 요청이 실어 온 wire view_id. 자료형 `std::uint32_t`.
   * @return `std::optional<std::uint32_t>` — 종류·wire id 가 정합하면 내부 view_id, 아니면 nullopt
   *         (미지원 종류·wire id 불일치).
   */
  [[nodiscard]] std::optional<std::uint32_t> internal_view_id_for(
    ViewKind kind, std::uint32_t wire_view_id) const;

  /**
   * @brief 균일 뷰를 조회해 `GetUniformView` 응답에 채운다 (콜백 분리 — 길이 상한 준수).
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::GetUniformView::Response`.
   * @return `bool` — 성공이면 true. 실패 시 사유별 카운터·로그를 남기고 false(호출자가 비운다).
   */
  [[nodiscard]] bool fill_uniform_response(
    mrs_interfaces::srv::GetUniformView::Response & response);

  /**
   * @brief 골격 뷰를 조회하고 D-j wire view_id 변환 후 `GetDependencySkeleton` 응답에 채운다.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::GetDependencySkeleton::Response`.
   * @return `bool` — 성공이면 true. 실패 시 사유별 카운터·로그를 남기고 false(호출자가 비운다).
   */
  [[nodiscard]] bool fill_skeleton_response(
    mrs_interfaces::srv::GetDependencySkeleton::Response & response);

  /**
   * @brief 검사기를 돌려 `ValidateMap` 응답에 채운다 (콜백 분리).
   * @param[in] request 요청(기하 상수). 자료형 `mrs_interfaces::srv::ValidateMap::Request`.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::ValidateMap::Response`.
   * @return `bool` — 성공이면 true. 실패 시 카운터·로그를 남기고 false(호출자가 fail-closed).
   */
  [[nodiscard]] bool fill_validate_response(
    const mrs_interfaces::srv::ValidateMap::Request & request,
    mrs_interfaces::srv::ValidateMap::Response & response);

  /**
   * @brief `TransformNode` MODE_NODE_TO_POSE — 노드 id 열을 좌표 열로 채운다.
   * @param[in] internal_view_id 해석된 내부 뷰 id. 자료형 `std::uint32_t`.
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::TransformNode::Request`.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::TransformNode::Response`.
   * @return `bool` — 전량 성공이면 true(`result = RESULT_OK` 설정). 하나라도 실패면 false(부분
   *         결과 없음 — 호출자가 비우고 `RESULT_NOT_FOUND`).
   */
  [[nodiscard]] bool transform_node_to_pose(
    std::uint32_t internal_view_id, const mrs_interfaces::srv::TransformNode::Request & request,
    mrs_interfaces::srv::TransformNode::Response & response);

  /**
   * @brief `TransformNode` MODE_POSE_TO_NODE — 좌표 열을 최근접 노드 id·잔차 열로 채운다.
   * @param[in] internal_view_id 해석된 내부 뷰 id. 자료형 `std::uint32_t`.
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::TransformNode::Request`.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::TransformNode::Response`.
   * @return `bool` — 전량 성공이면 true(`result = RESULT_OK`). 하나라도 실패면 false(부분 결과
   * 없음).
   */
  [[nodiscard]] bool transform_pose_to_node(
    std::uint32_t internal_view_id, const mrs_interfaces::srv::TransformNode::Request & request,
    mrs_interfaces::srv::TransformNode::Response & response);

  /**
   * @brief `TransformNode` 요청을 스코프 해석 후 모드별로 처리해 응답 결과 코드를 채운다 (콜백
   * 분리).
   *
   * 스코프 종류·버전 불일치·미지원 뷰·대응 없음을 각 결과 코드로 갈라 채운다(D-j 해석 포함).
   * try/catch 는 호출 콜백이 진다.
   *
   * @param[in] request 요청. 자료형 `mrs_interfaces::srv::TransformNode::Request`.
   * @param[out] response 채울 응답. 자료형 `mrs_interfaces::srv::TransformNode::Response`.
   * @return void
   */
  void run_transform(
    const mrs_interfaces::srv::TransformNode::Request & request,
    mrs_interfaces::srv::TransformNode::Response & response);

  /**
   * @brief 변환 실패를 사유별로 적립한다 (계약 §0.2 — 사유를 뭉개지 않는다).
   * @param[in] status 변환 실패 사유. 자료형 `mrs::convert::ConvertStatus`.
   * @return void
   */
  void count_convert_failure(mrs::convert::ConvertStatus status);

  /** @brief `ConvertStatus` 열거자 개수 (계약 §0.2 정본표 7행). */
  static constexpr std::size_t CONVERT_STATUS_COUNT = 7U;
  /** @brief 반복 경고의 스로틀 주기 [ms] (계약 §0.2 THROTTLE). */
  static constexpr std::uint32_t LOG_THROTTLE_MS = 5000U;

  /** @brief 3뷰 정합의 단일 소유자 (C4). 기동 시 지도 로드·뷰 발급, 이후 조회·검증. */
  MapRegistry registry_;

  std::uint64_t roadmap_version_{ROADMAP_VERSION_UNSPECIFIED}; ///< 발급된 지도 버전(단조증가)
  std::uint32_t uniform_view_id_{0}; ///< 균일 뷰 내부 id = 균일/골격 wire view_id (D-j)
  std::uint32_t skeleton_internal_view_id_{0}; ///< 골격 뷰 **내부** id (wire 에는 노출하지 않는다)

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
   * 시뮬 시계도 0 에서 시작하므로 기동 직후 첫 스로틀 창이 삼켜진다([0a] 보고서 §4-④). 이 시계
   * 값은 와이어·도메인에 들어가지 않는다 — 진단 출력의 속도 제한 전용이다(계약 §0 시각 규약 무관).
   */
  rclcpp::Clock log_throttle_clock_{RCL_STEADY_TIME};
};

} // namespace mrs
