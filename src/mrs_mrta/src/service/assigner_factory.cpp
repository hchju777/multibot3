// SPDX-License-Identifier: Apache-2.0
#include "mrs_mrta/service/assigner_factory.hpp"

namespace mrs_mrta::service
{

void AssignerFactory::register_policy(std::string key, Creator creator)
{
    creators_[std::move(key)] = std::move(creator);
}

std::unique_ptr<core::IAssigner> AssignerFactory::create(const std::string& key) const
{
    const auto it = creators_.find(key);
    if (it == creators_.end())
    {
        throw core::ContractViolation("AssignerFactory: unknown policy key '" + key + "'");
    }
    return it->second();
}

bool AssignerFactory::has(const std::string& key) const
{
    return creators_.find(key) != creators_.end();
}

}  // namespace mrs_mrta::service
