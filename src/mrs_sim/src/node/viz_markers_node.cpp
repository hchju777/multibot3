// SPDX-License-Identifier: Apache-2.0
//
// viz_markers_node — rviz2 마커 발행 노드 (48차 웨이브 2-B, U48-8 · 366_u48_
// viz_and_pipeline_plan.md).
//
// 🔴 이 노드는 알고리즘 코드가 아니다 — 이미 있는 경계 토픽 셋(`/roadmap`,
// `/{robot}/odom`, `/execution_constraints`)을 구독해 `visualization_msgs`
// 마커로 재표현할 뿐이다. 네 모듈(mrs_mrta·mrs_mapf·mrs_sadg·mrs_trajopt)
// 코드는 0줄도 건드리지 않는다(366§ "형태가 이미 정해져 있다"). `mrs_sim`에
// 두는 배치는 오케스트레이터 판단이다(U48-4가 "발행 노드 = mrs_sim"으로
// 같은 형태를 이미 정했다) — 어긋난다고 보이면 `system-architect` 안건으로
// 신고할 것.
//
// 그리는 것(366§ "무엇을 그리는가 (최소)" — 그 이상은 이번 범위 밖):
//   1. 로드맵 그래프 — 정점(구, 엔드포인트는 다른 색으로 구분) + 간선(선).
//   2. 로봇 현재 위치(구) + 방향(화살표, `/odom`의 orientation을 그대로 씀).
//   3. 목표 세그먼트 — `/execution_constraints`의 그 로봇 **첫 세그먼트**
//      (`{robot}#{index}` 중 최소 index — `mrs_trajopt/trajopt_node.cpp`의
//      `ResolveGoalsFromExecConstraints`와 같은 파싱 규칙을 그대로 읽기만
//      한다. 그 파일을 고치지 않았다 — 이 노드는 별도 구독자일 뿐이다).
//
// `CN-2`(48차 신설 — core/에 ROS IDL include 금지, `visualization_msgs`도
// 포함)와 무관하다: `mrs_sim`은 `include/{pkg}/` 공개 헤더가 없는 도구
// 패키지라 CN-1 6레이어 규율 대상이 아니다(CMakeLists.txt 머리 주석,
// `check_layer_layout.py`가 "미검사"로 신고). 이 파일은 그 노드/골격
// 형태(공개 헤더 없이 `src/node/*.cpp` 단독)를 그대로 따른다.
//
// 색·크기·주기는 코드에 박지 않는다(CN-6 정신 — 이 패키지는 CN-7의
// `method.modules.*` 트리 대상은 아니지만 "값은 config에" 원칙은 지킨다).
// `mrs_bringup/config/viz.yaml`이 값의 출처이고, 유도/선택/근거 없음을
// 그 파일 머리말 표(trajopt.yaml 형식)에 적는다. `robots` 로스터는 이 파일이
// 만들지 않는다 — launch가 `mrta.yaml`에서 읽어 `state_integrator`와 같은
// 값을 이 노드에도 넘긴다(F48-6 "단일 출처" 정신).
//
// QoS(모두 코드에 하드코딩 — `qos.yaml` 머리 주석이 신고한 "이 라운드는
// 프로그램으로 로드되지 않는다"와 같은 처지, 다섯 노드의 기존 관행을 따른다):
//   `/roadmap`             — RELIABLE/TRANSIENT_LOCAL/KeepLast(1)
//                             (`roadmap_publisher.cpp`의 발행측과 일치).
//   `/execution_constraints` — RELIABLE/TRANSIENT_LOCAL/KeepLast(1)
//                             (`sadg_t0_node.cpp`의 발행측과 일치).
//   `/{robot}/odom`         — BEST_EFFORT/VOLATILE/KeepLast(1)
//                             (`state_integrator.cpp`의 발행측과 일치).
//   `/viz/markers`(발행)    — RELIABLE/VOLATILE/KeepLast(1). 이 토픽은 위
//                             네과 다른 성격이다 — latched 경계 아티팩트가
//                             아니라 이 노드가 매 틱 다시 계산해 내는
//                             파생물이라 "늦게 붙어도 마지막 값"이 필요
//                             없다(rviz가 늦게 뜨면 다음 틱을 그대로 받는다).
//
// `/clock`을 발행하지 않는다(그 발행자는 `clock_node` 유일 — 파일 어디에도
// 퍼블리셔를 만들지 않았다). `use_sim_time`은 다른 노드처럼 launch의
// `common_params`로 받는다 — 이 파일이 직접 켜거나 끄지 않는다. 마커
// `header.stamp`는 `this->get_clock()->now()`(노드 클록 — sim-time 준수)를
// 쓴다.

