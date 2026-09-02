// SPDX-License-Identifier: Apache-2.0
//
// state_integrator — mrs_sim's odometry node (D16 스모크 최소 · 347§4-2:
// "state_integrator: cmd_vel 적분 → /odom_batch (세계 기하 모름)"). 이 노드는
// **물리 어댑터가 아니다** — 벽·자유 공간을 모르고, 명령을 그대로 적분만 한다
// (347§4-4가 물리 어댑터는 "줄인다"로 판정했고 D16이 이 노드까지만 남겼다).
//
// 🔴 [결정] 토픽 형태 — 347§4-2 mermaid는 단일 배치 토픽 `/odom_batch`를
// 그렸으나, 그 이름을 채울 mrs_msgs 타입이 계약에 없다(새 IDL은 이번 라운드
// 금지 — schema/ 무접촉). `mrs_sim`은 스키마 소유자가 아닌 도구 패키지이므로
// 표준 ROS 타입으로 로봇당 토픽을 쓴다 — 357§3-3이 관측 이음매(`/{robot}/scan`)
// 에 적용한 것과 같은 형태의 선택이다: `/{robot}/cmd_vel`(geometry_msgs/Twist,
// 구독) → 적분 → `/{robot}/odom`(nav_msgs/Odometry, 발행).
//
// 🔴 367_pipeline_inputs_p3.md — 순환 대기 처리 이력(둘째 라운드, 이 파일이
// 그 처리 자리다):
//   p2(1차 처리): 기동 시 로드맵 좌표로 초기 odom **1회**를 발행해 "cmd_vel
//   없으면 odom도 없다"는 옛 이벤트 구동 설계의 순환 대기를 열었다. 그러나
//   그 1회는 BEST_EFFORT/VOLATILE이라 **늦게 붙는 구독자(정확히 trajopt_node
//   가 그 자리)가 놓칠 수 있었다** — 실행으로 확인됨(p3 발주서, `/r2/odom`은
//   잡히고 `/r0`·`/r1`은 0건).
//   p3(이 처리): **odom을 주기적으로 낸다.** `cmd_vel`이 있으면 그 값으로
//   적분, 없으면 **현재 pose를 그대로** 재발행한다 — 실제 오도메트리가 명령
//   유무와 무관하게 늘 나오는 것과 같다. 주기는 `odom_publish_period_ms`
//   파라미터(기본값 50 — 제어 주기 50 ms, `multibot3/CLAUDE.md` "핵심 불변식
//   5"와 같은 값. **새 수치를 고르지 않았다** — 이미 있는 시스템 하드 불변식을
//   그대로 재사용했다. `mrs_bringup/config/sim.yaml`이 명시적 출처를 남긴다).
//
//   🔴 **QoS 대안 평가·판정(p3)**: `/odom`을 `TRANSIENT_LOCAL`(latched)로
//   바꾸는 대안을 검토했다 — **기각**. 오도메트리는 "지금 어디 있는가"에 대한
//   흐름이지 되돌릴 수 있는 상태 스냅샷이 아니다 — latched 구독자가 뒤늦게
//   받는 "마지막 값"은 그 사이 로봇이 움직였다면 **거짓**이 된다(래치가
//   지키는 것은 "최후의 값"이지 "지금 값"이 아니다). `10_architecture.md`§7도
//   `/odom_batch`·`/robot_states`를 명시적으로 BEST_EFFORT로 뒀다 — latched로
//   바꾸는 것은 아키텍처 정본과 어긋난다. **옳은 해법은 QoS를 바꾸는 것이
//   아니라 발행을 실제로 주기적으로 만드는 것**이었다(BEST_EFFORT는 애초에
//   "계속 흐르는 채널"을 전제한 프로파일이고, 이 노드가 그 전제를 어긴 것이
//   진짜 결함이었다) — 그래서 **아키텍처 정본을 고치자는 안건을
//   `system-architect`에게 올리지 않는다.** 정본은 이미 옳았다.
//
// 콜백 내 wall timer 0건 — clock_node만의 예외(§9)를 지킨다. 아래
// `odom_timer_`는 `create_timer`(노드 클록 — `use_sim_time=true`면 `/clock`을
// 따른다)이지 `create_wall_timer`가 **아니다** — `trajopt_node.cpp`의 50 ms
// 제어 타이머와 같은 형태(§9가 금지하는 것은 그 함수 이름이지 주기적 콜백
// 자체가 아니다).

