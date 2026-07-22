#pragma once

/**
 * @file view_ids.hpp
 * @brief 뷰별 노드·엣지 id 강타입과 뷰 스코프 — 계약 §0.1 규칙 V1~V4 의 C++ 구현.
 *
 * MapRegistry(D-11 v2)는 **id 공간이 서로 다른 3개의 뷰**를 다룬다:
 *  - **물리(PHYSICAL)**   : 창고 레이아웃에서 추출한 원본 roadmap. 엣지 길이가 불균일하다.
 *  - **균일(UNIFORM)**    : 긴 엣지를 unit 길이 가상 노드로 세분화한 L2 제공용 뷰.
 *  - **골격(SKELETON)**   : 차수 2 체인의 가상 노드를 collapse 한 L3 의존성 그래프용 뷰.
 *
 * 세 뷰의 노드·엣지 id 를 전부 맨 `std::uint32_t` 로 두면 **잘못된 뷰의 id 를 넘겨도 컴파일된다**.
 * 이 오류가 특히 위험한 이유는 두 가지다:
 *  1. 뷰가 달라도 id 는 대개 상대 뷰에서 **유효한 값**이라 범위 검사로 잡히지 않는다.
 *  2. 증상이 "L2 계획이 L4 에서 무효"로 나타나는데 **그것이 정확히 D-11 의 통과 지표
 *     (planning error ≈ 0)** 다 — 타입 실수 1건이 seam 알고리즘 실패로 오독된다.
 *
 * 따라서 뷰를 **타입으로** 구분한다(사용자 결정 R-14 (a) = B). 부수 효과로, 왕복 무손실
 * 테스트가 타입상 f∘g 가 아니면 컴파일되지 않아 "항등 테스트인 척"이 원천 차단된다.
 *
 * ## 노드와 엣지도 서로 다른 타입이다 (L-27, 재잠금 8차)
 * `ViewNodeId<K>` 와 `ViewEdgeId<K>` 는 같은 뷰 종류라도 **엔티티 태그가 달라 서로 다른 타입**이다.
 * 노드 id 를 엣지 자리에(또는 그 반대로) 넘기는 실수도 컴파일 단계에서 막힌다. 이 구분은
 * `ValidateMap` 의 뷰별 위반 배열(`violating_physical_edge_ids` vs `violating_uniform_edge_ids`)을
 * **이름 규약이 아니라 타입으로** 강제하기 위한 것이다(재잠금 8차 (b)·(e-iii)).
 *
 * ## ⚠ 강타입은 **타입**만 구속하고 **값**은 구속하지 않는다
 * 강타입은 "이 자리에 이 뷰의 이 엔티티 id 가 와야 한다"를 컴파일 시각에 강제하지만,
 * `explicit ViewNodeId<UNIFORM>{physical_raw_value}` 처럼 **틀린 뷰의 원시 값을 생성자에 그대로
 * 넣는 실수는 컴파일러가 잡지 못한다** — 값의 출처는 타입에 실리지 않기 때문이다. 이 사각의 방어는
 * 강타입이 아니라 (i) CI 검사 [E](강타입 생성자 직접 호출을 뷰 소유자·`mrs_msg_convert` 로 한정)와
 * (ii) 비조밀(non-dense) 물리 id 픽스처(뷰마다 id 공간을 어긋나게 배치해 오사용이 왕복 테스트에서
 * 드러나게 함)가 맡는다. 강타입의 항등-테스트 차단(위)을 이 값 사각까지로 과장하지 말 것(R-14
 * 과주장 정정).
 *
 * ## 강타입만으로는 부족하다 — `ViewScope` 가 필요한 이유
 * 강타입은 **프로세스 안에서만** 유효하고 DDS 경계에서는 전부 `uint32` 로 직렬화되어 소실된다.
 * 경계 너머의 오용은 타입으로 잡히지 않으므로 런타임 대조가 필요하다(R-14 (a) = C).
 * **C 는 B 의 대안이 아니라 전제다.**
 *
 * ## 변환 단일 소유 (규칙 V3)
 * `std::uint32_t` ↔ 강타입 변환은 **`mrs_msg_convert` 에서만** 수행한다. 이 헤더가 변환을
 * `explicit` 으로 막아 두는 것은 그 규칙을 문법으로 거드는 장치이며, 규칙 자체의 강제는
 * CI 경계 검사(`tools/check_boundaries.sh`)가 맡는다. V3 는 wire↔도메인 경계 변환에 적용되며,
 * 뷰 소유자(`mrs_map_registry`)가 자기 뷰의 id 를 신규 발급하거나 내부 조밀 인덱스와 사상하는
 * 것은 V3 의 변환이 아니다(재잠금 8차 (k), 면제는 역할로 한정).
 *
 * @note 이 헤더는 `mrs_core_types` 에 놓이며 **rclcpp·mrs_interfaces 에 의존하지 않는다**
 *       (아키텍처 제약 C1).
 * @note 구현 형태(재잠금 8차): 노드·엣지의 공통 로직은 phantom 태그 기반 `ViewId<K, Entity>`
 *       하나에 두고, `ViewNodeId<K>`·`ViewEdgeId<K>` 는 그 별칭 템플릿이다. 공개 인터페이스
 *       (이름·의미·레이아웃·`std::hash`)는 노드 3종에 대해 종전과 동일하다 — 잠긴 계약면 불변.
 */

