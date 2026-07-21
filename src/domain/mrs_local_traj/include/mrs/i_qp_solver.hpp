#pragma once

/**
 * @file i_qp_solver.hpp
 * @brief L4 QP 백엔드 포트 (seam b, D-07). 계약 v1.0.0.
 *
 * 표준형: min 1/2 x^T P x + q^T x  s.t.  l <= A x <= u   (OSQP 표준형)
 * 근거: theory T1 §6 — P 는 **상수**이며 매 스텝 바뀌는 것은 q, l, u, 그리고 A 의 **값**뿐이고
 * **희소 패턴은 불변**이다. 따라서 setup 1회 후 update_* 만 호출한다(T1-R4).
 * 이웃 슬롯 M 은 고정 예약하고 비활성 슬롯은 l=-inf, u=+inf 로 무력화해 패턴을 고정한다.
 * SOC 제약은 지원하지 않는다 — 속도·가속도 한계는 내접 N_p=12 다각형으로 대체한다(T1 §6.3).
 *
 * 규모(T1 §6.5, H=4, n=5, N_p=12, K=4, M=4): n_x = 76, n_c = 724, 밀도 ~5%.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace mrs
{

/**
 * @brief CSC(Compressed Sparse Column) 희소 행렬.
 * @note 패턴(row_indices, col_pointers)은 setup 이후 **변경 금지**다.
 */
struct CscMatrix
{
  std::int64_t rows{0};                   ///< 행 수
  std::int64_t cols{0};                   ///< 열 수
  std::vector<double> values;             ///< 비영 원소 값 (nnz 개)
  std::vector<std::int64_t> row_indices;  ///< 각 비영 원소의 행 인덱스 (nnz 개)
  std::vector<std::int64_t> col_pointers; ///< 열 시작 오프셋 (cols + 1 개)
};

/**
 * @brief QP 해 상태.
 */
enum class QpStatus : std::uint8_t
{
  SOLVED = 0,           ///< 최적해
  SOLVED_INACCURATE = 1,///< 완화 허용오차로 수렴 — 사용 가능하나 계측에 기록
  MAX_ITER_REACHED = 2, ///< 반복 상한 도달 — 호출자는 초기궤적 폴백(T1 §6.7)
  PRIMAL_INFEASIBLE = 3,///< 원문제 실행불가. ⚠ 정리 T1.2 하에서는 발생하지 않아야 한다 — 버그 신호
  DUAL_INFEASIBLE = 4,  ///< 쌍대 실행불가(비유계)
  SOLVER_ERROR = 5      ///< 백엔드 내부 실패
};

/**
 * @brief QP 해 결과.
 */
struct QpResult
{
  QpStatus status{QpStatus::SOLVER_ERROR}; ///< 종료 상태
  std::vector<double> x;                   ///< 결정변수 해 (길이 n_x). 실패 시 비어 있음
  double objective_value{0.0};             ///< 목적함수 값
  double solve_time_s{0.0};                ///< 해결 소요 시간 [s] (D-07 예산 판정 원천)
  std::int64_t iterations{0};              ///< 반복 횟수
};

/**
 * @brief QP 백엔드 포트 (OSQP 1순위, 대체 qpOASES/HiGHS 는 미결).
 */
class IQpSolver
{
public:
  virtual ~IQpSolver() = default;

  /**
   * @brief 문제를 1회 구성한다. 이후 희소 패턴은 고정된다.
   * @param[in] p 목적함수 이차항 P (상수, PSD, 상삼각 CSC). eps*I 정규화 포함 상태여야 한다.
   * @param[in] q 목적함수 일차항 (길이 n_x).
   * @param[in] a 제약 행렬 A (CSC).
   * @param[in] l 제약 하한 (길이 n_c). -inf 허용.
   * @param[in] u 제약 상한 (길이 n_c). +inf 허용.
   * @return bool 구성 성공 여부. false 면 호출자는 초기궤적 폴백만 사용한다.
   */
  virtual bool setup(
    const CscMatrix & p, const std::vector<double> & q,
    const CscMatrix & a, const std::vector<double> & l,
    const std::vector<double> & u) noexcept = 0;

  /**
   * @brief 목적함수 일차항만 갱신한다.
   * @param[in] q 새 일차항 (길이 n_x, setup 과 동일).
   * @return bool 갱신 성공 여부.
   */
  virtual bool update_linear_cost(const std::vector<double> & q) noexcept = 0;

  /**
   * @brief 제약 상·하한을 갱신한다.
   * @param[in] l 새 하한 (길이 n_c).
   * @param[in] u 새 상한 (길이 n_c).
   * @return bool 갱신 성공 여부.
   */
  virtual bool update_bounds(
    const std::vector<double> & l, const std::vector<double> & u) noexcept = 0;

  /**
   * @brief A 의 **값만** 갱신한다(패턴 불변 — LSC 법선벡터 등).
   * @param[in] a_values setup 시 A 와 동일한 nnz 순서의 값 배열.
   * @return bool 갱신 성공 여부. 길이 불일치 시 false.
   */
  virtual bool update_a_values(const std::vector<double> & a_values) noexcept = 0;

  /**
   * @brief 현재 구성으로 QP 를 푼다.
   * @param[in] warm_start_x 웜스타트 원시 초기값 (길이 n_x). 비우면 웜스타트 없음.
   * @return QpResult 상태·해·소요시간. 예외를 던지지 않는다.
   */
  virtual QpResult solve(const std::vector<double> & warm_start_x) noexcept = 0;

  /**
   * @brief 구현체 식별 이름.
   * @return std::string "osqp" 등.
   */
  virtual std::string name() const noexcept = 0;
};

} // namespace mrs
