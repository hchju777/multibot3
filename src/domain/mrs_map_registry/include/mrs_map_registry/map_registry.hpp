#pragma once

/**
 * @file map_registry.hpp
 * @brief MapRegistry — 물리 roadmap ↔ L2 균일 뷰 ↔ L3 의존성 골격 3뷰 정합의 단일 소유자 (D-11).
 *
 * architecture 제약 C4: 3뷰 변환 함수는 이 패키지 밖에 존재하지 않는다(CI 검사 §2.4-3).
 * 왕복(세분화↔collapse) 무손실은 unit 티어 필수 테스트다(D-11 v2). 알고리즘 본문은 Phase 5
 * (coordination-builder)가 채운다 — 이 헤더는 시그니처와 계약만 고정한다.
 */

#include <cstdint>
#include <string>
#include <vector>

#include "mrs/contract_types.hpp"

namespace mrs
{

/**
 * @brief 뷰 하나(물리/균일/골격 공통 표현)의 노드 1건 — RoadmapNode.msg 대응 도메인 표현.
 */
struct RoadmapNodeView
{
  NodeId node_id{NODE_ID_NONE};   ///< 노드 id (뷰 내 유효)
  double x_m{0.0};                ///< 위치 x [m], map
  double y_m{0.0};                ///< 위치 y [m], map
  double claim_radius_m{0.0};     ///< r_clear [m]. 산정식 OPEN(theory T1 O6) — [0a] 실측 대상
  bool is_endpoint{false};        ///< well-formed 검사용 엔드포인트 여부
  bool is_task_endpoint{false};   ///< 작업 엔드포인트 여부
  bool is_virtual{false};         ///< 세분화로 생성된 가상 노드 여부 (물리 뷰는 항상 false)
};

/**
 * @brief 뷰 하나의 엣지 1건 — RoadmapEdge.msg 대응 도메인 표현.
 */
struct RoadmapEdgeView
{
  EdgeId edge_id{NODE_ID_NONE};     ///< 엣지 id
  NodeId node_a{NODE_ID_NONE};      ///< 양단 노드 a
  NodeId node_b{NODE_ID_NONE};      ///< 양단 노드 b
  double length_m{0.0};             ///< 엣지 길이 [m]
  double corridor_width_m{0.0};     ///< 최소 통로 폭 W_min [m] (검사기 3 입력)
  double speed_limit_mps{0.0};      ///< 허용 최대 속도 [m/s] (통로폭 디레이팅 반영값)
};

/**
 * @brief 뷰 간 노드 대응 1건 — NodeMapping.msg 대응 도메인 표현 (D-11 왕복 무손실의 표현체).
 */
struct NodeMappingView
{
  NodeId from_node_id{NODE_ID_NONE}; ///< 원본 뷰 노드 id
  NodeId to_node_id{NODE_ID_NONE};   ///< 대응 노드 id. NODE_ID_NONE = 해당 없음
  EdgeId to_edge_id{NODE_ID_NONE};   ///< 대응 물리 엣지 id. NODE_ID_NONE = 해당 없음
  double s{0.0};                     ///< to_edge 위 정규화 위치 [0,1] (to_edge 유효 시만 의미)
};

/**
 * @brief 조회 가능한 뷰 데이터 묶음 (GetRoadmap/GetUniformView/GetDependencySkeleton 공통 반환형).
 */
struct RoadmapViewData
{
  std::uint64_t roadmap_version{0};             ///< 지도 버전
  std::uint32_t view_id{0};                     ///< 뷰 id (물리 roadmap = 0)
  double unit_length_m{0.0};                    ///< 세분화 입도 [m] (균일 뷰에서만 의미)
  double unit_length_lower_bound_m{0.0};        ///< 이론 하한 2(r+L) [m] (T1)
  std::vector<RoadmapNodeView> nodes;            ///< 뷰 노드 목록
  std::vector<RoadmapEdgeView> edges;            ///< 뷰 엣지 목록
  std::vector<NodeMappingView> mappings;         ///< 세분화(subdivision) 또는 collapse 대응표
};

/**
 * @brief roadmap 정적 검사기 3종의 결과 — ValidateMap.srv 대응 도메인 표현 (D-11, T1-R5).
 */
struct RoadmapValidationResult
{
  bool well_formed{false};             ///< 검사기 1 (lifelong-mapd-2017 Def.1)
  bool biconnected{false};             ///< 검사기 2 (pibt-2019 Thm 2 reachability 전제)
  bool corridor_width_ok{false};       ///< 검사기 3-a: W_min > 2(r + v_max/omega_max)
  bool unit_granularity_ok{false};     ///< 검사기 3-b: 세분화 입도 >= 2(r+L)
  std::vector<NodeId> violating_node_ids; ///< 위반 노드
  std::vector<EdgeId> violating_edge_ids; ///< 위반 엣지 (통로 폭 미달)
  double required_min_width_m{0.0};       ///< 요구 최소 통로 폭 [m]
  double derated_v_max_mps{0.0};          ///< 디레이팅 적용 시 속도 [m/s]
  std::vector<std::string> messages;      ///< 사람이 읽을 위반 설명 (기계 판정 금지)
};

/**
 * @brief MapRegistry — 3뷰 정합의 단일 소유자 (D-11).
 * @note 이 클래스 밖에서 세분화·collapse·좌표 변환을 재구현하지 않는다(C4, CI 검사).
 */
class MapRegistry
{
public:
  MapRegistry() = default;
  ~MapRegistry() = default;

