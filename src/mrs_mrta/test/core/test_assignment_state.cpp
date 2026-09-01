// SPDX-License-Identifier: Apache-2.0
// CN-22: test/core/ — no node, no ROS. Locks the module's central claim
// ("없는 연산 여섯", `336_mrta_canon.md` §336-3) at compile time, then tests
// push_tail/pop_head/pending-pool behaviour.
#include <string>
#include <type_traits>
#include <vector>

#include "mrs_mrta/core/status.hpp"
#include "mrs_mrta/core/types.hpp"
#include "test_util.hpp"

using mrs_mrta::core::AssignmentState;
using mrs_mrta::core::ContractViolation;
using mrs_mrta::core::GoalRecord;
using mrs_mrta::core::PendingTask;
using mrs_mrta::core::Status;

// ---------------------------------------------------------------------------
// Compile-time probe: does AssignmentState have a member (of ANY signature)
// named erase / insert_at / swap / move_to_robot / sort / replace? Forming
// `&T::name` fails to compile (name lookup failure) iff no such member
// exists at all — it does not need to know the hypothetical signature.
// -----------------------------------------------------------------------------
#define MRTA_DEFINE_HAS_MEMBER(NAME)                                             \
    template<typename T, typename = void>                                        \
    struct HasMember_##NAME : std::false_type                                    \
    {                                                                            \
    };                                                                           \
    template<typename T>                                                         \
    struct HasMember_##NAME<T, std::void_t<decltype(&T::NAME)>> : std::true_type \
    {                                                                            \
    };

MRTA_DEFINE_HAS_MEMBER(erase)
MRTA_DEFINE_HAS_MEMBER(insert_at)
MRTA_DEFINE_HAS_MEMBER(swap)
MRTA_DEFINE_HAS_MEMBER(move_to_robot)
MRTA_DEFINE_HAS_MEMBER(sort)
MRTA_DEFINE_HAS_MEMBER(replace)

static_assert(!HasMember_erase<AssignmentState>::value, "AssignmentState must not have erase()");
static_assert(!HasMember_insert_at<AssignmentState>::value,
              "AssignmentState must not have insert_at()");
static_assert(!HasMember_swap<AssignmentState>::value, "AssignmentState must not have swap()");
static_assert(!HasMember_move_to_robot<AssignmentState>::value,
              "AssignmentState must not have move_to_robot()");
static_assert(!HasMember_sort<AssignmentState>::value, "AssignmentState must not have sort()");
static_assert(!HasMember_replace<AssignmentState>::value,
              "AssignmentState must not have replace()");

