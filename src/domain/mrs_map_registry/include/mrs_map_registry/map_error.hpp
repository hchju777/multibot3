#pragma once

/**
 * @file map_error.hpp
 * @brief MapRegistry 도메인 실패 사유(@ref mrs::MapStatus)와 값을 실은 결과(@ref mrs::MapResult) —
 *        이 패키지 공개 함수의 반환 규약. **예외를 던지지 않는다**(CLAUDE.md 규율 2, 계약 §0.1 V3).
 *
 * ## 왜 예외가 아니라 반환값인가
 * 계약과 코드 작성 규율은 실패를 **반환값으로** 나르고 콜백은 죽지 않게 하라고 규정한다. 과거
 * `map_registry.cpp` 의 6 스텁이 `throw std::logic_error` 였던 것(V-02)을 이 규약으로 교체한다.
 *
 * ## 왜 값을 실은 결과인가 (convert 의 @ref mrs::convert::ConvertResult 와 다른 점)
 * `mrs_msg_convert` 는 **out 파라미터 + status** 형태를 쓴다 — 변환 함수가 자연스러운 단일
 * 반환값을 갖지 않기 때문이다. 반대로 MapRegistry 함수는 **자연히 값을 반환한다**(새 지도 버전·
 * 발급된 view_id·뷰 데이터·변환된 노드). 그래서 값을 결과에 실어 호출부가 out 파라미터 없이
 * `result.value` 로 받게 한다(사용자 결정, 파일 3). 실패 시 `value` 는 기본값이며 사용하면 안 된다.
 *
 * ## 판정은 도메인, 보고는 노드
 * @ref mrs::MapStatus 는 **도메인 실패 어휘**다. 로그 발행·폐기 카운터 적립·안전 폴백·서비스
 * 응답 코드(`ValidateMap`/`GetUniformView`/`TransformNode` 의 `RESULT_*`)로의 접기는 호출자인
 * `mrs_ros_map_registry` 노드의 몫이다. 이 헤더는 **`rclcpp` 를 모른다**(아키텍처 제약 C1) —
 * `mrs_map_registry` 는 `mrs_core_types` 에만 의존하는 순수 도메인 패키지다.
 *
 * ## @ref mrs::MapStatus 는 계약 미러가 아니다
 * `ConvertStatus` 는 계약 레지스트리 §0.2 정본표(L-26)의 미러였다. 그러나 도메인 MapStatus 에
 * 대응하는 **단일 계약표는 없다** — 와이어 실패 코드는 서비스마다 제각각인 `RESULT_*` 다. 따라서
 * MapStatus 값 추가·삭제는 계약 변경 게이트 대상이 아니며 구현 진행에 맞춰 자유롭게 진화한다.
 * (호출 노드가 MapStatus → 서비스 `RESULT_*` 로 접는 표는 노드 헤더가 소유한다.)
 */

#include <cstdint>
#include <utility>

namespace mrs
{

/**
 * @brief MapRegistry 도메인 연산의 실패 사유. 성공은 @ref MapStatus::OK 하나뿐이다.
 *
 * @warning **사유를 뭉개지 말 것.** 서로 다른 실패를 한 값으로 접으면 사후 감사에서 원인을 잃는다
 *          (같은 함정이 계약 §0.2 와 convert 헤더에 기록돼 있다). 새 실패 유형이 생기면 값을
 *          추가한다 — 기존 값으로 억지로 밀어 넣지 않는다.
 */
enum class MapStatus : std::uint8_t
{
  /** @brief 성공. 결과의 `value` 가 유효하다. */
  OK = 0,

  /**
   * @brief 아직 구현되지 않은 연산 (전이값 — 파일 4 로더·검사기 착지 시 각 스텁에서 사라진다).
   * @note 스텁이 예외 대신 반환하는 안전 폴백이다. 실패로 취급되며 `value` 를 쓰면 안 된다.
   *       구현이 채워지면 이 값을 반환하는 경로가 0 이 되고, 그때 이 열거자를 제거한다.
   */
  NOT_IMPLEMENTED = 1,

  /** @brief 물리 roadmap 이 아직 로드되지 않았다 (load 전에 뷰·변환·검사를 요청). */
  MAP_NOT_LOADED = 2,

  /** @brief 지도 파일을 열 수 없다 (경로 부재·권한). */
  FILE_NOT_FOUND = 3,

  /** @brief 지도 파일을 열었으나 형식·불변식 위반으로 파싱 실패 (직선 전제·참조 무결성 등). */
  FILE_PARSE_ERROR = 4,

  /**
   * @brief 요청한 지도 버전이 현재 버전과 다르다 (지도 개정·재세분화로 id 공간이 갱신됨).
   * @note 서비스 응답으로는 각 서비스의 `RESULT_STALE_VERSION` 에 대응한다. 노드는 재조회로
   * 회복한다.
   */
  STALE_VERSION = 5,

  /** @brief 지정한 `view_id` 의 뷰가 존재하지 않는다. */
  VIEW_NOT_FOUND = 6,

  /** @brief 지정한 노드 id 가 해당 뷰에 존재하지 않는다. */
  NODE_NOT_FOUND = 7,

  /** @brief 뷰 간 대응(세분화·collapse 표)에 이 노드의 대응 항목이 없다 (왕복 무손실 위반 후보). */
  NO_MAPPING = 8,

  /**
   * @brief 세분화 입도가 이론 하한 미만이다 (`unit_length_m < 2(ρ + L*)`, T1-R6).
   * @note 하한을 코드가 강제한다 — 파라미터로 노출하되 미만 값은 뷰 생성을 거부한다(§6.3).
   */
  UNIT_LENGTH_BELOW_BOUND = 9,

