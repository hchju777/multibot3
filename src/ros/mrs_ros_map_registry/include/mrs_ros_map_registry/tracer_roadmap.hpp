#pragma once

/**
 * @file tracer_roadmap.hpp
 * @brief [0a] tracer bullet 전용 **하드코딩 물리 roadmap 픽스처** (architecture §7-1).
 *
 * ## 왜 여기에 있는가 (C4 침범이 아니다)
 * architecture 제약 C4 는 **3뷰 변환**(세분화·collapse·좌표 변환)의 단일 소유자를
 * `mrs_map_registry` 로 못박는다. 이 파일에는 그 변환이 **하나도 없다** — 물리 뷰 1장의
 * **좌표 리터럴**일 뿐이다. `MapRegistry::load_physical_roadmap()` 은 아직 스텁이고
 * (Phase 5 coordination-builder 대상), [0a] 의 목적은 배관 실측이므로 지도 파일 형식·
 * 로더를 확정하기 전에 배선을 굴려야 한다.
 * ⇒ **[1](D-11) 착수 시 이 픽스처는 `MapRegistry` 의 로더로 이관되고 이 파일은 사라진다.**
 * 그때까지 이 파일 밖에서 roadmap 을 만들지 않는다(단일 출처 유지).
 *
 * ## 격자 형태와 그 근거 (architecture §7-1: "노드 6~12개, 로봇 2대 왕복 통로")
 * **4열 × 2행 사다리(ladder) 격자 — 노드 8개, 엣지 10개.**
 * ```
 *   행 1 (y = s)   n4 ── n5 ── n6 ── n7      <- 상단 통로
 *                  │     │     │     │        <- 가로대(rung) 4개
 *   행 0 (y = 0)   n0 ── n1 ── n2 ── n3      <- 하단 통로
 *                  x=0   x=s  x=2s  x=3s
 * ```
 * 이 형태를 고른 이유는 4가지이며 전부 계약·아키텍처 문언에 대응한다:
 *  1. **로봇 2대가 왕복하고 교행할 수 있다.** 가로대가 4개라 마주친 두 로봇이 서로를 비껴갈
 *     경로가 항상 존재한다. 단일 복도(경로 그래프)로 만들면 [0a] 가 배관 실측이 아니라
 *     데드락 실험이 되어 버린다 — 데드락은 [1]~[2] 의 일이다.
 *  2. **막다른 골목이 없다**(모든 인접 노드쌍이 단순 사이클에 속한다) — 검사기 2(biconnected류,
 *     `pibt-2019` Thm 2 전제)를 통과할 형태다. ⚠ 이 형태를 **의도했을 뿐 검사는 하지 않는다**
 *     (검사기 3종은 [1] 대상 — 호출 금지).
 *  3. **비작업 엔드포인트가 정확히 2개**(n3·n4)다. well-formed 검사기 1(`lifelong-mapd-2017`
 *     Def.1)이 "비작업 엔드포인트 >= 로봇 수"를 요구하고 [0a] 의 로봇 수가 2 이므로,
 *     노드 수를 8 로 정한 것은 임의가 아니라 이 하한에서 나온다.
 *  4. **모든 엣지 길이가 격자 간격 s 로 같다.** 물리 뷰가 이미 균일하므로 [1] 의 세분화가
 *     항등이 되고, 왕복(세분화↔collapse) 무손실 테스트의 **최초 회귀 케이스**로 쓸 수 있다.
 *     ⚠ 반대로 말하면 이 픽스처는 **불균일 엣지를 검증하지 못한다** — [1] 에서 불균일
 *     지도를 반드시 추가해야 한다.
 *
 * 작업 엔드포인트는 **대각선**(n0·n7)에 둔다. 로봇 2대가 서로 반대 방향으로 격자 전체를
 * 가로지르게 되어, 창 릴리스·에스컬레이션·이웃 계획 공유가 모두 실제로 흐른다.
 *
 * ## 수치의 출처 (수치 창작 금지 규율)
 *  - `grid_spacing_m` : ROS2 param. [0a] 실측 대상이 아니라 **픽스처 선택값**이며 근거는 위 4번.
 *  - `claim_radius_m` : **파라미터가 아니다.** 계약 L-16 미결 U-01 이 정한 보수적 초기값
 *                       `unit_length_m / 2` 를 그대로 쓴다(산정식은 theory T1 O6 OPEN).
 *  - `corridor_width_m` / `speed_limit_mps` : ROS2 param, **[0a] 실측 대상**. 검사기 3의 조건
 *                       `W_min > 2(r + v_max/ω_max)` 은 r·v_max·ω_max 가 미정이라 **판정하지
 *                       않는다**([1] 대상). 여기 담긴 값은 계약을 만족한다는 주장이 아니다.
 */

#include <cstdint>

#include "mrs_map_registry/map_registry.hpp"

