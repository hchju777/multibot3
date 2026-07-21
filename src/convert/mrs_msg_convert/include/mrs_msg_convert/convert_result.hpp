#pragma once

/**
 * @file convert_result.hpp
 * @brief 변환 실패 사유(L-26)와 결과 표현 — 이 패키지 전 함수의 반환 규약.
 *
 * ## 왜 값 반환이 아니라 결과 struct 인가
 * 계약은 수신 측 실패를 **폐기 + 로그 + 카운터**로 규정하고 **예외를 금지**한다(계약 §0 실패 거동,
 * §0.1 V3). 값을 그대로 반환하면 "폐기했다"를 알릴 자리가 물리적으로 없다.
 *
 * `bool` 한 비트로도 부족하다 — `PartialReplan.srv` 는 **스코프 불일치(STALE_VERSION)**와
 * **그 밖의 위반(REJECTED_BAD_REQUEST)**을 서로 다른 응답 코드로 구별하라고 요구하기 때문이다.
 * 사유별 폐기 카운터도 1비트로는 만들 수 없다. 그래서 사유를 나르는 결과 struct 를 쓴다.
 * (사용자 결정 R-15 (a))
 *
 * ## 판정은 여기서, 보고는 노드에서
 * 이 패키지는 **판정만** 한다 — 사유를 반환하고 끝이다. 로그 발행·폐기 카운터 적립·안전 폴백은
 * 호출자인 `mrs_ros_*` 노드의 몫이다(사용자 결정 R-15 (b), 계약 §0.1 V3 patch). 이유:
 *  - 아키텍처가 이 패키지의 의존을 `core_types`·`interfaces`·(도메인 타입 헤더)로 못박았다.
 *    `rclcpp` 를 끌어들이면 아키텍처 이탈이고 unit 티어가 ROS 없이 돌지 못한다.
 *  - 로그 스로틀링(`THROTTLE`/`ONCE`)을 적용할 수 있는 곳은 `ros/` 뿐이다 — l4 는 10 Hz × N 으로
 *    돌기 때문에 스로틀 없는 경고는 로그를 덮어버린다.
 *  - 함수 지역 `static` 카운터는 금지다(컴포지션으로 로봇 ×N 이 한 프로세스에 뜨면 공유된다).
 *    순수 함수로 두면 이 함정이 성립하지 않는다.
 *
 * ## 이 헤더는 ROS 를 모른다
 * `ros/` 노드가 사유별로 분기·집계하려면 이 열거를 봐야 하므로 별도 헤더로 분리했다.
 * 그럼에도 `rclcpp` 의존은 없다.
 */

#include <cstdint>

namespace mrs::convert
{

/**
 * @brief 변환 실패 사유 — 계약 L-26 정본표의 미러.
 *
 * @note **값의 정본은 계약 레지스트리 §0.2 표이고 이 enum 은 미러다.** 값을 추가·삭제·병합하려면
 *       계약 변경 게이트를 거쳐야 한다(추가는 minor, 병합·삭제는 major — 과거에 적립된 폐기
 *       카운터의 비교 가능성이 깨지기 때문이다).
 * @warning **사유를 뭉개지 말 것.** 서로 다른 실패를 한 값으로 접으면 사후 감사에서 원인을 잃는다.
 *          같은 함정이 이미 계약에 기록돼 있다 — 동기 결손(SYNC_LOST)을 솔버 실패(QP_INFEASIBLE)로
 *          보내면 사다리가 계획 문제로 오인해 재계획을 오발동하고, 그 결과 신규성 판정(F2 R2
 *          발동률)과 수렴 판정(F6 스래싱)의 계측이 오염된다.
 */
enum class ConvertStatus : std::uint8_t
{
  /** @brief 성공. 출력 파라미터가 유효하다. */
  OK = 0,

  /**
   * @brief 뷰 **종류**가 계약이 그 필드에 고정한 종류와 다르다 (규칙 V1 위반).
   * @note 발행자가 잘못된 종류를 랩해 보낸 경우다. 강타입이 DDS 경계에서 소실되기 때문에
   *       타입으로 잡히지 않고 여기서 잡힌다.
   */
  VIEW_KIND_MISMATCH = 1,

