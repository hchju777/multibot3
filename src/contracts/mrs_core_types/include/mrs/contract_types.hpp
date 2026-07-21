#pragma once

/**
 * @file contract_types.hpp
 * @brief 계약 레지스트리의 ROS 무관 값 타입.
 *
 * 이 헤더는 `mrs_core_types` 패키지에 놓이며 **rclcpp·mrs_interfaces 에 의존하지 않는다**
 * (아키텍처 제약 C1 — 순수 도메인은 ROS 없이 gtest 로 돌아야 한다).
 * `.msg`/`.srv` 와의 변환은 `mrs_msg_convert` 가 **유일한 장소**로 담당한다.
 *
 * ## 단위·좌표계 규약 (전역)
 *  - 길이 [m], 시간 [s], 각도 [rad] (구간 [-pi, pi]), 속도 [m/s], 각속도 [rad/s]
 *  - 모든 좌표는 REP-105 `map` 프레임 (오른손, x 전방, y 좌측)
 *  - **모든 절대 시각은 시뮬 시계(`use_sim_time`) 기준이며 벽시계와 혼용하지 않는다.**
 *    창 만료 판정(불변식 W5)이 `now` 와 같은 시계를 전제하므로, 시계가 섞이면 창이
 *    엉뚱한 시점에 실효된다. (계약 §0, `ExecutionWindow.msg`)
 *  - **이 헤더가 나르는 `pose`·`theta` 는 전부 몸체점 q 다.** NID 오프셋점
 *    \f$ z = q + L \cdot e_1(\theta) \f$ 는 L4 내부 표현이며 계약에 노출하지 않는다.
 *    q/z 혼동은 T1 재유도 전체가 걸린 구분이다. (계약 §0)
 *
 * ## 노드 id
 * 노드 id 는 **뷰별 강타입**이다 — @ref view_ids.hpp 참조. 어느 필드가 어느 뷰인지는
 * 계약 §0.1 의 뷰 종류 정본표가 고정하며, 아래 각 필드 주석에 그대로 옮겨 적었다.
 */

#include <cstdint>
#include <limits>
#include <vector>

#include "mrs/view_ids.hpp"

