#pragma once

/**
 * @file contract_types.hpp
 * @brief 계약 레지스트리 v1.0.0 의 ROS 무관 값 타입.
 *
 * 이 헤더는 `mrs_core_types` 패키지에 놓이며 **rclcpp·mrs_interfaces 에 의존하지 않는다**
 * (architecture 제약 C1 — 순수 도메인은 ROS 없이 gtest 로 돌아야 한다).
 * `.msg`/`.srv` 와의 변환은 `mrs_msg_convert` 가 **유일한 장소**로 담당한다.
 *
 * 단위·좌표계 규약(전역):
 *  - 길이 [m], 시간 [s], 각도 [rad] (구간 [-pi, pi]), 속도 [m/s], 각속도 [rad/s]
 *  - 모든 좌표는 REP-105 `map` 프레임 (오른손, x 전방, y 좌측)
 */

#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace mrs
{

/** @brief 로봇 식별자. 65535 는 "해당 없음"을 뜻한다. */
using RobotId = std::uint16_t;
/** @brief roadmap 노드 식별자. 4294967295 는 "해당 없음"을 뜻한다. */
using NodeId = std::uint32_t;
/** @brief roadmap 엣지 식별자. */
using EdgeId = std::uint32_t;
/** @brief 교란 상관 키(theory T3-R3). 0 은 미할당이며 런타임에 금지된다. */
using EventId = std::uint64_t;

/** @brief "해당 없음"을 뜻하는 로봇 id 센티넬. */
constexpr RobotId ROBOT_ID_NONE = std::numeric_limits<RobotId>::max();
/** @brief "해당 없음"을 뜻하는 노드 id 센티넬. */
constexpr NodeId NODE_ID_NONE = std::numeric_limits<NodeId>::max();

/**
 * @brief 평면 자세 (map 프레임).
 */
struct Pose2D
{
  double x_m{0.0};       ///< 위치 x [m], map 프레임
  double y_m{0.0};       ///< 위치 y [m], map 프레임
  double theta_rad{0.0}; ///< 방위각 [rad], map 프레임 +x 기준 CCW, [-pi, pi]
};

/**
 * @brief 시각 부여 node-visit — L2 솔버 출력 표준형의 원소 (D-03).
 */
struct TimedNodeVisit
{
  NodeId node_id{NODE_ID_NONE}; ///< 균일 뷰 노드 id
  double arrival_time_s{0.0};   ///< 계획상 도착 시각 [s], 시뮬 시계 기준 절대시각
};

/**
 * @brief 로봇 1대의 시각 부여 경로 (D-03 표준형).
 */
struct RobotPath
{
  RobotId robot_id{ROBOT_ID_NONE};      ///< 로봇 id
  std::vector<TimedNodeVisit> visits;   ///< 시각 단조증가 방문열
};

/**
 * @brief 커밋 상태 — theory 정의 T2.1 의 s_hat_i = (beta_i, t_hat, theta_hat).
 * @note theta 는 필수다(T2-R1). 없으면 diff-drive 회전 비용을 L2 가 반영할 수 없다.
 */
struct CommitState
{
  RobotId robot_id{ROBOT_ID_NONE};                  ///< 로봇 id
  NodeId boundary_node{NODE_ID_NONE};               ///< 커밋 경계 beta_i 노드 id
  double predicted_arrival_s{0.0};                  ///< t_hat(beta_i) [s], 절대시각
  double predicted_theta_rad{0.0};                  ///< theta_hat(beta_i) [rad], [-pi, pi]
  std::uint32_t window_seq{0};                      ///< 산출 근거 창의 seq
  std::uint32_t committed_through_segment_index{0}; ///< 커밋 접두 끝 세그먼트 인덱스
};

/**
 * @brief 릴리스되어 뒤집을 수 없는 교차 통행순서 (theory T2 조건 (C3), T2-R2).
 */
struct FrozenOrder
{
  RobotId predecessor_robot_id{ROBOT_ID_NONE}; ///< 먼저 가야 하는 로봇
  NodeId predecessor_node_id{NODE_ID_NONE};    ///< 그 로봇이 클리어해야 하는 노드
  RobotId successor_robot_id{ROBOT_ID_NONE};   ///< 뒤에 가야 하는 로봇
  NodeId successor_node_id{NODE_ID_NONE};      ///< 그 로봇이 진입하려는 노드
};

/**
 * @brief 실행 창의 단위 구간 (D-05).
 */
struct WindowSegment
{
  NodeId node_from{NODE_ID_NONE}; ///< 출발 노드 id (균일 뷰)
  NodeId node_to{NODE_ID_NONE};   ///< 도착 노드 id (균일 뷰)
};

/**
 * @brief 하드 통행순서 제약 1건 (D-05). 시각을 포함하지 않는다(T1 §7.1).
 */
struct PredecessorConstraint
{
  RobotId predecessor_robot_id{ROBOT_ID_NONE}; ///< 선행 로봇 id
  NodeId node_id{NODE_ID_NONE};                ///< 선행 로봇이 클리어해야 하는 노드 id
};

/**
 * @brief 실행 창 — 계약 #1 (load-bearing seam, D-05).
 * @note window_valid_until 과 valid_through_segment_index 는 **다른 목적**이다(v3 NF7).
 */
struct ExecutionWindow
{
  RobotId robot_id{ROBOT_ID_NONE};                        ///< 창 소유 로봇
  std::uint32_t window_seq{0};                            ///< 로봇별 단조증가 창 시퀀스
  std::uint32_t plan_epoch{0};                            ///< 유래 계획 세대
  std::uint64_t roadmap_version{0};                       ///< 지도 버전
  std::uint32_t view_id{0};                               ///< 균일 뷰 id
  std::vector<WindowSegment> segments;                    ///< 실행 순서대로
  std::vector<PredecessorConstraint> predecessor_constraints; ///< hard, 미충족분만
  double window_valid_until_s{0.0};                       ///< 창 전체 만료 시각 [s] (staleness repair)
  bool is_truncation{false};                              ///< true = 부분 폐기 통지
  std::int32_t valid_through_segment_index{-1};           ///< -1 = 전 구간 유효, k = k 이하만 유효
};

/**
 * @brief 사다리 가로대 식별자 (D-08).
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
 * @brief 에스컬레이션 사유 — 확정 집합 (Q-4, v1.1.0). 확장은 계약 변경 게이트를 거친다.
 * @note SYNC_LOST 는 HARD 이지만 **R3/R4 로 라우팅하면 안 된다** — 안전 정지·동기 복구 경로다.
 *       계획 문제로 오인해 재계획/재할당을 오발동시키면 F2 발동률·F6 스래싱 카운터가 오염된다
 *       (오케스트레이터 R-06).
 */
enum class EscalationReason : std::uint8_t
{
  BLOCKED = 0,            ///< 진행 불가 지속 (HARD)
  WINDOW_EXPIRING = 1,    ///< 창 소진 임박 (SOFT)
  QP_INFEASIBLE = 2,      ///< QP 솔버 실패로 초기궤적 폴백 발동 (HARD). 이론적 infeasibility 아님
  LOCALIZATION_LOST = 3,  ///< 위치추정 상실 (HARD)
  SYNC_LOST = 4           ///< /plan_tick 연속 결번 k 회 초과 — N5 미성립 (HARD, 동기 복구 경로)
};

} // namespace mrs
