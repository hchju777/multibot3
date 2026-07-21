#include <stdexcept>

#include "mrs_metrics/metrics_collector.hpp"

namespace mrs
{

void MetricsCollector::on_rung_event(const RungEventRecord & record)
{
  (void)record;
  throw std::logic_error(
    "not implemented: MetricsCollector::on_rung_event — Phase 5 (coordination-builder) 대상");
}

void MetricsCollector::on_qp_result(RobotId robot_id, double solve_time_s, bool used_fallback)
{
  (void)robot_id;
  (void)solve_time_s;
  (void)used_fallback;
  throw std::logic_error(
    "not implemented: MetricsCollector::on_qp_result — Phase 5 (coordination-builder) 대상");
}

void MetricsCollector::on_metric_sample(
  const std::string & key, double value, RobotId robot_id, bool pending_isaac)
{
  (void)key;
  (void)value;
  (void)robot_id;
  (void)pending_isaac;
  throw std::logic_error(
    "not implemented: MetricsCollector::on_metric_sample — Phase 5 (coordination-builder) 대상");
}

} // namespace mrs
