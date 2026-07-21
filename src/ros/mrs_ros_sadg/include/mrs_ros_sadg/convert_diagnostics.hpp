#pragma once

/**
 * @file convert_diagnostics.hpp
 * @brief 변환 실패 사유별 폐기 카운터 — "판정은 convert, 보고는 노드"(R-15 (b) = b3)의 노드 측.
 *
 * `mrs_msg_convert` 는 사유만 반환하고 로그·카운터·안전 폴백은 호출 노드의 몫이다
 * (계약 §0.1 V3 patch, `convert_result.hpp` 머리말). 이 파일이 그 "카운터" 부분이다.
 *
 * ## 왜 사유별로 세는가
 * 계약 §0.2 는 사유를 뭉개는 것을 금지한다 — 서로 다른 실패를 한 칸에 접으면 사후 감사에서
 * "왜 폐기됐는가"를 복원할 수 없다. `ExecutionWindow`(★ load-bearing seam)의 폐기 사유가
 * 뭉개지면 [0a] 배관 실측이 곧바로 원인을 잃는다.
 *
 * ## 함수 지역 `static` 을 쓰지 않는 이유
 * Nav2 참조(§D2)가 명시 금지했다 — 컴포지션으로 노드 ×N 이 한 프로세스에 뜨면 카운터가 공유되어
 * 노드별 분해가 불가능해진다. 그래서 **노드 인스턴스가 소유하는 값 객체**로 둔다.
 *
 * @note ⚠ 이 헤더는 `mrs_ros_pp` 의 동명 헤더와 내용이 같다. [0a] 시점에 `ros/` 공통 패키지가
 *       없어 패키지별로 사본을 둔 것이며, **공통 `ros/` 헬퍼 패키지로 승격할 대상**이다.
 */

#include <array>
#include <cstddef>
#include <cstdint>

#include "mrs_msg_convert/convert_result.hpp"

namespace mrs::ros_sadg
{

/**
 * @brief `ConvertStatus` 열거자 개수 (OK 포함). 카운터 배열의 크기다.
 * @note 계약 §0.2 표에 사유가 추가되면 이 값도 늘어난다.
 */
inline constexpr std::size_t CONVERT_STATUS_COUNT = 7U;

/**
 * @brief 변환이 "미구현 스텁"에 부딪혔는지 판정한다.
 *
 * `mrs_msg_convert` 의 미구현 함수는 기본 생성 결과(`ok = false`, `reason = OK`)를 반환한다 —
 * 미구현을 실제 실패 사유처럼 보이는 값으로 채우지 않기 위한 규약이다
 * (`core_seam_convert.cpp` 머리말). 노드가 이를 구별하지 못하면 **배관이 막힌 것**과
 * **메시지가 잘못된 것**이 같은 로그로 보이고, [0a] 실측이 원인을 잃는다.
 *
 * @param[in] result 변환 결과. 자료형 `mrs::convert::ConvertResult`.
 * @return `bool` — 실패이면서 사유가 `OK`(= 미구현 스텁의 서명)이면 true, 그 밖이면 false.
 */
[[nodiscard]] inline bool is_unimplemented_convert(
  const mrs::convert::ConvertResult & result) noexcept
{
  return !result.ok && result.reason == mrs::convert::ConvertStatus::OK;
}

/**
 * @brief 변환 실패 사유별 폐기 카운터.
 *
 * 예외를 던지지 않으며 할당도 하지 않는다 — 릴리스 콜백에서 호출되기 때문이다.
 */
class ConvertDiagnostics final
{
public:
  ConvertDiagnostics() = default;

  /**
   * @brief 실패 1건을 사유 칸에 적립한다.
   * @param[in] status 변환 실패 사유. 자료형 `mrs::convert::ConvertStatus`.
   *            `OK` 도 유효한 인자다 — 미구현 스텁(@ref is_unimplemented_convert)의 적립처다.
   * @return `std::uint64_t` — 적립 후 그 사유의 누적 폐기 건수.
   */
  std::uint64_t record(mrs::convert::ConvertStatus status) noexcept
  {
    const std::size_t index = static_cast<std::size_t>(status);
    if (index >= CONVERT_STATUS_COUNT)
    {
      ++total_;
      return 0U;
    }

    ++counters_[index];
    ++total_;
    return counters_[index];
  }

  /**
   * @brief 특정 사유의 누적 폐기 건수를 조회한다.
   * @param[in] status 조회할 사유. 자료형 `mrs::convert::ConvertStatus`.
   * @return `std::uint64_t` — 누적 건수. 열거 밖 값이면 0.
   */
  [[nodiscard]] std::uint64_t count(mrs::convert::ConvertStatus status) const noexcept
  {
    const std::size_t index = static_cast<std::size_t>(status);
    return index < CONVERT_STATUS_COUNT ? counters_[index] : 0U;
  }

  /**
   * @brief 전체 폐기 건수를 조회한다.
   * @return `std::uint64_t` — 사유를 불문한 누적 폐기 건수.
   */
  [[nodiscard]] std::uint64_t total() const noexcept
  {
    return total_;
  }

private:
  std::array<std::uint64_t, CONVERT_STATUS_COUNT> counters_{}; ///< 사유별 누적 폐기 건수
  std::uint64_t total_{0};                                     ///< 전체 누적 폐기 건수
};

} // namespace mrs::ros_sadg
