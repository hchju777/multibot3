// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_PLUGINS_ASSIGNER_IMPLS_HPP
#define MRS_MRTA_PLUGINS_ASSIGNER_IMPLS_HPP

#include <cstdint>
#include <string_view>
#include <vector>

#include "mrs_mrta/core/i_assigner.hpp"

/// @file assigner_impls.hpp
/// @layer plugins [Clean Architecture: Infrastructure(swap)].
/// @brief `IAssigner` implementations (CN-5) — links `core` only.
///
/// 🔴 **Scope disclosure**: `12a_arch_mrta.md` §5-2 names six implementations
/// (the normal policy + five fault injectors: `FaultyDelete`, `FaultyReorder`,
/// `FaultyReassign`, `FaultyDeleteInEpisode`, `FaultyOverflow`). This round
/// implements **two** — `GreedyTailAppend` (the normal policy, fully per
/// canon) and `FaultyOverflow` (one fault variant, chosen because it is the
/// only one of the five whose injection is representable through the
/// existing two operations — see class doc below). The other four require
/// either operations `AssignmentState` deliberately does not have (`erase`,
/// `move_to_robot`, `replace` — `FaultyDelete`/`FaultyReassign`/`FaultyReorder`
/// would need to fabricate a stand-in that isn't the canon's actual forbidden
/// operation) or a config key that is `[값 부재]`
/// (`inject_at_revision`, `FaultyDeleteInEpisode`, `12a`§5-1). Building them
/// convincingly needs the NT-1..NT-6 negative-test harness and the checker
/// approvals (`336_mrta_canon_p3.md` §336-7 D1), which are out of this
/// round's scope — see the implementation notes' "알려진 한계".
namespace mrs_mrta::plugins
{

/// @brief The normal policy — online sequential greedy nearest-tail-append
/// with the 5-tier lexicographic tie-break (`336_mrta_canon.md` §336-2).
///
/// The 5 tiers, in order: (1) cost in millimetres, ascending (2) residual
/// queue length, ascending (3) cumulative goals ever assigned to that robot,
/// ascending (4) robot identifier, lexicographic ascending (5) release
/// sequence number. 🔴 Per canon, tier (4) already resolves every tie (robot
/// names are unique in the roster) — tier (5) is implemented for fidelity to
/// the "5단" specification but is unreachable in practice, exactly as
/// `336_mrta_canon.md` §336-2-2 notes ("④가 이미 동률을 안 남기므로
/// `tiebreak_seed`는 죽은 키").
class GreedyTailAppend : public core::IAssigner
{
public:
    core::Status configure(const core::AssignerContext& ctx) override;
    core::AssignerStatus on_release(const core::ReleaseEvent& ev,
                                    core::AssignmentState& state) override;
    core::AssignerStatus on_completion(const core::CompletionEvent& ev,
                                       core::AssignmentState& state) override;
    std::string_view policy_id() const override;
    void reset(core::AssignmentState& state) override;

private:
    core::AssignerContext ctx_;
    std::vector<std::uint64_t> cumulative_assigned_;  ///< Indexed by RobotIndex. Tier (3).
    std::uint64_t release_seq_counter_ = 0;           ///< Tier (5), monotonic per on_release call.
};

/// @brief Fault variant: ignores the policy capacity Q^ta on `on_release`,
/// permitting a robot's residual queue to grow up to the *physical* limit
/// (Q^ta + 1) — one further than the policy allows
/// (`12a_arch_mrta.md` §2-3's "+1 슬롯" exists precisely so this probe can be
/// represented and then caught downstream). `on_completion` is unaffected and
/// is **delegated** (has-a, not inheritance — `12a`§5-2 M6: "결함 5종은 정상
/// 구현을 상속하지 않고 위임한다") to an internal `GreedyTailAppend` so a
/// change to the normal completion rule cannot silently diverge between the
/// two.
class FaultyOverflow : public core::IAssigner
{
public:
    core::Status configure(const core::AssignerContext& ctx) override;
    core::AssignerStatus on_release(const core::ReleaseEvent& ev,
                                    core::AssignmentState& state) override;
    core::AssignerStatus on_completion(const core::CompletionEvent& ev,
                                       core::AssignmentState& state) override;
    std::string_view policy_id() const override;
    void reset(core::AssignmentState& state) override;

private:
    GreedyTailAppend delegate_;  ///< Owns cumulative-assigned/tie-break state; delegated to.
    core::AssignerContext ctx_;
};

}  // namespace mrs_mrta::plugins

#endif  // MRS_MRTA_PLUGINS_ASSIGNER_IMPLS_HPP
