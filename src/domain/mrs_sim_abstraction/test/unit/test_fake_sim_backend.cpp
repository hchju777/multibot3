// test_fake_sim_backend.cpp — FakeSimBackend 의 결정론·구성 거부·지연 반영·정직성 신고를
// 지키는 벽.
//
// ══════════════════════════════════════════════════════════════════════════════
//  ⚠ 여기 통과가 **물리 검증이 아니다.**
// ══════════════════════════════════════════════════════════════════════════════
// 이 백엔드의 물리 충실도는 `PhysicsFidelity::NONE`(기하만) 이다. 접촉·동역학·물리 비결정성이
// 없고, 충돌은 반지름 합의 기하 중첩으로만 판정한다. 따라서 이 파일의 어떤 assertion 도
// 다음을 말하지 않는다:
//   - **QP feasibility rate (D-07)** — 판정 불가. **isaac 필수.**
//   - **livelock / 데드락 수렴 (D-05·D-08)** — 판정 불가. **isaac 필수.**
//   - **실제 diff-drive 물리와 물리 재현성 (D-09)** — 판정 불가. **isaac 필수.**
//   - 충돌 "없음"의 안전 주장 — 여기 충돌 카운터가 0 이어도 실물리에서 접촉하지 않는다는 근거가
//     되지 못한다. 이 백엔드가 내는 **모든 metrics 표본이 `pending_isaac = true`** 인 이유가
//     그것이다(architecture §5.3).
// 이 파일이 지키는 것은 오직 **백엔드가 계약대로 거동하는가**다 — 그 위에서 도는 상위 로직
// 테스트가 백엔드 자체의 버그를 알고리즘 실패로 오독하지 않게 하는 기반 방어선이다.
//
// ROS 무의존(C1): 이 테스트는 rclcpp 에 링크하지 않으며 sleep 도 쓰지 않는다. 시간은 오직
// `step()` 호출로만 전진한다.

#include <cmath>
#include <cstdint>
#include <vector>

#include <gtest/gtest.h>

#include "mrs_sim_abstraction/fake_sim_backend.hpp"

namespace
{

/**
 * @brief 로봇 2대·노드 2개를 가진 표준 구성을 만든다.
 * @param[in] latency_s `actuate` → 상태 반영 지연 [s]. 자료형 `double`. 0 이면 즉시 반영.
 * @return `mrs::FakeSimConfig` — 유효한 구성. 로봇 1 은 원점(노드 10 위), 로봇 0 은 (5, 0).
 */
mrs::FakeSimConfig make_config(double latency_s = 0.0)
{
  mrs::FakeSimConfig config;
  config.step_dt_s = 0.05;
  config.robot_radius_m = 0.3;
  config.node_attach_radius_m = 0.25;
  config.actuate_to_state_latency_s = latency_s;
  config.view_scope.view_kind = mrs::ViewKind::UNIFORM;
  config.view_scope.roadmap_version = 7;
  config.view_scope.view_id = 1;
  // 일부러 id 내림차순으로 넣는다 — sense() 가 오름차순으로 되돌리는지 보기 위함.
  config.robots = {{mrs::RobotId{1}, mrs::Pose2D{0.0, 0.0, 0.0}},
                   {mrs::RobotId{0}, mrs::Pose2D{5.0, 0.0, 0.0}}};
  config.node_anchors = {{mrs::UniformNodeId{10}, 0.0, 0.0}, {mrs::UniformNodeId{11}, 1.0, 0.0}};
  return config;
}

/**
 * @brief 시드 하나로 고정 지령열 40스텝을 돌리고 최종 상태를 하나의 수로 접는다.
 * @param[in] seed 난수 시드. 자료형 `std::uint64_t`.
 * @return `double` — 로봇 1 의 최종 x 와 자세를 섞은 지문값. 같은 입력이면 같아야 한다.
 */
double run_fixed_sequence(std::uint64_t seed)
{
  mrs::FakeSimBackend backend;
  EXPECT_TRUE(backend.configure(make_config()));
  EXPECT_TRUE(backend.reset(seed));

  const std::vector<mrs::ActuationCommand> commands{{mrs::RobotId{1}, 1.0, 0.3},
                                                    {mrs::RobotId{0}, -0.5, 0.0}};
  double sim_time_s = 0.0;
  for (int i = 0; i < 40; ++i)
  {
    EXPECT_TRUE(backend.actuate(commands));
    EXPECT_TRUE(backend.step(1, sim_time_s));
  }

  std::vector<mrs::RobotObservation> observations;
  EXPECT_TRUE(backend.sense(observations));
  EXPECT_EQ(observations.size(), 2U);
  return observations[1].body_pose.x_m * 1e6 + observations[1].body_pose.theta_rad;
}

} // namespace

