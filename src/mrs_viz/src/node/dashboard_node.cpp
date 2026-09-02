// SPDX-License-Identifier: Apache-2.0
//
// dashboard_node — mrs_viz의 유일한 노드 (374_mrs_viz_dashboard.md, 48차 웨이브 3).
//
// 🔴 관찰 전용이다 — 이 파일에 지도 클릭·차단·해제 같은 조작 UI를 나르는 라우팅이
// 하나도 없다(GET만 서빙, http_server.hpp). 사유는 발주서 「왜 mrs_viz인가」 절과
// 같다: U46-3의 차단 관측 구간(`TickInput` 세계 관측 필드·`classify()` 프로덕션
// 호출·`exogenous_block` 발화)이 아직 없어, 지금 버튼을 만들면 "누를 데는 있는데
// 받는 데가 없다".
//
// 이미 있는 경계 토픽·표준 ROS 스트림을 구독해 브라우저가 폴링하는 `/api/state`
// JSON 한 벌로 재표현한다 — 모듈 코드 0줄(`viz_markers_node.cpp`와 같은 원칙).
// U45-5 관찰 4종(`_workspace/331`, `333_p2`§4) 판별을 그대로 따른다:
//   ① 위치·경로·튜브 — 위치는 `/{robot}/odom`(경계 밖, 시뮬 상태), 경로·튜브는
//      `/trajectories`(경계).
//   ② 서브골 열·재계획 사건 — 서브골은 trajopt 내부 T-C 단계라 진단 토픽이
//      없다(현재 발행자 0건, 실측 확인) → "no data" 신고만. 재계획은
//      `mrs_msgs/action/Replan`의 표준 상태 토픽 `/replan/_action/status`
//      (action_msgs/msg/GoalStatusArray, 액션 서버가 자동으로 낸다)를 구독해
//      **상태 전이만** 본다 — 요청/결과 payload는 이 채널로 못 본다(액션
//      클라이언트가 돼야 하는데 이 노드는 관찰자일 뿐이라 목표를 보내지 않는다).
//   ③ SADG 의존 그래프·스위칭 — `/execution_constraints`(경계) + 진행은
//      `/{robot}/segment_release`(경계). `/entry_events`는 현재 발행자가 없다
//      (실측 확인 — 이 라운드 밖의 상류 공백, mrs_viz 스텁이 아니다).
//   ④ 틱 예산·계측 게이지 — `/diagnostics`(CN-19)는 현재 어느 노드도 발행하지
//      않는다(실측 확인) → "no data" 신고. `result.json`은 `mrs_eval`이 아직
//      없어(레포에 패키지 0개) 파일이 없다 — `result_json_path`가 설정되고
//      파일이 있으면 그 내용을 그대로 통과시킬 뿐 재해석하지 않는다.
//
// D18("렌더러가 둘로 갈리지 않게"): `mrs_sim/viz_markers_node`와 같은 경계 토픽을
// 구독하지만 그 노드의 파생 로직(예: `{robot}#{index}`로 "그" 목표 세그먼트를
// 고르는 것)을 재구현하지 않는다 — 원 세그먼트·의존 목록을 그대로 보여준다
// (dashboard_state.hpp 파일 주석). 값→이름 사전(DependencyType·EdgeTraversal)은
// `mrs_core_msgs`(이미 있는 유일 소유자, `reason_codes.hpp`)를 그대로 쓴다 —
// 여기서 새로 만들지 않았다.
//
// `/clock`을 발행하지 않는다. `use_sim_time`은 다른 노드처럼 launch의
// `common_params`로 받는다. 색·크기·주기·포트는 코드에 박지 않고
// `mrs_bringup/config/viz_dashboard.yaml`에서 온다(CN-6 정신 — 이 패키지는
// CN-7 `method.modules.*` 대상은 아니지만 "값은 config에" 원칙을 지킨다).
//
// 패키지 레이아웃 판정(374§「패키지 레이아웃」): `mrs_sim`과 같은 형태 —
// `include/mrs_viz/` 공개 헤더를 두지 않는다. 알고리즘 교체 지점이 없는 도구
// 패키지에 CN-1 필수 넷(core/adapter/node/service)을 강제할 이유가 없다
// (`check_layer_layout.py`가 "미검사"로 신고할 뿐 위반이 아니다 — mrs_sim 선례,
// `check_layer_layout.py` 200행 주석).

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "action_msgs/msg/goal_status.hpp"
#include "action_msgs/msg/goal_status_array.hpp"
#include "dashboard_state.hpp"
#include "geometry_msgs/msg/quaternion.hpp"
#include "http_server.hpp"
#include "mrs_core/status.hpp"
#include "mrs_core_msgs/reason_codes.hpp"
#include "mrs_msgs/msg/execution_constraints.hpp"
#include "mrs_msgs/msg/roadmap.hpp"
#include "mrs_msgs/msg/segment_release.hpp"
#include "mrs_msgs/msg/trajectories.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "nlohmann/json.hpp"
#include "rclcpp/rclcpp.hpp"

