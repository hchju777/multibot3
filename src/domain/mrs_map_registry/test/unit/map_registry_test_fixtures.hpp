#pragma once

/**
 * @file map_registry_test_fixtures.hpp
 * @brief unit 티어 회귀 테스트 공용 픽스처 — 손선언 골든 상수, 임시 YAML 파일 RAII, YAML 조립 헬퍼.
 *
 * ## 이 헤더가 지키는 규율
 * - **손선언 우선(R-14)**: 골든 상수는 구현을 호출해 얻지 않고 D-11 v6 스펙에서 손으로 적는다.
 * - **공개 헤더만 include**: 테스트는 `mrs_map_registry/map_registry.hpp` 만 본다. 내부
 *   `src/map_registry_impl.hpp` 는 인용하지 않는다(설치되지 않으며, 테스트는 파사드 계약만 검증).
 * - **YAML-only 로드 경로**: 공개 API 에 메모리 주입이 없으므로 픽스처는 임시 파일을 런타임에 쓰고
 *   (@ref TempYamlFile) 소멸 시 정리한다. 좌표·폭 등 부동소수는 17 유효숫자로 직렬화해
 *   parse(print(x)) == x 왕복을 보장한다(경계 임계 비교의 결정성).
 *
 * @note 헬퍼 함수·타입에도 코드 작성 규율(문서화 주석·Allman·컨벤션)을 적용한다(test-authoring
 * 스킬).
 */

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <sstream>
#include <string>
#include <vector>

#include <unistd.h>

#include "mrs_map_registry/map_registry.hpp"

namespace mrs_map_registry_test
{

// ── 손선언 골든 상수 (D-11 v6 · iw.hub 실측 기하 — 구현에서 역산 금지) ──────────────────────
/** @brief NID점 몸체 외접 반경 ρ [m] (iw.hub 실측). 검사기 3 통과·90°점턴·U턴 임계의 근거. */
inline constexpr double RHO = 0.787904;
/** @brief NID 오프셋 L* [m] (iw.hub 실측). 90°점턴(2ρ+L*)·U턴(2(ρ+L*)) 임계의 근거. */
inline constexpr double LSTAR = 0.317855;
/** @brief 로봇 반경 r [m] (iw.hub 실측). r_clear 하드 하한. */
inline constexpr double ROBOT_R = 1.084724;

/** @brief 통과폭 하한 2ρ [m] — 손선언 (validate_map 이 ρ 에서 에코하는 값). strict `>` 비교. */
inline constexpr double PASS_MIN = 1.575808;
/** @brief 제자리 90° 회전 하한 2ρ+L* [m] — 손선언. 미만이면 turn90_incapable. */
inline constexpr double TURN90_MIN = 1.893663;
/** @brief 360° 무후진 U턴 하한 2(ρ+L*) [m] — 손선언. 세분화 입도 하한이자 build_uniform 하한. */
inline constexpr double UTURN_MIN = 2.211518;
/**
 * @brief 교행(검사기 4 advisory) 하한 4ρ [m] — 손선언.
 * @note 문서(D-11·HANDOFF)에는 3.151615 로 적혀 있으나 4×0.787904 = 3.151616 이 산술적으로 옳고
 *       코드가 4ρ 를 계산하므로 골든은 3.151616 이다. 문서값과의 ~1e-6 불일치는 검사기 테스트에서
 *       명시적으로 재확인한다(vacuous 통과 방지).
 */
inline constexpr double MEET_MIN = 3.151616;
/** @brief 세분화 입도 하한 [m] — build_uniform 의 두 번째 인자로 넘길 값 = 2(ρ+L*). */
inline constexpr double LOWER_BOUND = 2.211518;

/**
 * @brief double 을 17 유효숫자로 직렬화한다 — parse(print(x)) == x 왕복 보장(경계 임계 결정성).
 * @param[in] value 직렬화할 값. 자료형 `double`.
 * @return `std::string` — 왕복 무손실 십진 표현.
 */
inline std::string fmt(double value)
{
  std::ostringstream oss;
  oss << std::scientific;
  oss.precision(17);
  oss << value;
  return oss.str();
}

/**
 * @brief 임시 YAML 파일 RAII — 생성자에서 내용을 쓰고 소멸자에서 지운다.
 *
 * 공개 API 는 파일 경로만 받으므로(YAML-only 결정) 테스트는 실제 파일을 만들어 로드한다. 고유
 * 경로는 프로세스 임시 디렉터리 + 단조 증가 카운터로 생성하여 병렬 테스트 충돌을 피한다.
 */
class TempYamlFile
{
public:
  /**
   * @brief 고유 임시 경로에 내용을 쓴다.
   * @param[in] content 파일에 쓸 YAML 문자열. 자료형 `const std::string &`.
   */
  explicit TempYamlFile(const std::string & content)
  {
    static std::uint64_t counter = 0U;
    std::ostringstream name;
    name << "mrs_map_registry_test_" << ::getpid() << "_" << counter++ << ".yaml";
    path_ = std::filesystem::temp_directory_path() / name.str();
    std::ofstream ofs(path_, std::ios::out | std::ios::trunc);
    ofs << content;
    ofs.close();
  }

