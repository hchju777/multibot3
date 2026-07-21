# FrozenOrder — 이미 릴리스되어 뒤집을 수 없는 교차 통행순서 1건 (T2-R2)
#
# 계약 버전: 1.0.0
# 근거: theory T2 §4.4 — 정리 T2.2 의 조건 (C3) 무역전(no reversal)을 코드로 강제하는 유일한 수단.
#       이 필드가 없으면 L2 가 순서를 자유롭게 뒤집어 접합 후 비순환성 보장이 무너진다.
#       **계약 잠금 전 필수 4건 중 #3.**
#
# 의미: "predecessor 가 predecessor_node 를 클리어한 뒤에야 successor 가 successor_node 에 진입"
#        — 이 방향은 이미 ExecutionWindow 로 릴리스되어 로봇이 그것에 근거해 움직이고 있다.
# 범위: **릴리스된 것만** 싣는다. 릴리스되지 않은 순서는 자유롭게 뒤집어도 무방하다(T2 §4.4).
# ⚠ OPEN(theory O-T2-2): BTPG 양방향 쌍 중 실행 시점에 아직 방향이 정해지지 않은 것은
#    "릴리스된 순서가 아니다"로 보아 여기 **싣지 않는다**(보수적 안전). 접합 후에도 btpg-2024
#    valid 조건이 유지되는지는 미유도 — R1 구현(로드맵 [3]) 전 해소 필요.

uint16 predecessor_robot_id
uint32 predecessor_node_id  # **균일 뷰(UniformNodeId)** — 릴리스된 창의 노드 공간과 같아야 한다
uint16 successor_robot_id
uint32 successor_node_id    # **균일 뷰(UniformNodeId)**
# 스코프: 이 배열을 담은 PartialReplan.srv 요청의 roadmap_version·view_id (view_kind = UNIFORM)
