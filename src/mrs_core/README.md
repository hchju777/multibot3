# mrs_core

두 타깃(`CN-3`): `mrs_core_pure`(ROS·JSON·pluginlib 미링크) / `mrs_core_msgs`(경계 표현·JSON).

## 오늘 있는 것 (47차 P1, `347_arch_integration_delta.md` §3-1 D14)

- `mrs_core_pure`: `Status` / `Result<T,E>` / `ContractViolation`(`include/mrs_core/status.hpp`),
  `ISteadyClock`(`include/mrs_core/i_steady_clock.hpp`), `IInstrSink`(`include/mrs_core/i_instr_sink.hpp`).
- `mrs_core_msgs`: 문자열↔`uint8` 상수 사상 표(`include/mrs_core_msgs/reason_codes.hpp`,
  `_workspace/350_msg_idl_decisions.md` §3-2·§3-4 그대로).
- `startup_checker` 실행 파일: 🔴 **스텁이다.** C-2/C-3/C-4(로드맵-config 대조·W1·W4·필수 키)는
  구현되지 않았다 — `mrs_bringup`(P3, 이번 라운드 밖)이 있어야 로드맵 파일 경로·config.yaml이
  생긴다. `_workspace/21_integration.md`의 스텁 표에 등재돼 있다.

## 아직 없는 것 (D14가 명시적으로 미룬 것 — 이름만 적어 둔다)

- 경계 표현 C++ 구조체 + JSON 직렬화 — 네 모듈이 이미 각자 `adapter/boundary_types.hpp` +
  `io/json_io.{hpp,cpp}`를 갖고 있다. 통합의 전달 매체는 ROS 토픽이지 JSON 파일이 아니다.
- 그래프 술어(강연결·도달성·비순환) — 오늘 SADG가 `core/execution_graph.cpp`에 자기 것을
  갖고 있고 계획기 쪽 자문 선검사가 아직 구현되지 않았다(소비자 1명뿐).
- 링버퍼·분위수 집계 — 실물이 이미 `mrs_sadg/core/instr_ring.hpp` ·
  `mrs_trajopt/core/instrumentation.hpp`에 있다. 분위수 정의가 갈릴 위험은 실험(23) 착수
  전에 반드시 통일한다(D14 뒤집히는 조건).
