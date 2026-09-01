// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SIM_NODE_ROADMAP_JSON_HPP
#define MRS_SIM_NODE_ROADMAP_JSON_HPP

#include <string>

#include "mrs_core/status.hpp"
#include "mrs_core_msgs/reason_codes.hpp"
#include "mrs_msgs/msg/corridor.hpp"
#include "mrs_msgs/msg/edge_ref.hpp"
#include "mrs_msgs/msg/roadmap.hpp"
#include "mrs_msgs/msg/roadmap_edge.hpp"
#include "mrs_msgs/msg/roadmap_node.hpp"
#include "mrs_msgs/msg/roadmap_provenance.hpp"
#include "mrs_msgs/msg/robot_spec.hpp"
#include "mrs_msgs/msg/robot_specs.hpp"
#include "nlohmann/json.hpp"

/// @file roadmap_json.hpp
/// @brief `roadmap_publisher`가 재발행하는 JSON -> ROS 메시지 변환 (48차 코드 게이트
/// F48-1·F48-2 해소). `src/node/`에 두고 `include/mrs_sim/`에는 두지 않는다 — 공개
/// 헤더를 두는 순간 `check_layer_layout.py`가 `mrs_sim`을 CN-1 필수 넷(core/adapter/
/// node/service) 대상으로 잡는데, 이 패키지는 도구 패키지라 그 넷이 어울리지 않는다
/// (361_integration_p3p4.md §13). 이 헤더는 `roadmap_publisher.cpp`와
/// `test/test_roadmap_parse_roundtrip.cpp` 둘만 상대 경로로 include한다.
///
/// 🔴 두 규율(코드 게이트 F48-1·F48-2 해소):
///   1. **봉투 검사** — `schema`/`schema_version`이 계약 `const`와 다르면 게시 전에
///      `ContractViolation`을 던진다(CN-15, 기동 경로). 조용히 다른 값으로 재발행하지
///      않는다.
///   2. **필수 필드는 전부 `.at()`** — `roadmap.schema.json`/`robot_specs.schema.json`의
///      `required` 배열에서 그대로 옮겼다(추측으로 고르지 않았다). 부재 시
///      `nlohmann::json::exception`을 잡아 `ContractViolation`으로 다시 던진다 —
///      기본값을 조용히 채우지 않는다(F48-2가 지목한 `min_separation_m=0.0` 같은 자리).

