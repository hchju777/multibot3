#include "mrs_bringup/startup_check_node.hpp"
#include "mrs/startup_checks.hpp"

namespace mrs
{

StartupCheckNode::StartupCheckNode() : rclcpp::Node("bringup_startup_check")
{
  // [0a] 실측 대상: sim_step_s 기본값 0.05 는 임시 placeholder 다(수치 창작 금지 규율 —
  // [0a] tracer bullet 실측 전까지 문헌·확정 근거가 없다). replan_period_s 기본 0.1 은
  // theory T1 §6.1 확정값(Q-11)이다.
  const double replan_period_s = this->declare_parameter<double>("replan_period_s", 0.1);
  const double sim_step_s = this->declare_parameter<double>("sim_step_s", 0.05);

  passed_ = mrs::is_integer_multiple(replan_period_s, sim_step_s);

  if (!passed_)
  {
    RCLCPP_FATAL(
      this->get_logger(),
      "R-A2 위반: replan_period_s(%f) 가 sim_step_s(%f) 의 정수배가 아닙니다 — 기동을 "
      "거부합니다. (F5 가 정리 T1 을 검증하지 못하는 상태로 실험이 돌아갑니다.)",
      replan_period_s, sim_step_s);
    return;
  }
  RCLCPP_INFO(
    this->get_logger(), "R-A2 검사 통과 — replan_period_s=%f, sim_step_s=%f", replan_period_s,
    sim_step_s);
}

} // namespace mrs
