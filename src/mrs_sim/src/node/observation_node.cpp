// SPDX-License-Identifier: Apache-2.0
//
// observation_node — mrs_sim's world-observation publisher (375_observation_
// publisher.md, 48차 웨이브 4-A). This is the "발행 측" half of the missing
// "로봇이 감지" step:
//
//   [scenario file 시각별 차단 이벤트] -> observation_node -> /{robot}/scan
//   (sensor_msgs/msg/LaserScan, BEST_EFFORT/VOLATILE/1) -> mrs_trajopt 온보드
//
// 🔴 이음매는 357§Q1(`_workspace/357_arch_observation_seam.md`)이 이미 확정했다
// — 이 파일은 그 결정을 코드로 실현할 뿐 재설계하지 않는다:
//   - 매체: 로봇당 토픽 `/{robot}/scan` · 표준 `sensor_msgs/msg/LaserScan`.
//     새 IDL 0개 · `mrs_msgs/schema/` 무접촉(357§3-1의 다섯째 근거 — 계약에
//     넣으면 mapf/sadg가 그 타입을 "볼 수 있게" 된다).
//   - QoS: BEST_EFFORT/VOLATILE/KEEP_LAST 1(357§3-3 — 깊은 큐의 옛 스캔이
//     이미 열린 통로를 "막힘"으로 다시 보이게 하는 것을 막는다, 과소 보고=안전).
//   - 357§Q1-b: **귀속 판정을 싣지 않는다.** 이 노드는 "이 간선이 막혔다" 같은
//     판정 어휘를 만들지 않는다 — 만드는 것은 오직 **기하**(LaserScan의 거리값)
//     뿐이다. 표준 타입엔 판정을 실을 필드가 없다 — 타입이 그 성질을 강제한다.
//
// U48-1(`_workspace/356`): **로봇별 유한 범위 관측.** 각 로봇이 자기 센서 범위
// 안의 자유 간극만 본다 — 전역 진리를 주지 않는다. 이 노드는 로봇마다 독립된
// `/{robot}/scan`을 내고, 그 로봇의 알려진 pose(=/{robot}/odom) 근방의 간선만
// 본다(edge_lock_max_lateral_m 밖은 "이 로봇이 이 간선 위에 있다"고 주장하지
// 않는다).
//
// 🔴 이 라운드는 **시나리오 파일 시각별 차단 이벤트만** 연다(U46-3의 두 출처 중
// 하나) — 런타임 토글(대시보드 클릭)은 소비 측이 선 뒤인 다음 라운드.
//
// ============================================================================
// 관측 기하 — "그 기하를 어떻게 만드는지가 이 라운드의 본체다" (발주서 §하는 일 3)
// ============================================================================
//
// 로봇 pose(x, y, theta — /odom에서 theta는 순수 yaw 쿼터니언으로 복원,
// state_integrator.cpp가 orientation.z=sin(theta/2), .w=cos(theta/2)로 내는
// 것과 짝이 맞는다)와 /roadmap의 간선 기하만으로 매 관측 틱마다 다음을 한다:
//
//  1) **활성 간선 탐색**: 로봇 위치를 모든 간선의 선분에 투영해 매개변수
//     t∈[0,1](간선을 따른 위치)과 수직 거리(측방 오차)를 계산한다. 수직 거리가
//     `edge_lock_max_lateral_m` 이내인 간선 중 가장 가까운 것을 "이 로봇이 지금
//     있는 간선"으로 삼는다. 없으면 이 로봇은 "간선 밖"(교차로 등)이고 그 틱의
//     스캔은 전부 "관측 없음"(+Inf, REP 105/센서 관례 — 감지 거리 안에 아무것도
//     없다는 표준 표현)이다.
//
//  2) **레이마다 "간선 축 정렬 각도" 계산**: 각 레이의 절대 방향(robot theta +
//     상대각)과 간선의 단위 방향벡터 u 사이 끼인각 φ = acos(|ray·u|) ∈ [0, π/2]
//     를 구한다(부호를 버리는 이유 — 양방향 간선은 축이지 방향이 아니다).
//
//  3) **φ ≤ forward_cone_half_angle_rad — "간선을 따라 보는" 레이** (차단에
//     민감한 레이): 막힌 간선이면 range = range_min(≈0, "간극이 붕괴했다").
//     안 막혔으면 range = 로봇에서 그 레이가 가리키는 쪽 끝 정점까지 **남은
//     거리**(`length_m` × t 또는 (1-t), 레이가 향한 쪽) — 그 값이 센서 최대
//     거리 안이면 실수로, 밖이면 +Inf("이 거리 안에는 막힌 것이 없다"). 이것이
//     "막힌 간선에서는 통과 간극이 0이 되도록"(발주서)을 직접 만드는 자리다.
//
//  4) **φ > forward_cone_half_angle_rad — "간선을 가로질러 보는" 레이**(벽
//     레이, 차단과 무관): 거리 = (width_m/2) / max(sin φ, 최소치) — 코너에
//     가까울수록(φ→forward_cone) 벽이 멀어 보이고, 정확히 옆(φ=π/2)이면
//     width_m/2. 이 값은 **막힘 여부와 무관하게 항상 같다** — 물리적으로 벽은
//     차단 이벤트로 움직이지 않기 때문이며, 증거 절에서 "숫자가 라벨이 아니라
//     기하"임을 보여주는 대조군이다.
//
// 🔴 알려진 단순화(신고): 로봇 방향은 odom의 yaw로 정확히 복원하지만, 레이가
// "간선 축을 따라 보는가"의 판정은 로봇의 실제 진행 방향이 아니라 **간선 축
// 그 자체**(부호 없는 직선)에 대해서만 잰다 — 로봇이 간선을 향해 정확히 어느
// 쪽으로 걷는지는 스캔 자체가 담지 않는다(그것은 온보드가 자기 궤적에서 이미
// 안다). 이 단순화가 스캔의 기하적 사실성(거리·간극)을 바꾸지 않는다.
//
// 콜백 내 wall timer 0건(§9) — `create_timer`(노드 클록, use_sim_time 준수).
// 틱 안 파일·로그·문자열 조립 0건(CN-19) — 시나리오 이벤트 적용·스캔 계산은
// 전부 사전 파싱된 고정 크기 자료구조 위의 산술이다.

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <limits>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "blocked_edges_json.hpp"  // 같은 디렉터리(src/node/) — 공개 헤더가 아니다
#include "mrs_core/status.hpp"
#include "mrs_msgs/msg/roadmap.hpp"
#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

