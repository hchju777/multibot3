/**
 * @file map_registry_node.cpp
 * @brief `MapRegistryNode` 구현 — 실 `mrs::MapRegistry` 배선 (YAML 로드 + 뷰 발급 + 5 서비스).
 *
 * ⚠ 콜백에 3뷰 알고리즘이 없다. 변환은 전부 `mrs_msg_convert` 가, 3뷰 연산은 `mrs::MapRegistry`
 *   가 한다. 이 파일은 그 결과를 서비스 응답에 실어 보내며 실패를 사유별로 **보고**할 뿐이다
 *   (R-15 (b): 판정은 convert/registry, 보고는 노드).
 */

#include <cmath>
#include <cstdint>
#include <exception>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

#include "mrs_msg_convert/msg_convert.hpp"
#include "mrs_ros_map_registry/map_registry_node.hpp"

namespace mrs
{

MapRegistryNode::MapRegistryNode() : rclcpp::Node("map_registry")
{
  // ── 지도 파일 (YAML 필수 — tracer 폴백 없음, 사용자 결정) ────────────────────────────
  const std::string roadmap_file = this->declare_parameter<std::string>("roadmap_file", "");
  if (roadmap_file.empty())
  {
    throw std::invalid_argument(
      "roadmap_file parameter is required (YAML-only, no tracer fallback)");
  }

  // ── 세분화 입도 + 기하 상수 (iw.hub 실측 — 노드가 하한 2(ρ+L*) 를 계산해 넘긴다) ─────────
  const double unit_length_m = this->declare_parameter<double>("unit_length_m", 2.30);
  const double inflation_radius_m = this->declare_parameter<double>("inflation_radius_m", 0.787904);
  const double nid_offset_l_m = this->declare_parameter<double>("nid_offset_l_m", 0.317855);
  if (
    !std::isfinite(unit_length_m) || unit_length_m <= 0.0 || !std::isfinite(inflation_radius_m) ||
    inflation_radius_m < 0.0 || !std::isfinite(nid_offset_l_m) || nid_offset_l_m < 0.0)
  {
    throw std::invalid_argument("unit_length_m/inflation_radius_m/nid_offset_l_m must be finite");
  }
  const double unit_length_lower_bound_m = 2.0 * (inflation_radius_m + nid_offset_l_m);

  // ── 지도 로드 → 균일 뷰 → 골격 뷰 (실 MapRegistry 알고리즘) ──────────────────────────
  const MapResult<std::uint64_t> loaded = registry_.load_physical_roadmap(roadmap_file);
  if (!loaded.is_ok())
  {
    throw std::invalid_argument(
      "failed to load roadmap_file '" + roadmap_file +
      "' (status=" + std::string(mrs::to_string(loaded.status)) + ")");
  }
  roadmap_version_ = loaded.value;

  const MapResult<std::uint32_t> uniform =
    registry_.build_uniform_view(unit_length_m, unit_length_lower_bound_m);
  if (!uniform.is_ok())
  {
    throw std::invalid_argument(
      "failed to build uniform view (unit=" + std::to_string(unit_length_m) +
      ", lower_bound=" + std::to_string(unit_length_lower_bound_m) +
      ", status=" + std::string(mrs::to_string(uniform.status)) + ")");
  }
  uniform_view_id_ = uniform.value;

  const MapResult<std::uint32_t> skeleton = registry_.build_dependency_skeleton(uniform_view_id_);
  if (!skeleton.is_ok())
  {
    throw std::invalid_argument(
      "failed to build dependency skeleton (status=" +
      std::string(mrs::to_string(skeleton.status)) + ")");
  }
  skeleton_internal_view_id_ = skeleton.value;

  // ── 서비스 5종 (MutuallyExclusive — nav2-reference §2-A1) ────────────────────────────
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

  // ── 버전 latched 통지 (Q-6, contract-registry §3) ───────────────────────────────────
  rclcpp::QoS version_qos(1);
  version_qos.reliable();
  version_qos.transient_local();
  roadmap_version_pub_ =
    this->create_publisher<std_msgs::msg::UInt64>("/map_registry/roadmap_version", version_qos);

  std_msgs::msg::UInt64 version_msg;
  version_msg.data = roadmap_version_;
  roadmap_version_pub_->publish(version_msg);

  RCLCPP_INFO(
    this->get_logger(),
    "map_registry started — loaded '%s': version=%llu, unit_length_m=%.4f (lower_bound=%.6f), "
    "uniform view_id=%u, skeleton internal view_id=%u (wire=%u, D-j)",
    roadmap_file.c_str(), static_cast<unsigned long long>(roadmap_version_), unit_length_m,
    unit_length_lower_bound_m, uniform_view_id_, skeleton_internal_view_id_, uniform_view_id_);
}

std::optional<std::uint32_t> MapRegistryNode::internal_view_id_for(
  ViewKind kind, std::uint32_t wire_view_id) const
{
  switch (kind)
  {
    case ViewKind::PHYSICAL:
      return wire_view_id == VIEW_ID_PHYSICAL ? std::optional<std::uint32_t>{VIEW_ID_PHYSICAL}
                                              : std::nullopt;
    case ViewKind::UNIFORM:
      return wire_view_id == uniform_view_id_ ? std::optional<std::uint32_t>{uniform_view_id_}
                                              : std::nullopt;
    case ViewKind::SKELETON:
      // D-j: 골격 wire view_id 는 유래 균일 뷰 값과 같다 → 내부 골격 id 로 해석.
      return wire_view_id == uniform_view_id_
               ? std::optional<std::uint32_t>{skeleton_internal_view_id_}
               : std::nullopt;
  }
  return std::nullopt;
}

void MapRegistryNode::on_get_roadmap(
  const std::shared_ptr<mrs_interfaces::srv::GetRoadmap::Request> request,
  std::shared_ptr<mrs_interfaces::srv::GetRoadmap::Response> response)
{
  try
  {
    const MapResult<RoadmapViewData> view = registry_.get_view(VIEW_ID_PHYSICAL);
    if (!view.is_ok())
    {
      *response = mrs_interfaces::srv::GetRoadmap::Response{};
      return;
    }
    const mrs::convert::ConvertResult result =
      mrs::convert::fill_response(view.value, request->known_version, *response);
    if (result.ok)
    {
      return;
    }
    count_convert_failure(result.reason);
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "GetRoadmap 응답 변환 실패 (reason=%s) — 비워 응답한다(수신자 폐기)",
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
    *response = mrs_interfaces::srv::GetRoadmap::Response{};
  }
}

bool MapRegistryNode::fill_uniform_response(
  mrs_interfaces::srv::GetUniformView::Response & response)
{
  const MapResult<RoadmapViewData> view = registry_.get_view(uniform_view_id_);
  if (!view.is_ok())
  {
    return false;
  }
  const mrs::convert::ConvertResult result = mrs::convert::fill_response(view.value, response);
  if (!result.ok)
  {
    count_convert_failure(result.reason);
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "GetUniformView 응답 변환 실패 (reason=%s) — 비워 응답한다(수신자 폐기)",
      mrs::convert::to_string(result.reason));
    return false;
  }
  return true;
}

