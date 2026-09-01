// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_MRTA_SERVICE_ASSIGNER_FACTORY_HPP
#define MRS_MRTA_SERVICE_ASSIGNER_FACTORY_HPP

#include <functional>
#include <map>
#include <memory>
#include <string>

#include "mrs_mrta/core/i_assigner.hpp"

/// @file assigner_factory.hpp
/// @layer service [Clean Architecture: Application].
/// @brief `{역할}Factory` (CN-11) — the `method.modules.mrta` name-to-policy map.
///
/// 🔴 **Known limitation** (disclosed, not hidden): `12a_arch_mrta.md` §5-2
/// specifies `pluginlib::ClassLoader<IAssigner>` for this role. There is no
/// ROS 2 runtime in this checkout (brief §1), so `pluginlib` is unavailable —
/// this class is a plain in-process registry (name string -> factory
/// function) that stands in for it. The *keys* already follow the CN-12
/// format (`{package}/{ClassName}`, e.g. `mrs_mrta/GreedyTailAppend`) so the
/// registry can be swapped for a real `ClassLoader` textually at integration
/// time without renaming anything a caller depends on. **Load-failure-has-no-
/// fallback** (`12a`§5-2 "로드 실패 시 폴백 없음") is preserved: `create()`
/// throws `ContractViolation` on an unknown key — a startup-path throw only
/// (CN-15), never called from `drain_once`.
namespace mrs_mrta::service
{

class AssignerFactory
{
public:
    using Creator = std::function<std::unique_ptr<core::IAssigner>()>;

    /// @brief Register a policy under a CN-12-format key.
    /// @param key `{package}/{ClassName}` (e.g. "mrs_mrta/GreedyTailAppend").
    /// @param creator Factory function producing a fresh instance.
    void register_policy(std::string key, Creator creator);

    /// @brief Instantiate the policy registered under @p key.
    /// @param key `{package}/{ClassName}`.
    /// @return a freshly-constructed policy instance.
    /// @throws core::ContractViolation if @p key is not registered — startup
    ///         path only (CN-15), no silent fallback (`12a`§5-2 C-14).
    std::unique_ptr<core::IAssigner> create(const std::string& key) const;

    /// @brief Is @p key registered?
    /// @param key `{package}/{ClassName}`.
    /// @return true iff a creator is registered under @p key.
    bool has(const std::string& key) const;

private:
    std::map<std::string, Creator> creators_;
};

}  // namespace mrs_mrta::service

#endif  // MRS_MRTA_SERVICE_ASSIGNER_FACTORY_HPP