namespace mrs_sim::node
{

namespace
{

/// @brief 파일 전체를 문자열로 읽는다. 실패 시 ContractViolation(CN-15, 기동 경로).
std::string ReadFileOrThrow(const std::string& path, const char* purpose)
{
    std::ifstream in(path);
    if (!in.is_open())
    {
        throw mrs_core::ContractViolation("observation_node: cannot open " + std::string(purpose) +
                                          " file: " + path);
    }
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

/// @brief 필수 double 파라미터 하나를 선언하고, sentinel(미설정)이면 던진다.
/// 코드에 "가정값"을 박지 않기 위한 공통 자리 — `mrs_bringup/config/observation.yaml`
/// 이 없으면 이 함수가 기동을 거부한다(발주서 "코드에 박지 마라 … 종료 1").
double DeclareRequiredDouble(rclcpp::Node& node,
                             const std::string& key,
                             double sentinel,
                             const char* requirement)
{
    const auto v = node.declare_parameter<double>(key, sentinel);
    if (v == sentinel)
    {
        throw mrs_core::ContractViolation(
            "observation_node: '" + key + "' is required and has no code default (" +
            std::string(requirement) + ") — set it in mrs_bringup/config/observation.yaml");
    }
    return v;
}

}  // namespace

/// @brief 간선 하나의 기하 — `/roadmap`에서 그대로 옮긴다. 값을 지어내지 않는다.
struct EdgeGeom
{
    std::string from;
    std::string to;
    double x0 = 0.0, y0 = 0.0;
    double x1 = 0.0, y1 = 0.0;
    double length_m = 0.0;
    double width_m = 0.0;
};

/// @brief 로봇 한 대의 마지막 관측 pose. `/odom`이 아직 안 왔으면 `known=false`이고
/// 그 로봇의 스캔은 이번 틱에 내지 않는다(좌표를 지어내지 않는다).
struct RobotPose
{
    double x = 0.0;
    double y = 0.0;
    double theta = 0.0;
    bool known = false;
};

/// @brief `/{robot}/scan`(sensor_msgs/LaserScan)을 로봇마다 낸다. 입력은
/// `/roadmap`(latched, 간선 기하) + `/{robot}/odom`(BEST_EFFORT, 로봇 pose) +
/// 시나리오 파일(시각별 차단 이벤트, §머리 주석).
class ObservationNode : public rclcpp::Node
{
public:
    ObservationNode() : rclcpp::Node("observation_node")
    {
        const auto robots =
            this->declare_parameter<std::vector<std::string>>("robots", std::vector<std::string>{});
        const auto blocked_edges_path =
            this->declare_parameter<std::string>("blocked_edges_path", "");
        const auto scan_publish_period_ms =
            this->declare_parameter<int>("scan_publish_period_ms", -1);

        // 🔴 가정값(U48-1이 만든 [값 부재] 셋 — 센서 범위·분해능·갱신율)의 자리.
        // 값은 여기 없다 — mrs_bringup/config/observation.yaml에서만 온다.
        range_min_m_ = DeclareRequiredDouble(*this, "scan_range_min_m", -1.0, "must be > 0");
        range_max_m_ =
            DeclareRequiredDouble(*this, "scan_range_max_m", -1.0, "must be > range_min_m");
        angle_min_rad_ =
            DeclareRequiredDouble(*this, "scan_angle_min_rad", kAngleSentinel, "radians");
        angle_max_rad_ =
            DeclareRequiredDouble(*this, "scan_angle_max_rad", kAngleSentinel, "radians");
        angle_increment_rad_ =
            DeclareRequiredDouble(*this, "scan_angle_increment_rad", -1.0, "must be > 0");
        forward_cone_half_angle_rad_ =
            DeclareRequiredDouble(*this, "forward_cone_half_angle_rad", -1.0, "must be >= 0");
        edge_lock_max_lateral_m_ =
            DeclareRequiredDouble(*this, "edge_lock_max_lateral_m", -1.0, "must be > 0");

        if (range_max_m_ <= range_min_m_)
        {
            throw mrs_core::ContractViolation(
                "observation_node: 'scan_range_max_m' must be > 'scan_range_min_m'");
        }
        if (angle_max_rad_ <= angle_min_rad_)
        {
            throw mrs_core::ContractViolation(
                "observation_node: 'scan_angle_max_rad' must be > 'scan_angle_min_rad'");
        }
        if (scan_publish_period_ms <= 0)
        {
            throw mrs_core::ContractViolation(
                "observation_node: 'scan_publish_period_ms' is required and must be > 0 — set it "
                "in mrs_bringup/config/observation.yaml");
        }

        // BEST_EFFORT/VOLATILE/KEEP_LAST 1 — 357§3-3, D23. 깊은 큐의 옛 스캔이
        // 이미 열린 통로를 "막힘"으로 다시 보이게 하는 head-of-line 위험을 막는다.
        rclcpp::QoS scan_qos(rclcpp::KeepLast(1));
        scan_qos.best_effort();

        rclcpp::QoS odom_qos(rclcpp::KeepLast(1));
        odom_qos.best_effort();  // state_integrator의 발행측과 짝(qos.yaml best_effort_volatile).

        for (const auto& robot : robots)
        {
            poses_[robot] = RobotPose{};
            scan_pubs_[robot] =
                this->create_publisher<sensor_msgs::msg::LaserScan>("/" + robot + "/scan",
                                                                    scan_qos);
            odom_subs_.push_back(this->create_subscription<nav_msgs::msg::Odometry>(
                "/" + robot + "/odom",
                odom_qos,
                [this, robot](nav_msgs::msg::Odometry::ConstSharedPtr msg)
                {
                    this->OnOdom(robot, msg);
                }));
        }
        if (robots.empty())
        {
            RCLCPP_WARN(this->get_logger(),
                        "observation_node: 'robots' parameter is empty — no /{robot}/scan "
                        "topics created");
        }

        rclcpp::QoS roadmap_qos(rclcpp::KeepLast(1));
        roadmap_qos.reliable().transient_local();
        roadmap_sub_ = this->create_subscription<mrs_msgs::msg::Roadmap>(
            "/roadmap",
            roadmap_qos,
            [this](mrs_msgs::msg::Roadmap::ConstSharedPtr msg)
            {
                this->OnRoadmap(msg);
            });

        if (blocked_edges_path.empty())
        {
            RCLCPP_WARN(this->get_logger(),
                        "observation_node: 'blocked_edges_path' not set — no scenario "
                        "block/unblock events will fire this run (every edge stays open, "
                        "375_observation_publisher.md)");
        }
        else
        {
            const auto text = ReadFileOrThrow(blocked_edges_path, "blocked_edges");
            events_ = ParseBlockedEdgesScenario(text);
            RCLCPP_INFO(this->get_logger(),
                        "observation_node: loaded %zu blocked-edge event(s) from %s",
                        events_.size(),
                        blocked_edges_path.c_str());
        }

        period_s_ = static_cast<double>(scan_publish_period_ms) / 1000.0;
        timer_ = this->create_timer(std::chrono::milliseconds(scan_publish_period_ms),
                                    [this]()
                                    {
                                        this->OnTick();
                                    });
    }

private:
    static constexpr double kAngleSentinel = 999.0;  ///< 실제 각도로 나올 수 없는 값.