  /** @brief 임시 파일을 지운다(존재하면). 예외를 던지지 않는다. */
  ~TempYamlFile()
  {
    std::error_code ec;
    std::filesystem::remove(path_, ec);
  }

  TempYamlFile(const TempYamlFile &) = delete;
  TempYamlFile & operator=(const TempYamlFile &) = delete;

  /**
   * @brief 파일 경로 문자열을 반환한다.
   * @return `std::string` — load_physical_roadmap 에 넘길 경로.
   */
  [[nodiscard]] std::string path() const
  {
    return path_.string();
  }

  /**
   * @brief 파일을 지워 존재하지 않는 경로로 만든다(FILE_NOT_FOUND 유도용).
   * @return `std::string` — 이제 존재하지 않는 경로.
   */
  [[nodiscard]] std::string remove_and_get_missing_path()
  {
    std::error_code ec;
    std::filesystem::remove(path_, ec);
    return path_.string();
  }

private:
  std::filesystem::path path_; ///< 임시 파일 경로
};

/**
 * @brief 물리 노드 1건의 YAML 인라인 맵 라인을 만든다.
 * @param[in] id 노드 id. 자료형 `std::uint32_t`.
 * @param[in] x 위치 x [m]. 자료형 `double`.
 * @param[in] y 위치 y [m]. 자료형 `double`.
 * @param[in] claim_radius r_clear [m]. 자료형 `double`.
 * @param[in] is_endpoint 엔드포인트 여부. 자료형 `bool`.
 * @param[in] is_task_endpoint 작업 엔드포인트 여부. 자료형 `bool`.
 * @return `std::string` — `  - {id: .., x: .., ...}\n` 형태의 한 줄.
 */
inline std::string node_line(
  std::uint32_t id, double x, double y, double claim_radius, bool is_endpoint,
  bool is_task_endpoint)
{
  std::ostringstream oss;
  oss << "  - {id: " << id << ", x: " << fmt(x) << ", y: " << fmt(y)
      << ", claim_radius: " << fmt(claim_radius)
      << ", is_endpoint: " << (is_endpoint ? "true" : "false")
      << ", is_task_endpoint: " << (is_task_endpoint ? "true" : "false") << "}\n";
  return oss.str();
}

/**
 * @brief 물리 엣지 1건의 YAML 인라인 맵 라인을 만든다(speed_limit 생략 = 기본 0).
 * @param[in] id 엣지 id. 자료형 `std::uint32_t`.
 * @param[in] a 양단 노드 a id. 자료형 `std::uint32_t`.
 * @param[in] b 양단 노드 b id. 자료형 `std::uint32_t`.
 * @param[in] width 최소 통로 폭 [m]. 자료형 `double`.
 * @return `std::string` — `  - {id: .., a: .., b: .., width: ..}\n` 형태의 한 줄.
 */
inline std::string edge_line(std::uint32_t id, std::uint32_t a, std::uint32_t b, double width)
{
  std::ostringstream oss;
  oss << "  - {id: " << id << ", a: " << a << ", b: " << b << ", width: " << fmt(width) << "}\n";
  return oss.str();
}

/**
 * @brief 뷰별 강타입 엣지 id 벡터에 특정 bare id 가 들어 있는지 검사한다.
 * @tparam IdT 강타입 엣지 id (`PhysicalEdgeId`/`UniformEdgeId` 등). `.value()` 를 제공해야 한다.
 * @param[in] ids 검사할 위반/분류 배열. 자료형 `const std::vector<IdT> &`.
 * @param[in] raw 찾을 bare 엣지 id. 자료형 `std::uint32_t`.
 * @return `bool` — 포함되어 있으면 true.
 */
template <class IdT>
[[nodiscard]] bool contains_edge(const std::vector<IdT> & ids, std::uint32_t raw)
{
  for (const IdT & id : ids)
  {
    if (id.value() == raw)
    {
      return true;
    }
  }
  return false;
}

/**
 * @brief 뷰별 강타입 노드 id 벡터에 특정 bare id 가 들어 있는지 검사한다.
 * @tparam IdT 강타입 노드 id. `.value()` 를 제공해야 한다.
 * @param[in] ids 검사할 위반 배열. 자료형 `const std::vector<IdT> &`.
 * @param[in] raw 찾을 bare 노드 id. 자료형 `std::uint32_t`.
 * @return `bool` — 포함되어 있으면 true.
 */
template <class IdT>
[[nodiscard]] bool contains_node(const std::vector<IdT> & ids, std::uint32_t raw)
{
  for (const IdT & id : ids)
  {
    if (id.value() == raw)
    {
      return true;
    }
  }
  return false;
}

} // namespace mrs_map_registry_test
