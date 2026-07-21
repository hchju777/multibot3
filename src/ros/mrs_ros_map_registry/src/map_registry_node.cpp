/**
 * @file map_registry_node.cpp
 * @brief `MapRegistryNode` 구현 — [0a] 픽스처 지도 적재 + `GetRoadmap` 응답 + 스텁 4종.
 *
 * ⚠ 콜백에 알고리즘이 없다. 변환은 전부 `mrs_msg_convert` 가 하고, 이 파일은 그 결과를
 *   서비스 응답에 실어 보내며 실패를 사유별로 **보고**할 뿐이다(R-15 (b): 판정은 convert,
 *   보고는 노드).
 */

#include <cstdint>
#include <exception>
#include <functional>
#include <stdexcept>

#include "mrs_msg_convert/msg_convert.hpp"
#include "mrs_ros_map_registry/map_registry_node.hpp"
#include "mrs_ros_map_registry/tracer_roadmap.hpp"

namespace mrs
{

MapRegistryNode::MapRegistryNode() : rclcpp::Node("map_registry")
{
  TracerRoadmapParams params;

  // 지도 버전은 **1 이상**이어야 한다 — 0 은 계약이 런타임에 금지하며 수신자가 100% 폐기한다.
  // 단조증가(불변식 M2)는 [0a] 에서 지도가 바뀌지 않으므로 자명하게 성립한다.
  const std::int64_t declared_version = this->declare_parameter<std::int64_t>("roadmap_version", 1);
  if (declared_version < 1)
  {
    throw std::invalid_argument("roadmap_version must be >= 1 (0 is forbidden at runtime)");
  }
  params.roadmap_version = static_cast<std::uint64_t>(declared_version);

  // 픽스처 격자 간격 [m]. 근거는 tracer_roadmap.hpp 의 격자 설명(엣지 길이 균일·왕복 무손실
  // 최초 회귀 케이스). claim_radius_m 은 파라미터가 아니라 계약 U-01 의 보수적 초기값
  // (unit_length_m / 2)에서 유도된다.
  params.grid_spacing_m = this->declare_parameter<double>("grid_spacing_m", 2.0);
  // ⚠ 아래 둘은 **[0a] 실측 대상**이다. 검사기 3(W_min > 2(r + v_max/omega_max))은 r·v_max·
  //    omega_max 가 미정이라 [1] 대상이며, 이 값들이 그 조건을 만족한다는 주장이 아니다.
  params.corridor_width_m = this->declare_parameter<double>("corridor_width_m", 1.5);
  params.speed_limit_mps = this->declare_parameter<double>("speed_limit_mps", 0.5);

  // 균일 뷰 인스턴스 id. **1 이상 강제** — 0 은 물리 뷰의 예약값이라, 균일 뷰가 0 을 쓰면 두
  // 뷰의 (roadmap_version, view_id) 평면 쌍이 같아져 런타임 스코프 대조가 물리↔균일 혼동을
  // 잡아내지 못한다(계약 §0.1 규칙 C 의 목적 자체가 무력화된다).
  const std::int64_t declared_uniform_view_id =
    this->declare_parameter<std::int64_t>("uniform_view_id", 1);
  if (declared_uniform_view_id < 1)
  {
    throw std::invalid_argument("uniform_view_id must be >= 1 (0 is reserved for the physical view)");
  }

  if (!build_tracer_roadmap(params, physical_view_))
  {
    throw std::invalid_argument("failed to build the [0a] tracer roadmap from parameters");
  }
  if (!build_tracer_uniform_view(
        params, static_cast<std::uint32_t>(declared_uniform_view_id), uniform_view_))
  {
    throw std::invalid_argument("failed to build the [0a] tracer uniform view from parameters");
  }

  // 서비스 5종을 하나의 MutuallyExclusive 그룹에 묶는다(nav2-reference §2-A1). 응답이
  // 읽기 전용이라 병렬로 돌려도 되지만, 직렬 실행이면 [1] 에서 상태를 갖게 될 때
  // 잠금 정합성 부담과 결정론 소실을 처음부터 만들지 않는다.
  service_callback_group_ =
    this->create_callback_group(rclcpp::CallbackGroupType::MutuallyExclusive);

  get_roadmap_srv_ = this->create_service<mrs_interfaces::srv::GetRoadmap>(
    "/map_registry/get_roadmap",
    std::bind(&MapRegistryNode::on_get_roadmap, this, std::placeholders::_1, std::placeholders::_2),
    rclcpp::ServicesQoS(), service_callback_group_);
  get_uniform_view_srv_ = this->create_service<mrs_interfaces::srv::GetUniformView>(
    "/map_registry/get_uniform_view",
    std::bind(
      &MapRegistryNode::on_get_uniform_view, this, std::placeholders::_1, std::placeholders::_2),
    rclcpp::ServicesQoS(), service_callback_group_);
  get_dependency_skeleton_srv_ = this->create_service<mrs_interfaces::srv::GetDependencySkeleton>(
    "/map_registry/get_dependency_skeleton",
    std::bind(
      &MapRegistryNode::on_get_dependency_skeleton, this, std::placeholders::_1,
      std::placeholders::_2),
    rclcpp::ServicesQoS(), service_callback_group_);
  transform_node_srv_ = this->create_service<mrs_interfaces::srv::TransformNode>(
    "/map_registry/transform_node",
    std::bind(
      &MapRegistryNode::on_transform_node, this, std::placeholders::_1, std::placeholders::_2),
    rclcpp::ServicesQoS(), service_callback_group_);
  validate_map_srv_ = this->create_service<mrs_interfaces::srv::ValidateMap>(
    "/map_registry/validate_map",
    std::bind(
      &MapRegistryNode::on_validate_map, this, std::placeholders::_1, std::placeholders::_2),
    rclcpp::ServicesQoS(), service_callback_group_);

  // QoS 정본(contract-registry §3): reliable / transient_local / depth 1 — 늦게 뜬 구독자도
  // 캐시 무효화 키를 받아야 한다(Q-6). [0a] 는 지도가 바뀌지 않으므로 1회만 발행한다.
  rclcpp::QoS version_qos(1);
  version_qos.reliable();
  version_qos.transient_local();
  roadmap_version_pub_ =
    this->create_publisher<std_msgs::msg::UInt64>("/map_registry/roadmap_version", version_qos);

  std_msgs::msg::UInt64 version_msg;
  version_msg.data = physical_view_.roadmap_version;
  roadmap_version_pub_->publish(version_msg);

  RCLCPP_INFO(
    this->get_logger(),
    "map_registry started — [0a] fixture roadmap: version=%llu, nodes=%zu, edges=%zu, "
    "grid_spacing_m=%.3f, uniform view_id=%u (세분화 항등 — 픽스처가 이미 균일)",
    static_cast<unsigned long long>(physical_view_.roadmap_version), physical_view_.nodes.size(),
    physical_view_.edges.size(), params.grid_spacing_m, uniform_view_.view_id);
}

void MapRegistryNode::on_get_roadmap(
  const std::shared_ptr<mrs_interfaces::srv::GetRoadmap::Request> request,
  std::shared_ptr<mrs_interfaces::srv::GetRoadmap::Response> response)
{
  try
  {
    const mrs::convert::ConvertResult result =
      mrs::convert::fill_response(physical_view_, request->known_version, *response);
    if (result.ok)
    {
      return;
    }

    // 판정은 convert 가 했고, 보고는 여기서 한다(R-15 (b)). 사유를 뭉개지 않는다(계약 §0.2).
    count_convert_failure(result.reason);
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "GetRoadmap 응답 변환 실패 (reason=%s) — roadmap_version 0 으로 비워 응답한다(수신자 폐기)",
      mrs::convert::to_string(result.reason));
    *response = mrs_interfaces::srv::GetRoadmap::Response{};
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "on_get_roadmap failed (safe fallback: empty response): %s", e.what());
    *response = mrs_interfaces::srv::GetRoadmap::Response{};
  }
  catch (...)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "on_get_roadmap failed with unknown exception (safe fallback: empty response)");
    *response = mrs_interfaces::srv::GetRoadmap::Response{};
  }
}

