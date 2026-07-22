# CorridorFlipEvent — 통로 방향 flip 1건 (D-12/D-13 정상상태 정책 flip, D-08 M10 입력)
#
# 계약 버전: 1.0.0  (재잠금 8차 신설)
# 근거: D-08 v6(M10 정책-유발 진동 탐지기), D-12(통로 방향-mutex), theory T3/T7
#
# ── 왜 RungEvent 를 재사용하지 않는가 (오버로드 금지) ──────────────────────────────────
#  D-12/D-13 의 정상상태 coherent-flip 은 교란(EscalationReport)이 없어 event_id 를 만들지
#  않는다 — RungEvent.event_id 는 **교란 상관 키**(M1/M5 그룹핑)라 정상상태 flip 에 부재하고,
#  합성 event_id 를 끼우면 M1/M5 의미가 오염된다. 또 RungEvent.edge_set_fingerprint 는
#  **순서 무관 해시**라 A→B 와 B→A 가 동일 지문 = flip 이 지문에 안 잡힌다(방향 맹).
#  ⇒ flip 은 **방향 지문**을 실은 별도 이벤트로 낸다. M10 은 이 이벤트열을 관측한다.
#
# ── M10 이 이 이벤트로 하는 일 ─────────────────────────────────────────────────────────
#  동일 directed_config_fingerprint 가 재방문되는데 progress_counter 가 그 사이 불변이면
#  = 방향 config 가 진행 없이 되돌아온 것 = **livelock 증인**. 진행이 있으면(각 flip 에 완료
#  1건 이상) 정상 교대로 판정(오탐 없음). G-ε 이 R3 릴리스만 가드하는 사각(R0/R1 정상상태
#  정책 flip)을 M10 이 메운다. **M10 은 관측만 하며 종료를 증명하지 않는다**(O-C3/O-T3-1, 2차 범위).

uint32 corridor_id                 # flip 된 통로. **골격 뷰 엣지(SkeletonEdgeId)** — 차수 2 체인을
                                   # 삼킨 의존성 구간이 곧 통로다
uint8 DIR_A_TO_B=0                 # 통로 방향: node_a → node_b (골격 엣지 기준 진행 방향)
uint8 DIR_B_TO_A=1                 # 통로 방향: node_b → node_a
uint8 direction                    # flip **후**의 방향 (위 상수 중 하나)

uint64 directed_config_fingerprint # 전 mutex 통로의 {(corridor_id, direction)} 멀티셋 방향 지문.
                                   # 순서 무관이되 **방향 인지** — A→B 와 B→A 가 다른 지문이다
                                   # (RungEvent.edge_set_fingerprint 의 방향 맹을 메우는 필드)
uint64 progress_counter            # flip 시점의 완료 태스크 수(또는 Φ̂ 진행 척도) 스냅샷.
                                   # 동일 지문 재방문 + 이 값 불변 = M10 livelock 증인
uint64 roadmap_version             # 통로가 속한 지도 버전 (스코프)
