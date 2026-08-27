// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_SEARCH_ANCHOR_SET_HPP
#define MRS_TRAJOPT_CORE_SEARCH_ANCHOR_SET_HPP

#include <cstddef>
#include <vector>

/// @file anchor_set.hpp
/// @brief v=0 markings — only where the robot actually stops (321b §3-3).
///
/// Members: start, wait-outside-permission, stop candidate, reverse<->forward
/// transition, goal. 🔴 A PassWindow (subgoal) is **not** an anchor — "stoppable
/// != stopping" (315 §315-6). The set stores indices into the geometric chain.

namespace mrs_trajopt::core::search
{

/// @brief Set of chain indices that are v=0 anchor nodes.
class AnchorSet
{
public:
    /// @brief Mark chain index @p i as an anchor.
    /// @param i index into the geometric chain.
    void mark(std::size_t i)
    {
        indices_.push_back(i);
    }

    /// @brief Is chain index @p i an anchor?
    /// @param i index into the geometric chain.
    /// @return true iff @p i was marked.
    bool is_anchor(std::size_t i) const
    {
        for (std::size_t idx : indices_)
        {
            if (idx == i)
            {
                return true;
            }
        }
        return false;
    }

    /// @brief The marked anchor indices.
    /// @return const reference to the index list.
    const std::vector<std::size_t>& indices() const
    {
        return indices_;
    }

private:
    std::vector<std::size_t> indices_;
};

}  // namespace mrs_trajopt::core::search

#endif  // MRS_TRAJOPT_CORE_SEARCH_ANCHOR_SET_HPP