bool MapRegistryNode::uniform_convert_failed(
  const char * field, const mrs::convert::ConvertResult & result)
{
  if (result.ok)
  {
    return false;
  }

  count_convert_failure(result.reason);
  RCLCPP_ERROR_THROTTLE(
    this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
    "GetUniformView %s 변환 실패 (reason=%s) — 응답을 비워 roadmap_version 0 으로 돌려준다"
    "(수신자 폐기). 해당 convert 함수가 미구현 스텁이면 [1] 에서 먼저 구현할 것",
    field, mrs::convert::to_string(result.reason));
  return true;
}

bool MapRegistryNode::fill_uniform_view_response(
  mrs_interfaces::srv::GetUniformView::Response & response)
{
  // 노드 id 를 나르는 세 배열은 전부 convert 가 채운다(V3). 아래 스칼라는 노드 id 가 아니다.
  if (uniform_convert_failed("nodes", mrs::convert::nodes_to_msg(uniform_view_, response.nodes)))
  {
    return false;
  }
  if (uniform_convert_failed("edges", mrs::convert::edges_to_msg(uniform_view_, response.edges)))
  {
    return false;
  }

  // 이 픽스처의 균일 뷰는 가상 노드가 0 개다(세분화가 항등) ⇒ 대응표의 정확한 값이 **빈 배열**
  // 이고 변환할 원소가 없다. 호출을 건너뛰는 것은 검사 우회가 아니라 "변환 대상 없음"이다.
  // ⚠ 다만 조용히 비워 두면 [1] 에서 진짜 대응표가 생겼을 때 말없이 사라지므로, 원소가 생기는
  //   즉시 변환을 시도하고 실패하면 중단한다(현재 `mappings_to_msg` 는 미구현 스텁이다).
  response.subdivision_map.clear();
  if (
    !uniform_view_.mappings.empty() &&
    uniform_convert_failed(
      "subdivision_map", mrs::convert::mappings_to_msg(uniform_view_, response.subdivision_map)))
  {
    return false;
  }

  response.result = mrs_interfaces::srv::GetUniformView::Response::RESULT_OK;
  response.roadmap_version = uniform_view_.roadmap_version;
  response.view_id = uniform_view_.view_id;
  response.unit_length_m = uniform_view_.unit_length_m;
  // 0 = **미유도**(2(r+L) 의 r·L 이 [0a] 에 미정)이지 "하한이 0"이라는 주장이 아니다.
  // 이 값을 강제하는 검사기 3-b 는 [1] 대상이며 [0a] 는 호출하지 않는다.
  response.unit_length_lower_bound_m = uniform_view_.unit_length_lower_bound_m;
  return true;
}