#include <cmath>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "geometry_msgs/msg/twist.hpp"
#include "mrs_core/status.hpp"
#include "mrs_msgs/msg/roadmap.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"

namespace mrs_sim::node
{

/// @brief 로봇 한 대의 적분 상태 — 세계 좌표 (x, y, theta) + 마지막으로 받은 명령.
struct IntegratedPose
{
    double x = 0.0;
    double y = 0.0;
    double theta = 0.0;
    geometry_msgs::msg::Twist last_cmd{};  ///< 0으로 초기화 — 아직 명령 0건이면 정지.
    bool pose_known = false;               ///< 좌표를 지어내지 않고 확보했는가(§ 생성자·OnRoadmap).
};

/// @brief `/{robot}/cmd_vel`을 유니사이클 모델로 적분해 **주기적으로**
/// `/{robot}/odom`을 낸다(p3 — 실제 오도메트리는 명령 유무와 무관하게 늘
/// 나온다는 것을 반영, 파일 머리 주석 `367_pipeline_inputs_p3.md`).
class StateIntegrator : public rclcpp::Node
{
public:
    StateIntegrator() : rclcpp::Node("state_integrator")
    {
        const auto robots =
            this->declare_parameter<std::vector<std::string>>("robots", std::vector<std::string>{});
        // `robots`와 같은 순서로 대응 — `mrta.yaml`의 `initial_vertices`와 같은
        // 값(367_pipeline_inputs.md §3, prototype/out/assignment.json의
        // assignments[].start를 그대로 옮긴 것). 사슬이 길다는 것은 367_p2 §3이
        // 이미 신고했다 — 여기서 반복하지 않는다.
        const auto initial_vertices = this->declare_parameter<std::vector<std::string>>(
            "initial_vertices", std::vector<std::string>{});
        // 🔴 367_pipeline_inputs_p3.md §2 — 값을 지어내지 않았다: 제어 주기
        // 50 ms(multibot3/CLAUDE.md "핵심 불변식 5")를 그대로 재사용한 것이지
        // 새 수치를 고른 것이 아니다. `mrs_bringup/config/sim.yaml`이 명시적
        // 출처를 남긴다 — 그 파일이 없거나 이 키가 없으면 이 기본값(50)으로
        // 되돌아간다(clock_node.cpp의 `clock_step_ms` 기본값과 같은 패턴).
        const auto odom_publish_period_ms =
            this->declare_parameter<int>("odom_publish_period_ms", 50);

        rclcpp::QoS cmd_qos(rclcpp::KeepLast(1));
        cmd_qos.reliable();

        rclcpp::QoS odom_qos(rclcpp::KeepLast(1));
        odom_qos.best_effort();  // 10_architecture.md§7 "/odom_batch, /robot_states — BEST_EFFORT".
                                  // p3 파일 머리 주석 — 유지 판정, 바꾸지 않는다.

        if (!initial_vertices.empty() && initial_vertices.size() != robots.size())
        {
            // 기동 경로(CN-15) — 짝이 안 맞는 설정은 조용히 자르거나 순환하지 않는다.
            throw mrs_core::ContractViolation(
                "state_integrator: 'initial_vertices' size (" +
                std::to_string(initial_vertices.size()) + ") does not match 'robots' size (" +
                std::to_string(robots.size()) + ")");
        }
        if (odom_publish_period_ms <= 0)
        {
            throw mrs_core::ContractViolation(
                "state_integrator: 'odom_publish_period_ms' must be > 0");
        }

        for (std::size_t i = 0; i < robots.size(); ++i)
        {
            const auto& robot = robots[i];
            poses_[robot] = IntegratedPose{};
            if (!initial_vertices.empty())
            {
                initial_vertex_[robot] = initial_vertices[i];
            }
            else
            {
                // 초기 위치 정보 자체가 없다 — 이 노드의 원래 기본값(세계 원점)을
                // "이미 안다"로 표시한다. 새로 지어낸 좌표가 아니라 이 struct의
                // 원래 기본값(x=y=theta=0)을 그대로 쓰는 것이다(이 노드가 항상
                // 그래 왔다 — 367_p2 이전에도 poses_ 기본값은 원점이었다).
                poses_[robot].pose_known = true;
            }
            odom_pubs_[robot] =
                this->create_publisher<nav_msgs::msg::Odometry>("/" + robot + "/odom", odom_qos);
            cmd_subs_.push_back(this->create_subscription<geometry_msgs::msg::Twist>(
                "/" + robot + "/cmd_vel",
                cmd_qos,
                [this, robot](geometry_msgs::msg::Twist::ConstSharedPtr msg)
                {
                    this->OnCmdVel(robot, msg);
                }));
        }

        if (robots.empty())
        {
            RCLCPP_WARN(this->get_logger(),
                        "state_integrator: 'robots' parameter is empty — no cmd_vel/odom "
                        "topics created (no roster supplied by mrs_bringup/config).");
        }
        if (initial_vertices.empty())
        {
            RCLCPP_WARN(this->get_logger(),
                        "state_integrator: 'initial_vertices' not set — periodic /odom will "
                        "still publish (p3), but from the world-origin default rather than a "
                        "roadmap-derived start vertex.");
        }
        else
        {
            // 로드맵 정점 좌표 없이는 초기 pose를 지어낼 수 없다 — /roadmap을
            // 기다린다(latched — 늦게 떠도 마지막 값을 받는다. 이 판단은 로드맵이
            // run 내내 불변이라 유효하다, 139:215 — odom과는 다른 성격의 채널).
            rclcpp::QoS roadmap_qos(rclcpp::KeepLast(1));
            roadmap_qos.reliable().transient_local();
            roadmap_sub_ = this->create_subscription<mrs_msgs::msg::Roadmap>(
                "/roadmap",
                roadmap_qos,
                [this](mrs_msgs::msg::Roadmap::ConstSharedPtr msg)
                {
                    this->OnRoadmap(msg);
                });
        }

        // p3 핵심 변경 — 주기 발행. `create_timer`(노드 클록, sim-time 준수)를
        // 쓴다. `robots`가 비어 있으면 로봇별 항목이 없으니 타이머 콜백이
        // 그냥 아무 것도 안 한다(빈 루프) — 별도 분기 불필요.
        period_s_ = static_cast<double>(odom_publish_period_ms) / 1000.0;
        odom_timer_ = this->create_timer(std::chrono::milliseconds(odom_publish_period_ms),
                                         [this]()
                                         {
                                             this->OnTick();
                                         });
    }

private:
    /// @brief `/roadmap`이 도착하면 정점 좌표 테이블을 채우고, 아직 pose를 모르는
    /// 로봇마다(§ 생성자) 좌표를 확정한다. **발행은 여기서 하지 않는다** — 다음
    /// 주기 틱(`OnTick`)이 낸다(p3, 발행 경로를 하나로 모아 레이스를 없앤다).
    void OnRoadmap(mrs_msgs::msg::Roadmap::ConstSharedPtr msg)
    {
        node_coords_.clear();
        for (const auto& n : msg->nodes)
        {
            node_coords_[n.id] = std::make_pair(n.x, n.y);
        }
        for (const auto& kv : initial_vertex_)
        {
            const auto& robot = kv.first;
            auto& pose = poses_.at(robot);
            if (pose.pose_known)
            {
                continue;
            }
            const auto coord_it = node_coords_.find(kv.second);
            if (coord_it == node_coords_.end())
            {
                RCLCPP_ERROR(this->get_logger(),
                            "state_integrator: %s: initial_vertices='%s' not found in /roadmap "
                            "nodes — cannot seed initial pose (not fabricating a coordinate)",
                            robot.c_str(),
                            kv.second.c_str());
                continue;
            }
            pose.x = coord_it->second.first;
            pose.y = coord_it->second.second;
            pose.theta = 0.0;  // [근거 없음] — 초기 방위 정보가 이 라운드 어디에도 없다.
            pose.pose_known = true;
            RCLCPP_INFO(this->get_logger(),
                        "state_integrator: %s: resolved initial pose at vertex '%s' (%.3f, %.3f) "
                        "— periodic /odom starts on the next tick",
                        robot.c_str(),
                        kv.second.c_str(),
                        pose.x,
                        pose.y);
        }
    }