namespace mrs_viz::node
{

namespace
{

/// @brief 파일 전체를 문자열로 읽는다.
/// @param path 읽을 파일 경로.
/// @throws mrs_core::ContractViolation 못 열면(기동 경로 — CN-15).
std::string ReadFileOrThrow(const std::string& path)
{
    std::ifstream f(path, std::ios::binary);
    if (!f)
    {
        throw mrs_core::ContractViolation("dashboard_node: cannot open required asset: " + path);
    }
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

/// @brief `action_msgs::msg::GoalStatus::status`를 사람이 읽을 이름으로.
/// action_msgs는 `mrs_core_msgs`의 소유가 아니다(그 표는 mrs_msgs 어휘 전용) —
/// 표준 ROS 2 액션 어휘라 로컬 상수를 그대로 쓴다.
std::string GoalStatusName(std::int8_t status)
{
    switch (status)
    {
        case action_msgs::msg::GoalStatus::STATUS_ACCEPTED:
            return "accepted";
        case action_msgs::msg::GoalStatus::STATUS_EXECUTING:
            return "executing";
        case action_msgs::msg::GoalStatus::STATUS_CANCELING:
            return "canceling";
        case action_msgs::msg::GoalStatus::STATUS_SUCCEEDED:
            return "succeeded";
        case action_msgs::msg::GoalStatus::STATUS_CANCELED:
            return "canceled";
        case action_msgs::msg::GoalStatus::STATUS_ABORTED:
            return "aborted";
        default:
            return "unknown";
    }
}

/// @brief 16바이트 UUID의 앞 4바이트를 hex 8자로.
std::string ShortGoalId(const std::array<std::uint8_t, 16>& uuid)
{
    char buf[9];
    std::snprintf(buf, sizeof(buf), "%02x%02x%02x%02x", uuid[0], uuid[1], uuid[2], uuid[3]);
    return std::string(buf);
}

}  // namespace

/// @brief `/roadmap`·`/{robot}/odom`·`/execution_constraints`·`/trajectories`·
/// `/{robot}/segment_release`·`/replan/_action/status`를 구독해 `DashboardState`를
/// 주기 재계산하고, 별도 스레드의 `HttpServer`가 그 최신 JSON을 폴링에 응답한다.
class DashboardNode : public rclcpp::Node
{
public:
    /// @brief 파라미터를 읽고, 웹 자산을 메모리에 올리고, 구독을 걸고, HTTP 서버를
    /// 띄우고, 주기 스냅샷 타이머를 켠다.
    /// @throws mrs_core::ContractViolation 필수 파라미터 부재·자산 로드 실패·HTTP
    ///         서버 기동 실패 시(CN-15 — 전부 기동 경로).
    DashboardNode() : rclcpp::Node("dashboard_node")
    {
        robots_ =
            this->declare_parameter<std::vector<std::string>>("robots", std::vector<std::string>{});
        frame_id_ = this->declare_parameter<std::string>("frame_id", "map");
        const auto host = this->declare_parameter<std::string>("host", "0.0.0.0");
        const auto port = this->declare_parameter<int>("http_port", 8090);
        const auto web_root = this->declare_parameter<std::string>("web_root", "");
        const auto snapshot_period_ms = this->declare_parameter<int>("snapshot_period_ms", 300);
        const auto poll_period_ms = this->declare_parameter<int>("poll_period_ms", 300);
        max_traj_points_ = static_cast<std::size_t>(
            this->declare_parameter<int>("max_trajectory_points_per_robot", 200));
        max_dep_edges_ =
            static_cast<std::size_t>(this->declare_parameter<int>("max_dependency_edges", 200));
        result_json_path_ = this->declare_parameter<std::string>("result_json_path", "");

        if (web_root.empty())
        {
            throw mrs_core::ContractViolation(
                "dashboard_node: 'web_root' is required (launch must set it to "
                "ament_index/mrs_viz/web) — refusing to start with no assets to serve.");
        }
        if (port <= 0 || port > 65535)
        {
            throw mrs_core::ContractViolation("dashboard_node: 'http_port' must be in (0, 65535]");
        }
        if (snapshot_period_ms <= 0)
        {
            throw mrs_core::ContractViolation("dashboard_node: 'snapshot_period_ms' must be > 0");
        }

        index_html_ = ReadFileOrThrow(web_root + "/index.html");
        app_js_ = ReadFileOrThrow(web_root + "/app.js");
        style_css_ = ReadFileOrThrow(web_root + "/style.css");

        config_json_ = nlohmann::json{
            {"poll_period_ms", poll_period_ms},
            {"frame_id", frame_id_},
            {"robots",
             robots_}}.dump();

        SetupSubscriptions();

        {
            std::lock_guard<std::mutex> lock(state_mutex_);
            latest_state_json_ =
                nlohmann::json{{"stamp_sec", 0.0}, {"note", "waiting for first tick"}}.dump();
        }

        std::string http_error;
        http_server_ = std::make_unique<HttpServer>(host,
                                                    port,
                                                    [this](const HttpRequest& req)
                                                    {
                                                        return this->HandleHttpRequest(req);
                                                    });
        if (!http_server_->Start(http_error))
        {
            throw mrs_core::ContractViolation("dashboard_node: HTTP server failed to start — " +
                                              http_error);
        }
        RCLCPP_INFO(this->get_logger(),
                    "dashboard_node: serving on http://%s:%d (web_root=%s)",
                    host.c_str(),
                    static_cast<int>(port),
                    web_root.c_str());

        snapshot_timer_ = this->create_timer(std::chrono::milliseconds(snapshot_period_ms),
                                             [this]()
                                             {
                                                 this->OnSnapshotTick();
                                             });
    }

