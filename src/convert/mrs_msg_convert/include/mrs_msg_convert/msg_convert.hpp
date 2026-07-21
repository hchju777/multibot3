#pragma once

/**
 * @file msg_convert.hpp
 * @brief 도메인 타입 <-> mrs_interfaces 메시지 변환의 유일한 장소 (architecture §2.1 L2).
 *
 * ros/ 의 어떤 노드도 mrs_interfaces 메시지 필드를 직접 도메인 타입 필드에 대입하지 않는다 —
 * 전부 이 패키지의 함수를 거친다. 이 계층이 없으면 스키마 변경이 노드 코드 곳곳에 흩어져
 * 계약 변경 게이트(contract-registry §7)가 무력화된다.
 *
 * 함수는 예외를 던질 수 있다(L1 포트와 달리 noexcept 가 아니다) — 변환 실패는 호출자(ros/)가
 * try/catch 로 잡아 안전 폴백한다(CLAUDE.md 규율 2).
 */

#include <vector>

#include "mrs/contract_types.hpp"
#include "mrs/i_path_solver.hpp"        // PlanRequestEntry
#include "mrs/i_local_planner.hpp"      // NeighborTrajectory
#include "mrs/i_judge_policy.hpp"       // JudgeObservation, JudgeDecision, GuardDecision
#include "mrs/i_sim_backend.hpp"        // RobotObservation, SimCapabilities, FaultInjection, SimMetricSample
#include "mrs/reorder_types.hpp"        // ReorderRequest, ReorderAck
#include "mrs_task_alloc/lifelong_task_allocator.hpp" // ReassignTrigger, ReassignResult
#include "mrs_metrics/metrics_collector.hpp"           // RungEventRecord
#include "mrs_map_registry/map_registry.hpp"           // RoadmapViewData, RoadmapValidationResult

#include "mrs_interfaces/msg/execution_window.hpp"
#include "mrs_interfaces/msg/escalation_report.hpp"
#include "mrs_interfaces/msg/planned_paths.hpp"
#include "mrs_interfaces/msg/robot_path.hpp"
#include "mrs_interfaces/msg/task_assignment.hpp"
#include "mrs_interfaces/msg/commit_state.hpp"
#include "mrs_interfaces/msg/commit_status.hpp"
#include "mrs_interfaces/msg/frozen_order.hpp"
#include "mrs_interfaces/msg/local_plan_share.hpp"
#include "mrs_interfaces/msg/judge_verdict.hpp"
#include "mrs_interfaces/msg/rung_event.hpp"
#include "mrs_interfaces/msg/robot_state.hpp"
#include "mrs_interfaces/msg/observation_uncertainty.hpp"
#include "mrs_interfaces/msg/sim_capabilities.hpp"
#include "mrs_interfaces/msg/sim_metric_sample.hpp"
#include "mrs_interfaces/msg/roadmap_node.hpp"
#include "mrs_interfaces/msg/roadmap_edge.hpp"
#include "mrs_interfaces/msg/node_mapping.hpp"
#include "mrs_interfaces/srv/reassign_request.hpp"
#include "mrs_interfaces/srv/partial_replan.hpp"
#include "mrs_interfaces/srv/plan_paths.hpp"
#include "mrs_interfaces/srv/reorder_passing.hpp"
#include "mrs_interfaces/srv/validate_map.hpp"
#include "mrs_interfaces/srv/sim_inject.hpp"