    /// @brief 한 로봇의 `cmd_vel` 수신을 **캐시만** 한다 — 적분·발행은 `OnTick`
    /// 하나로 모았다(p3). 두 콜백이 각자 발행하던 p2 이전 설계는 레이스의
    /// 씨앗이었다(파일 머리 주석).
    void OnCmdVel(const std::string& robot, geometry_msgs::msg::Twist::ConstSharedPtr msg)
    {
        poses_.at(robot).last_cmd = *msg;
    }

    /// @brief 주기 틱 — 로봇마다 고정 `dt = period_s_`로 마지막 명령을 적분하고
    /// `/odom`을 낸다. 명령이 한 번도 온 적이 없으면 `last_cmd`가 기본값(0)이라
    /// **현재 pose 그대로** 재발행된다 — 이것이 조정자가 요구한 "명령이 없으면
    /// 현재 pose 그대로"다. `pose_known`이 아직 `false`인 로봇(로드맵 미도착)은
    /// 이번 틱에 건너뛴다 — 좌표를 지어내지 않는다.
    void OnTick()
    {
        const rclcpp::Time now = this->get_clock()->now();
        for (auto& kv : poses_)
        {
            const auto& robot = kv.first;
            auto& pose = kv.second;
            if (!pose.pose_known)
            {
                continue;
            }

            pose.x += pose.last_cmd.linear.x * std::cos(pose.theta) * period_s_;
            pose.y += pose.last_cmd.linear.x * std::sin(pose.theta) * period_s_;
            pose.theta += pose.last_cmd.angular.z * period_s_;

            nav_msgs::msg::Odometry odom;
            odom.header.stamp = now;
            odom.header.frame_id = "map";
            odom.child_frame_id = robot + "/base_link";  // CN(10_architecture.md§9): 선행 슬래시 없음.
            odom.pose.pose.position.x = pose.x;
            odom.pose.pose.position.y = pose.y;
            odom.pose.pose.orientation.z = std::sin(pose.theta * 0.5);
            odom.pose.pose.orientation.w = std::cos(pose.theta * 0.5);
            odom.twist.twist = pose.last_cmd;
            odom_pubs_.at(robot)->publish(odom);
        }
    }