#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "geometry_msgs/msg/point.hpp"
#include "mrs_core/status.hpp"
#include "mrs_msgs/msg/execution_constraints.hpp"
#include "mrs_msgs/msg/roadmap.hpp"
#include "mrs_msgs/msg/segment.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/color_rgba.hpp"
#include "visualization_msgs/msg/marker.hpp"
#include "visualization_msgs/msg/marker_array.hpp"

namespace mrs_sim::node
{

namespace
{

/// @brief `[r,g,b,a]` 4원소 파라미터를 `ColorRGBA`로 바꾼다. 길이가 다르면
/// 기동 거부(CN-15) — 잘못 적힌 config를 조용히 절반만 읽지 않는다.
std_msgs::msg::ColorRGBA ColorFromParam(const std::vector<double>& v, const std::string& param_name)
{
    if (v.size() != 4)
    {
        throw mrs_core::ContractViolation("viz_markers_node: parameter '" + param_name +
                                          "' must have exactly 4 elements [r,g,b,a] — got " +
                                          std::to_string(v.size()));
    }
    std_msgs::msg::ColorRGBA c;
    c.r = static_cast<float>(v[0]);
    c.g = static_cast<float>(v[1]);
    c.b = static_cast<float>(v[2]);
    c.a = static_cast<float>(v[3]);
    return c;
}

/// @brief `{robot}#{index}` 형태의 세그먼트 id에서 index를 뽑는다.
/// `mrs_trajopt/src/node/trajopt_node.cpp`의 `ResolveGoalsFromExecConstraints`가
/// 쓰는 것과 같은 파싱 규칙(367/368 계보) — 그 파일을 고치지 않고 같은 규칙을
/// 이 노드에도 다시 적었다(모듈 코드 0줄 원칙, `mrs_trajopt`는 읽기 전용).
/// @return 파싱된 index, 형식이 안 맞으면 `-1`.
long long ParseSegmentIndex(const std::string& segment_id)
{
    const auto hash_pos = segment_id.find('#');
    if (hash_pos == std::string::npos)
    {
        return -1;
    }
    try
    {
        return std::stoll(segment_id.substr(hash_pos + 1));
    }
    catch (const std::exception&)
    {
        return -1;
    }
}

}  // namespace

/// @brief `/roadmap` · `/{robot}/odom` · `/execution_constraints`를 구독해
/// `/viz/markers`(`visualization_msgs/MarkerArray`)를 주기 발행한다.
class VizMarkersNode : public rclcpp::Node
{
public:
    VizMarkersNode() : rclcpp::Node("viz_markers_node")
    {
        robots_ =
            this->declare_parameter<std::vector<std::string>>("robots", std::vector<std::string>{});
        frame_id_ = this->declare_parameter<std::string>("frame_id", "map");
        const auto period_ms = this->declare_parameter<int>("marker_publish_period_ms", 200);

        const auto node_radius_m = this->declare_parameter<double>("roadmap_node_radius_m", 0.15);
        const auto endpoint_radius_m =
            this->declare_parameter<double>("roadmap_endpoint_radius_m", 0.25);
        edge_width_m_ = this->declare_parameter<double>("roadmap_edge_width_m", 0.03);
        const auto robot_body_diameter_m =
            this->declare_parameter<double>("robot_body_diameter_m", 0.5);
        heading_length_m_ = this->declare_parameter<double>("robot_heading_length_m", 0.6);
        heading_shaft_diameter_m_ =
            this->declare_parameter<double>("robot_heading_shaft_diameter_m", 0.12);
        heading_head_diameter_m_ =
            this->declare_parameter<double>("robot_heading_head_diameter_m", 0.22);
        target_segment_width_m_ = this->declare_parameter<double>("target_segment_width_m", 0.08);

        node_diameter_m_ = 2.0 * node_radius_m;
        endpoint_diameter_m_ = 2.0 * endpoint_radius_m;
        robot_body_diameter_m_ = robot_body_diameter_m;

        color_roadmap_node_ =
            ColorFromParam(this->declare_parameter<std::vector<double>>("color_roadmap_node",
                                                                        {0.20, 0.55, 0.95, 0.90}),
                           "color_roadmap_node");
        color_roadmap_endpoint_ =
            ColorFromParam(this->declare_parameter<std::vector<double>>("color_roadmap_endpoint",
                                                                        {0.95, 0.55, 0.10, 0.95}),
                           "color_roadmap_endpoint");
        color_roadmap_edge_ =
            ColorFromParam(this->declare_parameter<std::vector<double>>("color_roadmap_edge",
                                                                        {0.55, 0.55, 0.55, 0.55}),
                           "color_roadmap_edge");
        color_robot_body_ =
            ColorFromParam(this->declare_parameter<std::vector<double>>("color_robot_body",
                                                                        {0.10, 0.75, 0.15, 0.90}),
                           "color_robot_body");
        color_robot_heading_ =
            ColorFromParam(this->declare_parameter<std::vector<double>>("color_robot_heading",
                                                                        {0.05, 0.35, 0.05, 0.95}),
                           "color_robot_heading");
        color_target_segment_ =
            ColorFromParam(this->declare_parameter<std::vector<double>>("color_target_segment",
                                                                        {0.90, 0.10, 0.10, 0.85}),
                           "color_target_segment");

        if (period_ms <= 0)
        {
            throw mrs_core::ContractViolation(
                "viz_markers_node: 'marker_publish_period_ms' must be > 0");
        }

        rclcpp::QoS latched_qos(rclcpp::KeepLast(1));
        latched_qos.reliable().transient_local();

        rclcpp::QoS odom_qos(rclcpp::KeepLast(1));
        odom_qos.best_effort();  // state_integrator.cpp의 발행측과 일치.

        rclcpp::QoS marker_qos(rclcpp::KeepLast(1));
        marker_qos.reliable();  // 파생물 스트림 — latched 아님(파일 머리 주석).

        marker_pub_ = this->create_publisher<visualization_msgs::msg::MarkerArray>("/viz/markers",
                                                                                   marker_qos);

        roadmap_sub_ = this->create_subscription<mrs_msgs::msg::Roadmap>(
            "/roadmap",
            latched_qos,
            [this](mrs_msgs::msg::Roadmap::ConstSharedPtr msg)
            {
                last_roadmap_ = msg;
            });

        exec_constraints_sub_ = this->create_subscription<mrs_msgs::msg::ExecutionConstraints>(
            "/execution_constraints",
            latched_qos,
            [this](mrs_msgs::msg::ExecutionConstraints::ConstSharedPtr msg)
            {
                last_exec_constraints_ = msg;
            });

        odom_subs_.reserve(robots_.size());
        for (const auto& robot : robots_)
        {
            has_odom_[robot] = false;
            odom_subs_.push_back(this->create_subscription<nav_msgs::msg::Odometry>(
                "/" + robot + "/odom",
                odom_qos,
                [this, robot](nav_msgs::msg::Odometry::ConstSharedPtr msg)
                {
                    latest_odom_[robot] = *msg;
                    has_odom_[robot] = true;
                }));
        }

        if (robots_.empty())
        {
            RCLCPP_WARN(this->get_logger(),
                        "viz_markers_node: 'robots' parameter is empty — no odom subscriptions, "
                        "no robot markers will be drawn (roadmap markers still publish).");
        }

        timer_ = this->create_timer(std::chrono::milliseconds(period_ms),
                                    [this]()
                                    {
                                        this->OnTick();
                                    });
    }

private:
    /// @brief 매 틱 — 캐시된 최신 `/roadmap` · `/odom` · `/execution_constraints`로
    /// `MarkerArray`를 새로 지어 발행한다. 오래된 마커를 지우지 않는다 — 같은
    /// (ns, id)로 `ADD`를 다시 보내면 rviz가 그 자리에서 갱신한다(알려진
    /// 한계: 로봇·엔드포인트가 런타임에 줄어들면 마지막 값이 남는다 — 이번
    /// 라운드 범위 밖, 366§ "무엇을 그리는가 (최소)").
    void OnTick()
    {
        const rclcpp::Time now = this->get_clock()->now();
        visualization_msgs::msg::MarkerArray array;

        std::unordered_map<std::string, std::pair<double, double>> node_coords;
        std::unordered_set<std::string> endpoints;
        if (last_roadmap_ != nullptr)
        {
            for (const auto& n : last_roadmap_->nodes)
            {
                node_coords[n.id] = std::make_pair(n.x, n.y);
            }
            for (const auto& e : last_roadmap_->endpoints)
            {
                endpoints.insert(e);
            }

            AppendRoadmapNodeMarkers(*last_roadmap_, endpoints, now, array);
            AppendRoadmapEdgeMarkers(*last_roadmap_, node_coords, now, array);
        }

        for (std::size_t i = 0; i < robots_.size(); ++i)
        {
            const std::string& robot = robots_[i];
            if (!has_odom_[robot])
            {
                continue;  // 아직 첫 odom을 못 받았다 — 좌표를 지어내지 않는다.
            }
            const auto& odom = latest_odom_.at(robot);
            AppendRobotPoseMarkers(robot, static_cast<int>(i), odom, now, array);
            AppendTargetSegmentMarker(robot, static_cast<int>(i), node_coords, now, array);
        }

        marker_pub_->publish(array);
    }

