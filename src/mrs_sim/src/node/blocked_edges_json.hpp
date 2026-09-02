// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SIM_NODE_BLOCKED_EDGES_JSON_HPP
#define MRS_SIM_NODE_BLOCKED_EDGES_JSON_HPP

#include <algorithm>
#include <string>
#include <vector>

#include "mrs_core/status.hpp"
#include "nlohmann/json.hpp"

/// @file blocked_edges_json.hpp
/// @brief `observation_node`가 읽는 "시각별 차단 이벤트" 시나리오 파일 파서
/// (375_observation_publisher.md 48차 웨이브 4-A · U46-3의 "시나리오 파일 시각별
/// 차단 이벤트" 절반 — 런타임 토글 절반은 다음 라운드).
///
/// 🔴 이 파일이 정의하는 JSON 형태는 **`mrs_msgs/schema/`의 계약 스키마가 아니다.**
/// `roadmap.json`·`task_release.json`과 같은 "결"(봉투 + `schema`/`schema_version`
/// 자기기술 필드)을 따르되, `schema` 값에 일부러 `mrs.` 접두사를 쓰지 않는다
/// (`mrs_sim.blocked_edges_scenario`) — `mrs.*`는 `mrs_msgs/schema/`에 등록된
/// 경계 스키마의 이름 공간이고, 이 파일은 `observation_node` 하나만 읽고 어떤
/// 경계 토픽으로도 그대로 재직렬화되지 않는 **도구 층 내부 데이터**이기 때문이다
/// (357§Q1 "도구 층" 판정과 같은 근거). 계약 `schema/` 무접촉 — 새 파일 0개.
///
/// 🔴 나르는 것은 **기하 이벤트**(어느 간선이, 언제, 막히거나 풀리는가)뿐이다.
/// "이 로봇이 막혔다고 판단해야 한다" 같은 판정 어휘는 이 파일에도, 이 파일이
/// 만드는 내부 자료구조에도 없다(357§Q1-b) — Q1 판정은 온보드(mrs_trajopt) 몫이고
/// `observation_node`는 그 판정에 쓰일 원시 기하(LaserScan)만 만든다.
namespace mrs_sim::node
{

/// @brief 시나리오 파일의 이벤트 원소 하나 — 어느 간선이 어느 시각에 막히거나
/// 풀리는가. 방향은 무시한다(양방향 간선의 물리적 차단은 방향이 없다) —
/// `NormalizeEdgeKey`가 순서를 정규화한다.
struct BlockedEdgeEvent
{
    double time_s =
        0.0;  ///< 이 노드 기동 후 경과 시각(관측 틱 카운트 기반, §observation_node.cpp).
    std::string from;    ///< `/roadmap`의 `edges[].from`(또는 `to`) 중 하나.
    std::string to;      ///< 나머지 한쪽.
    std::string action;  ///< 닫힌 어휘: "block" | "unblock".
};

/// @brief 방향 무관 간선 키를 만든다 — `from`·`to` 중 사전순으로 앞선 것을 앞에 둔다.
/// @param from 간선의 한쪽 정점 id.
/// @param to 간선의 다른 쪽 정점 id.
/// @return `"a|b"` 형태의 정규화 키(`a<=b`).
inline std::string NormalizeEdgeKey(const std::string& from, const std::string& to)
{
    return (from <= to) ? (from + "|" + to) : (to + "|" + from);
}

/// @brief 시나리오 파일 전체(단일 객체, 배열이 아니다 — `roadmap.json`과 같은 형태)를
/// 파싱해 `time_s` 오름차순으로 정렬한 이벤트 벌집을 만든다.
/// @param text 파일 전체 텍스트.
/// @return 정렬된 이벤트 벡터(빈 `events`면 빈 벡터 — 오류가 아니다).
/// @throws mrs_core::ContractViolation 파싱 실패·봉투 불일치·필수 필드 부재·
///   `action`이 닫힌 어휘 밖이면(CN-15, 기동 경로 — 값을 조용히 채우지 않는다).
inline std::vector<BlockedEdgeEvent> ParseBlockedEdgesScenario(const std::string& text)
{
    nlohmann::json doc;
    try
    {
        doc = nlohmann::json::parse(text);
    }
    catch (const nlohmann::json::parse_error& e)
    {
        throw mrs_core::ContractViolation(
            "observation_node: blocked_edges_path JSON parse error: " + std::string(e.what()));
    }

    // 자기기술 봉투 — mrs_msgs/schema/의 계약 const가 아니라 이 파일 자신의 정본
    // (파일 머리 주석). 다르면 사람이 다른 형식의 파일을 잘못 가리킨 것이다.
    const auto schema = doc.value("schema", std::string());
    const auto version = doc.value("schema_version", std::string());
    if (schema != "mrs_sim.blocked_edges_scenario" || version != "1.0.0")
    {
        throw mrs_core::ContractViolation(
            "observation_node: blocked_edges_path envelope mismatch — expected "
            "schema=mrs_sim.blocked_edges_scenario schema_version=1.0.0, got schema=" +
            schema + " schema_version=" + version);
    }

    std::vector<BlockedEdgeEvent> out;
    try
    {
        const auto& events = doc.at("events");
        out.reserve(events.size());
        for (const auto& e : events)
        {
            BlockedEdgeEvent ev;
            ev.time_s = e.at("time_s").get<double>();
            ev.from = e.at("from").get<std::string>();
            ev.to = e.at("to").get<std::string>();
            ev.action = e.at("action").get<std::string>();
            if (ev.action != "block" && ev.action != "unblock")
            {
                throw mrs_core::ContractViolation(
                    "observation_node: blocked_edges_path event has unknown action '" + ev.action +
                    "' — closed vocabulary is {block, unblock}");
            }
            out.push_back(std::move(ev));
        }
    }
    catch (const nlohmann::json::exception& e)
    {
        throw mrs_core::ContractViolation(
            "observation_node: blocked_edges_path missing/invalid required field "
            "(schema: schema/schema_version/instance_id/events[].{time_s,from,to,action}): " +
            std::string(e.what()));
    }

    // 파일 순서를 신뢰하지 않는다 — 발행 순서가 아니라 "언제 일어나는가"만 나르는
    // 시나리오 정의이므로, 소비 측(observation_node::OnTick)이 시각순으로 훑을 수
    // 있게 여기서 정렬한다(task_release_publisher의 "배열 순서 = 발행 순서"와는
    // 다른 성격 — 여기엔 그 계약이 없다).
    std::stable_sort(out.begin(),
                     out.end(),
                     [](const BlockedEdgeEvent& a, const BlockedEdgeEvent& b)
                     {
                         return a.time_s < b.time_s;
                     });
    return out;
}

}  // namespace mrs_sim::node

#endif  // MRS_SIM_NODE_BLOCKED_EDGES_JSON_HPP
