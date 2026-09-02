// SPDX-License-Identifier: Apache-2.0
// Self-contained assert main (no gtest dependency — module precedent,
// mrs_core/test/test_status.cpp, mrs_sim/test/test_roadmap_parse_roundtrip.cpp).
//
// 375_observation_publisher.md(48차 웨이브 4-A) — 시나리오 파일 "시각별 차단
// 이벤트" 파서를 잠근다: 유효 문서는 시각순 정렬로 통과, 봉투 불일치·닫힌
// 어휘 밖 action은 ContractViolation, 방향 무관 키 정규화가 대칭인지 확인한다.

#include <cassert>
#include <cstdio>
#include <string>

#include "mrs_core/status.hpp"
#include "nlohmann/json.hpp"

// src/node/의 비공개 헤더(roadmap_json.hpp와 같은 배치 사유).
#include "../src/node/blocked_edges_json.hpp"

namespace
{

nlohmann::json MakeValidDoc()
{
    return nlohmann::json{
        {"schema", "mrs_sim.blocked_edges_scenario"},
        {"schema_version", "1.0.0"},
        {"instance_id", "test_instance"},
        {"events",
         nlohmann::json::array({
             {{"time_s", 30.0}, {"from", "J0100"}, {"to", "J0101"}, {"action", "unblock"}},
             {{"time_s", 8.0}, {"from", "J0100"}, {"to", "J0101"}, {"action", "block"}},
         })},
    };
}

}  // namespace

int main()
{
    // --- 유효한 문서: 시각순 정렬(파일 순서와 무관), 필드 전수 왕복 ---
    {
        const auto doc = MakeValidDoc();
        const auto events = mrs_sim::node::ParseBlockedEdgesScenario(doc.dump());
        assert(events.size() == 2);
        assert(events[0].time_s == 8.0 &&
               events[0].action == "block");  // 정렬됨 — 파일은 30이 먼저였다.
        assert(events[1].time_s == 30.0 && events[1].action == "unblock");
        assert(events[0].from == "J0100" && events[0].to == "J0101");
    }

    // --- 빈 events는 오류가 아니다 ---
    {
        auto doc = MakeValidDoc();
        doc["events"] = nlohmann::json::array();
        const auto events = mrs_sim::node::ParseBlockedEdgesScenario(doc.dump());
        assert(events.empty());
    }

    // --- 봉투 불일치는 ContractViolation ---
    {
        auto doc = MakeValidDoc();
        doc["schema_version"] = "0.9.0";
        bool threw = false;
        try
        {
            (void)mrs_sim::node::ParseBlockedEdgesScenario(doc.dump());
        }
        catch (const mrs_core::ContractViolation&)
        {
            threw = true;
        }
        assert(threw && "봉투 버전 불일치가 조용히 통과하면 안 된다");
    }

    // --- 닫힌 어휘 밖 action은 ContractViolation ---
    {
        auto doc = MakeValidDoc();
        doc["events"][0]["action"] = "half_block";
        bool threw = false;
        try
        {
            (void)mrs_sim::node::ParseBlockedEdgesScenario(doc.dump());
        }
        catch (const mrs_core::ContractViolation&)
        {
            threw = true;
        }
        assert(threw && "닫힌 어휘 밖 action이 조용히 통과하면 안 된다(block/unblock 둘뿐)");
    }

    // --- 필수 필드 부재는 ContractViolation ---
    {
        auto doc = MakeValidDoc();
        doc["events"][0].erase("time_s");
        bool threw = false;
        try
        {
            (void)mrs_sim::node::ParseBlockedEdgesScenario(doc.dump());
        }
        catch (const mrs_core::ContractViolation&)
        {
            threw = true;
        }
        assert(threw);
    }

    // --- 파싱 실패(JSON 아님)는 ContractViolation ---
    {
        bool threw = false;
        try
        {
            (void)mrs_sim::node::ParseBlockedEdgesScenario("{not json");
        }
        catch (const mrs_core::ContractViolation&)
        {
            threw = true;
        }
        assert(threw);
    }

    // --- NormalizeEdgeKey는 방향 무관(대칭) ---
    {
        assert(mrs_sim::node::NormalizeEdgeKey("J0100", "J0101") ==
               mrs_sim::node::NormalizeEdgeKey("J0101", "J0100"));
        assert(mrs_sim::node::NormalizeEdgeKey("A", "B") == "A|B");
    }

    std::puts("test_blocked_edges_parse: OK");
    return 0;
}
