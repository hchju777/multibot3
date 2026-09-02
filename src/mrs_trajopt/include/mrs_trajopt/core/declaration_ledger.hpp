// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_DECLARATION_LEDGER_HPP
#define MRS_TRAJOPT_CORE_DECLARATION_LEDGER_HPP

#include <cstdint>

/// @file declaration_ledger.hpp
/// @brief Stateful Q1 hysteresis
/// (`_workspace/357_arch_observation_seam.md` §5 Q3: kept OUT of the
/// stateless `DeclarationRegulator` on purpose — mixing state into that
/// class would contaminate its 3-way-order unit tests).
///
/// Realizes `OBS-7`/`OBS7-NZ` (`_workspace/364_formulation_anchors_p2.md`
/// §7-2): open = `n^open` consecutive raw-true ticks; a confirmed-open
/// declaration cannot close before `n^hold` ticks have elapsed since it
/// opened (`(O2)`) — `(O2)` is the ONLY thing the non-Zeno bound `(C1)` rests
/// on. 🔴 `364_p2`'s correction (over `355`§7-2's original reading): the
/// open/close threshold ASYMMETRY (`n^open != n^close`) contributes ZERO to
/// the (C1) bound by itself — it only helps suppress noise bursts, a
/// SEPARATE, conditional guarantee `(C2)`. Close = `n^close` consecutive
/// raw-false ticks, but ONLY once the hold has elapsed.

namespace mrs_trajopt::core
{

/// @brief Hysteresis thresholds (`method.modules.trajopt.*`, `[값 부재]`).
struct DeclarationLedgerConfig
{
    std::int64_t n_open = 0;   ///< `OBS7-NZ` (O1): consecutive raw-true ticks to open.
    std::int64_t n_close = 0;  ///< `OBS7-NZ` (O1): consecutive raw-false ticks to close.
    std::int64_t n_hold = 0;   ///< `OBS7-NZ` (O2): minimum hold before close is allowed.
};

/// @brief Per-robot Q1 open/close state machine over a raw per-tick predicate.
class DeclarationLedger
{
public:
    /// @brief Construct with the hysteresis thresholds.
    /// @param cfg the thresholds.
    explicit DeclarationLedger(DeclarationLedgerConfig cfg) : cfg_(cfg)
    {
    }

    /// @brief Feed one tick's raw (pre-hysteresis) predicate.
    /// @param predicate_true_this_tick `IsQ1RawPredicate()`'s result this tick.
    /// @return whether Q1 (`edge_impassable_for_any_robot`) is CONFIRMED open
    ///         as of this tick.
    bool update(bool predicate_true_this_tick)
    {
        if (predicate_true_this_tick)
        {
            open_run_ += 1;
            close_run_ = 0;
        }
        else
        {
            close_run_ += 1;
            open_run_ = 0;
        }

        if (!confirmed_open_)
        {
            if (predicate_true_this_tick && open_run_ >= cfg_.n_open)
            {
                confirmed_open_ = true;
                ticks_since_open_ = 0;
            }
        }
        else
        {
            ticks_since_open_ += 1;
            if (!predicate_true_this_tick && ticks_since_open_ >= cfg_.n_hold &&
                close_run_ >= cfg_.n_close)
            {
                confirmed_open_ = false;
                open_run_ = 0;
            }
        }
        return confirmed_open_;
    }

    /// @brief The current confirmed-open state (without feeding a new tick).
    /// @return true iff Q1 is currently confirmed open.
    bool confirmed_open() const
    {
        return confirmed_open_;
    }

private:
    DeclarationLedgerConfig cfg_;
    std::int64_t open_run_ = 0;
    std::int64_t close_run_ = 0;
    std::int64_t ticks_since_open_ = 0;
    bool confirmed_open_ = false;
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_DECLARATION_LEDGER_HPP