// ── 결정론 ──────────────────────────────────────────────────────────────────

// 같은 시드·같은 지령열이면 부동소수 수준까지 같은 궤적. baseline-repro 의 전제다.
TEST(FakeSimBackend, SameSeedReproducesIdenticalTrajectory)
{
  EXPECT_DOUBLE_EQ(run_fixed_sequence(42), run_fixed_sequence(42));
}

// 이 백엔드는 난수를 실제로 쓰지 않으므로 시드가 달라도 결과가 같아야 한다.
// (숨은 난수원이 생기면 이 테스트가 먼저 깨진다 — nav2_loopback_sim 의 시드 미고정 RNG 가
//  정확히 그 반면교사다)
TEST(FakeSimBackend, DifferentSeedsGiveSameResultBecauseNoRandomnessIsUsed)
{
  EXPECT_DOUBLE_EQ(run_fixed_sequence(42), run_fixed_sequence(99));
}

// 시뮬 시각이 step 호출로만, 정확히 dt 배수로 전진한다.
TEST(FakeSimBackend, SimTimeAdvancesOnlyByStepDt)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  double sim_time_s = -1.0;
  ASSERT_TRUE(backend.step(1, sim_time_s));
  EXPECT_DOUBLE_EQ(sim_time_s, 0.05); // 손으로 계산: 1 * 0.05

  ASSERT_TRUE(backend.step(3, sim_time_s));
  EXPECT_DOUBLE_EQ(sim_time_s, 0.20); // 0.05 + 3 * 0.05
}

// ── 구성 거부 ───────────────────────────────────────────────────────────────

// dt <= 0 은 시간이 전진하지 않는 구성이다 — 조용히 돌면 실험이 멈춘 채 통과한다.
TEST(FakeSimBackend, ConfigureRejectsNonPositiveStepDt)
{
  mrs::FakeSimBackend backend;
  mrs::FakeSimConfig config = make_config();
  config.step_dt_s = 0.0;
  EXPECT_FALSE(backend.configure(config));
}

// 관측 스코프는 UNIFORM 이어야 한다 — 포트가 균일 뷰 노드를 요구하기 때문이다.
TEST(FakeSimBackend, ConfigureRejectsNonUniformViewScope)
{
  mrs::FakeSimBackend backend;
  mrs::FakeSimConfig config = make_config();
  config.view_scope.view_kind = mrs::ViewKind::PHYSICAL;
  EXPECT_FALSE(backend.configure(config));
}

// 로봇 id 중복은 관측 배열의 의미를 무너뜨린다.
TEST(FakeSimBackend, ConfigureRejectsDuplicateRobotId)
{
  mrs::FakeSimBackend backend;
  mrs::FakeSimConfig config = make_config();
  config.robots.push_back({mrs::RobotId{1}, mrs::Pose2D{}});
  EXPECT_FALSE(backend.configure(config));
}

// 노드 id 중복은 부착 판정을 비결정적으로 만든다.
TEST(FakeSimBackend, ConfigureRejectsDuplicateNodeAnchorId)
{
  mrs::FakeSimBackend backend;
  mrs::FakeSimConfig config = make_config();
  config.node_anchors.push_back({mrs::UniformNodeId{10}, 9.0, 9.0});
  EXPECT_FALSE(backend.configure(config));
}

