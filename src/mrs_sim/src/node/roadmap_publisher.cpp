// SPDX-License-Identifier: Apache-2.0
//
// roadmap_publisher — /roadmap · /robot_specs latched publisher.
//
// 🔴 소유 패키지 = mrs_sim, NOT mrs_bringup (D29, 357§7-2 갈래(나)). `A10`의
// 문언("bringup이 띄우는 전용 발행 노드")을 문언대로 읽어 "띄우기"와 "소유"를
// 가른다 — mrs_bringup은 launch에서 이 실행 파일을 실행할 뿐, 소스는 여기
// 하나뿐이다("mrs_bringup 소스 0줄" 불변).
//
// 이 노드는 시나리오를 만들지 않는다 — 정본에 있는 파일(347§4-4: "오늘
// 생성기는 prototype/gen_roadmap.py이고 산출물 prototype/out/roadmap.json이
// 이미 있다")을 그대로 읽어 mrs.roadmap으로 재발행할 뿐이다. 값 하나 지어내지
// 않는다.
//
// 🔴 robot_specs 소스 데이터는 이 라운드에 없다(prototype/out에 robot_specs
// 생성기·산출물이 존재하지 않는다 — grep 확인). 물리 로봇 사양(v_max·
// circumradius_m 등)을 여기서 지어내면 그 값이 mapf/trajopt의 실질 판정에
// 들어가고, 어떤 정본도 뒷받침하지 않는 숫자가 실험 경로에 섞인다. 그래서
// `robot_specs_path`는 **선택**(빈 문자열 기본값)으로 두고, 비어 있으면
// `/robot_specs`를 발행하지 않고 그 사실만 WARN으로 남긴다 — `/roadmap`은
// 그것과 무관하게 정상 발행한다(부분 진행을 죽이지 않는다).

#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

#include "mrs_core/status.hpp"
#include "mrs_msgs/msg/roadmap.hpp"
#include "mrs_msgs/msg/robot_specs.hpp"
#include "nlohmann/json.hpp"
#include "rclcpp/rclcpp.hpp"
#include "roadmap_json.hpp"  // 같은 디렉터리(src/node/) — 공개 헤더가 아니다, §헤더 파일 주석

// 🔴 계약 봉투 상수(48차 코드 게이트 F48-1) — roadmap.schema.json/robot_specs.schema.json의
// `const`에서 그대로 옮겼다. 여기가 유일한 정의 자리이고, 값이 바뀌면 컴파일 에러가 아니라
// 런타임에 ContractViolation으로 드러난다 — 계약이 개정되면 이 두 줄만 고친다.
namespace
{
constexpr const char* kRoadmapSchema = "mrs.roadmap";
constexpr const char* kRoadmapSchemaVersion = "3.0.0";
constexpr const char* kRobotSpecsSchema = "mrs.robot_specs";
constexpr const char* kRobotSpecsSchemaVersion = "1.1.0";
}  // namespace