    /// @brief `/roadmap`이 도착(또는 갱신)하면 간선 기하 테이블을 다시 채운다.
    /// latched라 늦게 뜬 이 노드도 마지막 값을 받는다.
    void OnRoadmap(mrs_msgs::msg::Roadmap::ConstSharedPtr msg)
    {
        std::map<std::string, std::pair<double, double>> coords;
        for (const auto& n : msg->nodes)
        {
            coords[n.id] = std::make_pair(n.x, n.y);
        }
        edges_.clear();
        for (const auto& e : msg->edges)
        {
            const auto from_it = coords.find(e.from_id);
            const auto to_it = coords.find(e.to_id);
            if (from_it == coords.end() || to_it == coords.end())
            {
                RCLCPP_ERROR(this->get_logger(),
                             "observation_node: edge %s->%s references unknown node — skipped "
                             "(not fabricating a coordinate)",
                             e.from_id.c_str(),
                             e.to_id.c_str());
                continue;
            }
            EdgeGeom g;
            g.from = e.from_id;
            g.to = e.to_id;
            g.x0 = from_it->second.first;
            g.y0 = from_it->second.second;
            g.x1 = to_it->second.first;
            g.y1 = to_it->second.second;
            g.length_m = e.length_m;
            g.width_m = e.width_m;
            edges_.push_back(std::move(g));
        }
        RCLCPP_INFO(this->get_logger(),
                    "observation_node: /roadmap loaded (%zu edges)",
                    edges_.size());
    }