namespace mrs::convert
{

// ── ExecutionWindow (★ load-bearing seam, L-01) ─────────────────────────────
/**
 * @brief 도메인 ExecutionWindow 를 ExecutionWindow.msg 로 변환한다.
 * @param[in] window 도메인 표현 (contract_types.hpp).
 * @return mrs_interfaces::msg::ExecutionWindow 발행용 메시지.
 */
mrs_interfaces::msg::ExecutionWindow to_msg(const mrs::ExecutionWindow & window);

/**
 * @brief ExecutionWindow.msg 를 도메인 ExecutionWindow 로 변환한다.
 * @param[in] msg 수신 메시지.
 * @return mrs::ExecutionWindow 도메인 표현.
 */
mrs::ExecutionWindow from_msg(const mrs_interfaces::msg::ExecutionWindow & msg);

// ── EscalationReport ─────────────────────────────────────────────────────
/**
 * @brief 에스컬레이션 사유·심각도를 담아 EscalationReport.msg 를 만든다.
 * @param[in] robot_id 보고 로봇.
 * @param[in] event_id 교란 상관 키.
 * @param[in] reason 도메인 에스컬레이션 사유.
 * @param[in] window_seq 보고 시점 수용 중이던 창 seq.
 * @return mrs_interfaces::msg::EscalationReport 발행용 메시지.
 */
mrs_interfaces::msg::EscalationReport make_escalation_report(
  mrs::RobotId robot_id, mrs::EventId event_id, mrs::EscalationReason reason,
  std::uint32_t window_seq);

/**
 * @brief EscalationReport.msg 의 reason 필드를 도메인 enum 으로 변환한다.
 * @param[in] msg 수신 메시지.
 * @return mrs::EscalationReason 도메인 사유.
 */
mrs::EscalationReason reason_from_msg(const mrs_interfaces::msg::EscalationReport & msg);

// ── PlannedPaths / RobotPath / PlanRequestEntry ──────────────────────────
/**
 * @brief 도메인 로봇 경로 목록을 PlannedPaths.msg 로 변환한다.
 * @param[in] paths 로봇별 시각 부여 경로 (D-03 표준형).
 * @param[in] is_partial true = R3 부분 재계획 결과.
 * @return mrs_interfaces::msg::PlannedPaths 발행용 메시지.
 */
mrs_interfaces::msg::PlannedPaths to_msg(const std::vector<mrs::RobotPath> & paths, bool is_partial);

/**
 * @brief PlannedPaths.msg 를 도메인 로봇 경로 목록으로 변환한다.
 * @param[in] msg 수신 메시지.
 * @return std::vector<mrs::RobotPath> 도메인 표현.
 */
std::vector<mrs::RobotPath> from_msg(const mrs_interfaces::msg::PlannedPaths & msg);

/**
 * @brief PlanPaths.srv 요청의 PlanRequestEntry 배열을 도메인 표현으로 변환한다.
 * @param[in] req PlanPaths::Request.
 * @return std::vector<mrs::PlanRequestEntry> 도메인 표현.
 */
std::vector<mrs::PlanRequestEntry> from_request(const mrs_interfaces::srv::PlanPaths::Request & req);

// ── TaskAssignment ────────────────────────────────────────────────────────
/**
 * @brief 배정 결과로 TaskAssignment.msg 를 만든다.
 * @param[in] task_id 작업 id.
 * @param[in] robot_id 배정 로봇 id (회수는 ROBOT_ID_NONE).
 * @param[in] pickup_node 픽업 노드 (물리 roadmap 기준).
 * @param[in] delivery_node 딜리버리 노드 (물리 roadmap 기준).
 * @param[in] assignment_epoch 배정 세대 번호.
 * @return mrs_interfaces::msg::TaskAssignment 발행용 메시지.
 */
mrs_interfaces::msg::TaskAssignment make_task_assignment(
  std::uint64_t task_id, mrs::RobotId robot_id, mrs::NodeId pickup_node,
  mrs::NodeId delivery_node, std::uint32_t assignment_epoch);

// ── CommitState / CommitStatus / FrozenOrder ─────────────────────────────
/**
 * @brief 도메인 CommitState 를 CommitState.msg 로 변환한다 (PartialReplan.srv 요청 원소).
 * @param[in] state 도메인 표현.
 * @return mrs_interfaces::msg::CommitState 변환 결과.
 */
mrs_interfaces::msg::CommitState to_msg(const mrs::CommitState & state);

/**
 * @brief CommitState.msg 를 도메인 CommitState 로 변환한다.
 * @param[in] msg 원본 메시지.
 * @return mrs::CommitState 도메인 표현.
 */
mrs::CommitState from_msg(const mrs_interfaces::msg::CommitState & msg);

/**
 * @brief 도메인 FrozenOrder 목록을 FrozenOrder.msg 목록으로 변환한다.
 * @param[in] orders 도메인 표현.
 * @return std::vector<mrs_interfaces::msg::FrozenOrder> 변환 결과.
 */
std::vector<mrs_interfaces::msg::FrozenOrder> to_msg(const std::vector<mrs::FrozenOrder> & orders);

/**
 * @brief L4 커밋 상태로 CommitStatus.msg 를 만든다 (T2-R6).
 * @param[in] robot_id 보고 로봇.
 * @param[in] window_seq 현재 수용 중인 창 seq.
 * @param[in] committed_through_segment_index 커밋 접두 끝 인덱스.
 * @param[in] commit_hull_vertices 커밋 볼록포 정점 (map 프레임).
 * @param[in] qp_status AdoptionStatus 값 (msg 의 qp_status 필드와 일치).
 * @return mrs_interfaces::msg::CommitStatus 발행용 메시지.
 */
mrs_interfaces::msg::CommitStatus make_commit_status(
  mrs::RobotId robot_id, std::uint32_t window_seq, std::uint32_t committed_through_segment_index,
  const std::vector<mrs::Pose2D> & commit_hull_vertices, std::uint8_t qp_status);

// ── LocalPlanShare <-> NeighborTrajectory (T4-R3) ────────────────────────
/**
 * @brief 채택 계획으로 LocalPlanShare.msg 를 만든다 (매 틱 발행, 정의 T4.1-(3)).
 * @param[in] robot_id 발행 로봇.
 * @param[in] tick_seq 스텝 지수 h.
 * @param[in] control_points 채택 계획 c^(h) 제어점 (평탄 (x,y) 배열).
 * @param[in] shift_ratio_u u = Δt_h/Δt.
 * @return mrs_interfaces::msg::LocalPlanShare 발행용 메시지.
 */
mrs_interfaces::msg::LocalPlanShare make_local_plan_share(
  mrs::RobotId robot_id, std::uint32_t tick_seq,
  const std::vector<double> & control_points, double shift_ratio_u);

/**
 * @brief LocalPlanShare.msg 를 이웃 궤적 도메인 표현으로 변환한다.
 * @param[in] msg 수신 메시지.
 * @return mrs::NeighborTrajectory 도메인 표현 (fresh_j 판정 입력).
 */
mrs::NeighborTrajectory from_msg(const mrs_interfaces::msg::LocalPlanShare & msg);

// ── JudgeVerdict ──────────────────────────────────────────────────────────
/**
 * @brief 판정 결과로 JudgeVerdict.msg 를 만든다 (D-06, R2↔R3 단일 경계).
 * @param[in] event_id 교란 상관 키.
 * @param[in] decision 판정 결과 (JudgeDecision).
 * @param[in] completion_excess_s 판정량.
 * @param[in] threshold_s 적용 임계.
 * @return mrs_interfaces::msg::JudgeVerdict 발행용 메시지.
 */
mrs_interfaces::msg::JudgeVerdict make_judge_verdict(
  mrs::EventId event_id, const mrs::JudgeDecision & decision,
  double completion_excess_s, double threshold_s);

// ── RungEvent (계측, L-13) ────────────────────────────────────────────────
/**
 * @brief 계측 레코드로 RungEvent.msg 를 만든다.
 * @param[in] record 도메인 표현 (mrs_metrics).
 * @return mrs_interfaces::msg::RungEvent 발행용 메시지.
 */
mrs_interfaces::msg::RungEvent to_msg(const mrs::RungEventRecord & record);

// ── RobotState / ObservationUncertainty ──────────────────────────────────
/**
 * @brief 시뮬 백엔드 관측을 RobotState.msg 로 변환한다.
 * @param[in] observation 도메인 관측 (ISimBackend::sense() 결과).
 * @return mrs_interfaces::msg::RobotState 발행용 메시지.
 */
mrs_interfaces::msg::RobotState to_msg(const mrs::RobotObservation & observation);

/**
 * @brief RobotState.msg 를 도메인 관측으로 변환한다.
 * @param[in] msg 수신 메시지.
 * @return mrs::RobotObservation 도메인 표현.
 */
mrs::RobotObservation from_msg(const mrs_interfaces::msg::RobotState & msg);

// ── Sim 경계 (Capabilities / MetricSample / FaultInjection) ──────────────
/**
 * @brief 도메인 SimCapabilities 를 SimCapabilities.msg 로 변환한다 (Q-8).
 * @param[in] caps 백엔드 자기신고 값.
 * @return mrs_interfaces::msg::SimCapabilities 변환 결과.
 */
mrs_interfaces::msg::SimCapabilities to_msg(const mrs::SimCapabilities & caps);

/**
 * @brief 도메인 SimMetricSample 을 SimMetricSample.msg 로 변환한다.
 * @param[in] sample metrics tap 표본.
 * @return mrs_interfaces::msg::SimMetricSample 변환 결과.
 */
mrs_interfaces::msg::SimMetricSample to_msg(const mrs::SimMetricSample & sample);

/**
 * @brief SimInject.srv 요청을 도메인 FaultInjection 으로 변환한다.
 * @param[in] req SimInject::Request.
 * @return mrs::FaultInjection 도메인 표현.
 */
mrs::FaultInjection from_request(const mrs_interfaces::srv::SimInject::Request & req);

// ── MapRegistry 3뷰 (D-11) ────────────────────────────────────────────────
/**
 * @brief 도메인 뷰 데이터를 RoadmapNode.msg 배열로 변환한다.
 * @param[in] view 도메인 뷰 데이터.
 * @return std::vector<mrs_interfaces::msg::RoadmapNode> 노드 배열.
 */
std::vector<mrs_interfaces::msg::RoadmapNode> nodes_to_msg(const mrs::RoadmapViewData & view);

/**
 * @brief 도메인 뷰 데이터를 RoadmapEdge.msg 배열로 변환한다.
 * @param[in] view 도메인 뷰 데이터.
 * @return std::vector<mrs_interfaces::msg::RoadmapEdge> 엣지 배열.
 */
std::vector<mrs_interfaces::msg::RoadmapEdge> edges_to_msg(const mrs::RoadmapViewData & view);

/**
 * @brief 도메인 뷰 대응표를 NodeMapping.msg 배열로 변환한다.
 * @param[in] view 도메인 뷰 데이터.
 * @return std::vector<mrs_interfaces::msg::NodeMapping> 대응표 배열.
 */
std::vector<mrs_interfaces::msg::NodeMapping> mappings_to_msg(const mrs::RoadmapViewData & view);

/**
 * @brief 도메인 검증 결과를 ValidateMap.srv 응답 필드로 변환해 채운다.
 * @param[in] result 도메인 검증 결과.
 * @param[out] resp 채울 ValidateMap::Response.
 * @return void
 */
void fill_response(const mrs::RoadmapValidationResult & result, mrs_interfaces::srv::ValidateMap::Response & resp);

// ── ReassignRequest.srv <-> ReassignTrigger/ReassignResult (D-02 v2) ────
/**
 * @brief ReassignRequest.srv 요청의 trigger_type 을 도메인 enum 으로 변환한다.
 * @param[in] req ReassignRequest::Request.
 * @return mrs::ReassignTrigger 도메인 표현.
 */
mrs::ReassignTrigger trigger_from_request(const mrs_interfaces::srv::ReassignRequest::Request & req);

/**
 * @brief 도메인 처리 결과를 ReassignRequest.srv 응답 필드로 채운다.
 * @param[in] result 도메인 처리 결과.
 * @param[out] resp 채울 ReassignRequest::Response.
 * @return void
 */
void fill_response(mrs::ReassignResult result, mrs_interfaces::srv::ReassignRequest::Response & resp);

// ── PartialReplan.srv (D-03 v3 splice) ───────────────────────────────────
/**
 * @brief PartialReplan.srv 요청의 commit_states/frozen_orders 를 도메인 표현으로 변환한다.
 * @param[in] req PartialReplan::Request.
 * @param[out] out_commit_states 변환된 커밋 상태 목록.
 * @param[out] out_frozen_orders 변환된 무역전 강제 목록.
 * @return void
 */
void from_request(
  const mrs_interfaces::srv::PartialReplan::Request & req,
  std::vector<mrs::CommitState> & out_commit_states,
  std::vector<mrs::FrozenOrder> & out_frozen_orders);

// ── ReorderPassing.srv <-> ReorderRequest/ReorderAck (Q-2 service 바인딩) ─
/**
 * @brief ReorderPassing.srv 요청을 도메인 ReorderRequest 로 변환한다.
 * @param[in] req ReorderPassing::Request.
 * @return mrs::ReorderRequest 도메인 표현 (IReorderR1/IReorderR2 공용).
 */
mrs::ReorderRequest from_request(const mrs_interfaces::srv::ReorderPassing::Request & req);

/**
 * @brief 도메인 ReorderAck 를 ReorderPassing.srv 응답 필드로 채운다.
 * @param[in] ack 도메인 접수 결과.
 * @param[in] job_id 접수 식별자.
 * @param[out] resp 채울 ReorderPassing::Response.
 * @return void
 */
void fill_response(
  mrs::ReorderAck ack, std::uint64_t job_id, mrs_interfaces::srv::ReorderPassing::Response & resp);

} // namespace mrs::convert
