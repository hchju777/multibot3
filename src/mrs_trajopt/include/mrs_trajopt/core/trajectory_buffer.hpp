// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_TRAJECTORY_BUFFER_HPP
#define MRS_TRAJOPT_CORE_TRAJECTORY_BUFFER_HPP

#include <cstddef>
#include <vector>

#include "mrs_trajopt/core/types.hpp"

/// @file trajectory_buffer.hpp
/// @brief The committed state chain ③ and the INV-2 fallback source.
///
/// `hold_previous()` keeps the last committed remainder (INV-2 a1): a partial
/// solve is never adopted (BT-FAIL-SOUND). `adopt()` replaces it with a new
/// chain (only inside the tube or via the commit gate). `truncate()` cuts the
/// horizon at the last stoppable index (CT19).

namespace mrs_trajopt::core
{

/// @brief Holds the committed trajectory and supports hold/adopt/truncate.
class TrajectoryBuffer
{
public:
    /// @brief Replace the committed chain with @p chain (ST29 / TT18).
    /// @param chain the new state chain (copied).
    void adopt(const std::vector<StateSample>& chain)
    {
        committed_ = chain;
    }

    /// @brief Keep the previous committed chain unchanged (INV-2 a1).
    ///
    /// A no-op by design — the previous remainder stays. Named so the intent is
    /// explicit at call sites (TT10, ST22, ST33).
    void hold_previous()
    {
        // Intentionally does nothing: the committed remainder is the fallback.
    }

    /// @brief Cut the horizon at index @p k inclusive (CT19).
    /// @param k the last stoppable index; samples after k are dropped.
    void truncate(std::size_t k)
    {
        if (k + 1 < committed_.size())
        {
            committed_.resize(k + 1);
        }
    }

    /// @brief Read-only view of the committed chain.
    /// @return const reference to the committed state chain.
    const std::vector<StateSample>& view() const
    {
        return committed_;
    }

    /// @brief Is the committed chain empty?
    /// @return true iff no chain has been adopted.
    bool empty() const
    {
        return committed_.empty();
    }

private:
    std::vector<StateSample> committed_;
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_TRAJECTORY_BUFFER_HPP