    /// @brief 한 로봇의 pose를 캐시만 한다(state_integrator.cpp의 OnCmdVel과 같은
    /// 패턴 — 발행은 OnTick 하나로 모아 레이스를 없앤다). yaw는 odom의 순수-yaw
    /// 쿼터니언에서 복원한다(state_integrator.cpp가 그 형태로 내므로 짝이 맞는다).
    void OnOdom(const std::string& robot, nav_msgs::msg::Odometry::ConstSharedPtr msg)
    {
        auto& pose = poses_.at(robot);
        pose.x = msg->pose.pose.position.x;
        pose.y = msg->pose.pose.position.y;
        const double qz = msg->pose.pose.orientation.z;
        const double qw = msg->pose.pose.orientation.w;
        pose.theta = 2.0 * std::atan2(qz, qw);
        pose.known = true;
    }

    /// @brief 매 관측 틱: (1) 경과 시각(관측 틱 카운트 기반, §머리 주석)이 지난
    /// 시나리오 이벤트를 순서대로 적용하고, (2) pose를 아는 로봇마다 스캔을
    /// 계산해 발행한다.
    void OnTick()
    {
        ++ticks_;
        const double elapsed_s = static_cast<double>(ticks_) * period_s_;
        while (next_event_idx_ < events_.size() && events_[next_event_idx_].time_s <= elapsed_s)
        {
            const auto& ev = events_[next_event_idx_];
            const auto key = NormalizeEdgeKey(ev.from, ev.to);
            if (ev.action == "block")
            {
                blocked_keys_.insert(key);
            }
            else
            {
                blocked_keys_.erase(key);
            }
            RCLCPP_INFO(this->get_logger(),
                        "observation_node: scenario event t=%.3fs %s %s-%s applied",
                        ev.time_s,
                        ev.action.c_str(),
                        ev.from.c_str(),
                        ev.to.c_str());
            ++next_event_idx_;
        }

        const rclcpp::Time now = this->get_clock()->now();
        for (auto& kv : poses_)
        {
            const auto& robot = kv.first;
            const auto& pose = kv.second;
            if (!pose.known)
            {
                continue;  // 좌표를 지어내지 않는다 — /odom을 아직 못 받은 로봇은 건너뛴다.
            }
            scan_pubs_.at(robot)->publish(BuildScan(robot, pose, now));
        }
    }

