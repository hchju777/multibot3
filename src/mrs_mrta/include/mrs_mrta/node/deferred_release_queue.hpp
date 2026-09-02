// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_NODE_DEFERRED_RELEASE_QUEUE_HPP
#define MRS_MRTA_NODE_DEFERRED_RELEASE_QUEUE_HPP

#include <utility>
#include <vector>

/// @file deferred_release_queue.hpp
/// @layer node [Clean Architecture: Presentation].
/// @brief `DeferredQueue<T>` — the "371" determinism fix's order-preserving
/// hold-and-replay buffer, factored out of `mrta_node.cpp` as a standalone,
/// zero-ROS-dependency template so its one guarantee (replay in exactly push
/// order) is unit testable without `rclcpp` (`371_mrta_determinism.md` §2).
///
/// Root cause this exists for: `/roadmap` (latched) and `/task_release`
/// (volatile stream) are two independent topics with no cross-topic
/// delivery-order guarantee. `mrta_node` needs the roadmap-derived distance
/// table built before it can tell "unreachable" apart from "don't know yet"
/// for a `/task_release` that arrives first. Feeding such a release straight
/// to the assigner collapses that distinction and the assigner pools the
/// task — permanently, since nothing else in the pipeline re-triggers a
/// pool retry except a later goal completion. Two runs with byte-identical
/// `/roadmap` and `/task_release` boundary inputs then produced two
/// different `/assignment` outputs, keyed only on wall-clock arrival order —
/// a `CN-18` violation (`multibot3/CLAUDE.md`: "재현성은 시계·난수·경계
/// 입력 셋에만 걸려 있다").
///
/// This class only remembers *that* something arrived too early and *in
/// what order*; it has no opinion on what "too early" means or what to do
/// once the wait is over — both are `mrta_node.cpp`'s job (call `push` while
/// `distance_oracle_.IsBuilt()` is false, call `drain` once `OnRoadmap`
/// builds the table, and feed the result back through the exact same
/// per-release adapter/ingest path a same-timed release would have taken).
namespace mrs_mrta::node
{

/// @brief Order-preserving hold-and-replay buffer.
/// @tparam T Type of the staged item (this class never inspects it).
template<typename T>
class DeferredQueue
{
public:
    /// @brief Stage one item at the back of the queue.
    /// @param item The item to hold.
    void push(T item)
    {
        items_.push_back(std::move(item));
    }

    /// @brief Is the queue empty?
    /// @return true iff nothing is currently staged.
    bool empty() const noexcept
    {
        return items_.empty();
    }

    /// @brief How many items are currently staged.
    /// @return the count.
    std::size_t size() const noexcept
    {
        return items_.size();
    }

    /// @brief Remove and return every staged item, in the exact order they
    /// were pushed, leaving the queue empty.
    /// @return the drained items, oldest first.
    std::vector<T> drain()
    {
        std::vector<T> out;
        out.swap(items_);
        return out;
    }

private:
    std::vector<T> items_;
};

}  // namespace mrs_mrta::node

#endif  // MRS_MRTA_NODE_DEFERRED_RELEASE_QUEUE_HPP
