#pragma once

/**
 * @file map_registry_impl.hpp
 * @brief MapRegistry 의 PIMPL 정의 (@ref mrs::MapRegistry::Impl) — 저장 상태와 내부 연산.
 *
 * **내부 헤더다 — 설치되지 않는다**(CMake 는 include/ 만 설치). 공개 헤더가 노출하지 않는 저장
 * 상태(뷰 3종·대응표·graph_index)를 여기 담아 숨긴다. yaml-cpp 는 이 헤더에 포함하지 않는다 —
 * 파싱은 `map_registry_loader.cpp` 한 곳에만 격리한다.
 *
 * 이 struct 의 멤버 함수는 관심사별 .cpp 로 나뉘어 정의된다:
 *  - `map_registry_loader.cpp`      : @ref Impl::load (YAML)
 *  - `map_registry_ingest.cpp`      : @ref Impl::ingest·@ref Impl::get_view·인덱스 구축
 *  - `map_registry_validators.cpp`  : (하위 B) 검사기 1·3·4
 *  - `map_registry_subdivision.cpp` : (하위 C) 세분화·collapse·transform
 */

#include <cstddef>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

#include "mrs_map_registry/map_error.hpp"
#include "mrs_map_registry/map_registry.hpp"
#include "mrs_map_registry/roadmap_types.hpp"

namespace mrs::mapreg_detail
{

/**
 * @brief 한 뷰의 인접 인덱스 — 노드 id → 인접 노드·부속 엣지. 검사기 1(엔드포인트 간 도달성)과
 *        세분화·collapse 의 차수 계산에 쓴다. 뷰가 저장될 때 한 번 구축된다.
 *
 * 노드 id 는 담고 있는 뷰 안에서 유효한 bare `std::uint32_t` 다(뷰별 강타입은 저장하지 않는다 —
 * 인덱스는 항상 단일 뷰 내부에서만 쓰이므로 뷰 혼동이 성립하지 않는다).
 */
struct GraphIndex
{
  std::unordered_map<std::uint32_t, std::vector<std::uint32_t>> neighbors; ///< 노드 → 인접 노드 id
  std::unordered_map<std::uint32_t, std::vector<std::uint32_t>>
    incident_edges; ///< 노드 → 부속 엣지 id

  /**
   * @brief 노드의 차수(인접 노드 수)를 반환한다.
   * @param[in] node_id 조회할 노드 id. 자료형 `std::uint32_t`.
   * @return `std::size_t` — 인접 노드 수. 미등록 노드면 0.
   */
  [[nodiscard]] std::size_t degree(std::uint32_t node_id) const noexcept
  {
    const auto it = neighbors.find(node_id);
    return it == neighbors.end() ? 0U : it->second.size();
  }

  /** @brief 인덱스를 비운다. */
  void clear() noexcept
  {
    neighbors.clear();
    incident_edges.clear();
  }
};

} // namespace mrs::mapreg_detail

namespace mrs
{

/**
 * @brief MapRegistry 의 저장 상태와 내부 연산 (PIMPL 실체).
 *
 * 한 roadmap_version 에 균일 뷰는 정확히 1개다(GetUniformView 규약, 재잠금 8차 (d)). 골격 뷰도
 * 그 균일 뷰에서 유래한 1개다. 입도를 바꾸는 재세분화는 재구성이며 @ref roadmap_version 증가를
 * 동반한다(이전 뷰 폐기).
 */
struct MapRegistry::Impl
{
  bool loaded{false};                                         ///< 물리 roadmap 로드 여부
  std::uint64_t roadmap_version{ROADMAP_VERSION_UNSPECIFIED}; ///< 현재 지도 버전 (단조증가)
  std::uint32_t next_view_id{1}; ///< 균일·골격 뷰 id 할당기 (물리 = 0 예약)

  RoadmapViewData physical;                 ///< 물리 뷰 (view_kind = PHYSICAL, view_id = 0)
  mapreg_detail::GraphIndex physical_index; ///< 물리 뷰 인접 인덱스

  bool has_uniform{false};  ///< 균일 뷰 존재 여부
  RoadmapViewData uniform;  ///< 균일 뷰 (view_kind = UNIFORM) — 하위 C 에서 채움
  bool has_skeleton{false}; ///< 골격 뷰 존재 여부
  RoadmapViewData skeleton; ///< 골격 뷰 (view_kind = SKELETON) — 하위 C 에서 채움

  // ── 로더 (map_registry_loader.cpp — yaml-cpp 격리) ──────────────────────────────────
  /**
   * @brief YAML 물리 roadmap 파일을 파싱해 주입한다. 예외를 던지지 않는다(전량 try/catch).
   * @param[in] map_file_path 지도 파일 경로. 자료형 `const std::string &`.
   * @return `MapResult<std::uint64_t>` — 성공 시 새 roadmap_version.
   *         실패: FILE_NOT_FOUND / FILE_PARSE_ERROR.
   */
  MapResult<std::uint64_t> load(const std::string & map_file_path);

  // ── 주입·인덱스 (map_registry_ingest.cpp) ──────────────────────────────────────────
  /**
   * @brief 파싱된 물리 뷰의 구조 무결성을 확인하고 저장·인덱스 구축·버전 발급한다.
   * @param[in] physical_in 파싱된 물리 뷰. 자료형 `RoadmapViewData` (이동 소비).
   * @return `MapResult<std::uint64_t>` — 성공 시 새 roadmap_version.
   *         실패: FILE_PARSE_ERROR (중복 노드 id·매달린 엣지 참조).
   */
  MapResult<std::uint64_t> ingest(RoadmapViewData physical_in);

  /**
   * @brief 저장된 뷰를 조회한다.
   * @param[in] view_id 조회할 뷰 id (물리 = 0). 자료형 `std::uint32_t`.
   * @return `MapResult<RoadmapViewData>` — 성공 시 뷰 데이터.
   *         실패: MAP_NOT_LOADED / VIEW_NOT_FOUND.
   */
  [[nodiscard]] MapResult<RoadmapViewData> get_view(std::uint32_t view_id) const;
};

} // namespace mrs
