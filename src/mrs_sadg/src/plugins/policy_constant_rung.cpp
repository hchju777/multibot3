// SPDX-License-Identifier: Apache-2.0
#include "mrs_sadg/plugins/policy_constant_rung.hpp"

namespace mrs_sadg::plugins
{

core::Rung PolicyConstantRung::decide(const core::DeficitView& deficit,
                                      const core::BlockedView& blocked,
                                      core::Rung cur)
{
    (void)deficit;
    (void)blocked;
    (void)cur;
    return core::Rung::kNone;
}

std::string_view PolicyConstantRung::key() const
{
    return "mrs_sadg/PolicyConstantRung";
}

}  // namespace mrs_sadg::plugins