  /** @brief 기하 전제 위반 (물리 엣지 비직선 `|length − ‖p_a−p_b‖| > tol` 등, RoadmapEdge.msg (g)).
   */
  GEOMETRY_VIOLATION = 10,

  /** @brief 이 연산이 지원하지 않는 뷰 종류다 (`TransformNode` 의 `RESULT_UNSUPPORTED_VIEW` 대응).
   */
  UNSUPPORTED_VIEW = 11
};

/**
 * @brief 실패 사유를 로그·카운터 키로 쓸 짧은 문자열로 바꾼다.
 *
 * 값이 아니라 **이름**을 남겨야 사후 감사에서 사유를 복원할 수 있다. 호출 노드가 사유별 카운터
 * 라벨과 로그 메시지에 쓴다.
 *
 * @param[in] status 실패 사유. 자료형 @ref MapStatus.
 * @return `const char *` — 널 종료 문자열. 정적 수명이라 소유권을 넘기지 않는다. 미지값이면
 * `"UNKNOWN"`.
 */
[[nodiscard]] constexpr const char * to_string(MapStatus status) noexcept
{
  switch (status)
  {
    case MapStatus::OK:
      return "OK";
    case MapStatus::NOT_IMPLEMENTED:
      return "NOT_IMPLEMENTED";
    case MapStatus::MAP_NOT_LOADED:
      return "MAP_NOT_LOADED";
    case MapStatus::FILE_NOT_FOUND:
      return "FILE_NOT_FOUND";
    case MapStatus::FILE_PARSE_ERROR:
      return "FILE_PARSE_ERROR";
    case MapStatus::STALE_VERSION:
      return "STALE_VERSION";
    case MapStatus::VIEW_NOT_FOUND:
      return "VIEW_NOT_FOUND";
    case MapStatus::NODE_NOT_FOUND:
      return "NODE_NOT_FOUND";
    case MapStatus::NO_MAPPING:
      return "NO_MAPPING";
    case MapStatus::UNIT_LENGTH_BELOW_BOUND:
      return "UNIT_LENGTH_BELOW_BOUND";
    case MapStatus::GEOMETRY_VIOLATION:
      return "GEOMETRY_VIOLATION";
    case MapStatus::UNSUPPORTED_VIEW:
      return "UNSUPPORTED_VIEW";
  }
  return "UNKNOWN";
}

/**
 * @brief 값을 실은 결과 — 성공 여부(@ref status)와 성공 시의 반환값(@ref value).
 *
 * 호출부는 `result.is_ok()` 로 성공을 확인한 뒤에만 `result.value` 를 읽는다. 실패 시 `value` 는
 * 기본 생성 상태(정의되지 않은 유효값)이며 사용해서는 안 된다. 이 타입은 집합체(aggregate)라
 * `MapResult<T>{MapStatus::OK, v}`(성공)·`MapResult<T>{reason}`(실패, value 기본)로 초기화한다.
 *
 * @tparam T 성공 시 반환할 값의 자료형. 기본 생성 가능해야 한다.
 */
template <class T> struct MapResult
{
  /** @brief 결과 상태. 실패 시 `value` 를 쓰지 말 것. */
  MapStatus status{MapStatus::OK};
  /** @brief 성공 시의 반환값. `status != OK` 이면 정의되지 않는다. */
  T value{};

  /**
   * @brief 성공 여부.
   * @return `bool` — @ref status 가 @ref MapStatus::OK 이면 true.
   */
  [[nodiscard]] constexpr bool is_ok() const noexcept
  {
    return status == MapStatus::OK;
  }

  /**
   * @brief 성공 결과를 만든다.
   * @param[in] value 반환할 값. 자료형 `T`. 이동 대입된다.
   * @return `MapResult<T>` — `{OK, value}`.
   */
  [[nodiscard]] static MapResult ok(T value)
  {
    return MapResult{MapStatus::OK, std::move(value)};
  }

  /**
   * @brief 실패 결과를 만든다.
   * @param[in] reason 실패 사유. 자료형 @ref MapStatus. @ref MapStatus::OK 를 넘기지 말 것.
   * @return `MapResult<T>` — `{reason, T{}}`. `value` 는 기본값이며 소비 금지.
   */
  [[nodiscard]] static MapResult fail(MapStatus reason)
  {
    return MapResult{reason, T{}};
  }
};

/**
 * @brief 반환값이 없는 연산의 결과 — 성공 여부만 나른다 (@ref MapResult 의 `void` 특수화).
 */
template <> struct MapResult<void>
{
  /** @brief 결과 상태. */
  MapStatus status{MapStatus::OK};

  /**
   * @brief 성공 여부.
   * @return `bool` — @ref status 가 @ref MapStatus::OK 이면 true.
   */
  [[nodiscard]] constexpr bool is_ok() const noexcept
  {
    return status == MapStatus::OK;
  }

  /**
   * @brief 성공 결과를 만든다.
   * @return `MapResult<void>` — `{OK}`.
   */
  [[nodiscard]] static constexpr MapResult ok() noexcept
  {
    return MapResult{MapStatus::OK};
  }

  /**
   * @brief 실패 결과를 만든다.
   * @param[in] reason 실패 사유. 자료형 @ref MapStatus. @ref MapStatus::OK 를 넘기지 말 것.
   * @return `MapResult<void>` — `{reason}`.
   */
  [[nodiscard]] static constexpr MapResult fail(MapStatus reason) noexcept
  {
    return MapResult{reason};
  }
};

} // namespace mrs