void MapRegistryNode::on_get_uniform_view(
  const std::shared_ptr<mrs_interfaces::srv::GetUniformView::Request> request,
  std::shared_ptr<mrs_interfaces::srv::GetUniformView::Response> response)
{
  using Resp = mrs_interfaces::srv::GetUniformView::Response;
  try
  {
    *response = Resp{};
    // 요청 roadmap_version 은 기준 물리 지도 버전(0 = 최신). 다른 값이면 모르는 지도 기준이다.
    if (
      request->roadmap_version != ROADMAP_VERSION_UNSPECIFIED &&
      request->roadmap_version != roadmap_version_)
    {
      response->result = Resp::RESULT_STALE_VERSION;
      return;
    }
    if (!fill_uniform_response(*response))
    {
      *response = Resp{}; // 사유는 헬퍼가 남겼다. 빈 응답 → roadmap_version 0 → 수신자 폐기.
    }
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS, "on_get_uniform_view failed: %s",
      e.what());
    *response = Resp{};
  }
  catch (...)
  {
    *response = Resp{};
  }
}

bool MapRegistryNode::fill_skeleton_response(
  mrs_interfaces::srv::GetDependencySkeleton::Response & response)
{
  MapResult<RoadmapViewData> view = registry_.get_view(skeleton_internal_view_id_);
  if (!view.is_ok())
  {
    return false;
  }
  // D-j 경계 변환 — 내부 골격 id 를 노출하지 않고 유래 균일 뷰 값을 wire view_id 로 에코한다.
  view.value.view_id = uniform_view_id_;

  const mrs::convert::ConvertResult result = mrs::convert::fill_response(view.value, response);
  if (!result.ok)
  {
    count_convert_failure(result.reason);
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "GetDependencySkeleton 응답 변환 실패 (reason=%s) — 비워 응답한다",
      mrs::convert::to_string(result.reason));
    return false;
  }
  return true;
}

