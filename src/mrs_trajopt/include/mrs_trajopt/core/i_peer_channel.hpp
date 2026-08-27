// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_CORE_I_PEER_CHANNEL_HPP
#define MRS_TRAJOPT_CORE_I_PEER_CHANNEL_HPP

#include <string>

#include "mrs_trajopt/core/types.hpp"

/// @file i_peer_channel.hpp
/// @brief Extension point (iv): the lateral peer channel (CN-10, 321b §2).
///
/// 🔴 In-process seam, NOT a boundary. What a publication carries: anchor,
/// tube geometry, ordinal age (delta_k, delta_rd), iota (plan-step index).
/// A missed/stale neighbor becomes a growing disk E1 using v_max only (RQ-5a).
/// Implementations: PeerChannelIdeal / PeerChannelAdhoc.

namespace mrs_trajopt::core
{

/// @brief A peer view delivered to a robot for one round (ST06).
struct PeerView
{
    Pose2 anchor;              ///< the neighbor's published anchor (position, tau_rel=0).
    PeerAge age;               ///< ordinal age (delta_k, delta_rd) — never seconds.
    std::int64_t iota = 0;     ///< the neighbor's plan-step index (ordinal).
    bool static_disk = false;  ///< true iff stale/holed => treat as growing disk E1.
    double v_max = 0.0;        ///< the neighbor's v_max [m/s] (E1 uses this only).
};

/// @brief The lateral peer channel.
class IPeerChannel
{
public:
    virtual ~IPeerChannel() = default;

    /// @brief Deliver the current peer view of @p robot at round @p rd (ST06).
    /// @param robot the observed neighbor.
    /// @param rd the current round index.
    /// @param out filled with the neighbor's anchor, age, iota, static-disk flag.
    /// @return true iff a view was produced (false => no such neighbor).
    virtual bool deliver(const std::string& robot, std::int64_t rd, PeerView& out) = 0;

    /// @brief Publish this robot's intent for round @p rd (ST11, intent slot).
    /// @param robot this robot.
    /// @param rd the current round index.
    /// @param anchor the published anchor.
    /// @param iota this robot's plan-step index.
    virtual void publish(const std::string& robot,
                         std::int64_t rd,
                         const Pose2& anchor,
                         std::int64_t iota) = 0;
};

}  // namespace mrs_trajopt::core

#endif  // MRS_TRAJOPT_CORE_I_PEER_CHANNEL_HPP