    /// @brief 로드맵 정점 — 엔드포인트는 다른 색·다른 크기로 구분한다.
    void AppendRoadmapNodeMarkers(const mrs_msgs::msg::Roadmap& roadmap,
                                  const std::unordered_set<std::string>& endpoints,
                                  const rclcpp::Time& stamp,
                                  visualization_msgs::msg::MarkerArray& out) const
    {
        int regular_id = 0;
        int endpoint_id = 0;
        for (const auto& n : roadmap.nodes)
        {
            const bool is_endpoint = endpoints.count(n.id) > 0;
            visualization_msgs::msg::Marker m;
            m.header.frame_id = frame_id_;
            m.header.stamp = stamp;
            m.ns = is_endpoint ? "roadmap_endpoints" : "roadmap_nodes";
            m.id = is_endpoint ? endpoint_id++ : regular_id++;
            m.type = visualization_msgs::msg::Marker::SPHERE;
            m.action = visualization_msgs::msg::Marker::ADD;
            m.pose.position.x = n.x;
            m.pose.position.y = n.y;
            m.pose.position.z = 0.0;
            m.pose.orientation.w = 1.0;
            const double d = is_endpoint ? endpoint_diameter_m_ : node_diameter_m_;
            m.scale.x = d;
            m.scale.y = d;
            m.scale.z = d;
            m.color = is_endpoint ? color_roadmap_endpoint_ : color_roadmap_node_;
            out.markers.push_back(m);
        }
    }

