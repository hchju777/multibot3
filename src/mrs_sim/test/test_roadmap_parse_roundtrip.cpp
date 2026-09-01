// SPDX-License-Identifier: Apache-2.0
// Self-contained assert main (no gtest dependency — module precedent,
// mrs_core/test/test_status.cpp).
//
// F48-4(48차 코드 게이트) — 로드맵/로봇사양 JSON -> ROS 메시지 왕복을 확인한다.
// F48-1(봉투 검사)·F48-2(필수 필드 엄격성)를 잠근다: 옳은 봉투/전 필드가 있으면
// 통과하고, 봉투가 다르거나 필수 필드가 빠지면 `mrs_core::ContractViolation`을
// 던지는 것을 **되돌려서 실패하는 것까지** 확인한다(1.4.3 관례 — 시험이 실제로
// 그 결함을 잡는지 보인다).

#include <cassert>
#include <cstdio>
#include <string>

#include "mrs_core/status.hpp"
#include "nlohmann/json.hpp"

// src/node/에 사는 비공개 헤더(공개 include/mrs_sim/를 두지 않는 이유는
// roadmap_json.hpp의 파일 doc 참조 — CN-1 필수 넷을 이 도구 패키지에 강제하지
// 않기 위함이다). 상대 경로로 직접 include한다.
#include "../src/node/roadmap_json.hpp"

namespace
{

/// @brief `roadmap.schema.json`의 `required` 전부를 갖춘 최소 유효 문서.
nlohmann::json MakeValidRoadmapDoc()
{
    return nlohmann::json{
        {"schema", "mrs.roadmap"},
        {"schema_version", "3.0.0"},
        {"instance_id", "test_instance"},
        {"nodes",
         nlohmann::json::array({{{"id", "J0"}, {"x", 0.0}, {"y", 0.0}, {"clearance_m", 1.0}}})},
        {"edges",
         nlohmann::json::array({{{"from", "J0"},
                                 {"to", "J1"},
                                 {"traversal", "bidirectional"},
                                 {"length_m", 5.0},
                                 {"width_m", 2.0},
                                 {"capacity_robots", 1},
                                 {"corridor", ""}}})},
        {"endpoints", nlohmann::json::array({"J0"})},
        {"corridors", nlohmann::json::array()},
        {"provenance",
         {{"min_separation_m", 0.5},
          {"wall_inflation_m", 0.0},
          {"capacity_rule_id", "rule_a"},
          {"generator_version", "test-0.1.0"}}},
    };
}

/// @brief `robot_specs.schema.json`의 `required` 전부를 갖춘 최소 유효 문서.
nlohmann::json MakeValidRobotSpecsDoc()
{
    return nlohmann::json{
        {"schema", "mrs.robot_specs"},
        {"schema_version", "1.1.0"},
        {"instance_id", "test_instance"},
        {"robots",
         nlohmann::json::array({{{"robot", "r0"},
                                 {"avg_traversal_speed_mps", 1.0},
                                 {"v_max", 1.5},
                                 {"brake_decel_min_mps2", 1.0},
                                 {"curvature_max_invm", 0.5},
                                 {"reverse_motion_allowed", false},
                                 {"circumradius_m", 0.3}}})},
    };
}

}  // namespace

int main()
{
    // --- 유효한 문서: 봉투 검사 통과 + 필드 전수 왕복 ---
    {
        const auto doc = MakeValidRoadmapDoc();
        mrs_sim::node::CheckEnvelopeOrThrow(doc, "mrs.roadmap", "3.0.0", "roadmap");  // 안 던짐.
        const auto msg = mrs_sim::node::ParseRoadmapJson(doc);
        assert(msg.schema == "mrs.roadmap");
        assert(msg.schema_version == "3.0.0");
        assert(msg.instance_id == "test_instance");
        assert(msg.nodes.size() == 1);
        assert(msg.nodes[0].id == "J0");
        assert(msg.edges.size() == 1);
        assert(msg.edges[0].from_id == "J0");
        assert(msg.edges[0].to_id == "J1");
        assert(msg.endpoints.size() == 1);
        assert(msg.provenance.min_separation_m == 0.5);  // F48-2가 지목한 자리 — 이제 진짜 값.
    }

    // --- F48-1: 봉투 불일치는 ContractViolation ---
    {
        auto doc = MakeValidRoadmapDoc();
        doc["schema_version"] = "2.0.0";  // 낡은 라벨(고침 전 실물 그대로).
        bool threw = false;
        try
        {
            mrs_sim::node::CheckEnvelopeOrThrow(doc, "mrs.roadmap", "3.0.0", "roadmap");
        }
        catch (const mrs_core::ContractViolation&)
        {
            threw = true;
        }
        assert(threw && "봉투 버전 불일치가 조용히 통과하면 안 된다");
    }

    // --- F48-2: 필수 필드(provenance.min_separation_m) 부재는 조용한 0.0이 아니라
    //     ContractViolation이어야 한다. ---
    {
        auto doc = MakeValidRoadmapDoc();
        doc["provenance"].erase("min_separation_m");
        bool threw = false;
        try
        {
            (void)mrs_sim::node::ParseRoadmapJson(doc);
        }
        catch (const mrs_core::ContractViolation&)
        {
            threw = true;
        }
        assert(threw && "필수 필드 부재가 기본값 0.0으로 조용히 채워지면 안 된다(F48-2)");
    }

    // --- robot_specs: 같은 세 가지를 확인 ---
    {
        const auto doc = MakeValidRobotSpecsDoc();
        mrs_sim::node::CheckEnvelopeOrThrow(doc, "mrs.robot_specs", "1.1.0", "robot_specs");
        const auto msg = mrs_sim::node::ParseRobotSpecsJson(doc);
        assert(msg.robots.size() == 1);
        assert(msg.robots[0].robot == "r0");
        assert(msg.robots[0].circumradius_m == 0.3);
    }
    {
        auto doc = MakeValidRobotSpecsDoc();
        doc["schema"] = "mrs.wrong_schema";
        bool threw = false;
        try
        {
            mrs_sim::node::CheckEnvelopeOrThrow(doc, "mrs.robot_specs", "1.1.0", "robot_specs");
        }
        catch (const mrs_core::ContractViolation&)
        {
            threw = true;
        }
        assert(threw);
    }
    {
        auto doc = MakeValidRobotSpecsDoc();
        doc["robots"][0].erase("circumradius_m");
        bool threw = false;
        try
        {
            (void)mrs_sim::node::ParseRobotSpecsJson(doc);
        }
        catch (const mrs_core::ContractViolation&)
        {
            threw = true;
        }
        assert(threw);
    }

    std::puts("test_roadmap_parse_roundtrip: OK");
    return 0;
}
