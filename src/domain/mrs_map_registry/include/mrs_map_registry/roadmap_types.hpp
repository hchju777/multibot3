#pragma once

/**
 * @file roadmap_types.hpp
 * @brief MapRegistry 도메인 값 타입 — 물리/균일/골격 3뷰의 노드·엣지·뷰 간 대응·검사 결과 (D-11).
 *
 * 이 헤더의 struct 는 잠긴 계약(`RoadmapNode.msg`·`RoadmapEdge.msg`·`NodeMapping.msg`·
 * `ValidateMap.srv` v3.0.0)의 **도메인 미러**다 — 필드가 1:1 대응한다. 변환은 `mrs_msg_convert`
 * 가 단독 소유하며(규칙 V3), 이 헤더는 `rclcpp`·`mrs_interfaces` 에 의존하지 않는다(아키텍처 C1).
 *
 * ## id 타입 규약 (사용자 결정, 파일 3)
 *  - **뷰 컨테이너(@ref RoadmapNodeView·@ref RoadmapEdgeView·@ref RoadmapViewData)의 노드·엣지
 *    id 는 bare `std::uint32_t`** 다. 뷰 종류는 원소가 아니라 컨테이너의 @ref
 * RoadmapViewData::view_kind
 *    (= `.msg` 를 담은 응답이 종류를 고정하는 것의 미러)가 지목한다. `get_view` 가 런타임에 뷰
 *    종류를 골라 반환하므로 컨테이너를 뷰별 강타입으로 못박을 수 없다 — 강타입 강제는 종류가
 *    시그니처에 드러나는 `MapRegistry::transform_node`(파일 4)와 아래 검사 결과 배열이 맡는다.
 *  - **@ref RoadmapValidationResult 의 위반 배열과 @ref NodeMappingView 의 대응 id 는 뷰별 강타입**
 *    (`PhysicalNodeId`/`UniformEdgeId` 등, @ref mrs/view_ids.hpp)이다 — 계약이 배열마다 뷰·엔티티를
 *    못박았고(재잠금 8차), 이름이 아니라 타입으로 그 구분을 강제하기 위함이다.
 *
 * @note `mrs_map_registry` 는 `mrs_core_types` 에만 의존한다(강타입·`ViewScope` 는 거기서 온다).
 */

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

#include "mrs/view_ids.hpp"

