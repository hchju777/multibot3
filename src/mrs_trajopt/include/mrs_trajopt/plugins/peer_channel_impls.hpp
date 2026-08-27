// SPDX-License-Identifier: Apache-2.0
#ifndef MRS_TRAJOPT_PLUGINS_PEER_CHANNEL_IMPLS_HPP
#define MRS_TRAJOPT_PLUGINS_PEER_CHANNEL_IMPLS_HPP

#include <string>
#include <unordered_map>

#include "mrs_trajopt/core/i_peer_channel.hpp"

/// @file peer_channel_impls.hpp
/// @brief Extension point (iv) implementations (communication-model axis).
///
/// PeerChannelIdeal (every publication delivered fresh, age 0) and
/// PeerChannelAdhoc (a fixed staleness => static-disk downgrade). Both are
/// in-process (no serialization). E1 uses v_max only (RQ-5a).

namespace mrs_trajopt::plugins
{

/// @brief Ideal channel: publications always delivered fresh (age 0).
class PeerChannelIdeal : public core::IPeerChannel
{
public:
    /// @brief Deliver the last published view of @p robot with age 0.
    /// @param robot the observed neighbor.
    /// @param rd the current round index.
    /// @param out filled with the fresh view.
    /// @return true iff @p robot has published at least once.
    bool deliver(const std::string& robot, std::int64_t rd, core::PeerView& out) override;

    /// @brief Store this robot's published anchor/iota (always fresh).
    /// @param robot this robot.
    /// @param rd the current round index.
    /// @param anchor the published anchor.
    /// @param iota this robot's plan-step index.
    void publish(const std::string& robot,
                 std::int64_t rd,
                 const core::Pose2& anchor,
                 std::int64_t iota) override;

private:
    struct Entry
    {
        core::Pose2 anchor;
        std::int64_t iota = 0;
        std::int64_t last_rd = 0;
    };

    std::unordered_map<std::string, Entry> board_;
};

/// @brief Ad-hoc channel: publications age by a fixed staleness, may downgrade.
class PeerChannelAdhoc : public core::IPeerChannel
{
public:
    /// @brief Construct with an age threshold that triggers static-disk downgrade.
    /// @param age_threshold_rounds delivered age (in rounds) at/above which the
    ///        neighbor is seen as a growing disk E1 (static_disk = true).
    explicit PeerChannelAdhoc(std::int64_t age_threshold_rounds)
        : age_threshold_(age_threshold_rounds)
    {
    }

    /// @brief Deliver @p robot's view with a nonzero round age; downgrade if stale.
    /// @param robot the observed neighbor.
    /// @param rd the current round index.
    /// @param out filled with the (possibly stale) view.
    /// @return true iff @p robot has published at least once.
    bool deliver(const std::string& robot, std::int64_t rd, core::PeerView& out) override;

    /// @brief Store this robot's published anchor/iota with its round stamp.
    /// @param robot this robot.
    /// @param rd the current round index.
    /// @param anchor the published anchor.
    /// @param iota this robot's plan-step index.
    void publish(const std::string& robot,
                 std::int64_t rd,
                 const core::Pose2& anchor,
                 std::int64_t iota) override;

private:
    struct Entry
    {
        core::Pose2 anchor;
        std::int64_t iota = 0;
        std::int64_t published_rd = 0;
    };

    std::int64_t age_threshold_;
    std::unordered_map<std::string, Entry> board_;
};

}  // namespace mrs_trajopt::plugins

#endif  // MRS_TRAJOPT_PLUGINS_PEER_CHANNEL_IMPLS_HPP