namespace mrs
{

/**
 * @brief [0a] 픽스처 roadmap 을 만들 때 쓰는 값 묶음 (전부 ROS2 param 에서 온다).
 */
struct TracerRoadmapParams
{
  /** @brief 발급할 지도 버전. **1 이상**이어야 한다 — 0 은 계약이 런타임에 금지한다. */
  std::uint64_t roadmap_version{1U};
  /** @brief 격자 간격 s [m]. 모든 엣지의 길이이자 균일 뷰 입도의 기준값. */
  double grid_spacing_m{2.0};
  /** @brief 각 엣지 구간의 최소 통로 폭 W_min [m]. **[0a] 실측 대상** (검사기 3은 [1]). */
  double corridor_width_m{1.5};
  /** @brief 각 엣지 구간의 허용 최대 몸체 속도 [m/s]. **[0a] 실측 대상**. */
  double speed_limit_mps{0.5};
};

/**
 * @brief [0a] 하드코딩 물리 roadmap 1장을 만든다 (4열 × 2행 사다리 격자, 노드 8·엣지 10).
 *
 * 만들어진 뷰의 종류는 **PHYSICAL** 이다(계약 §0.1 정본표 `GetRoadmap 응답` 행). 따라서
 * `view_id` 는 항상 0 이고 `nodes[].is_virtual` 은 전부 false 다.
 * `mappings` 는 **비운다** — 대응표는 세분화·collapse 의 산물이며 물리 뷰에는 존재하지 않는다.
 *
 * @param[in] params 픽스처 파라미터. 자료형 `mrs::TracerRoadmapParams`.
 *            `roadmap_version >= 1`, `grid_spacing_m > 0`, `corridor_width_m >= 0`,
 *            `speed_limit_mps >= 0` 이어야 하며 전부 유한해야 한다.
 * @param[out] out 생성된 물리 뷰 데이터. 자료형 `mrs::RoadmapViewData`.
 *             **성공 시에만 대입되며, 실패 시 기존 내용이 보존된다**(부분 채움 없음).
 * @return `bool` — 생성에 성공하면 true, @p params 가 위 조건을 어기면 false.
 *         false 일 때 호출 노드는 서비스 응답을 비운 채(지도 버전 0) 돌려주어 수신자가
 *         100% 폐기하게 한다 — 조용히 그럴듯한 지도를 만들지 않는다.
 */
[[nodiscard]] bool build_tracer_roadmap(const TracerRoadmapParams & params, RoadmapViewData & out);

/**
 * @brief [0a] 픽스처의 **균일 뷰**를 만든다 — 이 픽스처에 한해 물리 뷰와 노드·엣지가 같다.
 *
 * ## 이것은 D-11 세분화의 구현이 아니다 (반드시 읽을 것)
 * D-11 의 알맹이는 **불균일 물리 엣지를 가상 노드로 잘라 균일 격자를 만들고, collapse 로
 * 되돌렸을 때 무손실인지**를 보장하는 것이며 그것은 [1] 의 일이다. 이 함수는 그 알고리즘을
 * 하나도 갖고 있지 않다. 여기서 균일 뷰가 물리 뷰와 같아지는 이유는 **픽스처를 그렇게
 * 만들었기 때문**이다 — `tracer_roadmap.hpp` 격자 근거 4번이 "모든 엣지 길이가 격자 간격 s 로
 * 같다"고 못박았고, 이미 균일한 격자를 균일하게 만드는 세분화는 **항등**이다. 즉 이 함수가
 * 주장하는 것은 알고리즘이 아니라 **픽스처의 성질**이다.
 *
 * 따라서 다음 3가지가 자동으로 따라온다(지어낸 값이 하나도 없다):
 *  - `nodes[].is_virtual` 전부 false — 자를 엣지가 없으니 가상 노드가 생기지 않는다.
 *  - `mappings`(= `subdivision_map`) **비어 있음** — 대응표는 가상 노드 → 물리 엣지 위 위치의
 *    표이고, 가상 노드가 0 개면 원소도 0 개다. "비었다"가 곧 정확한 값이다.
 *  - `unit_length_m` = 격자 간격 s — 균일 뷰 입도가 곧 엣지 길이다.
 *
 * ## 왜 [0a] 에 필요한가 (architecture §7 최소 서비스 목록과의 관계)
 * §7 은 [0a] 최소 서비스로 `GetRoadmap` 만 적었다. 그러나 계약 §0.1 정본표는 L2 가 내는
 * `PlannedPaths` 와 L3 가 내는 `ExecutionWindow` 의 노드 id 를 **UNIFORM** 으로 고정했고,
 * 균일 뷰 스코프의 발급처를 `GetUniformView` 응답으로 못박았다. 물리 뷰를 균일 뷰로
 * 재해석하는 것은 **C4(MapRegistry 가 3뷰의 단일 소유자) 위반**이므로 소비자 쪽에서 메울 수
 * 없다. ⇒ 발급을 **소유자인 이 노드**가 한다. 자세한 판단 근거는 `map_registry_node.hpp` 참조.
 *
 * ## ⚠ [1] 이 반드시 해야 할 일
 * 이 뷰로만 D-11 왕복 무손실 테스트를 돌리면 **항등 테스트인 척**이 된다. [1] 에서
 * **불균일 엣지 지도**를 추가하기 전까지 이 픽스처의 통과를 D-11 검증으로 인용하지 말 것.
 *
 * @param[in] params 픽스처 파라미터. 자료형 `mrs::TracerRoadmapParams`.
 *            @ref build_tracer_roadmap 과 같은 조건을 만족해야 한다.
 * @param[in] uniform_view_id 발급할 균일 뷰 인스턴스 id. 자료형 `std::uint32_t`.
 *            **1 이상이어야 한다** — 0 은 물리 뷰의 예약값(`VIEW_ID_PHYSICAL`)이라, 균일 뷰에
 *            0 을 쓰면 두 뷰의 `(roadmap_version, view_id)` 평면 쌍이 같아져 런타임 스코프
 *            대조(계약 §0.1 규칙 C)가 물리↔균일 혼동을 **잡아내지 못한다**.
 * @param[out] out 생성된 균일 뷰 데이터. 자료형 `mrs::RoadmapViewData`.
 *             **성공 시에만 대입되며, 실패 시 기존 내용이 보존된다**(부분 채움 없음).
 * @return `bool` — 생성에 성공하면 true, @p params 가 조건을 어기거나
 *         @p uniform_view_id 가 0 이면 false.
 */
[[nodiscard]] bool build_tracer_uniform_view(
  const TracerRoadmapParams & params, std::uint32_t uniform_view_id, RoadmapViewData & out);

} // namespace mrs