namespace mrs
{

/** @brief 뷰 컨테이너의 bare 노드·엣지 id 센티넬 ("해당 없음"). 강타입 센티넬과 원시 값이 같다. */
inline constexpr std::uint32_t ROADMAP_ID_NONE = std::numeric_limits<std::uint32_t>::max();

/**
 * @brief 한 뷰의 노드 1건 — `RoadmapNode.msg` 의 도메인 미러 (물리/균일/골격 공통 표현).
 *
 * 노드가 어느 뷰에 속하는지는 이 원소가 아니라 담고 있는 @ref RoadmapViewData::view_kind 가
 * 고정한다.
 */
struct RoadmapNodeView
{
  std::uint32_t node_id{ROADMAP_ID_NONE}; ///< 뷰 내 노드 id (bare — 종류는 컨테이너가 고정)
  double x_m{0.0};                        ///< 위치 x [m], map
  double y_m{0.0};                        ///< 위치 y [m], map
  double claim_radius_m{0.0}; ///< r_clear [m]. 뷰별 규약(재잠금 8차 (j)): 균일 뷰 = 실제 인접
                              ///< 간격 최솟값/2. 하드 하한 r_clear >= r. 정밀화 OPEN(T1 O6)
  bool is_endpoint{false}; ///< well-formed 검사용 엔드포인트(대기 가능 지점) 여부
  bool is_task_endpoint{false}; ///< 작업 엔드포인트 여부 (비작업 = endpoint ∧ ¬task)
  bool is_virtual{false}; ///< 세분화로 생성된 가상 노드 여부 (물리 뷰는 항상 false)
};

/**
 * @brief 한 뷰의 엣지 1건 — `RoadmapEdge.msg` 의 도메인 미러.
 *
 * 물리 뷰 엣지는 직선이다(`|length_m − ‖p_a−p_b‖| <= tol`, 재잠금 8차 (g)). 골격 뷰 엣지는
 * 여러 물리 엣지를 삼킨 폴리라인이라 이 전제가 적용되지 않는다.
 */
struct RoadmapEdgeView
{
  std::uint32_t edge_id{ROADMAP_ID_NONE}; ///< 뷰 내 엣지 id (bare — 종류는 컨테이너가 고정)
  std::uint32_t node_a{ROADMAP_ID_NONE}; ///< 양단 노드 a (같은 뷰의 노드 id)
  std::uint32_t node_b{ROADMAP_ID_NONE}; ///< 양단 노드 b (같은 뷰의 노드 id)
  double length_m{0.0}; ///< 엣지 길이 [m]. 물리 뷰는 직선이라 ‖p_a−p_b‖ 와 tol 이내 일치
  double corridor_width_m{0.0}; ///< 최소 통로 폭 W_min [m] — 순수기하 3단 검사기 입력
  double speed_limit_mps{
    0.0}; ///< 허용 최대 몸체 속도 [m/s]. 디레이팅 반영값(v_max 의존, pending-[0b])
};

/**
 * @brief @ref NodeMappingView 의 용례 판별자 — 어느 두 뷰를 잇는 대응인지 고정한다.
 *
 * `NodeMapping.msg` 는 한 원소에 서로 다른 뷰의 id 를 담는 유일한 계약이며, 방향은 용례별로
 * 계약이 고정한다(규칙 V4). 도메인에서는 그 용례를 이 enum + 뷰별 강타입 필드로 명시해
 * bare 원시 id 오사용을 타입으로 막는다(사용자 결정, 파일 3).
 */
enum class MappingKind : std::uint8_t
{
  /** @brief 용례 1 — 균일 가상 노드 → 물리 엣지 위 위치. `to_physical_edge_id`·`s`·서수 사용. */
  SUBDIVISION = 0,
  /** @brief 용례 2 — 균일 노드 → 골격 노드. `to_skeleton_node_id` 사용, 서수 없음(denominator = 0).
   */
  COLLAPSE_NODE = 1,
  /** @brief 용례 3 — 삼켜진 균일 노드 → 골격 엣지 + 체인 서수. `to_skeleton_edge_id`·서수 사용. */
  COLLAPSE_CHAIN = 2
};

/**
 * @brief 뷰 간 노드 대응 1건 — `NodeMapping.msg` 의 도메인 미러 (D-11 왕복 무손실의 표현체).
 *
 * `from_node_id` 는 **항상 균일 뷰**다. `to_*` 는 @ref kind 가 지목하는 하나만 유효하고 나머지는
 * 센티넬이다. 순서 복원은 부동소수 `s` 가 아니라 정수 서수(@ref index / @ref denominator)로만 한다
 * — `s` 정렬은 경계에서 오차가 새어 "무손실 0"을 프로세스 밖에서 무너뜨린다(GATE Q4).
 */
struct NodeMappingView
{
  MappingKind kind{MappingKind::SUBDIVISION}; ///< 용례 판별자. 유효한 `to_*` 필드를 고정한다
  UniformNodeId from_node_id{UNIFORM_NODE_ID_NONE}; ///< 원본 — 항상 균일 뷰 노드

  SkeletonNodeId to_skeleton_node_id{SKELETON_NODE_ID_NONE}; ///< 용례 2 전용 대응 골격 노드
  PhysicalEdgeId to_physical_edge_id{PHYSICAL_EDGE_ID_NONE}; ///< 용례 1 전용 대응 물리 엣지
  SkeletonEdgeId to_skeleton_edge_id{SKELETON_EDGE_ID_NONE}; ///< 용례 3 전용 삼킨 골격 엣지