    /// @brief HTTP 서버의 accept 스레드를 멈추고 소켓을 닫는다.
    ~DashboardNode() override
    {
        if (http_server_)
        {
            http_server_->Stop();
        }
    }

private:
    /// @brief 경계 토픽·표준 ROS 스트림 구독을 전부 건다(파일 머리말 ①~④ 매핑).
    void SetupSubscriptions()
    {
        rclcpp::QoS latched_qos(rclcpp::KeepLast(1));
        latched_qos.reliable().transient_local();

        rclcpp::QoS odom_qos(rclcpp::KeepLast(1));
        odom_qos.best_effort();  // state_integrator.cpp 발행측과 일치.

        rclcpp::QoS action_status_qos(rclcpp::KeepLast(1));
        action_status_qos.reliable().transient_local();  // rcl_action_qos_profile_status_default.

        roadmap_sub_ = this->create_subscription<mrs_msgs::msg::Roadmap>(
            "/roadmap",
            latched_qos,
            [this](mrs_msgs::msg::Roadmap::ConstSharedPtr msg)
            {
                std::lock_guard<std::mutex> lock(cache_mutex_);
                last_roadmap_ = msg;
            });

        exec_constraints_sub_ = this->create_subscription<mrs_msgs::msg::ExecutionConstraints>(
            "/execution_constraints",
            latched_qos,
            [this](mrs_msgs::msg::ExecutionConstraints::ConstSharedPtr msg)
            {
                std::lock_guard<std::mutex> lock(cache_mutex_);
                last_exec_constraints_ = msg;
            });

        traj_sub_ = this->create_subscription<mrs_msgs::msg::Trajectories>(
            "/trajectories",
            latched_qos,
            [this](mrs_msgs::msg::Trajectories::ConstSharedPtr msg)
            {
                std::lock_guard<std::mutex> lock(cache_mutex_);
                last_trajectories_ = msg;
            });

        replan_status_sub_ = this->create_subscription<action_msgs::msg::GoalStatusArray>(
            "/replan/_action/status",
            action_status_qos,
            [this](action_msgs::msg::GoalStatusArray::ConstSharedPtr msg)
            {
                std::lock_guard<std::mutex> lock(cache_mutex_);
                last_replan_status_ = msg;
            });

        odom_subs_.reserve(robots_.size());
        segment_release_subs_.reserve(robots_.size());
        for (const auto& robot : robots_)
        {
            odom_subs_.push_back(this->create_subscription<nav_msgs::msg::Odometry>(
                "/" + robot + "/odom",
                odom_qos,
                [this, robot](nav_msgs::msg::Odometry::ConstSharedPtr msg)
                {
                    std::lock_guard<std::mutex> lock(cache_mutex_);
                    latest_odom_[robot] = *msg;
                    has_odom_[robot] = true;
                }));
            segment_release_subs_.push_back(
                this->create_subscription<mrs_msgs::msg::SegmentRelease>(
                    "/" + robot + "/segment_release",
                    latched_qos,
                    [this, robot](mrs_msgs::msg::SegmentRelease::ConstSharedPtr msg)
                    {
                        std::lock_guard<std::mutex> lock(cache_mutex_);
                        last_segment_release_seq_[robot] = msg->release_seq;
                    }));
        }

        if (robots_.empty())
        {
            RCLCPP_WARN(this->get_logger(),
                        "dashboard_node: 'robots' parameter is empty — no odom/segment_release "
                        "subscriptions, robot panel will stay empty.");
        }
    }