namespace mrs
{

/** @brief 로봇 식별자. 65535 는 "해당 없음"을 뜻한다. */
using RobotId = std::uint16_t;
/** @brief roadmap 엣지 식별자. 4294967295 는 "해당 없음"을 뜻한다. */
using EdgeId = std::uint32_t;
/** @brief 교란 상관 키(theory T3-R3). 0 은 미할당이며 런타임에 금지된다. */
using EventId = std::uint64_t;

/**
 * @brief roadmap 노드 식별자 — **과도기 별칭. 신규 코드에서 사용 금지.**
 * @deprecated 뷰별 강타입(@ref PhysicalNodeId / @ref UniformNodeId / @ref SkeletonNodeId)으로
 *             대체됐다(R-14 (a) = B). 이 별칭이 남아 있는 곳이 곧 마이그레이션 잔여물 목록이며,
 *             전량 교체 시점은 [1](D-11 MapRegistry)이다.
 */
using NodeId = std::uint32_t;

/** @brief "해당 없음"을 뜻하는 로봇 id 센티넬. */
constexpr RobotId ROBOT_ID_NONE = std::numeric_limits<RobotId>::max();
/** @brief "해당 없음"을 뜻하는 노드 id 센티넬 (과도기 별칭용 — 강타입은 `*_NODE_ID_NONE` 사용). */
constexpr NodeId NODE_ID_NONE = std::numeric_limits<NodeId>::max();
/** @brief "해당 없음"을 뜻하는 엣지 id 센티넬. */
constexpr EdgeId EDGE_ID_NONE = std::numeric_limits<EdgeId>::max();
/** @brief 미할당 상관 키. 런타임에 이 값이 실려 오면 계측이 상관관계를 잃는다. */
constexpr EventId EVENT_ID_UNASSIGNED = 0U;

/**
 * @brief 상관 키가 실제로 할당된 값인지 검사한다.
 *
 * 불변식 E1(`event_id != 0`)의 유일한 강제 지점이다. 산문으로만 규정하면 grep 으로 찾을 수
 * 없고 맨 리터럴 `0` 비교가 흩어진다.
 *
 * @param[in] event_id 검사할 상관 키. 자료형 `EventId`(`std::uint64_t`).
 * @return `bool` — 할당된 키이면 true, 미할당(0)이면 false.
 */
[[nodiscard]] constexpr bool is_valid_event_id(EventId event_id) noexcept
{
  return event_id != EVENT_ID_UNASSIGNED;
}

/**
 * @brief 평면 자세 (map 프레임, 몸체점 q).
 */
struct Pose2D
{
  double x_m{0.0};       ///< 위치 x [m], map 프레임, 몸체점 q
  double y_m{0.0};       ///< 위치 y [m], map 프레임, 몸체점 q
  double theta_rad{0.0}; ///< 방위각 [rad], map 프레임 +x 기준 CCW, [-pi, pi]
};

/**
 * @brief 시각 부여 node-visit — L2 솔버 출력 표준형의 원소 (D-03).
 */
struct TimedNodeVisit
{
  UniformNodeId node_id{}; ///< 노드 id. 뷰 종류 **UNIFORM**(계약 §0.1). 기본값은 센티넬
  double arrival_time_s{0.0}; ///< 계획상 도착 시각 [s], 시뮬 시계 기준 절대시각
};

/**
 * @brief 로봇 1대의 시각 부여 경로 (D-03 표준형).
 */
struct RobotPath
{
  RobotId robot_id{ROBOT_ID_NONE};    ///< 로봇 id
  std::vector<TimedNodeVisit> visits; ///< 시각 단조증가 방문열
};

/**
 * @brief 커밋 상태 — theory 정의 T2.1 의 \f$ \hat{s}_i = (\beta_i, \hat{t}, \hat{\theta}) \f$.
 * @note theta 는 필수다(T2-R1). 없으면 diff-drive 회전 비용을 L2 가 반영할 수 없다.
 */
struct CommitState
{
  RobotId robot_id{ROBOT_ID_NONE};                  ///< 로봇 id
  UniformNodeId boundary_node{};                    ///< 커밋 경계 \f$\beta_i\f$. 뷰 **UNIFORM**
  double predicted_arrival_s{0.0};                  ///< \f$\hat{t}(\beta_i)\f$ [s], 시뮬 시계 절대시각
  double predicted_theta_rad{0.0};                  ///< \f$\hat{\theta}(\beta_i)\f$ [rad], [-pi, pi], 몸체점 q
  std::uint32_t window_seq{0};                      ///< 산출 근거 창의 seq
  std::uint32_t committed_through_segment_index{0}; ///< 커밋 접두 끝 세그먼트 인덱스
};

/**
 * @brief 릴리스되어 뒤집을 수 없는 교차 통행순서 (theory T2 조건 (C3), T2-R2).
 */
struct FrozenOrder
{
  RobotId predecessor_robot_id{ROBOT_ID_NONE}; ///< 먼저 가야 하는 로봇
  UniformNodeId predecessor_node_id{};         ///< 그 로봇이 클리어해야 하는 노드. 뷰 **UNIFORM**
  RobotId successor_robot_id{ROBOT_ID_NONE};   ///< 뒤에 가야 하는 로봇
  UniformNodeId successor_node_id{};           ///< 그 로봇이 진입하려는 노드. 뷰 **UNIFORM**
};

/**
 * @brief 실행 창의 단위 구간 (D-05).
 */
struct WindowSegment
{
  UniformNodeId node_from{}; ///< 출발 노드. 뷰 **UNIFORM**
  UniformNodeId node_to{};   ///< 도착 노드. 뷰 **UNIFORM**
};

/**
 * @brief 하드 통행순서 제약 1건 (D-05). 시각을 포함하지 않는다(T1 §7.1).
 */
struct PredecessorConstraint
{
  RobotId predecessor_robot_id{ROBOT_ID_NONE}; ///< 선행 로봇 id
  UniformNodeId node_id{};                     ///< 선행 로봇이 클리어해야 하는 노드. 뷰 **UNIFORM**
};

/**
 * @brief 실행 창 개정의 종류 (D-05, `ExecutionWindow.msg` 의 `REVISION_*` 미러).
 *
 * @note `window_valid_until` 하나로 staleness repair 와 접합 시 부분 폐기를 겸할 수 없다(v3 NF7).
 *       전자는 창 **전체** 실효이나 후자는 커밋 경계 안쪽(L4 가 이미 QP 를 푼 구간)을 보호해야
 *       하며, 전체 폐기는 T1 의 I-QP 전제를 깨 feasibility 귀납을 무너뜨린다.
 */
enum class RevisionKind : std::uint8_t
{
  NEW = 0,     ///< 새 창 릴리스
  TRUNCATE = 1 ///< 기존 창의 부분 폐기 통지 — `valid_through_segment_index` 이하만 유효
};

/**
 * @brief 실행 창 — 계약 #1 (load-bearing seam, D-05).
 *
 * @note `schema_version` 검사는 `mrs_msg_convert` 경계에서 수행되며 **이 구조체에는 실리지
 *       않는다** — 도메인은 와이어 스키마 버전을 몰라야 한다. 비교 대상 값은
 *       @ref EXECUTION_WINDOW_SCHEMA_VERSION 이다.
 * @note 모든 노드 id 는 뷰 **UNIFORM** 이며, 인스턴스 스코프는 이 구조체의
 *       `roadmap_version` + `view_id` 쌍이 담당한다(규칙 V2 — 별도 `ViewScope` 를 두지 않는다).
 */
struct ExecutionWindow
{
  RobotId robot_id{ROBOT_ID_NONE};      ///< 창 소유 로봇
  std::uint32_t window_seq{0};          ///< 로봇별 단조증가 창 시퀀스
  std::uint32_t plan_epoch{0};          ///< 유래 계획 세대
  std::uint64_t roadmap_version{ROADMAP_VERSION_UNSPECIFIED}; ///< 지도 버전 (인스턴스 스코프)
  std::uint32_t view_id{0};             ///< 균일 뷰 id (인스턴스 스코프)
  std::vector<WindowSegment> segments;  ///< 실행 순서대로
  std::vector<PredecessorConstraint> predecessor_constraints; ///< hard, 미충족분만
  double window_valid_until_s{0.0};     ///< 창 전체 만료 시각 [s], 시뮬 시계 (staleness repair)
  RevisionKind revision_kind{RevisionKind::NEW}; ///< 개정 종류
  std::int32_t valid_through_segment_index{-1};  ///< -1 = 전 구간 유효, k = k 이하만 유효
};

/**
 * @brief 사다리 가로대 식별자 (D-08).
 * @note `RungEvent.msg` 의 `rung` 값과 일치한다. 이 enum 은 미러이지 정본이 아니다.
 */
enum class Rung : std::uint8_t
{
  R0_LOCAL_ABSORB = 0, ///< 창 내 국소 흡수 (L4)
  R1_BTPG = 1,         ///< BTPG 순서 유연화
  R2_MILP = 2,         ///< SADG MILP 재조정
  R3_REPLAN = 3,       ///< 부분 경로 재계획
  R4_REASSIGN = 4,     ///< 작업 재할당
  R5_OPERATOR = 5      ///< 운영자 알림(사람)
};

/**
 * @brief 에스컬레이션 사유 — 확정 집합 (Q-4).
 *
 * @note 이 enum 은 `EscalationReport.msg` 의 **미러이지 정본이 아니다** — severity 매핑의
 *       정본은 계약 레지스트리 L-04 표이며, 그 매핑을 코드로 옮긴 것이 @ref severity_of 다.
 * @note SYNC_LOST 는 HARD 이지만 **R3/R4 로 라우팅하면 안 된다** — 안전 정지·동기 복구 경로다.
 *       계획 문제로 오인해 재계획/재할당을 오발동시키면 F2 발동률·F6 스래싱 카운터가 오염된다
 *       (오케스트레이터 R-06). 그 라우팅 판정은 `mrs_ladder` 소관이며 이 헤더에 두지 않는다.
 */
enum class EscalationReason : std::uint8_t
{
  BLOCKED = 0,           ///< 진행 불가 지속 (HARD)
  WINDOW_EXPIRING = 1,   ///< 창 소진 임박 (SOFT)
  QP_INFEASIBLE = 2,     ///< QP 솔버 실패로 인증 폴백 발동 (HARD). 이론적 infeasibility 아님
  LOCALIZATION_LOST = 3, ///< 위치추정 상실 (HARD)
  SYNC_LOST = 4          ///< /plan_tick 연속 결번 k 회 초과 — N5 미성립 (HARD, 동기 복구 경로)
};

/**
 * @brief 에스컬레이션 보고의 심각도 (`EscalationReport.msg` 의 `severity` 미러).
 *
 * @warning @ref EscalationReason 과 값 코딩이 같아 보이는 열거가 둘 더 있으나(`RungTriggerKind`
 *          = 트리거 **출처**, `ReassignTrigger`) 계약이 서로 다른 이름을 붙인 **별개 열거**다.
 *          병합하면 이름 기반 변환 검증이 무너진다 — 합치지 말 것.
 */
enum class EscalationSeverity : std::uint8_t
{
  HARD = 0, ///< 즉시 대응 필요 — 로봇이 더 진행할 수 없거나 전제가 깨졌다
  SOFT = 1  ///< 선제적 통지 — 실행은 가능하나 여유가 줄고 있다
};

/**
 * @brief 에스컬레이션 사유로부터 심각도를 결정한다 (계약 L-04 매핑표의 코드 정본).
 *
 * 두 곳에서 따로 정의하면 즉시 드리프트가 나므로 여기 하나만 둔다. 사다리 라우팅은 이 값을
 * 입력으로 쓰되, 라우팅 정책 자체는 `mrs_ladder` 소관이다.
 *
 * @param[in] reason 에스컬레이션 사유. 자료형 `EscalationReason`.
 * @return `EscalationSeverity` — WINDOW_EXPIRING 이면 SOFT, 나머지 4종은 HARD.
 */
[[nodiscard]] constexpr EscalationSeverity severity_of(EscalationReason reason) noexcept
{
  return reason == EscalationReason::WINDOW_EXPIRING ? EscalationSeverity::SOFT
                                                     : EscalationSeverity::HARD;
}

/**
 * @brief 가로대 사건의 전이 종류 (`RungEvent.msg` 미러, F2·F6 계측의 축).
 * @note `RungEvent` 는 스스로 "스키마 드리프트 = 실험 무효"를 선언하며, 계약이 이 메시지의
 *       필드를 예외적으로 major 취급한다. 값을 바꾸려면 계약 변경 게이트를 거쳐야 한다.
 */
enum class RungTransition : std::uint8_t
{
  ENTER = 0,        ///< 이 가로대에 진입
  ABSORBED = 1,     ///< 이 가로대에서 교란을 흡수 — 상위로 올리지 않았다
  ESCALATE = 2,     ///< 흡수 실패로 상위 가로대에 반환
  GUARD_REJECT = 3, ///< G-ε 수용 가드가 거부 (D-08 v3 — 스래싱 억제)
  ESCAPE_HATCH = 4  ///< 규정 경로 밖의 탈출 — 관측되면 설계 가정이 깨진 것이다
};

/**
 * @brief 가로대를 발동시킨 트리거의 **출처** (`RungEvent.msg` 미러, D-08 이원 트리거).
 * @warning @ref EscalationSeverity 와 값 코딩이 같으나 의미가 다르다(심각도 vs 출처).
 *          계약이 별개 열거로 규정했으므로 병합 금지.
 */
enum class RungTriggerKind : std::uint8_t
{
  HARD = 0, ///< feasibility 트리거 — L4 EscalationReport '주행 불가', 반응형
  SOFT = 1  ///< efficiency 트리거 — judge slack 임계 초과, 선제형
};

/**
 * @brief `ExecutionWindow.msg` 의 와이어 스키마 버전.
 * @note 발행자(`mrs_depgraph`)·변환자(`mrs_msg_convert`)·수신자(`mrs_ros_l4`, `mrs_bench`)가
 *       같은 리터럴을 공유해야 하므로 여기 한 곳에만 둔다. 불일치 시 수신자는 폐기 + 로그.
 */
constexpr std::uint16_t EXECUTION_WINDOW_SCHEMA_VERSION = 1U;

/**
 * @brief `RungEvent.msg` 의 와이어 스키마 버전.
 * @note 이 메시지의 스키마 드리프트는 곧 실험 무효다 — F2 발동률·F6 스래싱 카운터가 여기 걸려 있다.
 */
constexpr std::uint16_t RUNG_EVENT_SCHEMA_VERSION = 1U;

} // namespace mrs
