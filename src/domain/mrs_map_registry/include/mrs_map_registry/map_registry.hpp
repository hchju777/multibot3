#pragma once

/**
 * @file map_registry.hpp
 * @brief MapRegistry — 물리 roadmap ↔ L2 균일 뷰 ↔ L3 의존성 골격 3뷰 정합의 단일 소유자 (D-11).
 *
 * architecture 제약 C4: 3뷰 변환 함수는 이 패키지 밖에 존재하지 않는다(CI 검사 §2.4-3).
 * 왕복(세분화↔collapse) 무손실은 unit 티어 필수 테스트다(D-11 v2). **이 헤더는 파사드 시그니처와
 * 계약만 고정한다** — 값 타입은 @ref roadmap_types.hpp, 실패 규약은 @ref map_error.hpp, 로더·
 * 검사기·세분화·collapse·변환의 알고리즘 본문은 파일 4(코어)가 채운다.
 *
 * ## 실패는 예외가 아니라 반환값 (CLAUDE.md 규율 2, 계약 §0.1 V3)
 * 모든 공개 함수는 @ref MapResult 를 반환한다 — 성공 시 값을 싣고, 실패 시 @ref MapStatus 사유를
 * 나른다. **예외를 던지지 않는다**(과거 스텁의 `throw std::logic_error` = V-02 해소).
 */

#include <cstdint>
#include <string>

#include "mrs_map_registry/map_error.hpp"
#include "mrs_map_registry/roadmap_types.hpp"

namespace mrs
{

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
   * @brief 물리(불균일) roadmap 을 로드하고 새 버전을 발급한다.
   * @param[in] map_file_path 지도 파일 경로. 형식은 [0a] 실측 후 확정(수치 창작 금지). 자료형
   * `const std::string &`.
   * @return `MapResult<std::uint64_t>` — 성공 시 새로 발급된 단조증가 roadmap_version.
   *         실패 사유: FILE_NOT_FOUND / FILE_PARSE_ERROR / GEOMETRY_VIOLATION.
   */
  [[nodiscard]] MapResult<std::uint64_t> load_physical_roadmap(const std::string & map_file_path);

  /**
   * @brief 물리 roadmap 의 긴 엣지를 unit 길이로 세분화해 L2 균일 뷰를 만든다 (D-11 v2).
   * @param[in] unit_length_m 세분화 입도 [m]. 이론 하한 2(ρ+L*) 미만이면 거부한다(§6.3). 자료형
   * `double`.
   * @return `MapResult<std::uint32_t>` — 성공 시 생성된 균일 뷰 id.
   *         실패 사유: MAP_NOT_LOADED / UNIT_LENGTH_BELOW_BOUND.
   */
  [[nodiscard]] MapResult<std::uint32_t> build_uniform_view(double unit_length_m);

  /**
   * @brief 균일 뷰의 차수 2 체인 가상 노드를 collapse 해 L3 의존성 골격을 만든다 (D-11 v2).
   * @param[in] uniform_view_id 원본 균일 뷰 id. 자료형 `std::uint32_t`.
   * @return `MapResult<std::uint32_t>` — 성공 시 생성된 골격 뷰 id.
   *         실패 사유: MAP_NOT_LOADED / VIEW_NOT_FOUND.
   */
  [[nodiscard]] MapResult<std::uint32_t> build_dependency_skeleton(std::uint32_t uniform_view_id);

  /**
   * @brief 임의 뷰의 데이터를 조회한다 (GetRoadmap/GetUniformView/GetDependencySkeleton 공통 경로).
   * @param[in] view_id 조회할 뷰 id (물리 roadmap = 0). 자료형 `std::uint32_t`.
   * @return `MapResult<RoadmapViewData>` — 성공 시 뷰 데이터(@ref RoadmapViewData::view_kind 로
   * 종류 지목). 실패 사유: MAP_NOT_LOADED / VIEW_NOT_FOUND.
   */
  [[nodiscard]] MapResult<RoadmapViewData> get_view(std::uint32_t view_id) const;