    /// @brief 캐시된 최신 메시지들로 `DashboardState`를 새로 짓고 JSON으로
    /// 직렬화해 `latest_state_json_`을 갱신한다(값을 지어내지 않는다 — 아직
    /// 못 받은 토픽은 `have_*=false`로 남는다).
    void OnSnapshotTick()
    {
        DashboardState state;
        state.stamp_sec = this->get_clock()->now().seconds();
        state.frame_id = frame_id_;
        state.robot_roster = robots_;

        std::lock_guard<std::mutex> lock(cache_mutex_);

        if (last_roadmap_ != nullptr)
        {
            state.have_roadmap = true;
            std::map<std::string, bool> endpoint_lookup;
            for (const auto& e : last_roadmap_->endpoints)
            {
                endpoint_lookup[e] = true;
            }
            for (const auto& n : last_roadmap_->nodes)
            {
                RoadmapNodeLite node_lite;
                node_lite.id = n.id;
                node_lite.x = n.x;
                node_lite.y = n.y;
                node_lite.is_endpoint = endpoint_lookup.count(n.id) > 0;
                state.roadmap_nodes.push_back(node_lite);
            }
            for (const auto& e : last_roadmap_->edges)
            {
                state.roadmap_edges.push_back(RoadmapEdgeLite{e.from_id, e.to_id});
            }
        }

        for (const auto& robot : robots_)
        {
            RobotPoseState pose;
            if (has_odom_.count(robot) != 0 && has_odom_.at(robot))
            {
                const auto& odom = latest_odom_.at(robot);
                pose.has_odom = true;
                pose.x = odom.pose.pose.position.x;
                pose.y = odom.pose.pose.position.y;
                pose.yaw = YawFromQuaternion(odom.pose.pose.orientation);
            }
            state.robots[robot] = pose;
        }
        state.segment_release_seq = last_segment_release_seq_;

        if (last_trajectories_ != nullptr)
        {
            state.have_trajectories = true;
            state.trajectories_instance_id = last_trajectories_->instance_id;
            state.min_separation_m = last_trajectories_->limits.min_separation_m;
            for (const auto& rt : last_trajectories_->trajectories)
            {
                RobotTrajectoryLite lite;
                lite.robot = rt.robot;
                lite.points.reserve(rt.points.size());
                for (const auto& p : rt.points)
                {
                    lite.points.push_back(TrajectoryPointLite{p.x, p.y});
                }
                state.trajectories.push_back(std::move(lite));
            }
        }

        if (last_exec_constraints_ != nullptr)
        {
            state.have_exec_constraints = true;
            state.commit_seq = last_exec_constraints_->commit_seq;
            state.n_segments = last_exec_constraints_->segments.size();
            state.n_dependencies_total = last_exec_constraints_->dependencies.size();
            for (const auto& d : last_exec_constraints_->dependencies)
            {
                if (state.dependencies.size() >= max_dep_edges_)
                {
                    break;
                }
                DependencyLite lite;
                lite.from_id = d.from_id;
                lite.to_id = d.to_id;
                const auto name = mrs_core_msgs::DependencyTypeToString(d.type);
                lite.type_name = name.ok() ? name.value() : "unknown";
                state.dependencies.push_back(std::move(lite));
            }
            for (const auto& g : last_exec_constraints_->switch_groups)
            {
                state.switch_groups.push_back(
                    SwitchGroupLite{g.id, g.location, g.alternatives.size()});
            }
        }

        if (last_replan_status_ != nullptr)
        {
            state.have_replan_status = true;
            for (const auto& gs : last_replan_status_->status_list)
            {
                if (state.replan_goals.size() >= max_dep_edges_)
                {
                    break;
                }
                std::array<std::uint8_t, 16> uuid{};
                std::copy(gs.goal_info.goal_id.uuid.begin(),
                          gs.goal_info.goal_id.uuid.end(),
                          uuid.begin());
                state.replan_goals.push_back(
                    ReplanGoalStatusLite{ShortGoalId(uuid), GoalStatusName(gs.status)});
            }
        }

        if (!result_json_path_.empty())
        {
            std::ifstream f(result_json_path_);
            if (f)
            {
                try
                {
                    nlohmann::json parsed;
                    f >> parsed;
                    state.have_result_summary = true;
                    state.result_summary_raw = std::move(parsed);
                }
                catch (const nlohmann::json::exception&)
                {
                    // 파싱 실패 — 지어내지 않는다. have_result_summary는 false로 남는다.
                }
            }
        }

        const auto json_body = ToJson(state, max_traj_points_, max_dep_edges_).dump();
        std::lock_guard<std::mutex> state_lock(state_mutex_);
        latest_state_json_ = json_body;
    }

