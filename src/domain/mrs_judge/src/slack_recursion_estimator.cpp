#include <stdexcept>

#include "mrs_judge/slack_recursion_estimator.hpp"

namespace mrs
{

JudgeObservation SlackRecursionEstimator::estimate(EventId event_id) const
{
  (void)event_id;
  throw std::logic_error(
    "not implemented: SlackRecursionEstimator::estimate — Phase 5 (coordination-builder) 대상");
}

GuardDecision SlackRecursionEstimator::check_guard(
  double phi_hat_old_s, double phi_hat_new_s, std::uint32_t consecutive_rejects) const
{
  (void)phi_hat_old_s;
  (void)phi_hat_new_s;
  (void)consecutive_rejects;
  throw std::logic_error(
    "not implemented: SlackRecursionEstimator::check_guard — Phase 5 (coordination-builder) 대상");
}

} // namespace mrs
