// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SIM_NODE_TASK_RELEASE_JSON_HPP
#define MRS_SIM_NODE_TASK_RELEASE_JSON_HPP

#include <string>
#include <vector>

#include "mrs_core/status.hpp"
#include "mrs_msgs/msg/task_goal.hpp"
#include "mrs_msgs/msg/task_release.hpp"
#include "nlohmann/json.hpp"

/// @file task_release_json.hpp
/// @brief `task_release_publisher`가 읽는 소스 파일 -> `mrs_msgs::msg::TaskRelease`
/// 변환. `src/node/`에 두고 공개 헤더에는 두지 않는다 — `roadmap_json.hpp`와 같은
/// 배치 사유(`mrs_sim`은 CN-1 필수 레이어 대상이 아닌 도구 패키지, `361_
/// integration_p3p4.md`§13).
///
/// 🔴 소스 파일 형식은 `mrs_msgs/schema/task_release.schema.json` 그 자체가
/// **아니다** — 그 스키마는 메시지 한 통(release 하나)의 봉투를 규정하고,
/// `/task_release`는 run 중 여러 통이 시간에 걸쳐 발행되는 스트림이다(계약이
/// 릴리스 시각을 싣지 않는 이유와 같다, 스키마 `x-forbidden-note`). 이 파일이
/// 읽는 소스는 **그 봉투 객체의 JSON 배열**이고, 배열의 각 원소는 개별적으로
/// `task_release.schema.json` 2.0.0에 유효해야 한다(`mrs_bringup/config/scenario/
/// README.md` "task_release.json" 절). 배열 그 자체는 계약 스키마가 아니므로
/// `additionalProperties`류 계약 검사 대상이 아니다 — 원소 하나하나만 계약이다.
namespace mrs_sim::node
{

/// @brief 배열 원소 하나(`mrs.task_release` 봉투)를 검사·변환한다.
/// @param doc 파싱된 JSON 원소.
/// @param expected_schema 계약 스키마의 `schema` `const` 값.
/// @param expected_version 계약 스키마의 `schema_version` `const` 값.
/// @param index 배열 안에서의 위치(오류 메시지용, `release_index`와 다를 수 있다).
/// @return 채워진 `TaskRelease` 메시지.
/// @throws mrs_core::ContractViolation 봉투 불일치 또는 `task_release.schema.json`의
///   `required` 필드가 하나라도 부재하면(F48-1·F48-2와 같은 규율 — 기본값을
///   조용히 채우지 않는다). `goals`가 비어 있어도 던진다(schema `minItems:1`).
inline mrs_msgs::msg::TaskRelease ParseTaskReleaseElement(const nlohmann::json& doc,
                                                           const std::string& expected_schema,
                                                           const std::string& expected_version,
                                                           std::size_t index)
{
    const auto schema = doc.value("schema", std::string());
    const auto version = doc.value("schema_version", std::string());
    if (schema != expected_schema || version != expected_version)
    {
        throw mrs_core::ContractViolation(
            "task_release_publisher: element " + std::to_string(index) +
            " envelope mismatch — expected schema=" + expected_schema +
            " schema_version=" + expected_version + ", got schema=" + schema +
            " schema_version=" + version);
    }

    try
    {
        mrs_msgs::msg::TaskRelease out;
        out.schema = doc.at("schema").get<std::string>();
        out.schema_version = doc.at("schema_version").get<std::string>();
        out.instance_id = doc.at("instance_id").get<std::string>();
        out.release_index = doc.at("release_index").get<std::uint64_t>();
        out.task = doc.at("task").get<std::string>();

        const auto& goals = doc.at("goals");
        if (goals.empty())
        {
            throw mrs_core::ContractViolation(
                "task_release_publisher: element " + std::to_string(index) +
                " has 0 goals (task_release.schema.json minItems:1)");
        }
        out.goals.reserve(goals.size());
        for (const auto& g : goals)
        {
            mrs_msgs::msg::TaskGoal tg;
            tg.goal_id = g.at("goal_id").get<std::string>();
            tg.location = g.at("location").get<std::string>();
            out.goals.push_back(tg);
        }
        return out;
    }
    catch (const nlohmann::json::exception& e)
    {
        throw mrs_core::ContractViolation(
            "task_release_publisher: element " + std::to_string(index) +
            " missing/invalid required field (task_release.schema.json required list): " +
            std::string(e.what()));
    }
}

/// @brief 소스 파일(배열) 전체를 파싱해 순서대로 `TaskRelease` 벌집을 만든다.
/// @param text 파일 전체 텍스트.
/// @param expected_schema 계약 스키마의 `schema` `const` 값.
/// @param expected_version 계약 스키마의 `schema_version` `const` 값.
/// @return `release_index` 오름차순을 신뢰해 원본 배열 순서 그대로 반환한다
///   (재정렬하지 않는다 — 배열 순서가 발행 순서다, README "task_release.json").
/// @throws mrs_core::ContractViolation 최상위가 배열이 아니거나, 파싱 실패거나,
///   원소 하나라도 계약 위반이면. `release_index`가 0부터 시작해 단조 증가하지
///   않으면(스키마 2.0.0 B-19 기점 규약) 던진다 — 소스 파일 자체가 계약을
///   어기는 스트림을 만들지 않는다.
inline std::vector<mrs_msgs::msg::TaskRelease> ParseTaskReleaseArray(
    const std::string& text, const std::string& expected_schema, const std::string& expected_version)
{
    nlohmann::json doc;
    try
    {
        doc = nlohmann::json::parse(text);
    }
    catch (const nlohmann::json::parse_error& e)
    {
        throw mrs_core::ContractViolation("task_release_publisher: task_release_path JSON parse "
                                          "error: " +
                                          std::string(e.what()));
    }
    if (!doc.is_array())
    {
        throw mrs_core::ContractViolation(
            "task_release_publisher: task_release_path top level must be a JSON array of "
            "mrs.task_release envelopes");
    }

    std::vector<mrs_msgs::msg::TaskRelease> out;
    out.reserve(doc.size());
    std::uint64_t expected_index = 0;
    for (std::size_t i = 0; i < doc.size(); ++i)
    {
        auto rel = ParseTaskReleaseElement(doc[i], expected_schema, expected_version, i);
        if (rel.release_index != expected_index)
        {
            throw mrs_core::ContractViolation(
                "task_release_publisher: release_index gap/out-of-order at array position " +
                std::to_string(i) + " — expected " + std::to_string(expected_index) + ", got " +
                std::to_string(rel.release_index) + " (schema 2.0.0 B-19: first value 0, +1 each)");
        }
        ++expected_index;
        out.push_back(std::move(rel));
    }
    return out;
}

}  // namespace mrs_sim::node

#endif  // MRS_SIM_NODE_TASK_RELEASE_JSON_HPP