void MapRegistryNode::on_get_uniform_view(
  const std::shared_ptr<mrs_interfaces::srv::GetUniformView::Request> request,
  std::shared_ptr<mrs_interfaces::srv::GetUniformView::Response> response)
{
  try
  {
    *response = mrs_interfaces::srv::GetUniformView::Response{};

    // 요청의 roadmap_version 은 기준 물리 지도 버전이다(0 = 최신). [0a] 는 지도가 1 장뿐이므로
    // 그 밖의 값은 클라이언트가 모르는 지도를 기준으로 삼았다는 뜻이라 거절한다.
    if (
      request->roadmap_version != ROADMAP_VERSION_UNSPECIFIED &&
      request->roadmap_version != uniform_view_.roadmap_version)
    {
      response->result = mrs_interfaces::srv::GetUniformView::Response::RESULT_STALE_VERSION;
      return;
    }

    if (!fill_uniform_view_response(*response))
    {
      // 판정은 convert, 보고는 노드(R-15 (b)). 사유는 위에서 이미 남겼다. 응답을 비워
      // roadmap_version 0 으로 내보내 수신자가 폐기하게 한다 — "빈 성공"을 만들지 않는다.
      *response = mrs_interfaces::srv::GetUniformView::Response{};
    }
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS, "on_get_uniform_view failed: %s",
      e.what());
    *response = mrs_interfaces::srv::GetUniformView::Response{};
  }
  catch (...)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "on_get_uniform_view failed with unknown exception");
    *response = mrs_interfaces::srv::GetUniformView::Response{};
  }
}

void MapRegistryNode::on_get_dependency_skeleton(
  const std::shared_ptr<mrs_interfaces::srv::GetDependencySkeleton::Request> request,
  std::shared_ptr<mrs_interfaces::srv::GetDependencySkeleton::Response> response)
{
  (void)request;
  try
  {
    log_unimplemented("get_dependency_skeleton");
    *response = mrs_interfaces::srv::GetDependencySkeleton::Response{};
    // collapse 는 [1](D-11) 대상 — 발급한 골격 뷰가 없으므로 UNKNOWN_VIEW 가 정확한 사유다.
    response->result = mrs_interfaces::srv::GetDependencySkeleton::Response::RESULT_UNKNOWN_VIEW;
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "on_get_dependency_skeleton failed: %s", e.what());
  }
}

void MapRegistryNode::on_transform_node(
  const std::shared_ptr<mrs_interfaces::srv::TransformNode::Request> request,
  std::shared_ptr<mrs_interfaces::srv::TransformNode::Response> response)
{
  (void)request;
  try
  {
    log_unimplemented("transform_node");
    *response = mrs_interfaces::srv::TransformNode::Response{};
    // 좌표 변환은 [1](D-11) 대상. 계약이 "부분 결과를 반환하지 않는다"고 못박았으므로
    // 빈 배열 + NOT_FOUND 로 닫는다.
    response->result = mrs_interfaces::srv::TransformNode::Response::RESULT_NOT_FOUND;
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS, "on_transform_node failed: %s",
      e.what());
  }
}

void MapRegistryNode::on_validate_map(
  const std::shared_ptr<mrs_interfaces::srv::ValidateMap::Request> request,
  std::shared_ptr<mrs_interfaces::srv::ValidateMap::Response> response)
{
  (void)request;
  try
  {
    log_unimplemented("validate_map");
    // ⛔ 검사기 3종을 호출하지 않는다([1] 대상). 미실행을 RESULT_PASS(=0)로 내보내면
    //    **기본값이 곧 오탐**이므로 fail-closed 로 닫고 4개 bool 을 전부 false 로 둔다.
    *response = mrs_interfaces::srv::ValidateMap::Response{};
    response->result = mrs_interfaces::srv::ValidateMap::Response::RESULT_FAIL;
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS, "on_validate_map failed: %s",
      e.what());
  }
}

void MapRegistryNode::count_convert_failure(mrs::convert::ConvertStatus status)
{
  const std::size_t index = static_cast<std::size_t>(status);
  if (index < convert_failure_counts_.size())
  {
    ++convert_failure_counts_[index];
  }
}

void MapRegistryNode::log_unimplemented(const char * service_name)
{
  RCLCPP_WARN_THROTTLE(
    this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
    "%s 는 [0a] 범위 밖이다 ([1] D-11 대상) — 비-OK 결과로 거절한다", service_name);
}

} // namespace mrs
