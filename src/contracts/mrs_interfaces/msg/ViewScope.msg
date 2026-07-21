# ViewScope — 노드 id 가 유효한 뷰의 식별 (뷰 강타입의 wire 표현)
#
# 계약 버전: 1.0.0 (신설 — 사용자 결정 R-14 (a) = B + C 동시 채택)
# 근거: contract-registry §0 "노드 id 는 뷰마다 다르다", D-11 v2(3뷰), core-types-proposal §6.1
#
# ── 왜 필요한가 ────────────────────────────────────────────────────────────────────────
#  C++ 층은 뷰별 **강타입**(PhysicalNodeId/UniformNodeId/SkeletonNodeId, phantom tag)으로
#  뷰 혼동을 컴파일 에러로 만든다(B). 그러나 강타입은 **DDS 경계에서 소실**되어 전부 uint32 로
#  직렬화되므로, 경계 너머의 오용은 타입으로 잡히지 않는다. 이 메시지가 그 자리를 메운다(C).
#  C 는 B 의 대안이 아니라 **전제**다(R-14 (a)).
#
#  뷰 혼동이 위험한 이유: 뷰가 달라도 id 는 대개 상대 뷰에서 **유효한 값**이라 범위 검사로
#  잡히지 않고, 증상이 "L2 계획이 L4 에서 무효"로 나타나는데 **그것이 정확히 D-11 의 통과 지표
#  (planning error ≈ 0)** 다 — 타입 실수 1건이 seam 알고리즘 실패로 오독된다.
#
# ── 두 축 ─────────────────────────────────────────────────────────────────────────────
#  * `view_kind`  = **종류**. C++ phantom tag 의 wire 대응물. 계약이 필드마다 고정한다(§0 표).
#                   실어 보내는 이유: 발행자가 잘못된 종류를 랩해 보낸 경우를 수신 측에서
#                   탐지하려면 기대값과 대조할 무언가가 필요하기 때문이다.
#  * (`roadmap_version`, `view_id`) = **인스턴스**. 같은 종류라도 지도 개정·재세분화가 있으면
#                   id 공간이 달라진다. Q-6 캐시 무효화 키와 동일한 값이다.
#
# ── 수신 측 규약 (mrs_msg_convert 유일 담당) ──────────────────────────────────────────
#  1. 랩(unwrap→강타입): `view_kind` 가 그 필드에 계약이 고정한 종류와 다르면 **메시지 폐기**
#     + 로그 + 계측 카운터. 예외를 던지지 않는다(안전 폴백 — CLAUDE.md 규율 2).
#  2. 인스턴스 대조: (`roadmap_version`, `view_id`)가 수신자의 현재 캐시와 다르면 폐기 + 재조회.
#  3. 언랩(강타입→uint32): 발행 시 계약 고정 종류를 그대로 실어 보낸다.
#  ⛔ msg_convert 밖에서 uint32 ↔ 강타입 변환을 하지 말 것(C4 와 같은 취지의 단일 소유 규칙).

uint8 VIEW_KIND_PHYSICAL=0   # 물리 roadmap (불균일 엣지, 자동 추출 원본). view_id 는 항상 0
uint8 VIEW_KIND_UNIFORM=1    # L2 세분화 균일 뷰 (가상 노드 포함). view_id = GetUniformView 발급값
uint8 VIEW_KIND_SKELETON=2   # L3 collapse 골격. view_id = 유래 균일 뷰의 id
uint8 view_kind

uint64 roadmap_version   # MapRegistry 지도 버전. 0 = 미지정(런타임 금지)
uint32 view_id           # VIEW_KIND_PHYSICAL 이면 0. 그 외는 발급된 뷰 id