namespace mrs_sim::node
{

/// @brief `doc["schema"]`/`doc["schema_version"]`이 계약 `const`와 일치하는지 확인한다.
/// @param doc 파싱된 JSON 문서.
/// @param expected_schema 계약 스키마의 `schema` `const` 값.
/// @param expected_version 계약 스키마의 `schema_version` `const` 값.
/// @param purpose 오류 메시지에 실을 자리 이름(예: "roadmap").
/// @throws mrs_core::ContractViolation 봉투가 부재하거나 다르면.
inline void CheckEnvelopeOrThrow(const nlohmann::json& doc,
                                 const std::string& expected_schema,
                                 const std::string& expected_version,
                                 const std::string& purpose)
{
    const auto schema = doc.value("schema", std::string());
    const auto version = doc.value("schema_version", std::string());
    if (schema != expected_schema || version != expected_version)
    {
        throw mrs_core::ContractViolation("roadmap_publisher: " + purpose +
                                          " envelope mismatch — expected schema=" +
                                          expected_schema + " schema_version=" + expected_version +
                                          ", got schema=" + schema + " schema_version=" + version);
    }
}

/// @brief `mrs.roadmap` JSON을 `mrs_msgs::msg::Roadmap`으로 변환한다.
/// @param doc 파싱된 JSON 문서(봉투는 이미 `CheckEnvelopeOrThrow`로 확인됐다고 가정).
/// @return 채워진 `Roadmap` 메시지.
/// @throws mrs_core::ContractViolation `roadmap.schema.json`의 `required` 필드가 하나라도
///   부재하면(값을 조용히 채우지 않는다, F48-2).
inline mrs_msgs::msg::Roadmap ParseRoadmapJson(const nlohmann::json& doc)
{
    try
    {
        mrs_msgs::msg::Roadmap out;
        out.schema = doc.at("schema").get<std::string>();
        out.schema_version = doc.at("schema_version").get<std::string>();
        out.instance_id = doc.at("instance_id").get<std::string>();

        for (const auto& n : doc.at("nodes"))
        {
            mrs_msgs::msg::RoadmapNode nm;
            nm.id = n.at("id").get<std::string>();
            nm.x = n.at("x").get<double>();
            nm.y = n.at("y").get<double>();
            nm.clearance_m = n.at("clearance_m").get<double>();
            out.nodes.push_back(nm);
        }

        for (const auto& e : doc.at("edges"))
        {
            mrs_msgs::msg::RoadmapEdge em;
            em.from_id = e.at("from").get<std::string>();
            em.to_id = e.at("to").get<std::string>();
            const auto trav =
                mrs_core_msgs::EdgeTraversalFromString(e.at("traversal").get<std::string>());
            em.traversal = trav.ok() ? trav.value() : mrs_msgs::msg::RoadmapEdge::TRAVERSAL_UNKNOWN;
            em.length_m = e.at("length_m").get<double>();
            em.width_m = e.at("width_m").get<double>();
            em.capacity_robots = e.at("capacity_robots").get<std::uint8_t>();
            em.corridor = e.at("corridor").get<std::string>();
            out.edges.push_back(em);
        }

        for (const auto& ep : doc.at("endpoints"))
        {
            out.endpoints.push_back(ep.get<std::string>());
        }

        for (const auto& c : doc.at("corridors"))
        {
            mrs_msgs::msg::Corridor cm;
            cm.id = c.at("id").get<std::string>();
            cm.length_m = c.at("length_m").get<double>();
            for (const auto& pair : c.at("edges"))
            {
                mrs_msgs::msg::EdgeRef ref;
                ref.from_id = pair.at(0).get<std::string>();
                ref.to_id = pair.at(1).get<std::string>();
                cm.edges.push_back(ref);
            }
            out.corridors.push_back(cm);
        }

        const auto& p = doc.at("provenance");
        out.provenance.min_separation_m = p.at("min_separation_m").get<double>();
        out.provenance.wall_inflation_m = p.at("wall_inflation_m").get<double>();
        out.provenance.capacity_rule_id = p.at("capacity_rule_id").get<std::string>();
        out.provenance.generator_version = p.at("generator_version").get<std::string>();

        return out;
    }
    catch (const nlohmann::json::exception& e)
    {
        throw mrs_core::ContractViolation(
            "roadmap_publisher: roadmap JSON missing/invalid required field (roadmap.schema.json "
            "required list): " +
            std::string(e.what()));
    }
}

/// @brief `mrs.robot_specs` JSON을 `mrs_msgs::msg::RobotSpecs`로 변환한다.
/// @param doc 파싱된 JSON 문서(봉투는 이미 `CheckEnvelopeOrThrow`로 확인됐다고 가정).
/// @return 채워진 `RobotSpecs` 메시지.
/// @throws mrs_core::ContractViolation `robot_specs.schema.json`의 `required` 필드가
///   하나라도 부재하면.
inline mrs_msgs::msg::RobotSpecs ParseRobotSpecsJson(const nlohmann::json& doc)
{
    try
    {
        mrs_msgs::msg::RobotSpecs out;
        out.schema = doc.at("schema").get<std::string>();
        out.schema_version = doc.at("schema_version").get<std::string>();
        out.instance_id = doc.at("instance_id").get<std::string>();
        for (const auto& r : doc.at("robots"))
        {
            mrs_msgs::msg::RobotSpec rs;
            rs.robot = r.at("robot").get<std::string>();
            rs.avg_traversal_speed_mps = r.at("avg_traversal_speed_mps").get<double>();
            rs.v_max = r.at("v_max").get<double>();
            rs.brake_decel_min_mps2 = r.at("brake_decel_min_mps2").get<double>();
            rs.curvature_max_invm = r.at("curvature_max_invm").get<double>();
            rs.reverse_motion_allowed = r.at("reverse_motion_allowed").get<bool>();
            rs.circumradius_m = r.at("circumradius_m").get<double>();
            out.robots.push_back(rs);
        }
        return out;
    }
    catch (const nlohmann::json::exception& e)
    {
        throw mrs_core::ContractViolation(
            "roadmap_publisher: robot_specs JSON missing/invalid required field "
            "(robot_specs.schema.json required list): " +
            std::string(e.what()));
    }
}

}  // namespace mrs_sim::node

#endif  // MRS_SIM_NODE_ROADMAP_JSON_HPP