  /**
   * @brief 물리(불균일) roadmap을 로드하고 초기 버전을 발급한다.
   * @param[in] map_file_path 지도 파일 경로. 형식은 [0a] 실측 후 확정(수치 창작 금지).
   * @return bool 로드 성공 여부.
   */
  bool load_physical_roadmap(const std::string & map_file_path);

  /**
   * @brief 물리 roadmap의 긴 엣지를 unit 길이로 세분화해 L2 균일 뷰를 만든다 (D-11 v2).
   * @param[in] unit_length_m 세분화 입도 [m]. 이론 하한 2(r+L) 미만이면 실패.
   * @param[out] out_view_id 생성된 균일 뷰 id.
   * @return bool 생성 성공 여부.
   */
  bool build_uniform_view(double unit_length_m, std::uint32_t & out_view_id);

  /**
   * @brief 균일 뷰의 차수 2 체인 가상 노드를 collapse 해 L3 의존성 골격을 만든다 (D-11 v2).
   * @param[in] uniform_view_id 원본 균일 뷰 id.
   * @param[out] out_skeleton_view_id 생성된 골격 뷰 id.
   * @return bool 생성 성공 여부.
   */
  bool build_dependency_skeleton(std::uint32_t uniform_view_id, std::uint32_t & out_skeleton_view_id);

  /**
   * @brief 임의 뷰의 데이터를 조회한다 (GetRoadmap/GetUniformView/GetDependencySkeleton 공통 경로).
   * @param[in] view_id 조회할 뷰 id (물리 roadmap = 0).
   * @param[out] out_data 뷰 데이터.
   * @return bool 조회 성공 여부(view_id 미존재 시 false).
   */
  bool get_view(std::uint32_t view_id, RoadmapViewData & out_data) const;

  /**
   * @brief 한 뷰의 노드를 다른 뷰의 노드로 변환한다 (D-11 무손실 요건의 핵심 연산).
   * @param[in] from_view_id 원본 뷰 id.
   * @param[in] from_node 원본 노드 id.
   * @param[in] to_view_id 목적 뷰 id.
   * @param[out] out_node 변환된 노드 id.
   * @return bool 변환 성공 여부(대응 없음 = false).
   */
  bool transform_node(
    std::uint32_t from_view_id, NodeId from_node,
    std::uint32_t to_view_id, NodeId & out_node) const;

  /**
   * @brief roadmap 정적 검사기 3종을 실행한다 (D-11, T1-R5). 맵 로드 시점 + R3 접합 후 재검사.
   * @param[in] view_id 통로 폭·입도 검사 대상 뷰. 0 = 물리 roadmap만.
   * @param[in] robot_count well-formed 검사에 쓰이는 로봇 수.
   * @param[in] robot_radius_m 로봇 반경 r [m].
   * @param[in] v_max_mps 최대 선속도 [m/s].
   * @param[in] omega_max_rps 최대 각속도 [rad/s].
   * @param[in] nid_offset_l_m NID 오프셋 L [m].
   * @return RoadmapValidationResult 검사기 3종의 결과.
   */
  RoadmapValidationResult validate_map(
    std::uint32_t view_id, std::uint16_t robot_count, double robot_radius_m,
    double v_max_mps, double omega_max_rps, double nid_offset_l_m) const;

  /**
   * @brief 현재 지도 버전을 반환한다 (Q-6 캐시 무효화 키).
   * @return std::uint64_t 현재 roadmap_version. 미로드 상태면 0.
   */
  std::uint64_t roadmap_version() const noexcept { return roadmap_version_; }

private:
  std::uint64_t roadmap_version_{0}; ///< 현재 물리 지도 버전 (단조증가)
};

} // namespace mrs