// 정상 구성은 받아들여진다 — 위 거부들이 "무엇이든 거부"가 아님을 보이는 대조군.
TEST(FakeSimBackend, ConfigureAcceptsValidConfig)
{
  mrs::FakeSimBackend backend;
  EXPECT_TRUE(backend.configure(make_config()));
}

// ── reset 이전 거동 ─────────────────────────────────────────────────────────

// configure 만 하고 reset 을 안 했으면 아무 것도 하지 않고, out 도 건드리지 않는다.
TEST(FakeSimBackend, OperationsFailBeforeResetAndLeaveOutIntact)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));

  double sim_time_s = 0.0;
  std::vector<mrs::RobotObservation> observations;
  std::vector<mrs::SimMetricSample> samples;

  EXPECT_FALSE(backend.step(1, sim_time_s));
  EXPECT_FALSE(backend.sense(observations));
  EXPECT_TRUE(observations.empty());
  EXPECT_FALSE(backend.poll_metrics(samples));
  EXPECT_TRUE(samples.empty());
}

// ── 관측 순서·스코프·노드 부착 ──────────────────────────────────────────────

// 관측은 로봇 id 오름차순 — 구성 입력 순서(1, 0)와 무관해야 결정론이 성립한다.
TEST(FakeSimBackend, SenseReturnsObservationsInAscendingRobotIdOrder)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  std::vector<mrs::RobotObservation> observations;
  ASSERT_TRUE(backend.sense(observations));
  ASSERT_EQ(observations.size(), 2U);
  EXPECT_EQ(observations[0].robot_id, 0);
  EXPECT_EQ(observations[1].robot_id, 1);
}

// 구성의 뷰 스코프가 관측에 그대로 실린다 — 수신 측 스코프 대조의 근거.
TEST(FakeSimBackend, SensePropagatesConfiguredViewScope)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  std::vector<mrs::RobotObservation> observations;
  ASSERT_TRUE(backend.sense(observations));
  ASSERT_FALSE(observations.empty());
  EXPECT_EQ(observations[0].view_scope.view_kind, mrs::ViewKind::UNIFORM);
  EXPECT_EQ(observations[0].view_scope.roadmap_version, 7U);
  EXPECT_EQ(observations[0].view_scope.view_id, 1U);
}

// 부착 반경(0.25 m) 안이면 그 노드를 점유 중으로 본다. 로봇 1 은 원점 = 노드 10 위.
TEST(FakeSimBackend, SenseAttachesRobotWithinAnchorRadius)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  std::vector<mrs::RobotObservation> observations;
  ASSERT_TRUE(backend.sense(observations));
  ASSERT_EQ(observations.size(), 2U);
  EXPECT_EQ(observations[1].occupied_node, mrs::UniformNodeId{10});
}

// 반경 밖(로봇 0 은 x = 5, 가장 가까운 노드가 1 m 지점)이면 붙이지 않는다 — 추측 금지.
TEST(FakeSimBackend, SenseDoesNotAttachRobotOutsideAnchorRadius)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  std::vector<mrs::RobotObservation> observations;
  ASSERT_TRUE(backend.sense(observations));
  ASSERT_FALSE(observations.empty());
  EXPECT_TRUE(observations[0].occupied_node.is_none());
}

// next_node 는 추측하지 않는다 — 백엔드는 계획을 모른다.
TEST(FakeSimBackend, SenseNeverGuessesNextNode)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  std::vector<mrs::RobotObservation> observations;
  ASSERT_TRUE(backend.sense(observations));
  for (const mrs::RobotObservation & observation : observations)
  {
    EXPECT_TRUE(observation.next_node.is_none());
  }
}

// ── actuate → 상태 반영 지연 (누수 방지 장치) ───────────────────────────────

