#pragma once

/**
 * @file i_local_planner.hpp
 * @brief L4 국소 궤적 계획 포트 (seam b 확장, D-07·D-05). 계약 v2.0.0.
 *
 * v1.1.0 → v2.0.0 (**major**, 사용자 결정 R-14 (a)): 직접 노드 id 필드는 없으나, 입력
 * `ExecutionWindow` 의 `WindowSegment.node_from/node_to` 와 `PredecessorConstraint.node_id` 가
 * **`UniformNodeId`** 강타입이 되어 이 포트의 시그니처 의미가 바뀐다. 구현체가 맨 uint32 로
 * 창 노드를 다루면 컴파일되지 않는다. 정지선 노드 u_l 도 균일 뷰이며, 그것이 상향
 * `EscalationReport.blocking_node_id`(균일 뷰 확정)의 근거다.
 *
 * v1.0.0 → v1.1.0 (theory T4 / 확정서 v4): 폴백이 QP 실패 경로가 아니라 **매 틱 주 경로**로
 * 이동했다. 구현은 매 틱 (1) 시프트 연산자 S 를 무조건 선행 실행하고, (2) solve_ok(h) 로
 * QP 해와 인증 폴백 중 하나를 채택하며, (3) 채택 계획을 항상 발행한다(LocalPlanShare).
 * 별도 정지 궤적 경로는 **제거**한다(명제 T4.4 — 불안전하며 불필요).
 *
 * 구현체 2종(D-07 verification 의 양방향 ablation):
 *  - NeighborAwareLscPlanner : 이웃 고려 분산 MPC + NID-LSC corridor QP (1순위)
 *  - SingleRobotMpcPlanner   : 이웃 고려 없는 단일로봇 MPC + 추종 (ablation 비교군)
 *
 * 이론 구속 (theory T1):
 *  - 좌표는 NID 오프셋점 z = q + L*e1(theta), 속도 상한 u_max = min(v_max, L*omega_max) (T1-R2).
 *  - 하드 통행순서는 **이동하는 공간 정지선**으로 QP 에 편입되며(T1 §7.2), 정리 T1.2 에 의해
 *    QP 를 infeasible 로 만들지 않고 감속으로 변환된다 ⇒ 순서를 뒤집을 자유도가 없다(C6-1 소멸).
 *  - dlsc FORWARD/RIGHT/BACK 모드 상태기계는 **구현하지 않는다**(T1-R8). 감속 정지 + 에스컬레이션.
 *  - compute_initial_trajectory() (= 시프트 연산자 S) 는 **네 역할의 단일 함수**다(T1-R3 + T4-B1):
 *    존재 증서 · OSQP 웜스타트 · 솔버 실패 폴백 · **인증 폴백 시의 채택 계획**.
 *    매 틱 무조건 선행 실행하며, QP 성공 여부와 무관하다.
 *  - ⛔ 직전 궤적을 그대로 계속 실행하는 경로를 만들지 말 것(명제 T4.2 반례 — 이웃 상정보다
 *    1/(1-u) 배 빠르게 통과해 보장이 소멸한다). 폴백 대상은 항상 S(c^(h-1)) 다.
 *  - ⛔ 상수 정지 궤적 경로를 만들지 말 것(명제 T4.4 반례). 정지는 폴백 반복의 극한으로
 *    **도달**되며((1-u)^r 수렴, 이동거리 <= u_max*Δt), 명령되지 않는다.
 */

#include <cstdint>
#include <string>
#include <vector>

#include "mrs/contract_types.hpp"