namespace
{

GoalRecord make_goal(const std::string& id, const std::string& task, const std::string& loc)
{
    GoalRecord g;
    g.id = id;
    g.task = task;
    g.location = loc;
    return g;
}

void test_roster_validation()
{
    bool threw = false;
    try
    {
        AssignmentState bad({}, 4);
    }
    catch (const ContractViolation&)
    {
        threw = true;
    }
    MRTA_CHECK(threw);

    threw = false;
    try
    {
        AssignmentState bad({"r2", "r1"}, 4);  // not sorted
    }
    catch (const ContractViolation&)
    {
        threw = true;
    }
    MRTA_CHECK(threw);

    threw = false;
    try
    {
        AssignmentState bad({"r1", "r1"}, 4);  // duplicate
    }
    catch (const ContractViolation&)
    {
        threw = true;
    }
    MRTA_CHECK(threw);
}

void test_push_pop_and_physical_capacity()
{
    AssignmentState st({"r1", "r2"}, 2);  // Q^ta=2 -> physical capacity 3.
    MRTA_CHECK(st.robot_count() == 2);
    MRTA_CHECK(st.goal_queue_capacity() == 2);
    MRTA_CHECK(st.physical_capacity() == 3);

    const auto r1 = st.index_of("r1");
    MRTA_CHECK(r1 != mrs_mrta::core::kNoRobot);
    MRTA_CHECK(st.index_of("nope") == mrs_mrta::core::kNoRobot);

    MRTA_CHECK(st.peek_head(r1) == nullptr);
    MRTA_CHECK(st.peek_tail(r1) == nullptr);
    MRTA_CHECK(st.residual_len(r1) == 0);

    MRTA_CHECK(st.push_tail(r1, make_goal("g1", "t1", "L1")) == Status::kOk);
    MRTA_CHECK(st.push_tail(r1, make_goal("g2", "t1", "L2")) == Status::kOk);
    MRTA_CHECK(st.push_tail(r1, make_goal("g3", "t2", "L3")) ==
               Status::kOk);  // fills physical cap.
    MRTA_CHECK(st.residual_len(r1) == 3);
    // Physical limit refusal — not a policy decision (see class doc).
    MRTA_CHECK(st.push_tail(r1, make_goal("g4", "t3", "L4")) == Status::kError);

    MRTA_CHECK(st.peek_head(r1)->id == "g1");
    MRTA_CHECK(st.peek_tail(r1)->id == "g3");

    const auto view = st.residual_view(r1);
    MRTA_CHECK(view.size() == 3);
    MRTA_CHECK(view[0].id == "g1" && view[1].id == "g2" && view[2].id == "g3");

    auto popped = st.pop_head(r1);
    MRTA_CHECK(popped.has_value() && popped->id == "g1");
    MRTA_CHECK(st.residual_len(r1) == 2);
    MRTA_CHECK(st.peek_head(r1)->id == "g2");

    // r2 is untouched — proves mutation is per-robot, not global.
    MRTA_CHECK(st.residual_len(st.index_of("r2")) == 0);

    // Ring wraps around correctly: push after a pop, then drain fully.
    MRTA_CHECK(st.push_tail(r1, make_goal("g5", "t3", "L5")) == Status::kOk);
    MRTA_CHECK(st.residual_len(r1) == 3);
    std::vector<std::string> drained_ids;
    while (auto g = st.pop_head(r1))
    {
        drained_ids.push_back(g->id);
    }
    MRTA_CHECK((drained_ids == std::vector<std::string>{"g2", "g3", "g5"}));
    MRTA_CHECK(!st.pop_head(r1).has_value());  // Empty pop returns nullopt, not an error.
}

void test_location_in_use()
{
    AssignmentState st({"r1", "r2"}, 4);
    const auto r1 = st.index_of("r1");
    MRTA_CHECK(!st.location_in_use("L1"));
    st.push_tail(r1, make_goal("g1", "t1", "L1"));
    MRTA_CHECK(st.location_in_use("L1"));
    MRTA_CHECK(!st.location_in_use("L2"));
    st.pop_head(r1);
    MRTA_CHECK(!st.location_in_use("L1"));  // Completed/removed -> no longer "in use".
}

void test_pending_pool_fifo()
{
    AssignmentState st({"r1"}, 4);
    MRTA_CHECK(st.pending_empty());
    MRTA_CHECK(st.peek_pending_front() == nullptr);

    PendingTask t1{"taskA", {make_goal("g1", "taskA", "L1")}};
    PendingTask t2{"taskB", {make_goal("g2", "taskB", "L2")}};
    st.push_pending(t1);
    st.push_pending(t2);
    MRTA_CHECK(st.pending_size() == 2);
    MRTA_CHECK(st.peek_pending_front()->task == "taskA");
    MRTA_CHECK((st.pending_task_ids() == std::vector<std::string>{"taskA", "taskB"}));

    auto popped = st.pop_pending_front();
    MRTA_CHECK(popped.task == "taskA");
    MRTA_CHECK(st.pending_size() == 1);
    MRTA_CHECK(st.peek_pending_front()->task == "taskB");
}

void test_robot_vertex_read_model()
{
    AssignmentState st({"r1"}, 4);
    MRTA_CHECK(st.robot_vertex(st.index_of("r1")).empty());
    st.set_robot_vertex(st.index_of("r1"), "V7");
    MRTA_CHECK(st.robot_vertex(st.index_of("r1")) == "V7");
}

}  // namespace

int main()
{
    test_roster_validation();
    test_push_pop_and_physical_capacity();
    test_location_in_use();
    test_pending_pool_fifo();
    test_robot_vertex_read_model();
    return 0;
}
