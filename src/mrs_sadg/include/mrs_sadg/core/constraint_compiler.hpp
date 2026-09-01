// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_SADG_CORE_CONSTRAINT_COMPILER_HPP
#define MRS_SADG_CORE_CONSTRAINT_COMPILER_HPP

#include <string>
#include <vector>

#include "mrs_sadg/core/execution_graph.hpp"
#include "mrs_sadg/core/i_alternative_generator.hpp"
#include "mrs_sadg/core/status.hpp"
#include "mrs_sadg/core/types.hpp"

/// @file constraint_compiler.hpp
/// @brief IN## — the intake compiler (337 ConstraintCompiler). Runs off the tick,
/// only when a plan changes (RG-2). Builds the ExecutionGraph from a PlanView:
/// H-cons intake (IN01-03), segments (IN04-06), the fixed sequential chain E0
/// (IN07), the committed ordering deps derived from visit_order by the contract
/// mapping 퇴거->진입 (RG-3, IN08-IN13), and the production-side seam self-check
/// (IN14). Opposing pairs (［D5］) are excluded from switch alternatives (［D8］).

namespace mrs_sadg::core
{

/// @brief Why a compile failed / what it produced (mirrors §255 decision-3 style).
enum class CompileOutcome
{
    kOk,               ///< graph built; H-cons held; seam self-check consistent.
    kHconsViolation,   ///< IN01-03: same-direction shared edge has inconsistent endpoint order.
    kMalformedPlan,    ///< a step list / visit_order entry could not be mapped.
    kSeamInconsistent  ///< IN14: produced ordering edges are not the contract mapping.
};

/// @brief Detail for a CompileOutcome.
struct CompileReport
{
    CompileOutcome outcome = CompileOutcome::kOk;
    std::vector<std::string> messages;      ///< human-readable failure detail.
    std::uint32_t n_opposing_excluded = 0;  ///< ［D8］ opposing pairs kept out of alternatives.
    std::uint32_t n_ordering_edges = 0;     ///< committed ordering deps installed.
};

/// @brief Compiles a discrete plan into a switchable action-dependency graph.
class ConstraintCompiler
{
public:
    /// @brief Construct with the alternative generator (F37-1 opposing decision).
    /// @param gen the alternative generator (extension point 2).
    explicit ConstraintCompiler(const IAlternativeGenerator& gen) : gen_(gen)
    {
    }

    /// @brief Compile a plan into @p out (IN00-IN14). Off-tick; exceptions allowed.
    /// @param plan the intake plan view (mrs.discrete_plan 4.0.0, internally).
    /// @param out [out] the execution graph to fill (reset first).
    /// @param report [out] outcome and detail.
    /// @return Status::kOk on success, kError if H-cons/malformed (report has why).
    Status compile(const PlanView& plan, ExecutionGraph& out, CompileReport& report) const;

    /// @brief H-cons intake predicate (IN01): same-direction shared edges have a
    /// consistent endpoint visit order. The same rule `check_sadg_acyclic.py`'s
    /// seam check (d) applies.
    /// @param plan the plan view.
    /// @param messages [out] appended with each violating edge.
    /// @return true iff H-cons holds.
    static bool hcons_ok(const PlanView& plan, std::vector<std::string>& messages);

private:
    const IAlternativeGenerator& gen_;
};

}  // namespace mrs_sadg::core

#endif  // MRS_SADG_CORE_CONSTRAINT_COMPILER_HPP