namespace mrs
{

/**
 * @brief 이웃 로봇의 **직전 스텝 채택 계획** (LSC 제약 입력, LocalPlanShare 수신분).
 * @note 신선도 판정은 나이가 아니라 `tick_seq == h-1` 로 한다(T4 §6.1 fresh_j).
 *       `age_s` 는 계측량일 뿐이며 **안전 판정에 쓰지 않는다** — NF2' 도달집합 팽창 폴백은
 *       거울 성질 위반으로 **철회**됐다(T4 §7.3, 확정서 v4 D-10).
 */
struct NeighborTrajectory
{
  RobotId robot_id{ROBOT_ID_NONE};    ///< 이웃 로봇 id
  std::uint32_t tick_seq{0};          ///< 이 계획의 스텝 지수 h. fresh_j(h-1) 판정의 유일한 근거
  std::vector<double> control_points; ///< 채택 계획 c^(h) 의 오프셋점 z 제어점 [m], (x,y) 평탄 배열
  double shift_ratio_u{0.0};          ///< 발신자의 u = Δt_h/Δt. 자기 값과 다르면 (A1) 위반 → 폴백
  double age_s{0.0};                  ///< 수신 후 경과 시간 [s]. **계측 전용** (안전 근거 아님)
};

/**
 * @brief 국소 계획 입력.
 */
struct LocalPlanInput
{
  ExecutionWindow window;                        ///< 현재 수용 중인 실행 창 (hard 순서 포함)
  Pose2D body_pose;                              ///< 측정된 몸체 자세 q [m, rad], map
  double body_v_mps{0.0};                        ///< 몸체 선속도 [m/s]
  double body_omega_rps{0.0};                    ///< 몸체 각속도 [rad/s]
  std::vector<NeighborTrajectory> neighbors;     ///< 이웃 궤적. 단일로봇 ablation 에서는 무시된다
  std::uint32_t tick_seq{0};                     ///< 스텝 지수 h (틱 수신·국소 클록·이웃 tick_seq 중 복원)
  bool tick_index_known{false};                  ///< know_h — h 를 복원했는가. 거짓이면 폴백 강제
  double tick_time_s{0.0};                       ///< 이 틱의 계획 기준 시각 t_h [s]
  double tick_jitter_s{0.0};                     ///< 틱 도착 지터 [s]. **계측 전용** — 지터 자체는
                                                 ///< 폴백 사유가 아니다(T4 §6.1, L-15 S4)
};

/**
 * @brief 스텝 h 의 채택 결과 종류 (CommitStatus.msg 의 qp_status 값과 일치해야 한다).
 */
enum class AdoptionStatus : std::uint8_t
{
  QP_OK = 0,                  ///< solve_ok(h) 참 — QP 해 채택
  FALLBACK_GENERIC = 1,       ///< 인증 폴백 (사유 미분류)
  FALLBACK_NO_INDEX = 3,      ///< know_h 거짓
  FALLBACK_NEIGHBOR_STALE = 4,///< fresh_j(h-1) 거짓 — QP 금지(정리 T4.3)
  FALLBACK_SOLVER = 5         ///< solver_ok 거짓
};

/**
 * @brief 국소 계획 출력.
 */
struct LocalPlanOutput
{
  double cmd_v_mps{0.0};                    ///< 몸체 선속도 지령 [m/s]
  double cmd_omega_rps{0.0};                ///< 몸체 각속도 지령 [rad/s]
  std::vector<double> control_points;       ///< **채택 계획 c^(h)** 의 제어점 [m]. 매 틱
                                            ///< LocalPlanShare 로 발행한다(폴백 중에도 예외 없음).
                                            ///< 시프트 결과 c_hat 을 발행하면 한 스텝 어긋난다
  std::vector<Pose2D> commit_hull_vertices; ///< 커밋 볼록포 C_i 정점 [m], map — CommitStatus 로 상향
  bool used_fallback{true};                 ///< true = 인증 폴백 채택 (정의 T4.1)
  AdoptionStatus adoption{AdoptionStatus::FALLBACK_GENERIC}; ///< 채택 종류·폴백 사유 (계측 B5)
  double qp_solve_time_s{0.0};              ///< QP 소요 시간 [s]
  std::uint32_t cusp_count{0};              ///< 이 지평에서 관측된 첨점 수 (NF3 첨점 밀도 계측)
  bool escalation_requested{false};         ///< true = EscalationReport 발행 필요
  EscalationReason escalation_reason{EscalationReason::BLOCKED}; ///< 위 플래그가 true 일 때 유효
};

/**
 * @brief L4 국소 궤적 계획 포트.
 */
class ILocalPlanner
{
public:
  virtual ~ILocalPlanner() = default;

  /**
   * @brief 한 재계획 틱 분의 국소 궤적·지령을 계산한다.
   * @param[in] input 창·자기 상태·이웃 궤적·틱 정보 (LocalPlanInput).
   * @return LocalPlanOutput 지령·궤적·커밋 볼록포·폴백 여부. 예외를 던지지 않는다.
   */
  virtual LocalPlanOutput plan(const LocalPlanInput & input) noexcept = 0;

  /**
   * @brief 이웃 궤적을 실제로 사용하는 구현인지 신고한다(ablation 판별·계측 태깅용).
   * @return bool true = 이웃 고려 분산 MPC, false = 단일로봇 MPC.
   */
  virtual bool considers_neighbors() const noexcept = 0;

  /**
   * @brief 구현체 식별 이름.
   * @return std::string "neighbor_lsc" | "single_mpc".
   */
  virtual std::string name() const noexcept = 0;
};

} // namespace mrs
