#pragma once

/**
 * @file shift_operator.hpp
 * @brief 시프트 연산자 S — compute_initial_trajectory() (T1-R3, T4-B1). 계약 v2.0.0 핵심 함수.
 *
 * theory T4 §8-(a) S1: 인증 폴백은 (1) S 적용 (2) 채택 (3) 발행 (4) 실행 **네 항목 전부**다.
 * S 는 de Casteljau 분할로 1번 세그먼트의 [u,1] 을 [0,1] 로 신축한다(원문 식 14~16),
 * m>1 세그먼트는 제어점 불변·시각만 +Δt_h. **자기 데이터와 상수 u 만으로 계산**된다(T4 S-a).
 *
 * ⛔ 매 틱 무조건 선행 실행(QP 성공 여부 무관). ⛔ "직전 궤적 계속 실행"으로 대체 금지(명제 T4.2).
 */

#include <cstdint>
#include <vector>

namespace mrs
{

/**
 * @brief 시프트 연산자 S — 매 틱 선행 실행되는 네 역할(존재증서·웜스타트·솔버폴백·폴백채택)의
 * 단일 함수.
 */
class ShiftOperator
{
public:
  ShiftOperator() = default;
  ~ShiftOperator() = default;

  /**
   * @brief 직전 채택 계획에 시프트를 적용해 c_hat^(h) = S(c^(h-1)) 를 계산한다.
   * @param[in] previous_control_points 직전 스텝의 채택 계획 제어점 (평탄 (x,y) 배열, map 프레임).
   * @param[in] shift_ratio_u u = Δt_h/Δt (N4 에 의해 0 < u < 1).
   * @return std::vector<double> 시프트된 제어점 (같은 길이·형식).
   */
  std::vector<double> compute_initial_trajectory(
    const std::vector<double> & previous_control_points, double shift_ratio_u) const;
};

} // namespace mrs