  /**
   * @brief 한 뷰의 노드를 다른 뷰의 노드로 변환한다 (D-11 무손실 요건의 핵심 연산).
   * @param[in] from_view_id 원본 뷰 id. 자료형 `std::uint32_t`.
   * @param[in] from_node 원본 노드 id (bare — from_view_id 안에서 유효). 자료형 `std::uint32_t`.
   * @param[in] to_view_id 목적 뷰 id. 자료형 `std::uint32_t`.
   * @return `MapResult<std::uint32_t>` — 성공 시 목적 뷰의 대응 노드 id.
   *         실패 사유: MAP_NOT_LOADED / VIEW_NOT_FOUND / NODE_NOT_FOUND / NO_MAPPING /
   * UNSUPPORTED_VIEW.
   * @note 강타입 시그니처(종류가 시그니처에 드러나는 `(PhysicalNodeId) -> UniformNodeId` 형태,
   *       TransformNode.srv §6.1)로의 승격은 파일 4/5 소관이다. 이 시그니처는 그때 좁혀진다.
   */
  [[nodiscard]] MapResult<std::uint32_t> transform_node(
    std::uint32_t from_view_id, std::uint32_t from_node, std::uint32_t to_view_id) const;

  /**
   * @brief roadmap 정적 검사기를 실행한다 (D-11, T1-R6/T5/T6/T7). 맵 로드 시점 + R3 접합 후 재검사.
   *
   * 순수기하 3단(통과 2ρ / 90°점턴 2ρ+L* / U턴 2(ρ+L*))·세분화 입도·r_clear·검사기 4 advisory 는
   * v_max 없이 iw.hub 실측(ρ·L*)만으로 강제된다. v_max 의존 닫힌형은 v_max=0(미측정)이면 미산출
   * 이다(pending-[0b]). 검사기 2(biconnected)는 [1] 에서 미평가(U-23).
   *
   * @param[in] view_id 입도 검사 대상 균일 뷰 id. 0 = 물리 roadmap 만 검사(입도 검사 생략). 자료형
   * `std::uint32_t`.
   * @param[in] robot_count well-formed 검사의 로봇 수. 자료형 `std::uint16_t`.
   * @param[in] robot_radius_m 로봇 반경 r [m] — r_clear 하드 검사·닫힌형용. 자료형 `double`.
   * @param[in] inflation_radius_m NID점 기준 몸체 외접 반경 ρ [m] — 3단·검사기4 임계 근거(편심
   * 몸체라 r 에서 유도 불가, 별도 입력, T1-R6). 자료형 `double`.
   * @param[in] nid_offset_l_m NID 오프셋 L* [m] — 3단 임계(2ρ+L*, 2(ρ+L*))용. R_min <= L* 여야
   * 한다. 자료형 `double`.
   * @param[in] v_max_mps 최대 선속도 [m/s]. 0 = 미측정(pending-[0b]) → 동역학 폭 닫힌형 생략.
   * 자료형 `double`.
   * @param[in] omega_max_rps 최대 각속도 [rad/s]. 순수기하 3단은 이 값 없이 돈다. 자료형 `double`.
   * @return `MapResult<RoadmapValidationResult>` — 성공(검사기가 돌았음) 시 검사 결과. 통과 여부는
   *         결과의 @ref RoadmapValidationResult::result_pass 로 판정한다(검사기가 돈 것과 통과는
   * 다르다). 실패 사유: MAP_NOT_LOADED / VIEW_NOT_FOUND.
   */
  [[nodiscard]] MapResult<RoadmapValidationResult> validate_map(
    std::uint32_t view_id, std::uint16_t robot_count, double robot_radius_m,
    double inflation_radius_m, double nid_offset_l_m, double v_max_mps, double omega_max_rps) const;

  /**
   * @brief 현재 지도 버전을 반환한다 (Q-6 캐시 무효화 키).
   * @return `std::uint64_t` — 현재 roadmap_version. 미로드 상태면 0(@ref
   * ROADMAP_VERSION_UNSPECIFIED).
   */
  [[nodiscard]] std::uint64_t roadmap_version() const noexcept
  {
    return roadmap_version_;
  }

private:
  std::uint64_t roadmap_version_{ROADMAP_VERSION_UNSPECIFIED}; ///< 현재 물리 지도 버전 (단조증가)
};

} // namespace mrs