// 지연 0.2 s 구성에서 1스텝(0.05 s) 뒤에는 아직 반영되지 않아야 한다.
// 코어가 "지금 곧바로 반영됨"을 전제하면 isaac 부착 시 그 전제가 깨진다(architecture §5.2).
TEST(FakeSimBackend, ActuationIsNotAppliedBeforeConfiguredLatency)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config(0.2)));
  ASSERT_TRUE(backend.reset(1));

  const std::vector<mrs::ActuationCommand> commands{{mrs::RobotId{1}, 1.0, 0.0}};
  ASSERT_TRUE(backend.actuate(commands));

  double sim_time_s = 0.0;
  ASSERT_TRUE(backend.step(1, sim_time_s)); // t = 0.05 < 0.2

  std::vector<mrs::RobotObservation> observations;
  ASSERT_TRUE(backend.sense(observations));
  ASSERT_EQ(observations.size(), 2U);
  EXPECT_DOUBLE_EQ(observations[1].body_pose.x_m, 0.0);
}

// 지연을 넘기면 반영된다 — 지연이 "영구 무시"로 새지 않는다.
TEST(FakeSimBackend, ActuationIsAppliedAfterConfiguredLatency)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config(0.2)));
  ASSERT_TRUE(backend.reset(1));

  const std::vector<mrs::ActuationCommand> commands{{mrs::RobotId{1}, 1.0, 0.0}};
  ASSERT_TRUE(backend.actuate(commands));

  double sim_time_s = 0.0;
  for (int i = 0; i < 6; ++i) // t = 0.30 > 0.2
  {
    ASSERT_TRUE(backend.step(1, sim_time_s));
  }

  std::vector<mrs::RobotObservation> observations;
  ASSERT_TRUE(backend.sense(observations));
  ASSERT_EQ(observations.size(), 2U);
  EXPECT_GT(observations[1].body_pose.x_m, 0.0);
}

// ── actuate 입력 검증 ───────────────────────────────────────────────────────

TEST(FakeSimBackend, ActuateRejectsUnknownRobotId)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  const std::vector<mrs::ActuationCommand> commands{{mrs::RobotId{9}, 1.0, 0.0}};
  EXPECT_FALSE(backend.actuate(commands));
}

TEST(FakeSimBackend, ActuateRejectsNanCommand)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  const std::vector<mrs::ActuationCommand> commands{{mrs::RobotId{1}, std::nan(""), 0.0}};
  EXPECT_FALSE(backend.actuate(commands));
}

// 부분 적용 금지: 유효한 지령 하나 + 미지 로봇 하나면 **전체를 거부**하고 아무 것도 적용하지
// 않는다. 부분 적용은 "일부 로봇만 새 명령을 받은" 상태를 만들어 재현성을 깨뜨린다.
TEST(FakeSimBackend, ActuateRejectsWholeBatchAndAppliesNothingOnPartialInvalidity)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  const std::vector<mrs::ActuationCommand> partial{{mrs::RobotId{1}, 1.0, 0.0},
                                                   {mrs::RobotId{9}, 1.0, 0.0}};
  EXPECT_FALSE(backend.actuate(partial));

  double sim_time_s = 0.0;
  ASSERT_TRUE(backend.step(1, sim_time_s));

  std::vector<mrs::RobotObservation> observations;
  ASSERT_TRUE(backend.sense(observations));
  ASSERT_EQ(observations.size(), 2U);
  EXPECT_DOUBLE_EQ(observations[1].body_pose.x_m, 0.0); // 유효했던 지령도 적용되지 않았다
}

// ── 결함 주입: 지원 여부를 정직하게 신고한다 ────────────────────────────────

// 통신 결함은 comms_emul 소관이다 — 흉내내면 D-10 결과의 출처가 흐려진다.
TEST(FakeSimBackend, InjectRejectsCommunicationFaults)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  std::uint64_t injection_id = 0;
  EXPECT_FALSE(
    backend.inject({mrs::FaultKind::COMM_DELAY, mrs::RobotId{1}, 0, 1.0, 0.0}, injection_id));
}

// 통로 개폐는 지도를 아는 계층의 일이다 — 기하 전용 백엔드가 흉내내지 않는다.
TEST(FakeSimBackend, InjectRejectsCorridorFaults)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  std::uint64_t injection_id = 0;
  EXPECT_FALSE(
    backend.inject({mrs::FaultKind::CORRIDOR_CLOSE, mrs::RobotId{1}, 0, 0.0, 0.0}, injection_id));
}