    /// @brief 로봇 위치를 모든 간선 선분에 투영해 "지금 있는 간선"을 찾는다.
    /// @return 찾으면 포인터(edges_ 내부 원소를 가리킨다) + 매개변수 t∈[0,1],
    ///   없으면 `{nullptr, 0.0}`.
    std::pair<const EdgeGeom*, double> FindActiveEdge(double x, double y) const
    {
        const EdgeGeom* best = nullptr;
        double best_t = 0.0;
        double best_perp = std::numeric_limits<double>::infinity();
        constexpr double kOverhangFrac = 0.02;  // 정점 근방 약간의 허용 오차(고정 상수).

        for (const auto& e : edges_)
        {
            const double vx = e.x1 - e.x0;
            const double vy = e.y1 - e.y0;
            const double len2 = vx * vx + vy * vy;
            if (len2 < 1e-9)
            {
                continue;  // 퇴화 간선 — 스킵.
            }
            const double wx = x - e.x0;
            const double wy = y - e.y0;
            const double t = (wx * vx + wy * vy) / len2;
            if (t < -kOverhangFrac || t > 1.0 + kOverhangFrac)
            {
                continue;
            }
            const double cross = wx * vy - wy * vx;
            const double perp = std::abs(cross) / std::sqrt(len2);
            if (perp <= edge_lock_max_lateral_m_ && perp < best_perp)
            {
                best = &e;
                best_t = std::clamp(t, 0.0, 1.0);
                best_perp = perp;
            }
        }
        return {best, best_t};
    }