    /// @brief 로드맵 간선 — 한 `LINE_LIST` 마커에 전 간선을 담는다(점 쌍마다
    /// 한 선분). 정점 좌표를 못 찾은 간선(로드맵 데이터 결함)은 건너뛴다.
    void AppendRoadmapEdgeMarkers(
        const mrs_msgs::msg::Roadmap& roadmap,
        const std::unordered_map<std::string, std::pair<double, double>>& node_coords,
        const rclcpp::Time& stamp,
        visualization_msgs::msg::MarkerArray& out) const
    {
        visualization_msgs::msg::Marker m;
        m.header.frame_id = frame_id_;
        m.header.stamp = stamp;
        m.ns = "roadmap_edges";
        m.id = 0;
        m.type = visualization_msgs::msg::Marker::LINE_LIST;
        m.action = visualization_msgs::msg::Marker::ADD;
        m.pose.orientation.w = 1.0;
        m.scale.x = edge_width_m_;
        m.color = color_roadmap_edge_;

        for (const auto& e : roadmap.edges)
        {
            const auto from_it = node_coords.find(e.from_id);
            const auto to_it = node_coords.find(e.to_id);
            if (from_it == node_coords.end() || to_it == node_coords.end())
            {
                continue;  // 값을 지어내지 않는다 — 이 간선은 그냥 안 그린다.
            }
            geometry_msgs::msg::Point p_from;
            p_from.x = from_it->second.first;
            p_from.y = from_it->second.second;
            geometry_msgs::msg::Point p_to;
            p_to.x = to_it->second.first;
            p_to.y = to_it->second.second;
            m.points.push_back(p_from);
            m.points.push_back(p_to);
        }

        if (!m.points.empty())
        {
            out.markers.push_back(m);
        }
    }