// ROBOT_STALL 은 지원한다. injection_id 는 1 부터 시작(0 = 미할당과 구별).
TEST(FakeSimBackend, InjectAcceptsRobotStallAndIssuesIdFromOne)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  std::uint64_t injection_id = 0;
  EXPECT_TRUE(
    backend.inject({mrs::FaultKind::ROBOT_STALL, mrs::RobotId{1}, 0, 0.0, 0.1}, injection_id));
  EXPECT_EQ(injection_id, 1U);
}

// stall 지속 시간(0.1 s) 동안은 정지, 지난 뒤에는 다시 움직인다.
// (stall 이 영구화되거나 아예 무시되는 두 회귀를 함께 가른다)
TEST(FakeSimBackend, RobotStallHoldsRobotThenReleasesAfterDuration)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  std::uint64_t injection_id = 0;
  ASSERT_TRUE(
    backend.inject({mrs::FaultKind::ROBOT_STALL, mrs::RobotId{1}, 0, 0.0, 0.1}, injection_id));

  const std::vector<mrs::ActuationCommand> commands{{mrs::RobotId{1}, 1.0, 0.0}};
  ASSERT_TRUE(backend.actuate(commands));

  double sim_time_s = 0.0;
  for (int i = 0; i < 2; ++i) // t = 0.10 — 아직 stall 구간
  {
    ASSERT_TRUE(backend.step(1, sim_time_s));
  }
  std::vector<mrs::RobotObservation> observations;
  ASSERT_TRUE(backend.sense(observations));
  ASSERT_EQ(observations.size(), 2U);
  EXPECT_DOUBLE_EQ(observations[1].body_pose.x_m, 0.0);

  for (int i = 0; i < 4; ++i) // t = 0.30 — 해제 이후
  {
    ASSERT_TRUE(backend.step(1, sim_time_s));
  }
  ASSERT_TRUE(backend.sense(observations));
  ASSERT_EQ(observations.size(), 2U);
  EXPECT_GT(observations[1].body_pose.x_m, 0.0);
}

// ── metrics tap ─────────────────────────────────────────────────────────────

// 반지름 0.3 인 로봇 둘이 0.1 m 떨어져 있으면 기하 중첩(합 0.6 > 0.1)이다.
// ⚠ 이것은 **기하 판정**이며 물리 접촉이 아니다 — 안전 주장으로 쓸 수 없다.
TEST(FakeSimBackend, PollMetricsDetectsGeometricOverlapAsCollision)
{
  mrs::FakeSimBackend backend;
  mrs::FakeSimConfig config = make_config();
  config.robots = {{mrs::RobotId{0}, mrs::Pose2D{0.0, 0.0, 0.0}},
                   {mrs::RobotId{1}, mrs::Pose2D{0.1, 0.0, 0.0}}};
  ASSERT_TRUE(backend.configure(config));
  ASSERT_TRUE(backend.reset(1));

  double sim_time_s = 0.0;
  ASSERT_TRUE(backend.step(1, sim_time_s));

  std::vector<mrs::SimMetricSample> samples;
  ASSERT_TRUE(backend.poll_metrics(samples));
  ASSERT_EQ(samples.size(), 2U);
  EXPECT_EQ(samples[0].key, "collision_count");
  EXPECT_GT(samples[0].value, 0.0);
  EXPECT_EQ(samples[1].key, "min_separation_m");
}

