# PlanRequestEntry — 계획 요청의 로봇 1대분 항목
#
# 근거: D-03. start_theta_rad 는 PartialReplan 의 CommitState.predicted_theta_rad 와 동일 근거
#       (diff-drive 회전 비용 — T2-R1). 전역 계획에서는 현재 측정 자세를 싣는다.

uint16 robot_id
uint32 start_node          # 시작 노드 id (균일 뷰)
float64 start_theta_rad    # 시작 자세 [rad], map, [-pi, pi]
uint32 goal_node           # 목표 노드 id (균일 뷰)
