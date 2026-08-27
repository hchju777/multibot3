// SPDX-License-Identifier: Apache-2.0
#include "mrs_trajopt/plugins/peer_channel_impls.hpp"

namespace mrs_trajopt::plugins
{

bool PeerChannelIdeal::deliver(const std::string& robot, std::int64_t rd, core::PeerView& out)
{
    (void)rd;
    auto it = board_.find(robot);
    if (it == board_.end())
    {
        return false;
    }
    out = core::PeerView{};
    out.anchor = it->second.anchor;
    out.iota = it->second.iota;
    out.age = core::PeerAge{0, 0};  // ideal: always fresh.
    out.static_disk = false;
    return true;
}

void PeerChannelIdeal::publish(const std::string& robot,
                               std::int64_t rd,
                               const core::Pose2& anchor,
                               std::int64_t iota)
{
    Entry e;
    e.anchor = anchor;
    e.iota = iota;
    e.last_rd = rd;
    board_[robot] = e;
}

bool PeerChannelAdhoc::deliver(const std::string& robot, std::int64_t rd, core::PeerView& out)
{
    auto it = board_.find(robot);
    if (it == board_.end())
    {
        return false;
    }
    out = core::PeerView{};
    out.anchor = it->second.anchor;
    out.iota = it->second.iota;
    const std::int64_t age_rd = rd - it->second.published_rd;
    out.age = core::PeerAge{0, age_rd < 0 ? 0 : age_rd};
    // Stale => growing disk E1 (uses v_max only, RQ-5a).
    out.static_disk = age_rd >= age_threshold_;
    return true;
}

void PeerChannelAdhoc::publish(const std::string& robot,
                               std::int64_t rd,
                               const core::Pose2& anchor,
                               std::int64_t iota)
{
    Entry e;
    e.anchor = anchor;
    e.iota = iota;
    e.published_rd = rd;
    board_[robot] = e;
}

}  // namespace mrs_trajopt::plugins