  double s{0.0}; ///< **용례 1 전용** — to_physical_edge 위 정규화 기하 위치 [0,1] (node_a→node_b).
                 ///< 용례 2·3 에서는 사용하지 않는다(순서는 서수로 담는다)
  std::uint32_t index{
    0}; ///< 정수 서수. 용례 1 = 물리 엣지 위 k(0..n) / 용례 3 = 골격 엣지 체인 내 순서
  std::uint32_t denominator{
    0}; ///< 서수 분모. 용례 1 = 물리 엣지 분할 수 n / 용례 3 = 체인 길이 / 용례 2 = 0(서수 미사용)
};

/**
 * @brief 조회 가능한 뷰 데이터 묶음 — `GetRoadmap`/`GetUniformView`/`GetDependencySkeleton` 공통
 * 반환형.
 *
 * @ref view_kind 판별자로 어느 뷰인지 지목한다(계약 U-18 해소 — 과거에는 판별자가 없어 변환 시
 * V1 종류 대조를 못 했다). `roadmap_version`·`view_id` 평면 필드는 캐시 무효화 키(Q-6)이자
 * `mrs_msg_convert` 가 직접 읽는 스코프다 — @ref scope() 로 강타입 @ref ViewScope 를 조립한다.
 */
struct RoadmapViewData
{
  ViewKind view_kind{ViewKind::PHYSICAL}; ///< 뷰 종류 판별자 (물리 = 기본, view_id 는 0)
  std::uint64_t roadmap_version{
    ROADMAP_VERSION_UNSPECIFIED};          ///< 지도 버전 (0 = 미지정, 런타임 금지)
  std::uint32_t view_id{VIEW_ID_PHYSICAL}; ///< 뷰 인스턴스 id (물리 = 0)
  double unit_length_m{0.0};               ///< 세분화 입도 [m] (균일 뷰에서만 의미)
  double unit_length_lower_bound_m{0.0}; ///< 이론 하한 2(ρ + L*) [m] (T1-R6, iw.hub = 2.211518)
  double effective_unit_min_m{
    0.0}; ///< 실측 최소 실효 입도 [m] (준균일 — GetUniformView v1.1.0 (m))
  double effective_unit_max_m{0.0}; ///< 실측 최대 실효 입도 [m]. max/min 이 1 에 가까울수록 균일
  double subdivided_edge_ratio{0.0}; ///< 분할된 물리 엣지 비율 [0,1]. 0 이면 세분화가 사실상 항등
  std::vector<RoadmapNodeView> nodes;    ///< 뷰 노드 목록
  std::vector<RoadmapEdgeView> edges;    ///< 뷰 엣지 목록
  std::vector<NodeMappingView> mappings; ///< 세분화(용례 1)·collapse(용례 2·3) 대응표

  /**
   * @brief 이 뷰의 강타입 스코프를 조립한다.
   * @return `ViewScope` — `{view_kind, roadmap_version, view_id}`. DDS 경계 종류 대조·캐시 키에
   * 쓴다.
   */
  [[nodiscard]] ViewScope scope() const noexcept
  {
    return ViewScope{view_kind, roadmap_version, view_id};
  }
};

/**
 * @brief roadmap 정적 검사기의 결과 — `ValidateMap.srv` v3.0.0 의 도메인 미러 (D-11,
 * T1-R6/T5/T6/T7).
 *
 * 검사기 3(통로 폭)은 v_max 없이 도는 **순수기하 3단**(통과/90°점턴/U턴)으로, iw.hub 실측
 * (ρ·L*)만으로 지금 강제된다. v_max 의존 닫힌형은 pending-[0b]다. 위반 배열은 뷰별·엔티티별로
 * 분리된 단일 강타입이다(어느 노드가 어느 검사에 걸렸는지 복원 가능하게).
 *
 * @warning 판정은 `bool` 과 배열로 한다 — @ref messages 는 사람이 읽을 설명 전용, 기계 판정 금지.
 */
struct RoadmapValidationResult
{
  // ── 검사기별 하드 판정 ────────────────────────────────────────────────────────────────
  bool well_formed{false}; ///< 검사기 1 (lifelong-mapd-2017 Def.1)
  bool biconnected{
    false}; ///< 검사기 2 — **[1] 미평가(U-23)**. false 는 '실패'가 아니라 '미평가'다.
            ///< result_pass 판정에 넣지 않으며 소비자도 하드 실패로 읽지 말 것
  bool pass_width_ok{false}; ///< 검사기 3 통과폭 — 모든 물리 엣지 폭 > 2ρ (하드)
  bool unit_granularity_ok{false}; ///< 검사기 3 세분화 입도 하한 l_unit >= 2(ρ+L*) (균일 뷰, 하드)
  bool r_clear_ok{false}; ///< 검사기 3 정지선 반경 하한 r_clear >= r (하드)

