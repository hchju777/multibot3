// SPDX-License-Identifier: Apache-2.0
//
// task_release_publisher — /task_release producer (367_pipeline_inputs.md
// 웨이브 1-A ②).
//
// 🔴 소유 패키지 판정 = mrs_sim, node/'s own call (366§ "선결 미결" 표가
// "담당이 판정하고 사유를 적어라"로 명시적으로 넘긴 결정). D29(roadmap_
// publisher = mrs_sim 소유, 파일 -> latched 발행)를 **선례로 따랐다** — 같은
// 구조(파일을 읽어 경계 토픽으로 재발행하는 sim 프로세스 노드), 같은 사유
// (`mrs_bringup` 소스 0줄 규율을 이 라운드도 지킨다). 다른 점 하나: /roadmap
// 은 latched 1회이지만 /task_release는 스트림(qos.yaml reliable_bounded,
// RELIABLE/VOLATILE/depth 10) — 이 노드는 latched 재발행이 아니라 시간에
// 걸친 발행을 한다. system-architect 확인 요망(위 판정은 통합 담당의 임시
// 배치이지 아키텍처 결정이 아니다) — 367_pipeline_inputs.md에 신고했다.
//
// 이 노드도 시나리오를 만들지 않는다 — 정본 산출물(prototype/out/
// assignment.json)에서 역산한 파일(mrs_bringup/config/scenario/
// task_release.json, README "task_release.json" 절)을 그대로 재발행할
// 뿐이다.
//
// 타이밍: 다른 모든 노드처럼 wall timer 금지(pipeline.launch.py 파일 주석 ·
// clock_node.cpp 파일 주석). `create_timer`(Node clock — use_sim_time=true면
// /clock을 따른다, create_wall_timer와 다르다)를 쓴다. 매 틱마다 구독자
// 존재를 확인하고, 존재하면 큐에서 하나씩 꺼내 발행한다 — /task_release가
// VOLATILE이라(TRANSIENT_LOCAL이 아니다) 구독자가 아직 없을 때 발행하면
// 그 릴리스는 영구히 사라진다(계약이 재전송을 보장하지 않는다). 릴리스
// 사이 간격 자체는 시나리오 값이 아니라 이 노드의 엔지니어링 선택이다
// (clock_node.cpp의 step_ms 선택과 같은 성격 — 알고리즘·물리 파라미터가
// 아니므로 CN-6 대상이 아니다).

#include <cstddef>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "mrs_core/status.hpp"
#include "mrs_msgs/msg/task_release.hpp"
#include "rclcpp/rclcpp.hpp"
#include "task_release_json.hpp"  // 같은 디렉터리(src/node/) — 공개 헤더가 아니다

// 🔴 계약 봉투 상수(roadmap_publisher.cpp:36-45와 같은 규율) —
// task_release.schema.json의 const에서 그대로 옮겼다.
namespace
{
constexpr const char* kTaskReleaseSchema = "mrs.task_release";
constexpr const char* kTaskReleaseSchemaVersion = "2.0.0";
}  // namespace

namespace mrs_sim::node
{

/// @brief 파일 전체를 문자열로 읽는다. 실패 시 ContractViolation(CN-15, 기동 경로).
std::string ReadFileOrThrow(const std::string& path)
{
    std::ifstream in(path);
    if (!in.is_open())
    {
        throw mrs_core::ContractViolation("task_release_publisher: cannot open "
                                          "task_release_path file: " +
                                          path);
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

/// @brief `/task_release`를 시간에 걸쳐 발행한다 — 소스 배열(README "task_release.json")
/// 순서대로, 구독자가 붙은 뒤부터 한 틱에 하나씩.
class TaskReleasePublisher : public rclcpp::Node
{
public:
    TaskReleasePublisher() : rclcpp::Node("task_release_publisher")
    {
        const auto task_release_path = this->declare_parameter<std::string>("task_release_path", "");
        const auto release_check_period_ms =
            this->declare_parameter<int>("release_check_period_ms", 100);

        // qos.yaml "reliable_bounded" — RELIABLE / VOLATILE / KEEP_LAST 10
        // ([실물] mrta_node.cpp:129-135이 구독 측 정본).
        rclcpp::QoS release_qos(rclcpp::KeepLast(10));
        release_qos.reliable();
        release_pub_ =
            this->create_publisher<mrs_msgs::msg::TaskRelease>("/task_release", release_qos);

        if (task_release_path.empty())
        {
            // robot_specs_path와 같은 형태(U48-5가 세운 규율 — 값 없으면 발행하지
            // 않고 WARN만) — 이 노드는 latched가 아니라 optional 데이터원이라
            // roadmap_path(필수, 없으면 기동 거부)와는 다른 자리에 둔다: task_release
            // 없이도 나머지 파이프라인(로드맵·로봇 사양)은 유효하게 설 수 있다.
            RCLCPP_WARN(this->get_logger(),
                        "task_release_publisher: 'task_release_path' not set — /task_release "
                        "will NOT be published this run. mrta_node will receive /roadmap but "
                        "no task stream (367_pipeline_inputs.md).");
            return;
        }

        const auto text = ReadFileOrThrow(task_release_path);
        pending_ = ParseTaskReleaseArray(text, kTaskReleaseSchema, kTaskReleaseSchemaVersion);
        RCLCPP_INFO(this->get_logger(),
                    "task_release_publisher: loaded %zu release(s) from %s",
                    pending_.size(),
                    task_release_path.c_str());

        if (pending_.empty())
        {
            RCLCPP_WARN(this->get_logger(),
                        "task_release_publisher: task_release_path parsed to 0 releases — "
                        "nothing to publish");
            return;
        }

        if (release_check_period_ms <= 0)
        {
            throw mrs_core::ContractViolation(
                "task_release_publisher: release_check_period_ms must be > 0");
        }
        timer_ = this->create_timer(std::chrono::milliseconds(release_check_period_ms),
                                    [this]()
                                    {
                                        this->OnTick();
                                    });
    }

private:
    /// @brief 매 틱: 구독자가 있으면 큐 머리를 발행한다. 큐가 비면 타이머를 멈춘다.
    void OnTick()
    {
        if (cursor_ >= pending_.size())
        {
            timer_->cancel();
            return;
        }
        if (release_pub_->get_subscription_count() == 0)
        {
            return;  // 다음 틱에 다시 확인 — VOLATILE이라 지금 쏘면 유실이다.
        }
        release_pub_->publish(pending_[cursor_]);
        RCLCPP_INFO(this->get_logger(),
                    "task_release_publisher: published release_index=%lu task=%s",
                    static_cast<unsigned long>(pending_[cursor_].release_index),
                    pending_[cursor_].task.c_str());
        ++cursor_;
        if (cursor_ >= pending_.size())
        {
            timer_->cancel();
        }
    }

    rclcpp::Publisher<mrs_msgs::msg::TaskRelease>::SharedPtr release_pub_;
    rclcpp::TimerBase::SharedPtr timer_;
    std::vector<mrs_msgs::msg::TaskRelease> pending_;
    std::size_t cursor_ = 0;
};

}  // namespace mrs_sim::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    try
    {
        auto node = std::make_shared<mrs_sim::node::TaskReleasePublisher>();
        rclcpp::spin(node);
    }
    catch (const mrs_core::ContractViolation& cv)
    {
        RCLCPP_FATAL(rclcpp::get_logger("task_release_publisher"),
                     "startup contract violation: %s",
                     cv.what());
        rclcpp::shutdown();
        return 1;
    }
    rclcpp::shutdown();
    return 0;
}