    /// @brief 로봇 한 대의 `/{robot}/scan`을 계산한다 — §머리 주석 "관측 기하"의
    /// 구현. 값은 전부 이 함수 안에서 유도된다(코드에 시나리오 값을 박지 않는다).
    sensor_msgs::msg::LaserScan BuildScan(const std::string& robot,
                                          const RobotPose& pose,
                                          const rclcpp::Time& stamp) const
    {
        sensor_msgs::msg::LaserScan scan;
        scan.header.stamp = stamp;
        scan.header.frame_id = robot + "/base_link";  // odom의 child_frame_id와 짝.
        scan.angle_min = static_cast<float>(angle_min_rad_);
        scan.angle_max = static_cast<float>(angle_max_rad_);
        scan.angle_increment = static_cast<float>(angle_increment_rad_);
        scan.time_increment = 0.0f;  // 전 레이 동시 취득으로 가정(단순화, 신고).
        scan.scan_time = static_cast<float>(period_s_);
        scan.range_min = static_cast<float>(range_min_m_);
        scan.range_max = static_cast<float>(range_max_m_);

        const auto [active, t] = FindActiveEdge(pose.x, pose.y);
        const std::size_t n =
            static_cast<std::size_t>(
                std::floor((angle_max_rad_ - angle_min_rad_) / angle_increment_rad_ + 1e-6)) +
            1;
        scan.ranges.assign(n, std::numeric_limits<float>::infinity());

        if (active == nullptr)
        {
            return scan;  // "간선 밖" — 관측 없음(+Inf 전부, 좌표를 지어내지 않는다).
        }

        const double vx = active->x1 - active->x0;
        const double vy = active->y1 - active->y0;
        const double vlen = std::sqrt(vx * vx + vy * vy);
        const double ux = (vlen > 1e-9) ? vx / vlen : 1.0;
        const double uy = (vlen > 1e-9) ? vy / vlen : 0.0;
        const bool blocked = blocked_keys_.count(NormalizeEdgeKey(active->from, active->to)) > 0;

        for (std::size_t i = 0; i < n; ++i)
        {
            const double rel_angle = angle_min_rad_ + static_cast<double>(i) * angle_increment_rad_;
            const double abs_angle = pose.theta + rel_angle;
            const double rx = std::cos(abs_angle);
            const double ry = std::sin(abs_angle);
            const double dot_ru = rx * ux + ry * uy;  // 레이 · 간선 축(부호 있음).
            const double phi = std::acos(std::clamp(std::abs(dot_ru), -1.0, 1.0));

            double range;
            if (phi <= forward_cone_half_angle_rad_)
            {
                // 간선을 "따라" 보는 레이 — 차단에 민감(§머리 주석 3).
                if (blocked)
                {
                    range = range_min_m_;  // 간극 붕괴 — 발주서 "간극이 0이 되도록".
                }
                else
                {
                    const double remaining =
                        std::max(0.0, (dot_ru > 0.0 ? (1.0 - t) : t) * active->length_m);
                    range = (remaining <= range_max_m_) ? std::max(remaining, range_min_m_)
                                                        : std::numeric_limits<double>::infinity();
                }
            }
            else
            {
                // 간선을 "가로질러" 보는 레이 — 벽, 차단과 무관(§머리 주석 4).
                const double denom = std::max(std::sin(phi), 1e-3);
                const double wall_dist = (active->width_m / 2.0) / denom;
                range = (wall_dist <= range_max_m_) ? std::max(wall_dist, range_min_m_)
                                                    : std::numeric_limits<double>::infinity();
            }
            scan.ranges[i] = static_cast<float>(range);
        }
        (void)robot;
        return scan;
    }

    std::vector<EdgeGeom> edges_;
    std::map<std::string, RobotPose> poses_;
    std::set<std::string> blocked_keys_;
    std::vector<BlockedEdgeEvent> events_;
    std::size_t next_event_idx_ = 0;
    std::uint64_t ticks_ = 0;
    double period_s_ = 0.0;

    double range_min_m_ = 0.0;
    double range_max_m_ = 0.0;
    double angle_min_rad_ = 0.0;
    double angle_max_rad_ = 0.0;
    double angle_increment_rad_ = 0.0;
    double forward_cone_half_angle_rad_ = 0.0;
    double edge_lock_max_lateral_m_ = 0.0;

    std::map<std::string, rclcpp::Publisher<sensor_msgs::msg::LaserScan>::SharedPtr> scan_pubs_;
    std::vector<rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr> odom_subs_;
    rclcpp::Subscription<mrs_msgs::msg::Roadmap>::SharedPtr roadmap_sub_;
    rclcpp::TimerBase::SharedPtr timer_;
};

}  // namespace mrs_sim::node

int main(int argc, char** argv)
{
    rclcpp::init(argc, argv);
    try
    {
        auto node = std::make_shared<mrs_sim::node::ObservationNode>();
        rclcpp::spin(node);
    }
    catch (const mrs_core::ContractViolation& cv)
    {
        RCLCPP_FATAL(rclcpp::get_logger("observation_node"),
                     "startup contract violation: %s",
                     cv.what());
        rclcpp::shutdown();
        return 1;
    }
    rclcpp::shutdown();
    return 0;
}
