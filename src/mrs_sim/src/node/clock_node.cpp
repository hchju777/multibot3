// SPDX-License-Identifier: Apache-2.0
//
// clock_node — mrs_sim's sole /clock publisher (10_architecture.md S10 ·
// D16 스모크 최소 · 357§7-3 프로세스 sim).
//
// 🔴 이 노드는 파이프라인 전체에서 유일하게 wall timer를 쓰는 것이 합법인 자리다
// (357§7-3: "[추정] wall timer의 유일한 합법 사용자 — §9의 「create_wall_timer
// 금지」에 이 예외가 명시된 적이 없다. 확인 요구"). 시뮬 시간 자체를 무언가가
// 벽시계로 밀어야 하고, 그 무언가가 바로 이 노드다 — 다른 모든 노드는
// `use_sim_time: true`로 이 노드가 낸 /clock을 따른다.
//
// 모델: 벽시계 틱 1회당 시뮬 시간을 고정 스텝만큼 전진시킨다. 스텝 값을
// control_period_ms(50, CN-16 하드)와 같게 두어 **벽시계:시뮬 1:1** — 배수를
// 지어내지 않는 가장 단순한 선택이다. 이것은 시뮬레이터 엔지니어링 선택이지
// 알고리즘/물리 파라미터가 아니므로 CN-6(mrs_bringup/config) 대상이 아니다.
//
// QoS: /clock은 10_architecture.md§7 "BEST_EFFORT/VOLATILE/KEEP_LAST 1"과
// 정확히 같은 rclcpp::ClockQoS()를 쓴다.

#include <cstdint>
#include <memory>

#include "mrs_core/status.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rosgraph_msgs/msg/clock.hpp"

namespace mrs_sim::node
{

/// @brief `/clock`의 유일한 발행자. 자기 자신은 `use_sim_time`을 쓰지 않는다
/// (자기가 낸 시간을 자기가 구독하는 순환을 피한다 — 런치에서 이 노드만
/// `use_sim_time:=false`로 오버라이드한다).
class ClockNode : public rclcpp::Node
{
public:
    ClockNode() : rclcpp::Node("clock_node")
    {
        const auto step_ms = this->declare_parameter<int>("clock_step_ms", 50);
        if (step_ms <= 0)
        {
            // CN-15: 계약·불변식 위반은 ContractViolation — main()이 잡아 종료 1, 폴백 없음.
            // 🔴 F48-3(코드 게이트): std::invalid_argument였던 것을 이걸로 바꿨다 — 그것은
            // main()의 catch(ContractViolation)를 빠져나가 rclcpp::init() 이후 미포착
            // 예외로 SIGABRT(실측 종료 250)를 냈다. roadmap_publisher.cpp가 이미 지키는
            // 형태를 따른다.
            throw mrs_core::ContractViolation("clock_node: clock_step_ms must be > 0");
        }
        step_ns_ = static_cast<std::int64_t>(step_ms) * 1000000LL;

        clock_pub_ =
            this->create_publisher<rosgraph_msgs::msg::Clock>("/clock", rclcpp::ClockQoS());

        // 🔴 이 노드만의 예외 — 다른 모든 노드는 wall timer 금지(§9).
        timer_ = this->create_wall_timer(std::chrono::milliseconds(step_ms),
                                         [this]()
                                         {
                                             this->OnTick();
                                         });
    }

private:
    /// @brief 시뮬 시간을 한 스텝 전진시키고 `/clock`에 발행한다.
    void OnTick()
    {
        sim_ns_ += step_ns_;
        rosgraph_msgs::msg::Clock msg;
        msg.clock.sec = static_cast<std::int32_t>(sim_ns_ / 1000000000LL);
        msg.clock.nanosec = static_cast<std::uint32_t>(sim_ns_ % 1000000000LL);
        clock_pub_->publish(msg);
    }

    std::int64_t step_ns_ = 0;
    std::int64_t sim_ns_ = 0;
    rclcpp::Publisher<rosgraph_msgs::msg::Clock>::SharedPtr clock_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace mrs_sim::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    try
    {
        auto node = std::make_shared<mrs_sim::node::ClockNode>();
        rclcpp::spin(node);
    }
    catch (const mrs_core::ContractViolation& cv)
    {
        RCLCPP_FATAL(rclcpp::get_logger("clock_node"), "startup contract violation: %s", cv.what());
        rclcpp::shutdown();
        return 1;
    }
    rclcpp::shutdown();
    return 0;
}