  // ── 위반 목록 — 뷰별·엔티티별 5배열 (각 단일 강타입) ─────────────────────────────────
  std::vector<PhysicalNodeId> violating_wellformed_node_ids; ///< 검사기 1 위반 (well-formed 미달)
  std::vector<PhysicalNodeId>
    violating_biconnected_node_ids; ///< 검사기 2 위반. [1] 은 U-23 보류로 항상 비어 있음(예약)
  std::vector<PhysicalEdgeId> narrow_pass_edge_ids; ///< 검사기 3 통과폭 하드 위반 (폭 < 2ρ)
  std::vector<UniformNodeId>
    violating_uniform_node_ids; ///< 검사기 3 입도 하한 미달 가상 노드. 물리만 검사 시 empty
  std::vector<UniformEdgeId> violating_uniform_edge_ids; ///< 입도 하한 미달 균일 구간

  // ── 정보성 3단 분류 (하드 아님 — 사다리·D-13 복구 라우팅 입력, T6) ──────────────────
  std::vector<PhysicalEdgeId> turn90_incapable_edge_ids; ///< 폭 < 2ρ+L*, 제자리 90° 회전 불가
  std::vector<PhysicalEdgeId> uturn_incapable_edge_ids; ///< 폭 < 2(ρ+L*), 360° 무후진 U턴 불가

  // ── 검사기 4 advisory (통로 mutex-후보 — result_pass 를 막지 않는다, T6/T7) ────────────
  std::vector<PhysicalEdgeId>
    mutex_candidate_edge_ids; ///< 폭 < 4ρ, 교행 불가 → D-12 통로 자원 후보 입력원

  // ── 계산된 임계 에코 (골든 회귀 대상 — ρ·L* 기하에서 계산, v_max 입력 없음) ────────────
  double pass_width_min_m{0.0};      ///< 2ρ            (iw.hub = 1.575808)
  double turn90_width_min_m{0.0};    ///< 2ρ + L*       (iw.hub = 1.893663)
  double uturn_width_min_m{0.0};     ///< 2(ρ + L*)     (iw.hub = 2.211518 = l_unit)
  double meet_pass_width_min_m{0.0}; ///< 4ρ            (iw.hub = 3.151615, 검사기 4 임계)
  double required_r_clear_m{0.0};    ///< r             (r_clear 하드 하한, iw.hub = 1.084724)

  // ── v_max 의존 닫힌형 (pending-[0b] — 미산출이면 -1) ──────────────────────────────────
  double required_min_width_m{-1.0}; ///< 2(r + v_max/omega_max) [m]. v_max 미측정이면 -1 = 미산출
  double derated_v_max_mps{-1.0}; ///< 디레이팅 속도 (W_min/2 − r)*omega_max [m/s]. 미측정이면 -1

  std::vector<std::string> messages; ///< 사람이 읽을 위반 설명 (기계 판정 금지)

  /**
   * @brief 서비스 `RESULT_PASS` 판정 — 하드 검사기 통과 여부.
   * @return `bool` — well_formed ∧ pass_width_ok ∧ unit_granularity_ok ∧ r_clear_ok.
   * @note **biconnected 는 제외**한다 — [1] 에서 검사기 2 는 미평가(U-23)라 판정에 넣지 않는다.
   */
  [[nodiscard]] bool result_pass() const noexcept
  {
    return well_formed && pass_width_ok && unit_granularity_ok && r_clear_ok;
  }
};

} // namespace mrs
