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
// 구독) → 적분 → `/{robot}/odom`(nav_msgs/Odometry, 발행). 이번 라운드
// 어떤 소비자도 이 토픽을 구독하지 않는다(trajopt_node의 제어 틱 루프가
// 미배선 — P4 자체가 그 배선의 전제 조건이라 이 라운드에 함께 오지 않는다).
//
// 콜백 내 wall timer 0건 — clock_node만의 예외(§9)를 지킨다. 이 노드는
// cmd_vel 수신 이벤트로만 적분한다(수신 0건이면 발행도 0건 — "닫힌 고리가
// 선다"는 D16의 요구를 실행 파일 존재로 충족하고, 실제 데이터 흐름은
// trajopt의 cmd_vel 발행이 서는 다음 라운드에 발화한다).

#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "geometry_msgs/msg/twist.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"

namespace mrs_sim::node
{

/// @brief 로봇 한 대의 적분 상태 — 세계 좌표 (x, y, theta).
struct IntegratedPose
{
    double x = 0.0;
    double y = 0.0;
    double theta = 0.0;
    rclcpp::Time last_stamp = rclcpp::Time(0, 0, RCL_ROS_TIME);
    bool has_last_stamp = false;
};

/// @brief `/{robot}/cmd_vel`을 유니사이클 모델로 적분해 `/{robot}/odom`을 낸다.
class StateIntegrator : public rclcpp::Node
{
public:
    StateIntegrator() : rclcpp::Node("state_integrator")
    {
        const auto robots =
            this->declare_parameter<std::vector<std::string>>("robots", std::vector<std::string>{});

        rclcpp::QoS cmd_qos(rclcpp::KeepLast(1));
        cmd_qos.reliable();

        rclcpp::QoS odom_qos(rclcpp::KeepLast(1));
        odom_qos.best_effort();  // 10_architecture.md§7 "/odom_batch, /robot_states — BEST_EFFORT".

        for (const auto& robot : robots)
        {
            poses_[robot] = IntegratedPose{};
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
    }

private:
    /// @brief 한 로봇의 `cmd_vel` 수신을 유니사이클로 적분해 그 로봇의 `odom`을 발행한다.
    void OnCmdVel(const std::string& robot, geometry_msgs::msg::Twist::ConstSharedPtr msg)
    {
        auto& pose = poses_.at(robot);
        const rclcpp::Time now = this->get_clock()->now();
        if (pose.has_last_stamp)
        {
            const double dt = (now - pose.last_stamp).seconds();
            if (dt > 0.0)
            {
                pose.x += msg->linear.x * std::cos(pose.theta) * dt;
                pose.y += msg->linear.x * std::sin(pose.theta) * dt;
                pose.theta += msg->angular.z * dt;
            }
        }
        pose.last_stamp = now;
        pose.has_last_stamp = true;

        nav_msgs::msg::Odometry odom;
        odom.header.stamp = now;
        odom.header.frame_id = "map";
        odom.child_frame_id = robot + "/base_link";  // CN(10_architecture.md§9): 선행 슬래시 없음.
        odom.pose.pose.position.x = pose.x;
        odom.pose.pose.position.y = pose.y;
        odom.pose.pose.orientation.z = std::sin(pose.theta * 0.5);
        odom.pose.pose.orientation.w = std::cos(pose.theta * 0.5);
        odom.twist.twist = *msg;
        odom_pubs_.at(robot)->publish(odom);
    }

    std::map<std::string, IntegratedPose> poses_;
    std::map<std::string, rclcpp::Publisher<nav_msgs::msg::Odometry>::SharedPtr> odom_pubs_;
    std::vector<rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr> cmd_subs_;
};

}  // namespace mrs_sim::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<mrs_sim::node::StateIntegrator>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
