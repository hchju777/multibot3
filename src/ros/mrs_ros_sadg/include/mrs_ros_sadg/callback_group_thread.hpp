#pragma once

/**
 * @file callback_group_thread.hpp
 * @brief 전용 스레드 + 단일 실행자로 콜백그룹 하나를 돌리는 RAII 래퍼 (nav2-reference §A1 채택).
 *
 * ## 왜 필요한가 (계약 Q-3)
 * `sadg_service` 는 **릴리스 경로의 심장**이다. 계약 Q-3 은 "릴리스 콜백은 절대 무거운 작업을
 * 기다리지 않는다"를 규정하고, 그 이유를 명시한다 — 릴리스가 막히면 **D-05 창 소비율 지표가
 * 오염**되어 seam 실험의 판정이 무효가 된다.
 *
 * [0a] 에는 MILP 도 judge 도 없지만 **구조를 지금 잡는다**: 릴리스 타이머는 노드 기본 실행자에
 * 남기고, 계획 수신·에스컬레이션 수신·지도 조회처럼 "언젠가 무거워질" 경로는 이 래퍼가 도는
 * 별도 그룹에 둔다. [3] 에서 MILP 워커가 들어올 때 배선을 다시 짜면 그 시점의 릴리스 지연
 * 지표는 이미 오염된 뒤다.
 *
 * ## 왜 `Reentrant`·`MultiThreadedExecutor` 가 아닌가
 * Nav2 클론 전수 조사에서 `Reentrant`·`MultiThreadedExecutor` 사용이 **0건**이고
 * `MutuallyExclusive` 가 23개 파일이었다(nav2-reference §A1). Reentrant 로 가면 SADG 자료구조의
 * 잠금 정합성 부담과 결정론 소실이 함께 온다 — pysim 결정론은 이미 U-10 으로 유보 중이라
 * 여기서 더 잃을 여유가 없다.
 *
 * ## 수명 규약 (nav2-reference §B3)
 * 소멸자는 `cancel()` → `join()` 순으로 되감고, **예외를 밖으로 내지 않는다**. 정리 함수는
 * 여러 종료 경로에서 여러 번 불릴 수 있다는 전제를 지킨다(멱등).
 *
 * @warning 이 객체는 **노드 멤버 중 마지막에 만들어야 한다**. 생성 즉시 스레드가 콜백을 돌리므로,
 *          아직 초기화되지 않은 멤버를 콜백이 건드릴 수 있다.
 * @note ⚠ 공통 `ros/` 헬퍼 패키지로 승격할 대상이다([0a] 시점에 그 패키지가 없어 여기 둔다).
 */

#include <memory>
#include <thread>

#include <rclcpp/rclcpp.hpp>

namespace mrs::ros_sadg
{

/**
 * @brief 콜백그룹 1개를 전용 `SingleThreadedExecutor` + 전용 스레드로 실행하는 RAII 객체.
 */
class CallbackGroupThread final
{
public:
  /**
   * @brief 그룹을 전용 실행자에 등록하고 스레드를 띄운다.
   * @param[in] group 실행할 콜백그룹. 자료형 `rclcpp::CallbackGroup::SharedPtr`.
   *            **`automatically_add_to_executor_with_node = false`** 로 만든 그룹이어야 한다 —
   *            그렇지 않으면 노드 기본 실행자와 이 실행자가 같은 콜백을 두고 경합한다.
   * @param[in] node_base 그룹이 속한 노드의 base 인터페이스. 자료형
   *            `rclcpp::node_interfaces::NodeBaseInterface::SharedPtr`.
   * @param[in] logger 실패 보고에 쓸 로거. 자료형 `rclcpp::Logger`.
   */
  CallbackGroupThread(
    rclcpp::CallbackGroup::SharedPtr group,
    rclcpp::node_interfaces::NodeBaseInterface::SharedPtr node_base, rclcpp::Logger logger);

  /**
   * @brief 실행자를 취소하고 스레드를 합류시킨다. 예외를 밖으로 내지 않는다.
   */
  ~CallbackGroupThread();

  CallbackGroupThread(const CallbackGroupThread &) = delete;
  CallbackGroupThread & operator=(const CallbackGroupThread &) = delete;
  CallbackGroupThread(CallbackGroupThread &&) = delete;
  CallbackGroupThread & operator=(CallbackGroupThread &&) = delete;

  /**
   * @brief 실행자를 멈추고 스레드를 합류시킨다 (멱등 — 여러 번 불러도 안전).
   * @return void
   */
  void stop() noexcept;

private:
  rclcpp::executors::SingleThreadedExecutor executor_; ///< 이 그룹 전용 단일 실행자
  rclcpp::Logger logger_;                              ///< 실패 보고용 로거
  std::thread thread_;                                 ///< `executor_.spin()` 을 도는 전용 스레드
};

} // namespace mrs::ros_sadg
