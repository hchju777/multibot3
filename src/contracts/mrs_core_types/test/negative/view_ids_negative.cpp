// view_ids_negative.cpp — **컴파일이 거부되어야 하는** 코드 모음 (부정 컴파일 회귀).
//
// ## 어떻게 도는가
// 이 파일은 라이브러리에 링크되지 않는다. ctest 가 케이스마다 `-DCASE_*` 를 붙여 컴파일러를
// 한 번씩 부르고(`tools/expect_compile.cmake`), 컴파일이 **실패해야** 그 테스트가 통과한다.
// 런타임 gtest 로는 "거부되는가"를 원리적으로 검증할 수 없기 때문에 이 형태가 필요하다.
//
// ## 공허하게 참이 되지 않도록
// `CASE_OK` 는 **통과해야 하는 대조군**이다. include 경로가 깨지거나 컴파일러 호출이 잘못되면
// CASE_OK 가 먼저 실패하므로, "무엇을 넣어도 거부됨"을 "벽이 살아 있음"으로 오판할 수 없다.
//
// ## 이 파일이 검증하지 못하는 것
// 컴파일 시각의 벽만 본다. 런타임 거동·DDS 경계 오용·QP feasibility·livelock·물리 재현성은
// 여기서 아무것도 말하지 않는다(후자 3종은 isaac 필수 — architecture §5.3).

#include <cstdint>
#include <vector>

#include "mrs/contract_types.hpp"

void probe()
{
#if defined(CASE_CROSS_VIEW_ASSIGN)
  // 뷰 교차 대입 — 균일 뷰 id 를 골격 뷰 id 에 넣으면 D-11 무손실 판정이 조용히 거짓이 된다.
  const mrs::UniformNodeId u{42U};
  mrs::SkeletonNodeId s = u;
  (void)s;

#elif defined(CASE_STRONG_TO_RAW_IMPLICIT)
  // 강타입 → 원시 uint32 암묵 변환 (언랩이 흔적 없이 일어나는 경로)
  const mrs::UniformNodeId u{42U};
  const std::uint32_t raw = u;
  (void)raw;

#elif defined(CASE_RAW_TO_STRONG_IMPLICIT)
  // 원시 uint32 → 강타입 암묵 변환 (explicit 생성자 우회)
  const mrs::UniformNodeId x = 42U;
  (void)x;

#elif defined(CASE_WRONG_VIEW_IN_WINDOW_SEGMENT)
  // 잘못된 뷰를 창 세그먼트에 — 계약이 WindowSegment 를 UNIFORM 으로 고정했다
  mrs::WindowSegment segment;
  segment.node_from = mrs::SkeletonNodeId{7U};

#elif defined(CASE_WRONG_VIEW_ARRAY)
  // 잘못된 뷰의 배열 구성 — 원소 하나가 다른 뷰면 배열 전체의 뷰 주장이 거짓이 된다
  const mrs::PhysicalNodeId p{1U};
  const std::vector<mrs::UniformNodeId> ids{p};
  (void)ids;

#elif defined(CASE_CROSS_VIEW_COMPARE)
  // 뷰 교차 비교 — 서로 다른 id 공간의 값을 같다고 판정할 수 있으면 강타입의 의미가 없다
  const mrs::UniformNodeId u{5U};
  const mrs::PhysicalNodeId p{5U};
  const bool same = (u == p);
  (void)same;

#elif defined(CASE_OK)
  // ── 대조군: 반드시 컴파일되어야 한다 ──────────────────────────────────────
  const mrs::UniformNodeId u{42U};
  const mrs::UniformNodeId y{u.value()}; // 명시적 재랩(같은 뷰) 은 허용된다
  mrs::WindowSegment segment;
  segment.node_from = mrs::UniformNodeId{7U};
  const std::vector<mrs::UniformNodeId> ids{u, y};
  const bool same = (u == y);
  (void)segment;
  (void)ids;
  (void)same;
#endif
}