void MapRegistryNode::on_get_dependency_skeleton(
  const std::shared_ptr<mrs_interfaces::srv::GetDependencySkeleton::Request> request,
  std::shared_ptr<mrs_interfaces::srv::GetDependencySkeleton::Response> response)
{
  using Resp = mrs_interfaces::srv::GetDependencySkeleton::Response;
  try
  {
    *response = Resp{};
    if (
      request->roadmap_version != ROADMAP_VERSION_UNSPECIFIED &&
      request->roadmap_version != roadmap_version_)
    {
      response->result = Resp::RESULT_STALE_VERSION;
      return;
    }
    // 요청 view_id 는 **유래 균일 뷰 id**(wire 규약, D-j). 그 밖은 모르는 뷰다.
    if (request->view_id != uniform_view_id_)
    {
      response->result = Resp::RESULT_UNKNOWN_VIEW;
      return;
    }
    if (!fill_skeleton_response(*response))
    {
      *response = Resp{};
    }
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "on_get_dependency_skeleton failed: %s", e.what());
    *response = Resp{};
  }
  catch (...)
  {
    *response = Resp{};
  }
}

bool MapRegistryNode::transform_node_to_pose(
  std::uint32_t internal_view_id, const mrs_interfaces::srv::TransformNode::Request & request,
  mrs_interfaces::srv::TransformNode::Response & response)
{
  std::vector<Pose2D> poses;
  poses.reserve(request.node_ids.size());
  for (const std::uint32_t node_id : request.node_ids)
  {
    const MapResult<Pose2D> pose = registry_.node_to_pose(internal_view_id, node_id);
    if (!pose.is_ok())
    {
      return false; // 부분 결과를 반환하지 않는다(계약 문언).
    }
    poses.push_back(pose.value);
  }
  const mrs::convert::ConvertResult conv = mrs::convert::poses_to_msg(poses, response.poses);
  if (!conv.ok)
  {
    count_convert_failure(conv.reason);
    return false;
  }
  response.result = mrs_interfaces::srv::TransformNode::Response::RESULT_OK;
  return true;
}

bool MapRegistryNode::transform_pose_to_node(
  std::uint32_t internal_view_id, const mrs_interfaces::srv::TransformNode::Request & request,
  mrs_interfaces::srv::TransformNode::Response & response)
{
  for (const mrs_interfaces::msg::Pose2D & pose_msg : request.poses)
  {
    Pose2D pose;
    if (!mrs::convert::from_msg(pose_msg, pose).ok)
    {
      return false;
    }
    const MapResult<NearestNode> near =
      registry_.pose_to_node(internal_view_id, pose.x_m, pose.y_m);
    if (!near.is_ok())
    {
      return false;
    }
    response.node_ids.push_back(near.value.node_id);
    response.residual_m.push_back(near.value.residual_m);
  }
  response.result = mrs_interfaces::srv::TransformNode::Response::RESULT_OK;
  return true;
}