  /**
   * @brief 뷰 **인스턴스**가 수신자의 기대 스코프와 다르다 (규칙 V2 위반).
   * @note 지도 개정·재세분화로 id 공간이 달라진 상태다. 종류는 맞지만 가리키는 지도가 다르다.
   *       서비스 응답으로는 **STALE_VERSION** 에 대응하며, 노드는 MapRegistry 재조회로 회복한다.
   */
  VIEW_SCOPE_MISMATCH = 2,

  /** @brief 와이어 스키마 버전이 이 빌드가 아는 버전과 다르다. */
  SCHEMA_VERSION_MISMATCH = 3,

  /** @brief 필수 필드가 계약이 규정한 범위·불변식을 위반했다(미할당 상관 키, 음수 길이 등). */
  FIELD_RANGE_VIOLATION = 4,

  /**
   * @brief 열거 필드의 값이 이 빌드가 아는 집합 밖이다.
   * @warning **기본값으로 치환하지 말 것.** 상위 버전 발행자가 보낸 새 값을 조용히 오해석하는
   *          경로가 되며, 그것이 위 "사유 뭉갬"의 가장 흔한 형태다. 폐기가 정답이다.
   */
  ENUM_OUT_OF_RANGE = 5,

  /**
   * @brief 시각 변환 가드 위반 (NaN·음수·표현 범위 초과).
   * @note 시각 변환이 9곳에서 반복되므로 가드를 이 패키지 단일 헬퍼에 두고 사유도 하나로 모은다.
   */
  TIME_CONVERSION_GUARD = 6
};

/**
 * @brief 변환 결과 — 성공 여부와 실패 사유.
 *
 * 출력 값 자체는 별도 out 파라미터로 받는다. 실패 시 out 파라미터의 내용은 **정의되지 않으며**
 * 호출자는 이를 사용해서는 안 된다(부분 채움 상태일 수 있다).
 */
struct ConvertResult
{
  /** @brief 성공 여부. 실패 시 out 파라미터를 쓰지 말 것. */
  bool ok{false};
  /** @brief 실패 사유. `ok` 가 true 이면 항상 @ref ConvertStatus::OK 다. */
  ConvertStatus reason{ConvertStatus::OK};
};

/**
 * @brief 성공 결과를 만든다.
 * @return `ConvertResult` — `{ok = true, reason = OK}`.
 */
[[nodiscard]] constexpr ConvertResult convert_ok() noexcept
{
  return ConvertResult{true, ConvertStatus::OK};
}

/**
 * @brief 실패 결과를 만든다.
 * @param[in] reason 실패 사유. 자료형 `ConvertStatus`. @ref ConvertStatus::OK 를 넘기지 말 것.
 * @return `ConvertResult` — `{ok = false, reason = reason}`.
 */
[[nodiscard]] constexpr ConvertResult convert_fail(ConvertStatus reason) noexcept
{
  return ConvertResult{false, reason};
}

/**
 * @brief 실패 사유를 로그·카운터 키로 쓸 짧은 문자열로 바꾼다.
 *
 * 호출자(`ros/` 노드)가 사유별 카운터 라벨과 로그 메시지에 쓴다. 값이 아니라 **이름**을 남겨야
 * 사후 감사에서 사유를 복원할 수 있다.
 *
 * @param[in] status 실패 사유. 자료형 `ConvertStatus`.
 * @return `const char *` — 널 종료 문자열. 정적 수명이므로 소유권을 넘기지 않는다.
 *         알 수 없는 값이면 `"UNKNOWN"`.
 */
[[nodiscard]] constexpr const char * to_string(ConvertStatus status) noexcept
{
  switch (status)
  {
    case ConvertStatus::OK:
      return "OK";
    case ConvertStatus::VIEW_KIND_MISMATCH:
      return "VIEW_KIND_MISMATCH";
    case ConvertStatus::VIEW_SCOPE_MISMATCH:
      return "VIEW_SCOPE_MISMATCH";
    case ConvertStatus::SCHEMA_VERSION_MISMATCH:
      return "SCHEMA_VERSION_MISMATCH";
    case ConvertStatus::FIELD_RANGE_VIOLATION:
      return "FIELD_RANGE_VIOLATION";
    case ConvertStatus::ENUM_OUT_OF_RANGE:
      return "ENUM_OUT_OF_RANGE";
    case ConvertStatus::TIME_CONVERSION_GUARD:
      return "TIME_CONVERSION_GUARD";
  }
  return "UNKNOWN";
}

} // namespace mrs::convert