#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>

namespace mrs
{

/**
 * @brief id 가 속한 뷰의 **종류**. `ViewScope.msg` 의 `VIEW_KIND_*` 상수와 값이 일치한다.
 *
 * @note 이 enum 은 `ViewScope.msg` 의 **미러이지 정본이 아니다** — 정본은 계약 레지스트리
 *       §0.1 의 뷰 종류 정본표다. 값을 바꾸려면 계약 변경 게이트를 거쳐야 한다.
 */
enum class ViewKind : std::uint8_t
{
  PHYSICAL = 0, ///< 물리 roadmap (불균일 엣지, 자동 추출 원본). `view_id` 는 항상 0
  UNIFORM = 1, ///< L2 세분화 균일 뷰 (가상 노드 포함). `view_id` = GetUniformView 발급값
  SKELETON = 2 ///< L3 collapse 골격. `view_id` = 유래 균일 뷰의 id (종류만 다르다)
};

namespace detail
{

/** @brief phantom 엔티티 태그 — 노드. `ViewId` 를 노드용으로 특수화한다(엣지와 타입 분리). */
struct NodeEntity final
{
};
/** @brief phantom 엔티티 태그 — 엣지. `ViewId` 를 엣지용으로 특수화한다(노드와 타입 분리). */
struct EdgeEntity final
{
};

} // namespace detail

/**
 * @brief 뷰 종류와 엔티티 종류를 컴파일 시각에 고정한 식별자 (phantom tag 강타입).
 *
 * `std::uint32_t` 와도, 다른 뷰·다른 엔티티의 `ViewId` 와도 **암묵 변환되지 않는다**. 크기와
 * 표현은 `std::uint32_t` 와 같으므로 직렬화·메모리 비용이 늘지 않는다(파일 하단 `static_assert`).
 * 직접 쓰지 말고 별칭 `ViewNodeId<K>`·`ViewEdgeId<K>` 를 사용한다.
 *
 * @tparam K 이 id 가 유효한 뷰의 종류. 계약이 필드마다 고정한다(규칙 V1).
 * @tparam Entity 엔티티 태그(@ref detail::NodeEntity / @ref detail::EdgeEntity). 노드와 엣지를
 *         타입으로 가른다 — 값이 같아도 서로 대입되지 않는다.
 */
template <ViewKind K, class Entity> class ViewId final
{
public:
  /** @brief 이 타입이 속한 뷰 종류. 제네릭 코드에서 태그를 되읽을 때 쓴다. */
  static constexpr ViewKind VIEW_KIND = K;

  /** @brief 이 타입의 엔티티 태그. 노드/엣지 구분을 제네릭 코드에서 되읽을 때 쓴다. */
  using EntityTag = Entity;

  /** @brief 원시 표현형. `std::uint32_t`. */
  using ValueType = std::uint32_t;

  /**
   * @brief "해당 없음" 센티넬을 갖는 id 를 만든다.
   * @note 기본 생성은 항상 센티넬이다 — 0 은 유효한 id 이므로 기본값으로 쓰면 안 된다.
   */
  constexpr ViewId() noexcept = default;

  /**
   * @brief 원시 값으로부터 만든다. **`explicit` 이므로 암묵 변환되지 않는다**(규칙 V3).
   * @param[in] value 원시 id. 자료형 `std::uint32_t`, 의미는 뷰 @p K 안에서의 엔티티 번호.
   */
  explicit constexpr ViewId(ValueType value) noexcept : value_{value}
  {
  }

  /**
   * @brief 원시 값을 꺼낸다.
   * @return `std::uint32_t` — 뷰 @p K 안에서의 엔티티 번호. 센티넬이면 4294967295.
   * @warning 이 값을 다른 뷰의 `ViewId` 생성자에 그대로 넣지 말 것(값 사각 — 파일 상단 @note 참조).
   *          뷰 간 변환은 MapRegistry 의 변환 함수만이 수행한다(C4).
   */
  [[nodiscard]] constexpr ValueType value() const noexcept
  {
    return value_;
  }

  /**
   * @brief "해당 없음" 센티넬인지 검사한다.
   * @return `bool` — 센티넬(4294967295)이면 true, 유효한 id 이면 false.
   */
  [[nodiscard]] constexpr bool is_none() const noexcept
  {
    return value_ == NONE_VALUE;
  }

  /**
   * @brief "해당 없음" 센티넬 id 를 만든다.
   * @return `ViewId<K, Entity>` — 센티넬 값을 담은 id.
   */
  [[nodiscard]] static constexpr ViewId none() noexcept
  {
    return ViewId{};
  }

  /**
   * @brief 두 id 가 같은지 비교한다. 뷰·엔티티가 다르면 애초에 컴파일되지 않는다.
   * @param[in] lhs 좌변 id. 자료형 `ViewId<K, Entity>`.
   * @param[in] rhs 우변 id. 자료형 `ViewId<K, Entity>`.
   * @return `bool` — 원시 값이 같으면 true.
   */
  friend constexpr bool operator==(ViewId lhs, ViewId rhs) noexcept
  {
    return lhs.value_ == rhs.value_;
  }

  /**
   * @brief 두 id 가 다른지 비교한다.
   * @param[in] lhs 좌변 id. 자료형 `ViewId<K, Entity>`.
   * @param[in] rhs 우변 id. 자료형 `ViewId<K, Entity>`.
   * @return `bool` — 원시 값이 다르면 true.
   */
  friend constexpr bool operator!=(ViewId lhs, ViewId rhs) noexcept
  {
    return lhs.value_ != rhs.value_;
  }

  /**
   * @brief 정렬용 순서 비교. `std::map`·`std::set` 의 키로 쓰기 위한 것이며 기하적 의미는 없다.
   * @param[in] lhs 좌변 id. 자료형 `ViewId<K, Entity>`.
   * @param[in] rhs 우변 id. 자료형 `ViewId<K, Entity>`.
   * @return `bool` — 좌변의 원시 값이 작으면 true.
   */
  friend constexpr bool operator<(ViewId lhs, ViewId rhs) noexcept
  {
    return lhs.value_ < rhs.value_;
  }

private:
  /** @brief "해당 없음"을 뜻하는 원시 센티넬 값. 전 뷰·전 엔티티 공통(계약 §0 전역 규약). */
  static constexpr ValueType NONE_VALUE = std::numeric_limits<ValueType>::max();

  ValueType value_{NONE_VALUE};
};

/**
 * @brief 뷰 종류를 고정한 **노드** id 강타입. `ViewId<K, NodeEntity>` 의 별칭.
 * @tparam K 이 노드 id 가 유효한 뷰의 종류.
 */
template <ViewKind K> using ViewNodeId = ViewId<K, detail::NodeEntity>;

/**
 * @brief 뷰 종류를 고정한 **엣지** id 강타입. `ViewId<K, EdgeEntity>` 의 별칭.
 * @tparam K 이 엣지 id 가 유효한 뷰의 종류.
 */
template <ViewKind K> using ViewEdgeId = ViewId<K, detail::EdgeEntity>;

/** @brief 물리 roadmap 뷰의 노드 id. 불균일 엣지를 갖는 자동 추출 원본 지도 위의 노드. */
using PhysicalNodeId = ViewNodeId<ViewKind::PHYSICAL>;
/** @brief L2 세분화 균일 뷰의 노드 id. 가상 노드를 포함하며 LaCAM 계열의 이산 단위시간 전제를
 * 만족한다. */
using UniformNodeId = ViewNodeId<ViewKind::UNIFORM>;
/** @brief L3 collapse 골격 뷰의 노드 id. 차수 2 체인의 가상 노드가 접힌 의존성 그래프 위의 노드. */
using SkeletonNodeId = ViewNodeId<ViewKind::SKELETON>;

/** @brief 물리 roadmap 뷰의 엣지 id. 불균일 길이의 원본 통로 구간. */
using PhysicalEdgeId = ViewEdgeId<ViewKind::PHYSICAL>;
/** @brief L2 세분화 균일 뷰의 엣지 id. 각 엣지가 unit 길이 한 스텝에 대응한다. */
using UniformEdgeId = ViewEdgeId<ViewKind::UNIFORM>;
/** @brief L3 collapse 골격 뷰의 엣지 id. 차수 2 체인을 삼킨 의존성 결정 구간(스텝 수는 collapse
 * 대응표). */
using SkeletonEdgeId = ViewEdgeId<ViewKind::SKELETON>;

/** @brief "해당 없음"을 뜻하는 물리 뷰 노드 id 센티넬. */
inline constexpr PhysicalNodeId PHYSICAL_NODE_ID_NONE = PhysicalNodeId::none();
/** @brief "해당 없음"을 뜻하는 균일 뷰 노드 id 센티넬. */
inline constexpr UniformNodeId UNIFORM_NODE_ID_NONE = UniformNodeId::none();
/** @brief "해당 없음"을 뜻하는 골격 뷰 노드 id 센티넬. */
inline constexpr SkeletonNodeId SKELETON_NODE_ID_NONE = SkeletonNodeId::none();

/** @brief "해당 없음"을 뜻하는 물리 뷰 엣지 id 센티넬. */
inline constexpr PhysicalEdgeId PHYSICAL_EDGE_ID_NONE = PhysicalEdgeId::none();
/** @brief "해당 없음"을 뜻하는 균일 뷰 엣지 id 센티넬. */
inline constexpr UniformEdgeId UNIFORM_EDGE_ID_NONE = UniformEdgeId::none();
/** @brief "해당 없음"을 뜻하는 골격 뷰 엣지 id 센티넬. */
inline constexpr SkeletonEdgeId SKELETON_EDGE_ID_NONE = SkeletonEdgeId::none();

/** @brief 물리 뷰의 `view_id` 는 항상 0 이다 (`ViewScope.msg` 규약). */
inline constexpr std::uint32_t VIEW_ID_PHYSICAL = 0U;
/** @brief 미지정 지도 버전. 런타임에 금지되며, 관측되면 폐기 대상이다. */
inline constexpr std::uint64_t ROADMAP_VERSION_UNSPECIFIED = 0U;

/**
 * @brief id 가 유효한 뷰의 **인스턴스**까지 식별하는 스코프 (`ViewScope.msg` 미러, 규칙 V2).
 *
 * 종류(@ref ViewKind)만으로는 부족하다 — 같은 종류라도 지도가 개정되거나 재세분화되면
 * id 공간이 달라지기 때문이다. `(roadmap_version, view_id)` 쌍이 그 인스턴스를 지목하며,
 * 이는 MapRegistry 캐시 무효화 키(Q-6)와 같은 값이다.
 *
 * @note `view_kind` 를 wire 에 싣는 이유: 종류는 계약이 필드마다 고정하지만(V1), **발행자가
 *       잘못된 종류를 랩해 보낸 경우**를 수신 측이 탐지하려면 기대값과 대조할 대상이 필요하다.
 *       그 대조가 강타입이 소실되는 DDS 경계를 메운다.
 */
struct ViewScope
{
  ViewKind view_kind{ViewKind::PHYSICAL}; ///< 뷰 종류. 계약이 필드마다 고정
  std::uint64_t roadmap_version{
    ROADMAP_VERSION_UNSPECIFIED}; ///< MapRegistry 지도 버전. 0 = 미지정(런타임 금지)
  std::uint32_t view_id{VIEW_ID_PHYSICAL}; ///< 뷰 인스턴스 id. PHYSICAL 이면 0
};

/**
 * @brief 두 스코프가 같은 id 공간을 가리키는지 검사한다.
 * @param[in] lhs 좌변 스코프. 자료형 `ViewScope`.
 * @param[in] rhs 우변 스코프. 자료형 `ViewScope`.
 * @return `bool` — 세 필드가 모두 같으면 true.
 */
[[nodiscard]] constexpr bool operator==(const ViewScope & lhs, const ViewScope & rhs) noexcept
{
  return lhs.view_kind == rhs.view_kind && lhs.roadmap_version == rhs.roadmap_version &&
         lhs.view_id == rhs.view_id;
}

/**
 * @brief 두 스코프가 다른 id 공간을 가리키는지 검사한다.
 * @param[in] lhs 좌변 스코프. 자료형 `ViewScope`.
 * @param[in] rhs 우변 스코프. 자료형 `ViewScope`.
 * @return `bool` — 한 필드라도 다르면 true.
 */
[[nodiscard]] constexpr bool operator!=(const ViewScope & lhs, const ViewScope & rhs) noexcept
{
  return !(lhs == rhs);
}

/**
 * @brief 스코프가 런타임에 사용 가능한 상태인지 검사한다.
 *
 * 지도 버전 미지정(0)은 계약이 런타임에 금지한다. 물리 뷰인데 `view_id` 가 0 이 아닌 것도
 * 규약 위반이다(`ViewScope.msg`).
 *
 * @param[in] scope 검사할 스코프. 자료형 `ViewScope`.
 * @return `bool` — 사용 가능하면 true, 규약 위반이면 false(수신자는 메시지를 폐기한다).
 */
[[nodiscard]] constexpr bool is_usable_scope(const ViewScope & scope) noexcept
{
  if (scope.roadmap_version == ROADMAP_VERSION_UNSPECIFIED)
  {
    return false;
  }

  if (scope.view_kind == ViewKind::PHYSICAL && scope.view_id != VIEW_ID_PHYSICAL)
  {
    return false;
  }

  return true;
}

// 강타입이 원시 표현과 같은 크기·정렬을 갖는지 — 직렬화·메모리 비용이 늘지 않음을 보장한다.
static_assert(sizeof(PhysicalNodeId) == sizeof(std::uint32_t), "강타입이 uint32 보다 커졌다");
static_assert(sizeof(UniformNodeId) == sizeof(std::uint32_t), "강타입이 uint32 보다 커졌다");
static_assert(sizeof(SkeletonNodeId) == sizeof(std::uint32_t), "강타입이 uint32 보다 커졌다");
static_assert(sizeof(PhysicalEdgeId) == sizeof(std::uint32_t), "강타입이 uint32 보다 커졌다");
static_assert(sizeof(UniformEdgeId) == sizeof(std::uint32_t), "강타입이 uint32 보다 커졌다");
static_assert(sizeof(SkeletonEdgeId) == sizeof(std::uint32_t), "강타입이 uint32 보다 커졌다");
static_assert(alignof(PhysicalNodeId) == alignof(std::uint32_t), "강타입의 정렬이 달라졌다");
static_assert(alignof(UniformNodeId) == alignof(std::uint32_t), "강타입의 정렬이 달라졌다");
static_assert(alignof(SkeletonNodeId) == alignof(std::uint32_t), "강타입의 정렬이 달라졌다");
static_assert(alignof(PhysicalEdgeId) == alignof(std::uint32_t), "강타입의 정렬이 달라졌다");
static_assert(alignof(UniformEdgeId) == alignof(std::uint32_t), "강타입의 정렬이 달라졌다");
static_assert(alignof(SkeletonEdgeId) == alignof(std::uint32_t), "강타입의 정렬이 달라졌다");

// 노드와 엣지가 같은 뷰라도 서로 다른 타입임을 보장한다(오사용 컴파일 차단의 근거).
static_assert(
  !std::is_same<PhysicalNodeId, PhysicalEdgeId>::value, "노드와 엣지가 같은 타입이 되어 버렸다");

} // namespace mrs

namespace std
{

/**
 * @brief `mrs::ViewId` 의 해시 특수화 — `std::unordered_map`·`unordered_set` 키로 쓰기 위한 것.
 *
 * 뷰 종류·엔티티별로 서로 다른 타입이므로 이 부분 특수화 하나가 노드 3종·엣지 3종 전부를 덮는다.
 * 별칭 `ViewNodeId<K>`·`ViewEdgeId<K>` 도 이 특수화로 해시된다.
 *
 * @tparam K 뷰 종류.
 * @tparam E 엔티티 태그.
 */
template <mrs::ViewKind K, class E> struct hash<mrs::ViewId<K, E>>
{
  /**
   * @brief id 의 해시를 계산한다.
   * @param[in] id 해시할 id. 자료형 `mrs::ViewId<K, E>`.
   * @return `std::size_t` — 원시 값의 해시. 뷰·엔티티가 달라도 같은 원시 값이면 같은 해시이나,
   *         타입이 달라 서로 다른 컨테이너에만 들어가므로 충돌 문제가 되지 않는다.
   */
  [[nodiscard]] size_t operator()(mrs::ViewId<K, E> id) const noexcept
  {
    return hash<uint32_t>{}(id.value());
  }
};

} // namespace std
