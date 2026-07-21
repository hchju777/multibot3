# TimedNodeVisit — L2 솔버 출력의 표준형 원소 (D-03 "시각 부여 node-visit 시퀀스")
#
# 근거: D-03 implementation_target(솔버 플러그인 인터페이스 출력 표준화 — L3 는 솔버 불가지론),
#       lf-pp-trajectory-2025(이산↔연속 인터페이스 형식화, 정성 인용)
# 노드 id 는 **L2 균일 뷰** 기준(D-11 v2). L3 는 collapse 후 골격으로 되돌려 의존성 그래프를 만든다.

uint32 node_id                        # 균일 뷰 노드 id
builtin_interfaces/Time arrival_time  # 계획상 도착 시각(sim clock). 단조증가여야 한다