// **전 표본이 pending_isaac = true** — 이 백엔드의 수치가 연구 검증으로 승격되는 것을
// 구조적으로 막는 정직성 장치다. 하나라도 false 로 새면 §5.3 의 kill-gate 가 무력화된다.
TEST(FakeSimBackend, AllMetricSamplesAreTaggedPendingIsaac)
{
  mrs::FakeSimBackend backend;
  mrs::FakeSimConfig config = make_config();
  config.robots = {{mrs::RobotId{0}, mrs::Pose2D{0.0, 0.0, 0.0}},
                   {mrs::RobotId{1}, mrs::Pose2D{0.1, 0.0, 0.0}}};
  ASSERT_TRUE(backend.configure(config));
  ASSERT_TRUE(backend.reset(1));

  double sim_time_s = 0.0;
  ASSERT_TRUE(backend.step(1, sim_time_s));

  std::vector<mrs::SimMetricSample> samples;
  ASSERT_TRUE(backend.poll_metrics(samples));
  ASSERT_FALSE(samples.empty());
  for (const mrs::SimMetricSample & sample : samples)
  {
    EXPECT_TRUE(sample.pending_isaac);
  }
}

// 구간 지표는 회수 후 리셋된다 — 이전 구간의 잔류가 다음 판정을 오염시키지 않는다.
// (누적 지표 collision_count 는 남고 구간 지표 min_separation_m 은 사라진다)
TEST(FakeSimBackend, IntervalMetricsResetAfterPoll)
{
  mrs::FakeSimBackend backend;
  mrs::FakeSimConfig config = make_config();
  config.robots = {{mrs::RobotId{0}, mrs::Pose2D{0.0, 0.0, 0.0}},
                   {mrs::RobotId{1}, mrs::Pose2D{0.1, 0.0, 0.0}}};
  ASSERT_TRUE(backend.configure(config));
  ASSERT_TRUE(backend.reset(1));

  double sim_time_s = 0.0;
  ASSERT_TRUE(backend.step(1, sim_time_s));

  std::vector<mrs::SimMetricSample> samples;
  ASSERT_TRUE(backend.poll_metrics(samples));
  ASSERT_EQ(samples.size(), 2U);

  ASSERT_TRUE(backend.poll_metrics(samples));
  EXPECT_EQ(samples.size(), 1U);
}

// ── capabilities 정직성 신고 (Q-8) ──────────────────────────────────────────

// 백엔드가 스스로 "물리 없음·접촉 미지원·부분관측 미지원"을 신고해야 sim-runner 가
// OPEN(pending-isaac) 태깅을 자동화할 수 있다. 여기 값이 부풀려지면 isaac 필수 결정이
// pysim 결과로 조용히 종결된다.
TEST(FakeSimBackend, CapabilitiesReportNoPhysicsHonestly)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));

  const mrs::SimCapabilities capabilities = backend.capabilities();
  EXPECT_EQ(capabilities.physics_fidelity, mrs::PhysicsFidelity::NONE);
  EXPECT_FALSE(capabilities.supports_contact);
  EXPECT_FALSE(capabilities.supports_partial_observation);
  EXPECT_TRUE(capabilities.deterministic_with_seed);
  EXPECT_EQ(capabilities.backend_name, "fake");
}

// ── 각도 정규화 ─────────────────────────────────────────────────────────────

// 100스텝 × ω = 10 rad/s × 0.05 s = 50 rad 회전 후에도 자세가 [-pi, pi] 안이어야 한다.
// 정규화가 빠지면 Pose2D 계약(각도 구간)이 깨져 변환 경계에서 전량 폐기된다.
TEST(FakeSimBackend, HeadingStaysWithinPiIntervalAfterManyRotations)
{
  mrs::FakeSimBackend backend;
  ASSERT_TRUE(backend.configure(make_config()));
  ASSERT_TRUE(backend.reset(1));

  const std::vector<mrs::ActuationCommand> spin{{mrs::RobotId{1}, 0.0, 10.0}};
  double sim_time_s = 0.0;
  for (int i = 0; i < 100; ++i)
  {
    ASSERT_TRUE(backend.actuate(spin));
    ASSERT_TRUE(backend.step(1, sim_time_s));
  }

  std::vector<mrs::RobotObservation> observations;
  ASSERT_TRUE(backend.sense(observations));
  ASSERT_EQ(observations.size(), 2U);
  EXPECT_GE(observations[1].body_pose.theta_rad, -3.14159266);
  EXPECT_LE(observations[1].body_pose.theta_rad, 3.14159266);
}
