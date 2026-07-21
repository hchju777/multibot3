// common_convert_negative.cpp — **컴파일이 거부되어야 하는** 헬퍼 오용 모음 (부정 컴파일 회귀).
//
// 지키는 벽: `node_id_*` 헬퍼의 `static_assert(IS_VIEW_NODE_ID<NodeIdT>)` — 규칙 V3 가 금지한
// "맨 uint32 우회"를 컴파일 단계에서 끊는다. 이 벽이 사라지면 언랩 흔적만 남고 뷰 강타입이
// 무력화되는데, 런타임 테스트로는 그 사실이 전혀 드러나지 않는다.
//
// 도는 방식과 공허성 방지는 `tools/expect_compile.cmake` 주석 참조. `CASE_OK` 대조군이
// 통과해야만 아래 8건의 "거부됨"이 의미를 갖는다.
//
// 이 파일이 검증하지 못하는 것: 런타임 변환 거동(별도 gtest), DDS 경계 오용,
// QP feasibility·livelock·물리 재현성(**isaac 필수** — architecture §5.3).

#include <cstdint>
#include <vector>

#include "mrs/i_local_planner.hpp"
#include "mrs_interfaces/msg/commit_status.hpp"
#include "mrs_msg_convert/common_convert.hpp"

void probe()
{
#if defined(CASE_N1_INT_LITERAL_UNWRAP)
  // int 리터럴을 언랩 헬퍼에
  (void)mrs::convert::node_id_to_msg(5);

#elif defined(CASE_N2_BARE_UINT32_UNWRAP)
  // 맨 uint32 를 언랩 헬퍼에
  (void)mrs::convert::node_id_to_msg(static_cast<std::uint32_t>(5));

#elif defined(CASE_N3_BARE_UINT32_OUT)
  // 맨 uint32 를 랩 헬퍼의 out 으로 — 어느 뷰로 랩하는지 선언되지 않는다
  std::uint32_t bare = 0U;
  (void)mrs::convert::node_id_from_msg(1U, mrs::convert::NoneNodePolicy::REJECT, bare);

#elif defined(CASE_N4_BARE_UINT32_ARRAY_UNWRAP)
  // 맨 uint32 배열 언랩
  const std::vector<std::uint32_t> bare;
  (void)mrs::convert::node_ids_to_msg(bare);

#elif defined(CASE_N5_BARE_UINT32_ARRAY_OUT)
  // 맨 uint32 배열을 랩 헬퍼의 out 으로
  const std::vector<std::uint32_t> raw{1U};
  std::vector<std::uint32_t> bare;
  (void)mrs::convert::node_ids_from_msg(raw, mrs::convert::NoneNodePolicy::ALLOW, bare);

#elif defined(CASE_N6_CROSS_VIEW_ASSIGN_AFTER_WRAP)
  // 정상적으로 랩한 뒤 다른 뷰로 대입 — 헬퍼를 거쳤다고 뷰가 세탁되면 안 된다
  mrs::UniformNodeId u{};
  (void)mrs::convert::node_id_from_msg(1U, mrs::convert::NoneNodePolicy::ALLOW, u);
  const mrs::PhysicalNodeId p = u;
  (void)p;

#elif defined(CASE_N7_CROSS_VIEW_ARRAY)
  // 뷰 교차 배열 구성
  const mrs::PhysicalNodeId p{1U};
  const std::vector<mrs::UniformNodeId> ids{p};
  (void)ids;

#elif defined(CASE_MIRROR_DRIFT)
  // 미러 정합 벽이 **공허하게 참이 아님**을 회귀로 고정한다 (R-17 이 프로젝트 표준으로 승격).
  //
  // `common_convert.cpp` 는 도메인 enum 이 `.msg` 상수의 미러임을 static_assert 로 못박고, 발행
  // 방향은 그 사실을 근거로 static_cast 로 값을 그대로 내보낸다. 벽이 없으면 계약이 값을 바꿨을
  // 때 **아무 진단 없이 틀린 값이 와이어로 나간다.**
  //
  // 여기서는 일부러 **틀린 짝**을 걸어 컴파일이 거부되는지 본다. 이 케이스가 컴파일에 성공하면
  // QP_OK 와 QP_FALLBACK 이 같은 값이 됐다는 뜻이고, 그것이 곧 계약 드리프트다.
  static_assert(
    static_cast<std::uint8_t>(mrs::AdoptionStatus::QP_OK) ==
      mrs_interfaces::msg::CommitStatus::QP_FALLBACK,
    "드리프트 프로브 — 이 static_assert 는 실패해야 정상이다");

#elif defined(CASE_OK)
  // ── 대조군: 반드시 컴파일되어야 한다 ──────────────────────────────────────
  const mrs::UniformNodeId u{7U};
  (void)mrs::convert::node_id_to_msg(u);

  mrs::UniformNodeId wrapped{};
  (void)mrs::convert::node_id_from_msg(1U, mrs::convert::NoneNodePolicy::REJECT, wrapped);

  const std::vector<mrs::UniformNodeId> ids{u, wrapped};
  (void)mrs::convert::node_ids_to_msg(ids);

  const std::vector<std::uint32_t> raw{1U, 2U};
  std::vector<mrs::UniformNodeId> out;
  (void)mrs::convert::node_ids_from_msg(raw, mrs::convert::NoneNodePolicy::REJECT, out);
#endif
}
