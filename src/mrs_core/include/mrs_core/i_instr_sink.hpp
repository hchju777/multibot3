// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_CORE_I_INSTR_SINK_HPP
#define MRS_CORE_I_INSTR_SINK_HPP

#include <cstdint>
#include <string>

#include "mrs_core/status.hpp"

/// @file i_instr_sink.hpp
/// @brief Common seam (CN-4/CN-18): the one instrumentation sink modules write to.
///
/// The concrete ring-buffer/histogram/drop-counter aggregation (CN-19) is
/// explicitly deferred (`347_arch_integration_delta.md` §3-1 items 6~7 — each
/// module already has its own, and there is one consumer, so unifying now is
/// premature). This header only fixes the seam so a future aggregator can be
/// injected without touching call sites (D14). `mrs_core_pure` (CN-2/CN-3):
/// no ROS, no JSON.
namespace mrs_core
{

/// @brief Instrumentation sink seam (CN-18/CN-19). `core/`/`plugins/` observe
/// through this instead of writing files/logs/strings inside the tick.
class IInstrSink
{
public:
    virtual ~IInstrSink() = default;

    /// @brief Record one scalar observation under @p key.
    /// @param key Metric name (stable across a run — not built per-tick).
    /// @param value Observed value.
    /// @return Status::kOk on success, Status::kError if the sink is full or
    ///         the key is unknown (CN-16: never throws in the tick path).
    virtual Status Observe(const std::string& key, double value) = 0;

    /// @brief Increment a drop/fault counter by @p key (CN-19 drop counter).
    /// @param key Counter name.
    /// @param delta Increment amount (default 1).
    /// @return Status::kOk on success, Status::kError otherwise.
    virtual Status Increment(const std::string& key, std::uint64_t delta = 1) = 0;
};

}  // namespace mrs_core

#endif  // MRS_CORE_I_INSTR_SINK_HPP
