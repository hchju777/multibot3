// test_view_ids.cpp — 뷰 강타입(view_ids.hpp)의 런타임 거동을 지키는 벽.
//
// ## 이 파일이 지키는 것
// 센티넬 규약(4294967295 = "해당 없음", 0 은 유효한 노드 id), 뷰 스코프 동일성 판정,
// `is_usable_scope` 의 두 금지 규약, 강타입이 uint32 와 같은 크기를 유지한다는 표현 불변식.
//
// ## 이 파일이 검증하지 **못하는** 것 (충실도 티어 정직성 — architecture §6 / §5.3)
// - 컴파일이 거부되는가: 런타임 테스트로는 불가능하다. 그 방어선은 별도 부정 컴파일 테스트
//   (`test/negative/view_ids_negative.cpp` + ctest `neg_compile_*`)가 담당한다.
// - DDS 경계 너머의 오용: 강타입은 프로세스 안에서만 유효하고 와이어에서는 uint32 로 소실된다.
//   그 방어선은 `mrs_msg_convert` 의 스코프 대조이며 unit 티어가 아니라 계약 경계의 몫이다.
// - QP feasibility · livelock · 물리 재현성: **unit 티어로 판정할 수 없다. isaac 필수.**

#include <cstdint>
#include <functional>
#include <map>
#include <type_traits>
#include <unordered_map>

#include <gtest/gtest.h>

#include "mrs/view_ids.hpp"

namespace
{

// 손으로 계산한 센티넬 값 — 구현을 호출해 얻지 않는다. 2^32 - 1 = 4294967295.
constexpr std::uint32_t EXPECTED_NONE_VALUE = 4294967295U;

} // namespace

// 기본 생성이 0 이 아니라 센티넬이어야 한다 — 0 을 기본값으로 쓰면 0번 노드가 조용히 점유된다.
TEST(ViewIds, DefaultConstructedIsNoneSentinel)
{
  const mrs::UniformNodeId id{};
  EXPECT_TRUE(id.is_none());
  EXPECT_EQ(id.value(), EXPECTED_NONE_VALUE);
}

// 0 은 유효한 노드 id 다. "기본값 = 0" 으로 되돌아가는 회귀를 여기서 막는다.
TEST(ViewIds, ZeroIsAValidIdNotTheSentinel)
{
  const mrs::UniformNodeId id{0U};
  EXPECT_FALSE(id.is_none());
  EXPECT_EQ(id.value(), 0U);
}

// none() 팩토리와 헤더가 내보내는 상수 3종이 같은 값을 가리켜야 한다.
TEST(ViewIds, NoneFactoryAgreesWithExportedConstants)
{
  EXPECT_TRUE(mrs::PHYSICAL_NODE_ID_NONE.is_none());
  EXPECT_TRUE(mrs::UNIFORM_NODE_ID_NONE.is_none());
  EXPECT_TRUE(mrs::SKELETON_NODE_ID_NONE.is_none());
  EXPECT_EQ(mrs::UNIFORM_NODE_ID_NONE, mrs::UniformNodeId::none());
  EXPECT_EQ(mrs::PHYSICAL_NODE_ID_NONE.value(), EXPECTED_NONE_VALUE);
}

// 명시 생성자가 값을 있는 그대로 나른다(왕복 오차 0).
TEST(ViewIds, ExplicitConstructionRoundTripsRawValue)
{
  EXPECT_EQ(mrs::PhysicalNodeId{7U}.value(), 7U);
  EXPECT_EQ(mrs::UniformNodeId{9U}.value(), 9U);
  EXPECT_EQ(mrs::SkeletonNodeId{11U}.value(), 11U);
}

// 비교 연산자가 원시 값만 본다. 순서 비교는 map 키 용도이므로 전순서여야 한다.
TEST(ViewIds, ComparisonFollowsRawValue)
{
  const mrs::UniformNodeId a{3U};
  const mrs::UniformNodeId b{3U};
  const mrs::UniformNodeId c{4U};

  EXPECT_TRUE(a == b);
  EXPECT_FALSE(a != b);
  EXPECT_TRUE(a != c);
  EXPECT_TRUE(a < c);
  EXPECT_FALSE(c < a);
  EXPECT_FALSE(a < b);
}

// std::map / std::unordered_map 키로 실제로 쓸 수 있어야 한다(MapRegistry 대응 테이블의 전제).
TEST(ViewIds, UsableAsOrderedAndHashedMapKey)
{
  std::map<mrs::UniformNodeId, int> ordered;
  ordered[mrs::UniformNodeId{5U}] = 50;
  ordered[mrs::UniformNodeId{2U}] = 20;
  ASSERT_EQ(ordered.size(), 2U);
  EXPECT_EQ(ordered.begin()->first.value(), 2U); // 오름차순 정렬

  std::unordered_map<mrs::SkeletonNodeId, int> hashed;
  hashed[mrs::SkeletonNodeId{5U}] = 50;
  EXPECT_EQ(hashed.at(mrs::SkeletonNodeId{5U}), 50);
}

// 해시가 원시 값의 해시와 일치 — 뷰가 달라도 같은 원시 값이면 같은 해시라는 헤더의 주장.
TEST(ViewIds, HashMatchesRawValueHash)
{
  const std::size_t expected = std::hash<std::uint32_t>{}(7U);
  EXPECT_EQ(std::hash<mrs::UniformNodeId>{}(mrs::UniformNodeId{7U}), expected);
  EXPECT_EQ(std::hash<mrs::PhysicalNodeId>{}(mrs::PhysicalNodeId{7U}), expected);
}

