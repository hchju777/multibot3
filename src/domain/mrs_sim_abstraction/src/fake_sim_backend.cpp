/**
 * @file fake_sim_backend.cpp
 * @brief 결정론적 인프로세스 시뮬 백엔드 구현.
 */

#include "mrs_sim_abstraction/fake_sim_backend.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <unordered_set>

namespace mrs
{
namespace
{

/** @brief 원주율 — 자세 정규화 구간 `[-pi, pi]` 의 경계값. */
constexpr double PI_VALUE = 3.14159265358979323846;
/** @brief 두 배 원주율. */
constexpr double TWO_PI_VALUE = 2.0 * PI_VALUE;

/**
 * @brief 각도를 계약 구간 `[-pi, pi]` 로 정규화한다.
 *
 * 관측이 나가는 지점에서 구간을 지키지 않으면 `mrs_msg_convert` 가 범위 위반으로 폐기한다 —
 * 적분이 누적된 자세는 반드시 여기를 거친다.
 *
 * @param[in] angle_rad 정규화할 각도 [rad]. 자료형 `double`.
 * @return `double` — `[-pi, pi]` 구간의 등가 각도.
 */
[[nodiscard]] double normalize_angle(double angle_rad) noexcept
{
  double wrapped = std::fmod(angle_rad + PI_VALUE, TWO_PI_VALUE);
  if (wrapped < 0.0)
  {
    wrapped += TWO_PI_VALUE;
  }
  return wrapped - PI_VALUE;
}

/**
 * @brief 두 자세 사이의 평면 거리를 구한다.
 * @param[in] a 첫 자세. 자료형 `Pose2D`.
 * @param[in] b 둘째 자세. 자료형 `Pose2D`.
 * @return `double` — 유클리드 거리 [m].
 */
[[nodiscard]] double planar_distance(const Pose2D & a, const Pose2D & b) noexcept
{
  const double dx = a.x_m - b.x_m;
  const double dy = a.y_m - b.y_m;
  return std::sqrt(dx * dx + dy * dy);
}

} // namespace

bool FakeSimBackend::configure(const FakeSimConfig & config) noexcept
{
  if (!(config.step_dt_s > 0.0) || !std::isfinite(config.step_dt_s))
  {
    return false;
  }

  if (config.robot_radius_m < 0.0 || config.node_attach_radius_m < 0.0 ||
      config.actuate_to_state_latency_s < 0.0)
  {
    return false;
  }

  // 관측이 실어 보낼 스코프는 계약이 UNIFORM 으로 고정한다.
  if (config.view_scope.view_kind != ViewKind::UNIFORM)
  {
    return false;
  }

  std::unordered_set<RobotId> seen_robots;
  for (const FakeRobotSpawn & spawn : config.robots)
  {
    if (spawn.robot_id == ROBOT_ID_NONE || !seen_robots.insert(spawn.robot_id).second)
    {
      return false;
    }
  }

  std::unordered_set<std::uint32_t> seen_nodes;
  for (const FakeNodeAnchor & anchor : config.node_anchors)
  {
    if (anchor.node_id.is_none() || !seen_nodes.insert(anchor.node_id.value()).second)
    {
      return false;
    }
  }

  config_ = config;
  configured_ = true;
  ready_ = false;
  return true;
}

SimCapabilities FakeSimBackend::capabilities() const noexcept
{
  SimCapabilities caps;
  // 백엔드 이름은 계약이 정한 3종 중 하나여야 한다. 이 상수의 정본은 SimCapabilities.msg 다.
  caps.backend_name = "fake";
  caps.deterministic_with_seed = true;
  caps.seed = seed_;
  caps.physics_fidelity = PhysicsFidelity::NONE;
  // 부분관측·접촉은 흉내내지 않는다. 흉내내면 isaac 이 필요하다는 사실이 가려진다(§5.3).
  caps.supports_partial_observation = false;
  caps.supports_contact = false;
  caps.supports_fault_injection = true;
  caps.nominal_realtime_factor = 1.0;
  caps.actuate_to_state_latency_s = config_.actuate_to_state_latency_s;
  return caps;
}

bool FakeSimBackend::reset(std::uint64_t seed) noexcept
{
  if (!configured_)
  {
    return false;
  }

  seed_ = seed;
  rng_.seed(seed);
  sim_time_s_ = 0.0;
  next_injection_id_ = 1;
  next_command_sequence_ = 0;
  collision_count_ = 0;
  min_separation_m_ = 0.0;
  separation_seen_ = false;

  robots_.clear();
  robot_order_.clear();
  pending_commands_.clear();

  for (const FakeRobotSpawn & spawn : config_.robots)
  {
    RobotState state;
    state.pose = spawn.initial_pose;
    state.pose.theta_rad = normalize_angle(state.pose.theta_rad);
    robots_.emplace(spawn.robot_id, state);
    robot_order_.push_back(spawn.robot_id);
  }

  // 관측 순서를 로봇 id 오름차순으로 고정한다 — 컨테이너 순회 순서가 결정론을 좌우하면 안 된다.
  std::sort(robot_order_.begin(), robot_order_.end());

  ready_ = true;
  return true;
}

void FakeSimBackend::advance_one_step() noexcept
{
  // 이 스텝이 덮는 구간은 [step_start_s, sim_time_s_] 다. 정지 만료는 **구간 시작 시각**으로
  // 판정한다 — 끝 시각으로 판정하면 정지 구간에 완전히 포함된 스텝에서도 로봇이 움직여
  // 한 스텝 일찍 재개된다(프로브가 잡은 off-by-one).
  const double step_start_s = sim_time_s_;
  sim_time_s_ += config_.step_dt_s;

  // (1) 지연이 만료된 지령을 반영한다. 접수 순번 순으로 처리해 같은 시각의 지령도 결정론적이다.
  while (!pending_commands_.empty() && pending_commands_.front().apply_at_s <= sim_time_s_)
  {
    const PendingCommand & pending = pending_commands_.front();
    const auto it = robots_.find(pending.command.robot_id);
    if (it != robots_.end())
    {
      it->second.v_mps = pending.command.v_mps;
      it->second.omega_rps = pending.command.omega_rps;
    }
    pending_commands_.pop_front();
  }

  // (2) 유니사이클 전방 오일러 적분. 몸체점 q 기준이다.
  for (const RobotId robot_id : robot_order_)
  {
    RobotState & state = robots_.at(robot_id);

    // 영구 고장은 지령과 무관하게 정지, 일시 정지는 만료 시각까지 정지한다.
    if (state.faulted)
    {
      state.v_mps = 0.0;
      state.omega_rps = 0.0;
      continue;
    }

    if (state.stalled)
    {
      // stall_until_s == 0 은 "해제 전까지 무기한"이다(계약 FaultInjection.duration_s 규약).
      if (state.stall_until_s > 0.0 && step_start_s >= state.stall_until_s)
      {
        state.stalled = false;
      }
      else
      {
        continue;
      }
    }

    const double dt = config_.step_dt_s;
    state.pose.x_m += state.v_mps * std::cos(state.pose.theta_rad) * dt;
    state.pose.y_m += state.v_mps * std::sin(state.pose.theta_rad) * dt;
    state.pose.theta_rad = normalize_angle(state.pose.theta_rad + state.omega_rps * dt);
  }

  // (3) 기하 충돌·최소 이격 갱신. 접촉력은 모형화하지 않으며 겹침을 되밀지도 않는다 —
  //     겹친 채로 두고 세는 것이 "기하 전용"의 정직한 거동이다.
  const double contact_distance = 2.0 * config_.robot_radius_m;
  for (std::size_t i = 0; i < robot_order_.size(); ++i)
  {
    for (std::size_t j = i + 1; j < robot_order_.size(); ++j)
    {
      const double separation =
        planar_distance(robots_.at(robot_order_[i]).pose, robots_.at(robot_order_[j]).pose);

      if (!separation_seen_ || separation < min_separation_m_)
      {
        min_separation_m_ = separation;
        separation_seen_ = true;
      }

      if (separation < contact_distance)
      {
        ++collision_count_;
      }
    }
  }
}

bool FakeSimBackend::step(std::uint32_t steps, double & sim_time_s) noexcept
{
  if (!ready_)
  {
    return false;
  }

  const std::uint32_t effective_steps = (steps == 0U) ? 1U : steps;
  for (std::uint32_t i = 0; i < effective_steps; ++i)
  {
    advance_one_step();
  }

  sim_time_s = sim_time_s_;
  return true;
}

bool FakeSimBackend::attach_node(const Pose2D & pose, UniformNodeId & out) const noexcept
{
  if (config_.node_anchors.empty())
  {
    return false;
  }

  bool found = false;
  double best_distance = std::numeric_limits<double>::max();
  UniformNodeId best_node{};

  for (const FakeNodeAnchor & anchor : config_.node_anchors)
  {
    const double dx = pose.x_m - anchor.x_m;
    const double dy = pose.y_m - anchor.y_m;
    const double distance = std::sqrt(dx * dx + dy * dy);

    // 동률이면 노드 id 가 작은 쪽을 택해 결정론을 유지한다.
    const bool better =
      distance < best_distance ||
      (distance == best_distance && found && anchor.node_id.value() < best_node.value());
    if (better)
    {
      best_distance = distance;
      best_node = anchor.node_id;
      found = true;
    }
  }

  if (!found || best_distance > config_.node_attach_radius_m)
  {
    return false;
  }

  out = best_node;
  return true;
}

bool FakeSimBackend::sense(std::vector<RobotObservation> & observations) noexcept
{
  if (!ready_)
  {
    return false;
  }

  // 지역 버퍼에 쌓고 성공 시에만 넘긴다 — 실패 시 out 오염을 막는 규약과 같은 취지다.
  std::vector<RobotObservation> collected;
  collected.reserve(robot_order_.size());

  for (const RobotId robot_id : robot_order_)
  {
    const RobotState & state = robots_.at(robot_id);

    RobotObservation observation;
    observation.robot_id = robot_id;
    observation.body_pose = state.pose;
    observation.v_mps = state.v_mps;
    observation.omega_rps = state.omega_rps;
    observation.view_scope = config_.view_scope;

    // 부착에 실패하면 추측하지 않고 센티넬을 보고한다(계약 규약).
    UniformNodeId attached{};
    if (attach_node(state.pose, attached))
    {
      observation.occupied_node = attached;
    }

    // next_node 는 진행 방향 예측이 필요한데 이 백엔드는 경로를 모른다 — 채우지 않는다.
    // 추측해 채우면 L3 진행 판정이 실제와 어긋난 채로 조용히 돈다.

    observation.edge_progress = 0.0;

    // 기하 전용이므로 관측 불확실성은 이상화(0)다. 필드 자체는 남겨 두어야 isaac 부착 시
    // 코어가 구조를 바꾸지 않는다(architecture §5.2 누수 금지).
    observation.uncertainty.pos_std_m = 0.0;
    observation.uncertainty.theta_std_rad = 0.0;
    observation.uncertainty.partial_observation = false;
    observation.uncertainty.observation_time_s = sim_time_s_;

    collected.push_back(observation);
  }

  observations = std::move(collected);
  return true;
}

bool FakeSimBackend::actuate(const std::vector<ActuationCommand> & commands) noexcept
{
  if (!ready_)
  {
    return false;
  }

  // 부분 적용을 막기 위해 먼저 전수 검증한다 — 절반만 반영된 지령은 재현이 불가능해진다.
  for (const ActuationCommand & command : commands)
  {
    if (robots_.find(command.robot_id) == robots_.end())
    {
      return false;
    }

    if (!std::isfinite(command.v_mps) || !std::isfinite(command.omega_rps))
    {
      return false;
    }
  }

  const double apply_at = sim_time_s_ + config_.actuate_to_state_latency_s;
  for (const ActuationCommand & command : commands)
  {
    PendingCommand pending;
    pending.apply_at_s = apply_at;
    pending.sequence = next_command_sequence_++;
    pending.command = command;
    pending_commands_.push_back(pending);
  }

  return true;
}

bool FakeSimBackend::inject(const FaultInjection & fault, std::uint64_t & injection_id) noexcept
{
  if (!ready_)
  {
    return false;
  }

  // 기하 전용 백엔드가 정직하게 흉내낼 수 있는 결함만 받는다.
  // 통신 결함은 comms_emul 소관이고, 통로 개폐는 지도를 아는 계층의 일이다.
  // 여기서 받아들이고 아무것도 하지 않으면 "주입했는데 아무 일도 없었다"가 되어
  // 실험이 결함 없는 조건을 결함 조건으로 착각한다.
  if (fault.kind != FaultKind::ROBOT_STALL && fault.kind != FaultKind::ROBOT_FAULT)
  {
    return false;
  }

  const auto it = robots_.find(fault.target_robot_id);
  if (it == robots_.end())
  {
    return false;
  }

  if (fault.kind == FaultKind::ROBOT_STALL)
  {
    it->second.stalled = true;
    it->second.stall_until_s =
      (fault.duration_s > 0.0) ? (sim_time_s_ + fault.duration_s) : 0.0;
  }
  else
  {
    it->second.faulted = true;
    it->second.v_mps = 0.0;
    it->second.omega_rps = 0.0;
  }

  injection_id = next_injection_id_++;
  return true;
}

bool FakeSimBackend::poll_metrics(std::vector<SimMetricSample> & samples) noexcept
{
  if (!ready_)
  {
    return false;
  }

  std::vector<SimMetricSample> collected;

  SimMetricSample collisions;
  collisions.key = "collision_count";
  collisions.value = static_cast<double>(collision_count_);
  collisions.robot_id = ROBOT_ID_NONE;
  // 물리 충실도가 NONE 이므로 이 값을 연구 검증으로 승격해서는 안 된다(§5.3).
  collisions.pending_isaac = true;
  collected.push_back(collisions);

  if (separation_seen_)
  {
    SimMetricSample separation;
    separation.key = "min_separation_m";
    separation.value = min_separation_m_;
    separation.robot_id = ROBOT_ID_NONE;
    separation.pending_isaac = true;
    collected.push_back(separation);
  }

  // 회수 후 구간 지표는 리셋한다(누적 지표인 collision_count 는 유지).
  separation_seen_ = false;
  min_separation_m_ = 0.0;

  samples = std::move(collected);
  return true;
}

} // namespace mrs
