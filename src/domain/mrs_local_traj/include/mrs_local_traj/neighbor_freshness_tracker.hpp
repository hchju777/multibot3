#pragma once

/**
 * @file neighbor_freshness_tracker.hpp
 * @brief 이웃 신선도 추적기 — fresh_j(h-1) 술어 (T4 §6.1, S0). LocalPlanShare 수신 상태 보관.
 *
 * solve_ok(h) = know_h AND (모든 이웃 j 에 fresh_j(h-1)) AND solver_ok. 이 클래스는 두 번째
 * 조건(이웃 신선도)의 판정 근거를 보관한다. shift_ratio_u 불일치는 (A1) 위반으로 간주해
 * 그 이웃을 신선하지 않은 것으로 취급한다(로그).
 */

#include <cstdint>
#include <unordered_map>

#include "mrs/contract_types.hpp"

namespace mrs
{

/**
 * @brief 이웃별 최종 수신 tick_seq 를 추적해 fresh_j(h-1) 를 판정한다.
 */
class NeighborFreshnessTracker
{
public:
  NeighborFreshnessTracker() = default;
  ~NeighborFreshnessTracker() = default;

  /**
   * @brief 이웃의 LocalPlanShare 수신을 기록한다.
   * @param[in] robot_id 이웃 로봇 id.
   * @param[in] tick_seq 수신한 계획의 스텝 지수 h.
   * @param[in] shift_ratio_u 발신자의 u = Δt_h/Δt. 자기 값과 다르면 (A1) 위반.
   * @return void
   */
  void on_neighbor_plan(RobotId robot_id, std::uint32_t tick_seq, double shift_ratio_u);

  /**
   * @brief 이웃이 요구 스텝에 대해 신선한지 판정한다 (fresh_j(h-1)).
   * @param[in] robot_id 이웃 로봇 id.
   * @param[in] required_tick_seq 요구 스텝 지수 (보통 h-1).
   * @return bool true = 신선(QP 허용 조건 중 하나 충족). false = 폴백 강제(정리 T4.3).
   */
  bool is_fresh(RobotId robot_id, std::uint32_t required_tick_seq) const;

private:
  struct NeighborRecord
  {
    std::uint32_t last_tick_seq{0}; ///< 최종 수신 tick_seq
    double shift_ratio_u{0.0};      ///< 최종 수신 u
  };
  std::unordered_map<RobotId, NeighborRecord> neighbors_; ///< 이웃별 최신 기록
};

} // namespace mrs
