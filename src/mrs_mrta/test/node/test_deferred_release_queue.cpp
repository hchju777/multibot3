// SPDX-License-Identifier: Apache-2.0
// 371 — "배정 비결정성" fix: `DeferredQueue<T>`'s one guarantee (replay in
// exactly push order) is what removes the `/roadmap`-vs-first-`/task_release`
// race documented in `mrta_node.cpp`'s `OnTaskRelease`/`OnRoadmap` file docs.
// This header has zero ROS/rclcpp/rosidl dependency (deferred_release_queue.hpp
// file doc), so it is testable here with plain ints/strings — the ROS-specific
// wiring (`IsBuilt()` gating, adapter/ingest replay) is verified separately by
// the runtime 5-run reproduction in `371_mrta_determinism.md` §1, since this
// package's test binaries do not link rclcpp (CMakeLists.txt `add_mrta_test`).
#include <memory>
#include <string>
#include <vector>

#include "mrs_mrta/node/deferred_release_queue.hpp"

// Minimal assert macro — avoids pulling test_util.hpp (which requires
// mrs_mrta_core/ports, unnecessary for a header with zero mrs_mrta
// dependencies of its own) and matches the project-wide no-gtest style
// (`test/test_util.hpp` MRTA_CHECK, 20d precedent).
#include <cstdio>
#include <cstdlib>
#define DRQ_CHECK(cond)                                                                        \
    do                                                                                         \
    {                                                                                          \
        if (!(cond))                                                                           \
        {                                                                                      \
            std::fprintf(stderr, "DRQ_CHECK failed: %s (%s:%d)\n", #cond, __FILE__, __LINE__); \
            std::exit(1);                                                                      \
        }                                                                                      \
    } while (0)

using mrs_mrta::node::DeferredQueue;

namespace
{

void test_empty_queue_drains_to_nothing()
{
    DeferredQueue<int> q;
    DRQ_CHECK(q.empty());
    DRQ_CHECK(q.size() == 0);
    const auto out = q.drain();
    DRQ_CHECK(out.empty());
    DRQ_CHECK(q.empty());  // draining an empty queue leaves it empty, not an error.
}

void test_push_order_is_preserved_regardless_of_arrival_order()
{
    // The whole point of this class: whatever order items were *pushed* in
    // (which, for the 371 fix, is release_index order — `task_release_
    // publisher` never reorders its source array) is exactly the order
    // `drain()` returns them in, independent of anything else.
    DeferredQueue<std::string> q;
    DRQ_CHECK(q.empty());
    q.push("release_index=0");
    DRQ_CHECK(!q.empty());
    DRQ_CHECK(q.size() == 1);
    q.push("release_index=1");
    q.push("release_index=2");
    DRQ_CHECK(q.size() == 3);

    const auto out = q.drain();
    DRQ_CHECK(out.size() == 3);
    DRQ_CHECK(out[0] == "release_index=0");
    DRQ_CHECK(out[1] == "release_index=1");
    DRQ_CHECK(out[2] == "release_index=2");
    DRQ_CHECK(q.empty());  // drain() clears the queue.
}

void test_drain_then_push_again_starts_a_fresh_ordered_batch()
{
    // Mirrors `OnRoadmap`'s one-shot replay: after the first `/roadmap`
    // arrives and drains whatever raced ahead of it, the queue must behave
    // like new for any later relaunch/reset scenario (defensive — this
    // round's scenario only exercises one roadmap arrival, but the class
    // itself makes no one-shot assumption).
    DeferredQueue<int> q;
    q.push(1);
    q.push(2);
    const auto first_batch = q.drain();
    DRQ_CHECK(first_batch.size() == 2);
    DRQ_CHECK(q.empty());

    q.push(3);
    const auto second_batch = q.drain();
    DRQ_CHECK(second_batch.size() == 1);
    DRQ_CHECK(second_batch[0] == 3);
}

void test_move_only_like_usage_via_shared_ptr()
{
    // `mrta_node.cpp` stages `mrs_msgs::msg::TaskRelease::ConstSharedPtr` —
    // exercise the same shared-ownership pattern (no ROS type needed, a
    // plain `std::shared_ptr<int>` has the identical copy/aliasing shape).
    DeferredQueue<std::shared_ptr<int>> q;
    auto a = std::make_shared<int>(0);
    auto b = std::make_shared<int>(1);
    q.push(a);
    q.push(b);
    const auto out = q.drain();
    DRQ_CHECK(out.size() == 2);
    DRQ_CHECK(*out[0] == 0);
    DRQ_CHECK(*out[1] == 1);
    // Originals still valid — push() copies the shared_ptr, not the pointee.
    DRQ_CHECK(*a == 0);
    DRQ_CHECK(*b == 1);
}

}  // namespace

int main()
{
    test_empty_queue_drains_to_nothing();
    test_push_order_is_preserved_regardless_of_arrival_order();
    test_drain_then_push_again_starts_a_fresh_ordered_batch();
    test_move_only_like_usage_via_shared_ptr();
    return 0;
}