    /// @brief 평면(2D) 요(yaw)만 필요하므로 z축 성분만 뽑는다 — roll/pitch는
    /// 버린다(시뮬레이터가 평면 로봇만 다룬다, `state_integrator.cpp`와 같은 전제).
    /// @param q `/{robot}/odom`의 orientation 쿼터니언.
    /// @return 라디안 요(yaw), `atan2` 치역([-π, π]).
    static double YawFromQuaternion(const geometry_msgs::msg::Quaternion& q)
    {
        const double siny_cosp = 2.0 * (q.w * q.z + q.x * q.y);
        const double cosy_cosp = 1.0 - 2.0 * (q.y * q.y + q.z * q.z);
        return std::atan2(siny_cosp, cosy_cosp);
    }

    /// @brief 정적 자산(`/`, `/app.js`, `/style.css`) + `/api/config` + `/api/state`
    /// 다섯 경로만 서빙한다(GET만 오는 것은 `http_server.hpp`가 이미 걸렀다).
    /// @param req 파싱된 요청.
    /// @return 200(자산/상태) 또는 404(그 밖의 경로) 응답.
    HttpResponse HandleHttpRequest(const HttpRequest& req)
    {
        HttpResponse resp;
        if (req.path == "/" || req.path == "/index.html")
        {
            resp.content_type = "text/html; charset=utf-8";
            resp.body = index_html_;
        }
        else if (req.path == "/app.js")
        {
            resp.content_type = "application/javascript; charset=utf-8";
            resp.body = app_js_;
        }
        else if (req.path == "/style.css")
        {
            resp.content_type = "text/css; charset=utf-8";
            resp.body = style_css_;
        }
        else if (req.path == "/api/config")
        {
            resp.content_type = "application/json; charset=utf-8";
            resp.body = config_json_;
        }
        else if (req.path == "/api/state")
        {
            resp.content_type = "application/json; charset=utf-8";
            std::lock_guard<std::mutex> lock(state_mutex_);
            resp.body = latest_state_json_;
        }
        else
        {
            resp.status = 404;
            resp.status_text = "Not Found";
            resp.content_type = "text/plain; charset=utf-8";
            resp.body = "not found: " + req.path;
        }
        return resp;
    }

