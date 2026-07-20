# multibot3

다중로봇 궤적계획·회피주행 시스템 — ROS 2 Jazzy colcon 워크스페이스.

계층별 점증 흡수 사다리(R0~R4) + 판정자 기반 통합 아키텍처의 구현체.
설계 근거는 상위 리포([multibot3_ws](https://github.com/hchju777/multibot3_ws))의
`_workspace/06_design/design-decision.yaml`(서명본)에 있으며, 이 리포는 그 확정서의
구현 대상이다.

> 이전 세대 코드는 `multibot3_legacy`로 분리됐다. 본 리포는 확정서 §2의
> "컨셉만 계승, 재작성" 방침에 따라 새로 시작한다.

## 요구 환경

- ROS 2 **Jazzy** (Ubuntu 24.04)
- C++17, CMake ≥ 3.28
- OSQP (`ros-jazzy-osqp-vendor`) — L4 corridor QP
- CBC (`coinor-libcbc-dev`) — L3 재조정 MILP

## 빌드

```bash
cd multibot3
colcon build --symlink-install
source install/setup.bash
```

## 소스 구조

`src/` 아래는 **의존 계층**으로 나뉜다. 의존은 아래에서 위로만 흐르며 역방향은 CI가 거부한다.

| 디렉토리 | 내용 | 규칙 |
|---|---|---|
| `contracts/` | `mrs_interfaces`(.msg/.srv), `mrs_core_types` | 인터페이스 계약. 변경은 semver + 레지스트리 갱신 |
| `domain/` | 순수 알고리즘 11종 | **ROS 무의존.** `rclcpp` include 금지 — CI 검사 |
| `convert/` | `mrs_msg_convert` | 도메인 타입 ↔ 메시지 변환의 **유일한** 장소 |
| `ros/` | 얇은 노드 9종 | 콜백 안에 알고리즘 금지. I/O 어댑터만 |
| `assembly/` | bringup·bench·pysim·isaac·analysis | 조립과 실험 |

`domain/`이 ROS를 모르는 것이 이 구조의 핵심이다 — 알고리즘이 시뮬레이터·미들웨어 없이
결정론적으로 테스트된다.

## 충실도 티어

| 티어 | 실행 | 검증 대상 |
|---|---|---|
| `unit` | gtest (ROS 없음) | 순수 로직·불변식 |
| `pysim` | 경량 파이썬 추상 시뮬 | 프로토콜·배선·상태기계 |
| `isaac` | Isaac Sim | QP feasibility·livelock·물리 |

⚠ 티어 이름은 로봇 계층(L1~L4)·사다리(R0~R5)·실험(F1~F6)과 **무관하다**.
QP feasibility·livelock·실제 diff-drive 물리는 `pysim`으로 판정할 수 없으며,
해당 항목은 `isaac` 실측 전까지 `OPEN(pending-isaac)`으로 남는다.

## 라이선스

미정.
