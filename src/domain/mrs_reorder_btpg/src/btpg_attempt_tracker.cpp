#include <stdexcept>

#include "mrs_reorder_btpg/btpg_attempt_tracker.hpp"

namespace mrs
{

void BtpgAttemptTracker::record_attempt(EventId event_id, double improvement_s)
{
  (void)event_id;
  (void)improvement_s;
  throw std::logic_error(
    "not implemented: BtpgAttemptTracker::record_attempt — Phase 5 (coordination-builder) 대상");
}

GatingDecision BtpgAttemptTracker::evaluate_gating(EventId event_id)
{
  (void)event_id;
  throw std::logic_error(
    "not implemented: BtpgAttemptTracker::evaluate_gating — Phase 5 (coordination-builder) 대상");
}

} // namespace mrs