    std::map<std::string, IntegratedPose> poses_;
    std::map<std::string, std::string> initial_vertex_;      ///< robot -> initial roadmap vertex id.
    std::map<std::string, std::pair<double, double>> node_coords_;  ///< /roadmap node id -> (x, y).
    std::map<std::string, rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr> odom_pubs_;
    std::vector<rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr> cmd_subs_;
    rclcpp::Subscription<mrs_msgs::msg::Roadmap>::SharedPtr roadmap_sub_;
    rclcpp::TimerBase::SharedPtr odom_timer_;
    double period_s_ = 0.0;  ///< OnTick의 고정 dt — odom_publish_period_ms를 초로.
};

}  // namespace mrs_sim::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    try
    {
        auto node = std::make_shared<mrs_sim::node::StateIntegrator>();
        rclcpp::spin(node);
    }
    catch (const mrs_core::ContractViolation& cv)
    {
        // CN-15: startup-path violation -> exit 1, no fallback (F48-3's SIGABRT
        // lesson — an uncaught exception past rclcpp::init() aborts instead of
        // exiting 1, roadmap_publisher.cpp/clock_node.cpp already fixed this).
        RCLCPP_FATAL(rclcpp::get_logger("state_integrator"),
                     "startup contract violation: %s",
                     cv.what());
        rclcpp::shutdown();
        return 1;
    }
    rclcpp::shutdown();
    return 0;
}
