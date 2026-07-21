# PredecessorConstraint — 하드 통행순서 제약 1건 (D-05)
#
# 의미: "predecessor_robot_id 가 node_id 를 **클리어**하기 전에는 이 창의 소유 로봇이
#        node_id 의 claim region 에 진입할 수 없다." 순서는 soft 비용이 아니라 **hard** 다(D-05).
# 클리어 판정의 물리적 정의: 선행 로봇의 몸체가 node_id 의 claim region(반경 r_clear,
#        MapRegistry 제공 — T1 §7.2) 을 완전히 이탈. r_clear 산정식은 OPEN(T1 O6).
# 시각은 **싣지 않는다**. T1 §7.1: 순서는 시간 제약이 아니라 "활성화 조건이 시간 사건인
#        공간 제약"이며, L4 는 이를 이동 정지선(§7.2)으로 QP 에 편입한다.
# 게시 규약: 메시지에 실리는 제약은 **발행 시점에 아직 미충족인 것만**이다. 제약이 해소되면
#        L3 는 window_seq 를 올려 창을 재릴리스한다(adg-2019 이벤트 구동 실행).

uint16 predecessor_robot_id  # 선행 로봇 id
uint32 node_id               # 선행 로봇이 클리어해야 하는 균일 뷰 노드 id