    std::string frame_id_;
    std::vector<std::string> robots_;
    std::size_t max_traj_points_ = 200;
    std::size_t max_dep_edges_ = 200;
    std::string result_json_path_;

    std::string index_html_;
    std::string app_js_;
    std::string style_css_;
    std::string config_json_;

    std::mutex cache_mutex_;
    mrs_msgs::msg::Roadmap::ConstSharedPtr last_roadmap_;
    mrs_msgs::msg::ExecutionConstraints::ConstSharedPtr last_exec_constraints_;
    mrs_msgs::msg::Trajectories::ConstSharedPtr last_trajectories_;
    action_msgs::msg::GoalStatusArray::ConstSharedPtr last_replan_status_;
    std::map<std::string, nav_msgs::msg::Odometry> latest_odom_;
    std::map<std::string, bool> has_odom_;
    std::map<std::string, std::uint64_t> last_segment_release_seq_;

    std::mutex state_mutex_;
    std::string latest_state_json_;

    rclcpp::Subscription<mrs_msgs::msg::Roadmap>::SharedPtr roadmap_sub_;
    rclcpp::Subscription<mrs_msgs::msg::ExecutionConstraints>::SharedPtr exec_constraints_sub_;
    rclcpp::Subscription<mrs_msgs::msg::Trajectories>::SharedPtr traj_sub_;
    rclcpp::Subscription<action_msgs::msg::GoalStatusArray>::SharedPtr replan_status_sub_;
    std::vector<rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr> odom_subs_;
    std::vector<rclcpp::Subscription<mrs_msgs::msg::SegmentRelease>::SharedPtr>
        segment_release_subs_;
    rclcpp::TimerBase::SharedPtr snapshot_timer_;

    std::unique_ptr<HttpServer> http_server_;
};

}  // namespace mrs_viz::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    try
    {
        auto node = std::make_shared<mrs_viz::node::DashboardNode>();
        rclcpp::spin(node);
    }
    catch (const mrs_core::ContractViolation& cv)
    {
        RCLCPP_FATAL(rclcpp::get_logger("dashboard_node"),
                     "startup contract violation: %s",
                     cv.what());
        rclcpp::shutdown();
        return 1;
    }
    rclcpp::shutdown();
    return 0;
}