    /// @brief 한 로봇의 현재 위치(구)와 방향(화살표, `/odom`의 orientation을
    /// 그대로 씀 — 이 노드가 방위를 계산하지 않는다).
    void AppendRobotPoseMarkers(const std::string& robot,
                                int robot_index,
                                const nav_msgs::msg::Odometry& odom,
                                const rclcpp::Time& stamp,
                                visualization_msgs::msg::MarkerArray& out) const
    {
        visualization_msgs::msg::Marker body;
        body.header.frame_id = frame_id_;
        body.header.stamp = stamp;
        body.ns = "robot_body";
        body.id = robot_index;
        body.type = visualization_msgs::msg::Marker::SPHERE;
        body.action = visualization_msgs::msg::Marker::ADD;
        body.pose = odom.pose.pose;
        body.scale.x = robot_body_diameter_m_;
        body.scale.y = robot_body_diameter_m_;
        body.scale.z = robot_body_diameter_m_;
        body.color = color_robot_body_;
        body.text = robot;  // rviz는 SPHERE에 text를 렌더하지 않지만 echo로는 어느 로봇인지 보인다.
        out.markers.push_back(body);

        visualization_msgs::msg::Marker heading;
        heading.header.frame_id = frame_id_;
        heading.header.stamp = stamp;
        heading.ns = "robot_heading";
        heading.id = robot_index;
        heading.type = visualization_msgs::msg::Marker::ARROW;
        heading.action = visualization_msgs::msg::Marker::ADD;
        heading.pose = odom.pose.pose;  // orientation을 그대로 씀 — 방향 계산 없음.
        heading.scale.x = heading_length_m_;
        heading.scale.y = heading_shaft_diameter_m_;
        heading.scale.z = heading_head_diameter_m_;
        heading.color = color_robot_heading_;
        out.markers.push_back(heading);
    }