void MapRegistryNode::run_transform(
  const mrs_interfaces::srv::TransformNode::Request & request,
  mrs_interfaces::srv::TransformNode::Response & response)
{
  using Resp = mrs_interfaces::srv::TransformNode::Response;
  using Req = mrs_interfaces::srv::TransformNode::Request;

  // 스코프 종류·wire view_id 를 내부 view_id 로 해석한다(D-j). 종류 범위 위반은 미지원 뷰로 본다.
  ViewScope scope;
  if (!mrs::convert::from_msg(request.view_scope, scope).ok)
  {
    response.result = Resp::RESULT_UNSUPPORTED_VIEW;
    return;
  }
  if (scope.roadmap_version != roadmap_version_)
  {
    response.result = Resp::RESULT_STALE_VERSION;
    return;
  }
  const std::optional<std::uint32_t> internal =
    internal_view_id_for(scope.view_kind, scope.view_id);
  if (!internal.has_value())
  {
    response.result = Resp::RESULT_UNSUPPORTED_VIEW;
    return;
  }

  const bool ok = request.mode == Req::MODE_NODE_TO_POSE
                    ? transform_node_to_pose(*internal, request, response)
                  : request.mode == Req::MODE_POSE_TO_NODE
                    ? transform_pose_to_node(*internal, request, response)
                    : false; // 미지의 mode — fail-closed(빈 결과 + NOT_FOUND).
  if (!ok)
  {
    response = Resp{};
    response.result = Resp::RESULT_NOT_FOUND;
  }
}

void MapRegistryNode::on_transform_node(
  const std::shared_ptr<mrs_interfaces::srv::TransformNode::Request> request,
  std::shared_ptr<mrs_interfaces::srv::TransformNode::Response> response)
{
  using Resp = mrs_interfaces::srv::TransformNode::Response;
  try
  {
    *response = Resp{};
    run_transform(*request, *response);
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS, "on_transform_node failed: %s",
      e.what());
    *response = Resp{};
    response->result = Resp::RESULT_NOT_FOUND;
  }
  catch (...)
  {
    *response = Resp{};
    response->result = Resp::RESULT_NOT_FOUND;
  }
}

bool MapRegistryNode::fill_validate_response(
  const mrs_interfaces::srv::ValidateMap::Request & request,
  mrs_interfaces::srv::ValidateMap::Response & response)
{
  const MapResult<RoadmapValidationResult> result = registry_.validate_map(
    request.view_id, request.robot_count, request.robot_radius_m, request.inflation_radius_m,
    request.nid_offset_l_m, request.v_max_mps, request.omega_max_rps);
  if (!result.is_ok())
  {
    return false; // 검사기 미실행(MAP_NOT_LOADED/VIEW_NOT_FOUND) — PASS(=0)로 내보내지 않는다.
  }
  // 응답 스코프는 현재 지도 버전을 에코한다(요청 0 은 '최신'이므로 실제 버전으로 채운다).
  const mrs::convert::ConvertResult conv =
    mrs::convert::fill_response(result.value, roadmap_version_, request.view_id, response);
  if (!conv.ok)
  {
    count_convert_failure(conv.reason);
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS,
      "ValidateMap 응답 변환 실패 (reason=%s) — fail-closed", mrs::convert::to_string(conv.reason));
    return false;
  }
  return true;
}

void MapRegistryNode::on_validate_map(
  const std::shared_ptr<mrs_interfaces::srv::ValidateMap::Request> request,
  std::shared_ptr<mrs_interfaces::srv::ValidateMap::Response> response)
{
  using Resp = mrs_interfaces::srv::ValidateMap::Response;
  try
  {
    *response = Resp{};
    if (
      request->roadmap_version != ROADMAP_VERSION_UNSPECIFIED &&
      request->roadmap_version != roadmap_version_)
    {
      response->result = Resp::RESULT_STALE_VERSION;
      return;
    }
    if (!fill_validate_response(*request, *response))
    {
      *response = Resp{};
      response->result =
        Resp::RESULT_FAIL; // fail-closed — 미실행·변환 실패를 PASS 로 위장하지 않는다.
    }
  }
  catch (const std::exception & e)
  {
    RCLCPP_ERROR_THROTTLE(
      this->get_logger(), log_throttle_clock_, LOG_THROTTLE_MS, "on_validate_map failed: %s",
      e.what());
    *response = Resp{};
    response->result = Resp::RESULT_FAIL;
  }
  catch (...)
  {
    *response = Resp{};
    response->result = Resp::RESULT_FAIL;
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

} // namespace mrs
