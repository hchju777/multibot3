# multibot3 - MAPF Layer 설계 문서

> 작성 목적: multibot3 프로젝트의 MAPF(Multi-Agent Path Finding) 레이어 재설계 기준을 정의한다.  
> 대상 독자: 프로젝트를 처음 접하는 개발자, 리뷰어, 향후 구현 담당자

---

## 목차

1. [시스템 전체 구조](#1-시스템-전체-구조)
2. [설계 목표와 범위](#2-설계-목표와-범위)
3. [MAPF Layer 내부 구조](#3-mapf-layer-내부-구조)
4. [그래프 모델과 전처리](#4-그래프-모델과-전처리)
5. [계획 모델과 공개 인터페이스](#5-계획-모델과-공개-인터페이스)
6. [목적 함수와 품질 메트릭](#6-목적-함수와-품질-메트릭)
7. [Collision 및 Reservation 제약](#7-collision-및-reservation-제약)
8. [Timeout, 실패 의미, 운영 정책](#8-timeout-실패-의미-운영-정책)
9. [SADG Layer와의 인터페이스](#9-sadg-layer와의-인터페이스)
10. [설계 결정 요약](#10-설계-결정-요약)
11. [미해결 과제 및 향후 확장](#11-미해결-과제-및-향후-확장)
12. [참고 문헌](#12-참고-문헌)

---

## 1. 시스템 전체 구조

multibot3는 다수의 로봇(AGV)을 물류 환경에서 운용하기 위한 멀티 로봇 조율 시스템이다. 전체 파이프라인은 다음 세 레이어로 구성된다.

```text
┌─────────────────────────────────────────────────┐
│              multibot3 Pipeline                │
│                                                 │
│  [MAPF Layer] -> [SADG Layer] -> [Execution]   │
│                                                 │
│   경로 생성        실행 순서 조율     로봇 제어   │
└─────────────────────────────────────────────────┘
```

| 레이어 | 역할 | 핵심 책임 |
|---|---|---|
| **MAPF Layer** | 충돌 없는 경로 생성 | 유효한 path만 반환 |
| **SADG Layer** | 실행 의존성 관리 | invalid plan 감지, affected set 결정 |
| **Execution Layer** | 실제 로봇 제어 | hold / resume / stop 수행 |

핵심 관계는 단순하다.

- MAPF의 출력은 SADG의 입력이다.
- MAPF는 `valid path generator`여야 한다.
- SADG는 경로의 실행 순서와 재계획 트리거를 담당한다.
- Execution은 실제 hold/stop/resume를 수행한다.

---

## 2. 설계 목표와 범위

### 2.1 설계 목표

이번 재설계의 목표는 다음과 같다.

- `lifelong planning`만 남기고 구조를 단순화한다.
- 일반 그래프 `G = (V, E)`를 그대로 지원한다.
- planner는 `valid path만 반환`하고, 실패 시 명시적인 상태를 반환한다.
- `prioritized fallback`은 제거한다.
- `biconnected` 검증은 runner가 아니라 `library boundary`에서 강제한다.
- replanning은 장기적으로 `affected robot 기반`으로 수행한다.

### 2.2 범위

이 문서에서 정의하는 MAPF Layer의 책임은 다음과 같다.

- planning graph 준비 및 검증
- 초기 lifelong assignment에 대한 경로 생성
- execution 상태 변화에 대한 replanning
- collision / physical validity 보장
- 명시적인 성공 / 실패 상태 반환

다음 항목은 MAPF Layer의 책임 범위 밖으로 둔다.

- affected robot set 계산
- affected replan 실패 후 global replan 승격 여부 결정
- safe stop 진입 여부 결정
- 실제 로봇 제어 명령 전송

### 2.3 Single-Shot 제거

재설계 기준에서 `single-shot planning`은 공개 목표 모드가 아니다.

초기 계획은 `lifelong planning`의 특수한 경우로 취급한다.

- 시각 `t=0`의 fleet snapshot
- 초기 goal assignment 집합
- 그 결과로 생성된 valid plan

즉, “초기 계획”과 “재계획”은 같은 lifelong 인터페이스 위에서 다뤄진다.

---

## 3. MAPF Layer 내부 구조

MAPF Layer 내부는 크게 세 부분으로 나눈다.

```text
MAPF Layer
├── PreparedMap      : 그래프 전처리 및 검증
├── MapfPlanner      : public planning API / planner core 진입점
└── ScenarioRuntime  : replan 정책 및 escalation 담당
```

### 3.1 PreparedMap

`PreparedMap`은 planner가 사용할 planning graph를 보관한다.

역할:
- raw graph 로드
- virtual lock preprocessing 적용
- vertex remap 보관
- biconnected 검증

중요한 점:
- planner는 raw graph가 아니라 `PreparedMap`만 사용한다.
- invalid graph는 이 단계에서 차단한다.

### 3.2 MapfPlanner / Planner Core

`MapfPlanner`는 공개 API entry point이며, 내부적으로 `Planner Core`를 사용한다.

역할:
- graph search 수행
- collision-free candidate 생성
- timeout 전까지 best valid solution 개선
- valid solution 또는 명시적 실패 반환

비역할:
- fallback planner 수행
- global replan 승격 여부 판단
- 시스템 hold/stop 정책 결정

### 3.3 ScenarioRuntime

`ScenarioRuntime`은 planner 바깥의 정책 레이어다.

역할:
- affected replan 호출
- 필요 시 global replan으로 승격
- planner 실패를 상위 레이어가 이해할 수 있는 운영 상태로 변환

예시 정책:

```text
safe hold
-> affected replan
-> if failed, global replan
-> if failed, safe stop
```

### 3.4 SADG와의 역할 분리

역할 분리는 다음과 같이 고정한다.

- `SADG`
  - invalid plan 감지
  - affected robot 집합 계산
  - `ScenarioRuntime`에 replanning 요청

- `ScenarioRuntime`
  - replanning 정책 집행
  - affected -> global escalation 수행

- `Planner Core`
  - 주어진 제약 하에서 path 탐색

이 구조는 “경로 생성”과 “운영 판단”을 분리하기 위한 것이다.

---

## 4. 그래프 모델과 전처리

### 4.1 그래프 모델

multibot3는 grid 전용 모델을 쓰지 않고 일반 무방향 그래프를 사용한다.

```text
G = (V, E)
```

- `vertex`
  - 로봇이 점유할 수 있는 위치
  - 예: 교차점, 대기 지점, 스테이션 앞 위치

- `edge`
  - 두 vertex 사이의 직접 이동 가능 연결
  - 예: 복도 구간, 교차 연결

추가 제약:
- vertex id는 `0`부터 시작하는 contiguous integer여야 한다.
- planning graph는 preprocessing 후 biconnected여야 한다.

### 4.2 Biconnected 요건

PIBT 계열 configuration generator는 biconnected graph에서 reachability 성질이 가장 안정적으로 성립한다.

따라서 재설계 기준에서는 다음을 강제한다.

```python
def prepare_map(raw_graph, options):
    planning_graph = preprocess(raw_graph, options)
    assert is_biconnected(planning_graph)
    return planning_graph
```

핵심은 검증 위치다.

- 기존: runner 수준에서 검사
- 목표: `library boundary`에서 검사

즉, invalid map은 planner 내부로 들어오면 안 된다.

### 4.3 Virtual Lock Vertex

좁은 단일 통행 복도는 bridge chain으로 나타나기 쉽다. 이는 biconnected 조건을 깨뜨리고 실제로는 “용량 1인 공유 자원”에 가깝다.

따라서 복도 전체를 하나의 virtual lock vertex로 collapse한다.

```text
변환 전:
  [Area A] - a - b - c - d - [Area B]

변환 후:
  [Area A] - [V_lock] - [Area B]
```

효과:
- bridge chain 제거
- corridor occupancy를 vertex occupancy로 단순화
- 용량 1 제약을 vertex collision으로 표현 가능
- planner core 수정 없이 전처리 단계에서 해결 가능

### 4.4 Remap 정보

전처리 이후에는 다음 remap 정보가 유지되어야 한다.

- original vertex -> planning vertex
- planning vertex -> backing original vertices

이 정보는 다음 용도로 필요하다.

- visualization
- SADG / execution 디버깅
- virtual lock 내부 의미 복원

---

## 5. 계획 모델과 공개 인터페이스

### 5.1 Fleet Snapshot

planner는 실제 시스템에서 “늘 새로 시작하는 문제”를 풀지 않는다. 대신 특정 시점의 fleet 상태를 입력으로 받는다.

`FleetSnapshot`에는 최소한 다음이 포함된다.

- current planning time
- 각 로봇의 현재 planning vertex
- 각 로봇의 현재 active goal
- 각 로봇의 hold / frozen 상태

### 5.2 Goal Assignment

goal assignment는 다음 역할을 한다.

- 초기 계획 시 각 로봇의 첫 목표 정의
- replanning 시 변경된 목표만 전달

request validity 조건:
- 현재 로봇 위치는 서로 distinct여야 한다.
- 같은 planning 순간에 충돌을 유도하는 malformed assignment는 `INVALID_REQUEST`로 거절한다.

### 5.3 FrozenPlanSet

affected replanning 시 unaffected robot은 재계획 대상이 아니다. 대신 기존 미래 경로를 reservation으로 제공해야 한다.

`FrozenPlanSet`은 다음 정보를 담는다.

- unaffected robot의 현재 시점 이후 plan suffix
- path 종료 뒤 마지막 점유 vertex
- 필요 시 edge usage 정보

### 5.4 공개 인터페이스 초안

```cpp
PreparedMap prepared = PreparedMap::from_graph(raw_graph, preprocessing);
MapfPlanner planner(std::move(prepared), params);

PlanResult initial = planner.plan_initial(InitialPlanningRequest{
    .snapshot = snapshot,
    .assignments = assignments,
    .params_override = std::nullopt,
});

PlanResult replanned = planner.replan_affected(AffectedReplanningRequest{
    .snapshot = snapshot,
    .assignments = updated_assignments,
    .affected_robot_indices = affected_set,
    .frozen = frozen_paths,
    .params_override = std::nullopt,
});
```

구현 메모:
- `PlanningRequest`는 내부 구현 타입으로 숨긴다.
- 외부 공개 요청 타입은 `InitialPlanningRequest`, `AffectedReplanningRequest` 중심으로 유지한다.

### 5.5 출력 형식

출력은 각 로봇의 timestep-indexed vertex sequence다.

```text
P_i = [(vertex_id, timestep), ...]
```

이 형식은 SADG가 직접 소비할 수 있어야 한다.

---

## 6. 목적 함수와 품질 메트릭

### 6.1 목적 함수

재설계에서는 목적 함수를 하나로 고정하지 않는다.

```cpp
enum class ObjectiveType {
    SOC,
    MAKESPAN
};
```

의미:
- `SOC`: 각 로봇의 goal 최초 도달 시각 합
- `MAKESPAN`: 마지막 로봇의 도착 시각

### 6.2 기본 정책

v1 기본값은 다음을 권장한다.

- optimization objective: `SOC`
- result metrics: `SOC`, `makespan` 둘 다 반환

즉, 탐색은 하나의 objective로 수행하더라도 반환 메트릭은 일반적으로 유지한다.

### 6.3 메트릭 구조

```cpp
struct PlanMetrics {
    int soc;
    int makespan;
};
```

---

## 7. Collision 및 Reservation 제약

MAPF Layer는 아래 제약을 만족하지 않는 해를 절대 반환하면 안 된다.

### 7.1 Vertex Collision

같은 시각 `t`에 두 로봇이 같은 vertex를 점유하면 안 된다.

```text
Q_t[i] == Q_t[j]
```

### 7.2 Swap Collision

같은 timestep에 두 로봇이 서로 위치를 바꾸면 안 된다.

```text
Q_t[i]     == Q_{t+1}[j]
Q_t[j]     == Q_{t+1}[i]
Q_t[i]     != Q_{t+1}[i]
```

예시:

```text
robot A: u(t) -> v(t+1)
robot B: v(t) -> u(t+1)
```

### 7.3 Non-Passing Collision

한 로봇이 edge `(u -> v)`를 timestep `t`에 사용했다면, 다른 로봇은 바로 다음 timestep `t+1`에 `(v -> u)`를 사용할 수 없다.

```text
robot A: u(t)   -> v(t+1)
robot B: v(t+1) -> u(t+2)
```

이는 “delayed reverse-edge collision” 규칙이다.

왜 필요한가:
- vertex 충돌과 swap 충돌만으로는 좁은 통로의 물리적 위험을 충분히 표현하지 못한다.
- 실제 AGV는 edge 점유 직후의 역방향 재진입이 안전하지 않을 수 있다.
- SADG에 전달되는 path 자체가 이 제약을 만족해야 downstream이 안전하다.

### 7.4 Physical Edge Validity

반환 path의 각 step은 반드시 다음 둘 중 하나여야 한다.

- 같은 vertex에서 wait
- planning graph의 실제 edge를 따라 이동

### 7.5 Frozen Plan Reservation

unaffected robot의 기존 유효 plan은 reservation table로 변환된다.

필수 예약:
- 미래 timestep별 vertex occupancy
- 미래 timestep별 directed edge usage
- path 종료 후 마지막 vertex의 지속 점유

### 7.6 Hold Reservation

hold 중인 로봇은 “멈춘 로봇”이 아니라 planner 입장에서 reservation obstacle이다.

기본 규칙:
- 현재 vertex를 현재 시각 이후 계속 예약
- 필요 시 직전 move의 reverse edge에 대해 1-step cooldown 예약

예시:

```text
R3가 t=10에 u -> v 이동 후 v에서 hold
-> v는 t>=10 동안 점유
-> reverse edge (v -> u)는 t=11에 금지
```

### 7.7 Virtual Lock 위의 Hold

hold된 로봇이 virtual lock vertex 위에 있으면 corridor 전체가 잠긴 것으로 본다.

즉:
- 해당 `V_lock`은 현재 시점 이후 reservation된다.
- 다른 로봇은 corridor 진입 불가다.

---

## 8. Timeout, 실패 의미, 운영 정책

### 8.1 Prioritized Fallback 제거

재설계에서는 prioritized fallback을 제거한다.

이유:
- planner core의 의미가 흐려진다.
- “응급용 임시 path”와 “정식 valid path”가 섞인다.
- timeout 이후 동작을 planner 내부가 아니라 runtime 정책으로 관리하는 편이 더 명확하다.

### 8.2 Initial Planning의 Timeout 의미

초기 계획에서 timeout이 발생했을 때:

- valid solution을 하나라도 찾았으면 `best-so-far` 반환
- valid solution이 없으면 `TIMEOUT_NO_SOLUTION` 반환

즉, planner는 유효한 incumbent가 있을 때만 timeout-with-solution을 반환할 수 있다.

### 8.3 Replanning의 Timeout 의미

affected replanning에서 timeout이 발생했을 때:

- valid replanning 결과가 있으면 그것을 반환
- 없으면 `TIMEOUT_NO_SOLUTION` 반환

planner는 “이후 global replan을 할지”를 스스로 결정하지 않는다.

### 8.4 실패 상태

```cpp
enum class PlanningStatus {
    SOLVED,
    TIMEOUT_WITH_SOLUTION,
    TIMEOUT_NO_SOLUTION,
    UNSAT,
    INVALID_MAP,
    INVALID_REQUEST,
    SAFE_STOP_REQUIRED
};
```

의미:

- `SOLVED`
  - valid solution 발견

- `TIMEOUT_WITH_SOLUTION`
  - timeout 전에 valid incumbent 발견

- `TIMEOUT_NO_SOLUTION`
  - timeout 전에 valid solution 미발견

- `UNSAT`
  - 현재 제약 하에서 해가 존재하지 않음

- `INVALID_MAP`
  - preprocessing 실패 또는 biconnected 불만족

- `INVALID_REQUEST`
  - malformed snapshot, duplicate occupancy, 잘못된 goal assignment

- `SAFE_STOP_REQUIRED`
  - runtime-level escalation 실패 후 시스템 정지 필요

### 8.5 운영 정책

SADG가 현재 execution plan이 invalid하다고 판단했을 때 권장 정책은 다음과 같다.

```text
safe hold
-> affected replan
-> if failed, global replan
-> if failed, safe stop
```

이 정책에서:
- `SADG`는 invalid 감지와 affected set 계산을 담당
- `ScenarioRuntime`은 escalation을 담당
- `MapfPlanner / Planner Core`는 탐색만 담당

---

## 9. SADG Layer와의 인터페이스

### 9.1 MAPF Output -> SADG Input

MAPF는 각 로봇에 대해 다음 형식의 path를 반환한다.

```text
P_i = [(v0, 0), (v1, 1), (v2, 2), ...]
```

SADG는 이 시퀀스를 직접 dependency analysis의 입력으로 사용한다.

### 9.2 Initial Planning 요청

초기 계획은 lifelong interface의 특수한 경우다.

```python
planner = MapfPlanner(prepared_map, params)
planner.plan_initial(
    InitialPlanningRequest(
        snapshot=fleet_at_t0,
        assignments=initial_goals,
    )
)
```

### 9.3 Affected Replanning 요청

replanning은 전체 fleet을 항상 다시 푸는 것이 아니라, 우선 affected robot만 대상으로 수행한다.

```python
planner.replan_affected(
    AffectedReplanningRequest(
        snapshot=current_fleet_state,
        assignments=new_assignments,
        affected_robot_indices=affected_set,
        frozen=frozen_paths,
    )
)
```

### 9.4 SADG와 MAPF의 책임 경계

다음 경계를 명확히 유지한다.

- `SADG`
  - invalid/stale plan 감지
  - affected set 계산

- `ScenarioRuntime`
  - affected replan 호출
  - global replan 승격
  - safe stop 필요 여부 보고

- `MapfPlanner / Planner Core`
  - 순수 탐색

---

## 10. 설계 결정 요약

| 결정 사항 | 선택 | 이유 |
|---|---|---|
| 공개 planning 모드 | Lifelong only | 초기 계획과 재계획을 하나의 모델로 통합 |
| planner core | `MapfPlanner` + LaCAM 기반 core | 공개 API와 탐색 엔진 분리 |
| fallback | 제거 | valid-path-only 원칙 유지 |
| 그래프 모델 | 일반 그래프 `G=(V,E)` | 창고의 비정형 구조 표현 |
| 그래프 검증 위치 | library boundary | invalid map 차단 |
| narrow corridor 처리 | virtual lock vertex | biconnected 복원 + 용량 1 모델링 |
| replanning 단위 | affected robot 우선 | 불필요한 global replan 감소 |
| unaffected robot 처리 | frozen reservation | moving obstacle로 모델링 |
| 목적 함수 | objective 옵션화 | `SOC`, `makespan` 모두 지원 |
| 반환 원칙 | valid path 또는 명시적 실패 | 시스템 의미 명확화 |

---

## 11. 미해결 과제 및 향후 확장

### 11.1 Affected Set 품질

affected robot 집합을 너무 작게 잡으면 replan 실패가 잦아지고, 너무 크게 잡으면 사실상 global replan과 다를 바 없어진다. SADG 측 기준 정교화가 필요하다.

### 11.2 UNSAT 판정 기준

timeout과 진짜 infeasible request를 어떻게 구분할지 구현 정책이 더 필요하다. v1에서는 보수적으로 `TIMEOUT_NO_SOLUTION` 위주로 시작할 수 있다.

### 11.3 Hold Reservation의 보수성

hold vertex의 indefinite reservation은 안전하지만 보수적이다. 향후 time window 기반 완화 정책을 검토할 수 있다.

### 11.4 Objective별 탐색 전략

현재는 objective를 옵션화했지만, 실제로 `SOC`와 `makespan` 각각에 대해 heuristic과 pruning 전략을 다르게 가져갈 필요가 있다.

### 11.5 Non-Biconnected 장기 지원

장기적으로는 non-biconnected graph도 직접 지원할 수 있어야 한다. 다만 v1 목표는 아니다.

### 11.6 공유 YAML 스키마

팀 환경에서 `.vscode/settings.json`에 의존하지 않기 위해, lifelong scenario YAML은 repo 안의 공유 스키마를 직접 가리키는 방식을 사용한다.

- 스키마 파일: `mapf/schema/lifelong-scenario.schema.json`
- 각 scenario YAML 상단:

```yaml
# yaml-language-server: $schema=../../../schema/lifelong-scenario.schema.json
```

---

## 12. 참고 문헌

| 논문 | 내용 |
|---|---|
| Okumura et al. (2022). *PIBT: Priority Inheritance with Backtracking*. AIJ. | PIBT 알고리즘 |
| Okumura (2023). *LaCAM: Search-Based Algorithm for Quick MAPF*. AAAI. | LaCAM 알고리즘 |
| Okumura (2023). *Improving LaCAM for Scalable Eventually Optimal MAPF*. IJCAI. | LaCAM★ 알고리즘 |
| Okumura (2024). *Engineering LaCAM★*. AAMAS. | LaCAM★ 엔지니어링 최적화 |
| Berndt et al. (2024). *Receding Horizon Re-Ordering of Multi-Agent Execution Schedules*. TRO. | SADG / SE-ADG |

---

문서 최종 수정: 2026-04-05