    /// @brief 로봇의 목표 세그먼트(첫 세그먼트, `ParseSegmentIndex` 최소값) —
    /// `from_id`→`to_id`를 로드맵 좌표로 이은 `LINE_STRIP`. 세그먼트가 없거나
    /// 좌표를 못 찾으면 아무것도 추가하지 않는다(지어내지 않는다).
    void AppendTargetSegmentMarker(
        const std::string& robot,
        int robot_index,
        const std::unordered_map<std::string, std::pair<double, double>>& node_coords,
        const rclcpp::Time& stamp,
        visualization_msgs::msg::MarkerArray& out) const
    {
        if (last_exec_constraints_ == nullptr)
        {
            return;
        }

        const mrs_msgs::msg::Segment* first = nullptr;
        long long first_index = -1;
        for (const auto& seg : last_exec_constraints_->segments)
        {
            if (seg.robot != robot)
            {
                continue;
            }
            const long long idx = ParseSegmentIndex(seg.id);
            if (idx < 0)
            {
                continue;
            }
            if (first == nullptr || idx < first_index)
            {
                first = &seg;
                first_index = idx;
            }
        }
        if (first == nullptr)
        {
            return;
        }

        const auto from_it = node_coords.find(first->from_id);
        const auto to_it = node_coords.find(first->to_id);
        if (from_it == node_coords.end() || to_it == node_coords.end())
        {
            return;  // 좌표를 지어내지 않는다.
        }

        visualization_msgs::msg::Marker m;
        m.header.frame_id = frame_id_;
        m.header.stamp = stamp;
        m.ns = "target_segment";
        m.id = robot_index;
        m.type = visualization_msgs::msg::Marker::LINE_STRIP;
        m.action = visualization_msgs::msg::Marker::ADD;
        m.pose.orientation.w = 1.0;
        m.scale.x = target_segment_width_m_;
        m.color = color_target_segment_;

        geometry_msgs::msg::Point p_from;
        p_from.x = from_it->second.first;
        p_from.y = from_it->second.second;
        geometry_msgs::msg::Point p_to;
        p_to.x = to_it->second.first;
        p_to.y = to_it->second.second;
        m.points.push_back(p_from);
        m.points.push_back(p_to);

        out.markers.push_back(m);
    }

    std::string frame_id_;
    std::vector<std::string> robots_;

    double node_diameter_m_ = 0.0;
    double endpoint_diameter_m_ = 0.0;
    double edge_width_m_ = 0.0;
    double robot_body_diameter_m_ = 0.0;
    double heading_length_m_ = 0.0;
    double heading_shaft_diameter_m_ = 0.0;
    double heading_head_diameter_m_ = 0.0;
    double target_segment_width_m_ = 0.0;

    std_msgs::msg::ColorRGBA color_roadmap_node_;
    std_msgs::msg::ColorRGBA color_roadmap_endpoint_;
    std_msgs::msg::ColorRGBA color_roadmap_edge_;
    std_msgs::msg::ColorRGBA color_robot_body_;
    std_msgs::msg::ColorRGBA color_robot_heading_;
    std_msgs::msg::ColorRGBA color_target_segment_;

    mrs_msgs::msg::Roadmap::ConstSharedPtr last_roadmap_;
    mrs_msgs::msg::ExecutionConstraints::ConstSharedPtr last_exec_constraints_;
    std::map<std::string, nav_msgs::msg::Odometry> latest_odom_;
    std::map<std::string, bool> has_odom_;

    rclcpp::Subscription<mrs_msgs::msg::Roadmap>::SharedPtr roadmap_sub_;
    rclcpp::Subscription<mrs_msgs::msg::ExecutionConstraints>::SharedPtr exec_constraints_sub_;
    std::vector<rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr> odom_subs_;
    rclcpp::Publisher<visualization_msgs::msg::MarkerArray>::SharedPtr marker_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace mrs_sim::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    try
    {
        auto node = std::make_shared<mrs_sim::node::VizMarkersNode>();
        rclcpp::spin(node);
    }
    catch (const mrs_core::ContractViolation& cv)
    {
        RCLCPP_FATAL(rclcpp::get_logger("viz_markers_node"),
                     "startup contract violation: %s",
                     cv.what());
        rclcpp::shutdown();
        return 1;
    }
    rclcpp::shutdown();
    return 0;
}