// 세 뷰가 **서로 다른 타입**이다. 이것이 무너지면 부정 컴파일 테스트 전체가 공허해진다.
TEST(ViewIds, ThreeViewsAreDistinctTypesWithCorrectTags)
{
  static_assert(!std::is_same_v<mrs::PhysicalNodeId, mrs::UniformNodeId>, "뷰 타입이 합쳐졌다");
  static_assert(!std::is_same_v<mrs::UniformNodeId, mrs::SkeletonNodeId>, "뷰 타입이 합쳐졌다");
  static_assert(!std::is_same_v<mrs::PhysicalNodeId, mrs::SkeletonNodeId>, "뷰 타입이 합쳐졌다");

  EXPECT_EQ(mrs::PhysicalNodeId::VIEW_KIND, mrs::ViewKind::PHYSICAL);
  EXPECT_EQ(mrs::UniformNodeId::VIEW_KIND, mrs::ViewKind::UNIFORM);
  EXPECT_EQ(mrs::SkeletonNodeId::VIEW_KIND, mrs::ViewKind::SKELETON);
}

// 강타입화가 표현 비용을 늘리지 않는다 — 직렬화·메모리 가정의 근거.
TEST(ViewIds, StrongTypeKeepsRawSizeAndAlignment)
{
  EXPECT_EQ(sizeof(mrs::UniformNodeId), sizeof(std::uint32_t));
  EXPECT_EQ(alignof(mrs::UniformNodeId), alignof(std::uint32_t));
  EXPECT_EQ(sizeof(mrs::UniformNodeId), 4U); // 손으로 적은 기대값
}

// ViewKind 값은 ViewScope.msg 의 미러다 — 계약이 고정한 숫자를 손으로 적는다.
TEST(ViewIds, ViewKindValuesMatchContract)
{
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::ViewKind::PHYSICAL), 0U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::ViewKind::UNIFORM), 1U);
  EXPECT_EQ(static_cast<std::uint8_t>(mrs::ViewKind::SKELETON), 2U);
}

// 스코프 동일성은 세 필드 전부를 본다. 한 필드만 달라도 다른 id 공간이다.
TEST(ViewScopeTest, EqualityRequiresAllThreeFields)
{
  mrs::ViewScope base;
  base.view_kind = mrs::ViewKind::UNIFORM;
  base.roadmap_version = 42U;
  base.view_id = 5U;

  EXPECT_TRUE(base == base);

  mrs::ViewScope other_kind = base;
  other_kind.view_kind = mrs::ViewKind::SKELETON;
  EXPECT_TRUE(base != other_kind);

  mrs::ViewScope other_version = base;
  other_version.roadmap_version = 43U;
  EXPECT_TRUE(base != other_version);

  mrs::ViewScope other_view_id = base;
  other_view_id.view_id = 6U;
  EXPECT_TRUE(base != other_view_id);
}

// roadmap_version = 0 은 런타임 금지값 — 미지정 지도로 도는 것을 막는 벽.
TEST(ViewScopeTest, UsableRejectsUnspecifiedRoadmapVersion)
{
  mrs::ViewScope scope;
  scope.view_kind = mrs::ViewKind::UNIFORM;
  scope.roadmap_version = mrs::ROADMAP_VERSION_UNSPECIFIED;
  scope.view_id = 5U;
  EXPECT_FALSE(mrs::is_usable_scope(scope));
}

// 물리 뷰의 view_id 는 항상 0 이어야 한다(ViewScope.msg 규약).
TEST(ViewScopeTest, UsableRejectsPhysicalViewWithNonZeroViewId)
{
  mrs::ViewScope scope;
  scope.view_kind = mrs::ViewKind::PHYSICAL;
  scope.roadmap_version = 42U;
  scope.view_id = 1U;
  EXPECT_FALSE(mrs::is_usable_scope(scope));
}

// 그러나 그 규약이 UNIFORM/SKELETON 에까지 번지면 안 된다 — 이 둘은 view_id != 0 이 정상이다.
// (물리 뷰 규칙을 전 뷰에 적용하는 회귀를 가르는 케이스)
TEST(ViewScopeTest, UsableAcceptsNonPhysicalViewWithNonZeroViewId)
{
  mrs::ViewScope uniform;
  uniform.view_kind = mrs::ViewKind::UNIFORM;
  uniform.roadmap_version = 42U;
  uniform.view_id = 5U;
  EXPECT_TRUE(mrs::is_usable_scope(uniform));

  mrs::ViewScope skeleton = uniform;
  skeleton.view_kind = mrs::ViewKind::SKELETON;
  EXPECT_TRUE(mrs::is_usable_scope(skeleton));
}

// 물리 뷰 + view_id 0 은 유효하다.
TEST(ViewScopeTest, UsableAcceptsPhysicalViewWithZeroViewId)
{
  mrs::ViewScope scope;
  scope.view_kind = mrs::ViewKind::PHYSICAL;
  scope.roadmap_version = 42U;
  scope.view_id = mrs::VIEW_ID_PHYSICAL;
  EXPECT_TRUE(mrs::is_usable_scope(scope));
}

// 기본 생성된 스코프는 사용 불가여야 한다 — 채우는 것을 잊으면 조용히 통과하면 안 된다.
TEST(ViewScopeTest, DefaultConstructedScopeIsNotUsable)
{
  EXPECT_FALSE(mrs::is_usable_scope(mrs::ViewScope{}));
}