namespace mrs_sim::node
{

namespace
{

/// @brief 파일 전체를 문자열로 읽는다. 실패 시 ContractViolation(CN-15, 기동 경로).
std::string ReadFileOrThrow(const std::string& path, const char* purpose)
{
    std::ifstream in(path);
    if (!in.is_open())
    {
        throw mrs_core::ContractViolation("roadmap_publisher: cannot open " + std::string(purpose) +
                                          " file: " + path);
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

}  // namespace

/// @brief `/roadmap`·`/robot_specs`를 latched로 1회 발행한다(D29).
class RoadmapPublisher : public rclcpp::Node
{
public:
    RoadmapPublisher() : rclcpp::Node("roadmap_publisher")
    {
        const auto roadmap_path = this->declare_parameter<std::string>("roadmap_path", "");
        const auto robot_specs_path = this->declare_parameter<std::string>("robot_specs_path", "");

        if (roadmap_path.empty())
        {
            // X8과 같은 형태 — 필수 키, 기본값 없음, 없으면 기동 거부(CN-15).
            throw mrs_core::ContractViolation(
                "roadmap_publisher: 'roadmap_path' is required and has no default");
        }

        rclcpp::QoS latched_qos(rclcpp::KeepLast(1));
        latched_qos.reliable().transient_local();

        roadmap_pub_ = this->create_publisher<mrs_msgs::msg::Roadmap>("/roadmap", latched_qos);
        robot_specs_pub_ =
            this->create_publisher<mrs_msgs::msg::RobotSpecs>("/robot_specs", latched_qos);

        PublishRoadmap(roadmap_path);

        if (robot_specs_path.empty())
        {
            RCLCPP_WARN(this->get_logger(),
                        "roadmap_publisher: 'robot_specs_path' not set — /robot_specs will "
                        "NOT be published this run. No canonical robot_specs source data "
                        "exists yet this round (361_integration_p3p4.md §5 신고). "
                        "mapf_node/trajopt_node cannot arm without it.");
            return;
        }
        PublishRobotSpecs(robot_specs_path);
    }

private:
    /// @brief roadmap.json(mrs.roadmap 봉투)을 읽어 봉투·필수 필드를 검사한 뒤
    /// 그대로 재발행한다(F48-1·F48-2).
    void PublishRoadmap(const std::string& path)
    {
        const auto text = ReadFileOrThrow(path, "roadmap");
        nlohmann::json doc;
        try
        {
            doc = nlohmann::json::parse(text);
        }
        catch (const nlohmann::json::parse_error& e)
        {
            throw mrs_core::ContractViolation("roadmap_publisher: roadmap_path JSON parse error: " +
                                              std::string(e.what()));
        }

        CheckEnvelopeOrThrow(doc, kRoadmapSchema, kRoadmapSchemaVersion, "roadmap");
        const auto out = ParseRoadmapJson(doc);

        roadmap_pub_->publish(out);
        RCLCPP_INFO(this->get_logger(),
                    "roadmap_publisher: /roadmap published (%zu nodes, %zu edges) from %s",
                    out.nodes.size(),
                    out.edges.size(),
                    path.c_str());
    }

    /// @brief robot_specs.json(mrs.robot_specs 봉투)을 읽어 봉투·필수 필드를 검사한 뒤
    /// 그대로 재발행한다(F48-1·F48-2).
    void PublishRobotSpecs(const std::string& path)
    {
        const auto text = ReadFileOrThrow(path, "robot_specs");
        nlohmann::json doc;
        try
        {
            doc = nlohmann::json::parse(text);
        }
        catch (const nlohmann::json::parse_error& e)
        {
            throw mrs_core::ContractViolation(
                "roadmap_publisher: robot_specs_path JSON parse error: " + std::string(e.what()));
        }

        CheckEnvelopeOrThrow(doc, kRobotSpecsSchema, kRobotSpecsSchemaVersion, "robot_specs");
        const auto out = ParseRobotSpecsJson(doc);

        robot_specs_pub_->publish(out);
        RCLCPP_INFO(this->get_logger(),
                    "roadmap_publisher: /robot_specs published (%zu robots) from %s",
                    out.robots.size(),
                    path.c_str());
    }

    rclcpp::Publisher<mrs_msgs::msg::Roadmap>::SharedPtr roadmap_pub_;
    rclcpp::Publisher<mrs_msgs::msg::RobotSpecs>::SharedPtr robot_specs_pub_;
};

}  // namespace mrs_sim::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    try
    {
        auto node = std::make_shared<mrs_sim::node::RoadmapPublisher>();
        rclcpp::spin(node);
    }
    catch (const mrs_core::ContractViolation& cv)
    {
        RCLCPP_FATAL(rclcpp::get_logger("roadmap_publisher"),
                     "startup contract violation: %s",
                     cv.what());
        rclcpp::shutdown();
        return 1;
    }
    rclcpp::shutdown();
    return 0;
}
