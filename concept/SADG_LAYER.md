# multibot3 - SADG Layer 설계 문서

> 작성 목적: multibot3 프로젝트의 SADG(Switchable Action Dependency Graph) 레이어 설계 기준을 정의한다.  
> 대상 독자: 프로젝트 개발자, 리뷰어, 향후 SADG 구현 담당자

---

## 목차

1. [시스템 내 역할](#1-시스템-내-역할)
2. [설계 목표와 범위](#2-설계-목표와-범위)
3. [SADG의 핵심 책임](#3-sadg의-핵심-책임)
4. [데이터 모델](#4-데이터-모델)
5. [Dependency Graph 모델](#5-dependency-graph-모델)
6. [실행 상태 모델](#6-실행-상태-모델)
7. [Invalid / Stale Plan 판정 규칙](#7-invalid--stale-plan-판정-규칙)
8. [Affected Set 계산 규칙](#8-affected-set-계산-규칙)
9. [MAPF Layer와의 인터페이스](#9-mapf-layer와의-인터페이스)
10. [Execution Layer와의 인터페이스](#10-execution-layer와의-인터페이스)
11. [재계획 반영 절차](#11-재계획-반영-절차)
12. [권장 구현 구조](#12-권장-구현-구조)
13. [미해결 과제](#13-미해결-과제)

---

## 1. 시스템 내 역할

multibot3의 전체 파이프라인은 다음과 같다.

```text
[MAPF Layer] -> [SADG Layer] -> [Execution Layer]
```

각 레이어의 기본 책임은 다음과 같이 고정한다.

| 레이어 | 역할 | 핵심 책임 |
|---|---|---|
| MAPF | 경로 생성 | collision-free path 반환 |
| SADG | 실행 조율 | dependency 관리, invalid 감지, affected set 계산 |
| Execution | 로봇 제어 | move / hold / resume / stop 수행 |

SADG는 이 세 레이어 사이의 중간 오케스트레이터다.

- MAPF가 만든 계획을 실행 가능한 dependency 구조로 바꾼다.
- 실행 중 실제 상태와 계획의 차이를 감지한다.
- 재계획이 필요할 때 affected set을 계산한다.
- Execution에는 지금 어떤 로봇이 움직여도 되는지를 알려준다.

---

## 2. 설계 목표와 범위

### 2.1 목표

SADG 레이어의 목표는 다음과 같다.

- MAPF의 timestep plan을 execution-friendly dependency graph로 변환한다.
- 실행 중 plan invalidation을 빠르게 감지한다.
- 재계획 범위를 `affected robot 중심`으로 최소화한다.
- 정상 실행 중에는 불필요한 global hold를 줄인다.
- safe-stop이 필요한 상황을 명확히 상위 시스템에 전달한다.

### 2.2 범위

SADG가 담당하는 범위는 다음과 같다.

- MAPF plan을 dependency graph로 구성
- action readiness 계산
- 실제 로봇 상태와 계획의 비교
- invalid / stale plan 판정
- affected robot set 계산
- replanning 요청 생성
- replan 결과를 현재 실행 상태에 merge

다음 항목은 SADG의 범위 밖이다.

- 충돌 없는 path 자체를 찾는 것
- affected replan 실패 후 global replan 승격 정책 결정
- low-level motor control
- localization / perception 알고리즘 자체

즉, SADG는 `planner`가 아니라 `execution coordinator`다.

---

## 3. SADG의 핵심 책임

SADG의 책임은 네 묶음으로 볼 수 있다.

### 3.1 계획 구조화

MAPF가 반환한 각 로봇 path:

```text
P_i = [(v0, 0), (v1, 1), (v2, 2), ...]
```

를 action 단위로 분해하고, 로봇 간 dependency를 만든다.

### 3.2 실행 가능성 판단

현재 시점에서:

- 어떤 action이 이미 완료되었는지
- 어떤 action이 아직 선행 조건을 기다리는지
- 어떤 로봇이 움직여도 안전한지

를 계산한다.

### 3.3 이상 상황 감지

다음을 감지해야 한다.

- 로봇이 예상보다 늦게 도달하거나 오래 정체됨
- 로봇이 계획과 다른 위치에 존재함
- 외부 hold / estop 때문에 계획이 오래 정지됨
- goal 변경으로 기존 suffix가 stale이 됨

### 3.4 재계획 범위 계산

invalid가 발생하면:

- seed robot을 결정하고
- dependency graph를 따라 영향 범위를 확장하고
- unaffected robot의 유효 suffix는 frozen reservation으로 남긴다

---

## 4. 데이터 모델

SADG는 최소한 다음 데이터를 가져야 한다.

### 4.1 SADG 입력

```cpp
struct SadgPlanInput {
    FleetSnapshot snapshot;
    Solution solution;
    std::vector<GoalAssignment> assignments;
    int base_timestep;
};
```

의미:

- `snapshot`
  - 현재 시각과 로봇 상태
- `solution`
  - MAPF가 반환한 timestep-indexed plan
- `assignments`
  - 각 로봇의 현재 목표
- `base_timestep`
  - 이 plan이 시작되는 절대 시각

### 4.2 Robot Execution State

```cpp
enum class RobotExecState {
    READY,
    WAITING_DEPENDENCY,
    EXECUTING,
    HELD,
    IDLE_AT_GOAL,
    SAFE_STOP
};
```

각 로봇에 대해 다음이 필요하다.

```cpp
struct RobotSadgState {
    std::string robot_id;
    int current_vertex;
    std::optional<int> current_goal;
    int committed_action_index;
    RobotExecState state;
    bool externally_held;
    bool faulted;
};
```

### 4.3 Action Node

SADG의 기본 노드는 `한 timestep 이동 또는 대기 action`이다.

```cpp
struct SadgActionNode {
    std::string node_id;
    int robot_index;
    int action_index;
    int from_vertex;
    int to_vertex;
    int start_timestep;
    int end_timestep;
    bool is_wait;
};
```

이 문서에서는 `path point`가 아니라 `transition`을 노드로 사용한다.

이유:

- Execution은 “다음 vertex로 이동”이라는 action 단위로 명령을 받기 쉽다.
- dependency도 “누가 누구보다 먼저 지나가야 하는가”로 표현하기 쉽다.
- wait도 동일한 형식으로 다룰 수 있다.

### 4.4 Dependency Edge

```cpp
enum class SadgEdgeType {
    INTRA_ROBOT_ORDER,
    VERTEX_RELEASE,
    EDGE_ORDER,
    HOLD_BARRIER
};

struct SadgDependencyEdge {
    std::string edge_id;
    std::string from_node_id;
    std::string to_node_id;
    SadgEdgeType type;
};
```

---

## 5. Dependency Graph 모델

### 5.1 노드 생성

각 로봇 path:

```text
[(v0,0), (v1,1), (v2,2), ... , (vn,n)]
```

에 대해 다음 node를 만든다.

```text
a0: v0 -> v1 @ [0,1]
a1: v1 -> v2 @ [1,2]
...
```

`vk == v(k+1)`이면 wait node다.

### 5.2 Intra-Robot Order Edge

같은 로봇의 action은 순서를 지켜야 한다.

```text
a(i,k) -> a(i,k+1)
```

이 edge는 항상 존재한다.

### 5.3 Vertex Release Edge

어떤 로봇 B가 vertex `v`에 진입하기 전에, 기존 점유 로봇 A가 `v`를 비워야 한다.

예:

```text
A: v -> x @ [t, t+1]
B: u -> v @ [t, t+1] 또는 [t+1, t+2]
```

계획에서 정해진 순서를 기준으로 `A release -> B enter` edge를 만든다.

의미:
- B는 A가 관련 action을 완료하기 전까지 dispatch되면 안 된다.

### 5.4 Edge Order Edge

swap 또는 non-passing을 막기 위한 edge다.

예:

```text
A: u -> v
B: v -> u
```

계획이 허용한 방향/순서를 기준으로 한쪽 action이 다른 action보다 먼저 끝나야 한다.

이 edge는 특히 좁은 corridor와 virtual lock에서 중요하다.

### 5.5 Hold Barrier Edge

로봇이 외부 hold 상태거나 safety barrier 안에 있으면, 해당 로봇의 다음 action은 임시 barrier에 막힌다.

이는 정적 계획 edge가 아니라 runtime edge다.

### 5.6 그래프 불변식

SADG는 다음을 유지해야 한다.

- 완료된 action의 선행 조건은 모두 완료 상태여야 한다.
- `READY` action은 모든 predecessor가 완료된 상태여야 한다.
- 같은 로봇에서 두 개 이상의 action이 동시에 `EXECUTING`이면 안 된다.
- replanning 이후 기존 무효 suffix를 가리키는 node/edge는 남기지 않는다.

---

## 6. 실행 상태 모델

### 6.1 Action State

```cpp
enum class ActionExecState {
    PENDING,
    READY,
    DISPATCHED,
    DONE,
    BLOCKED,
    CANCELLED
};
```

권장 전이는 다음과 같다.

```text
PENDING -> READY -> DISPATCHED -> DONE
PENDING/READY -> BLOCKED
PENDING/READY/DISPATCHED -> CANCELLED
```

### 6.2 Robot State

로봇 상태는 action 상태를 요약한다.

- `READY`
  - 즉시 다음 action dispatch 가능
- `WAITING_DEPENDENCY`
  - 선행 dependency 때문에 대기
- `EXECUTING`
  - 현재 action 수행 중
- `HELD`
  - 외부 또는 SADG 요청으로 정지
- `IDLE_AT_GOAL`
  - 현재 goal 완료 후 대기
- `SAFE_STOP`
  - 시스템 차원의 정지 상태

### 6.3 Dispatch 규칙

로봇의 다음 action은 다음 조건을 모두 만족할 때만 dispatch한다.

- 해당 action state가 `READY`
- 로봇 state가 `HELD`가 아님
- Execution Layer가 새 command를 받을 수 있음
- 같은 로봇의 이전 action이 `DONE`

### 6.4 Action 상태기계

권장 action 상태기계는 다음과 같다.

```text
PENDING
  -> READY        : 모든 predecessor 완료
  -> CANCELLED    : replanning 또는 safe stop으로 suffix 폐기

READY
  -> DISPATCHED   : SADG가 action command 전송
  -> BLOCKED      : 외부 hold / safety barrier / execution unavailable
  -> CANCELLED    : replanning 또는 safe stop

BLOCKED
  -> READY        : barrier 해제, dependency 여전히 만족
  -> CANCELLED    : replanning 또는 safe stop

DISPATCHED
  -> DONE         : ACTION_DONE 또는 위치/상태 확인으로 완료 판정
  -> CANCELLED    : estop / hard abort / 명시적 실행 중단

DONE
  -> terminal

CANCELLED
  -> terminal
```

중요한 해석은 다음과 같다.

- `PENDING`
  - 아직 dependency가 풀리지 않아 실행 자격이 없는 상태
- `READY`
  - dependency는 모두 만족했고, dispatch만 남은 상태
- `BLOCKED`
  - 논리적으로는 거의 준비됐지만 runtime barrier 때문에 못 나가는 상태
- `DISPATCHED`
  - Execution에 이미 명령을 넘겼고 완료 feedback을 기다리는 상태
- `DONE`
  - action 완료가 SADG에 의해 확정된 상태
- `CANCELLED`
  - replanning 또는 safe stop으로 더 이상 유효하지 않은 상태

실무적으로는 `PENDING`과 `BLOCKED`를 구분하는 것이 중요하다.

- `PENDING`
  - graph dependency가 원인
- `BLOCKED`
  - runtime barrier가 원인

### 6.5 Action 상태 전이표

| 현재 상태 | 입력 이벤트 | 다음 상태 | 의미 |
|---|---|---|---|
| `PENDING` | predecessor 전부 완료 | `READY` | 이제 dispatch 후보 |
| `PENDING` | replanning patch | `CANCELLED` | 기존 suffix 폐기 |
| `READY` | dispatch 성공 | `DISPATCHED` | command 전달 완료 |
| `READY` | hold / barrier 발생 | `BLOCKED` | 안전상 일시 중단 |
| `READY` | replanning patch | `CANCELLED` | future action 교체 |
| `BLOCKED` | hold 해제 / barrier 해제 | `READY` | 다시 dispatch 가능 |
| `BLOCKED` | replanning patch | `CANCELLED` | 기존 future action 무효 |
| `DISPATCHED` | action done feedback | `DONE` | 실행 완료 |
| `DISPATCHED` | hard abort / estop | `CANCELLED` | 실행 강제 종료 |

### 6.6 Robot 상태기계

로봇 상태는 “현재 action이 무엇 때문에 멈춰 있는가”를 상위 레벨에서 요약한다.

```text
IDLE_AT_GOAL
  -> READY                : 새 goal 또는 다음 suffix 생성

READY
  -> EXECUTING            : READY action dispatch
  -> WAITING_DEPENDENCY   : 다음 action이 아직 PENDING
  -> HELD                 : hold 요청 또는 barrier
  -> SAFE_STOP            : system safe stop

WAITING_DEPENDENCY
  -> READY                : 다음 action이 READY가 됨
  -> HELD                 : hold 요청 또는 barrier
  -> SAFE_STOP            : system safe stop

EXECUTING
  -> READY                : 현재 action 완료 후 다음 action이 READY
  -> WAITING_DEPENDENCY   : 현재 action 완료 후 다음 action이 아직 PENDING
  -> IDLE_AT_GOAL         : goal 도달 후 추가 action 없음
  -> HELD                 : 외부 hold 또는 fault로 정지
  -> SAFE_STOP            : system safe stop

HELD
  -> READY                : hold 해제 후 다음 action READY
  -> WAITING_DEPENDENCY   : hold 해제 후 dependency 대기
  -> SAFE_STOP            : safe stop 승격

SAFE_STOP
  -> terminal until operator recovery
```

### 6.7 Robot 상태 전이표

| 현재 상태 | 입력 이벤트 | 다음 상태 | 의미 |
|---|---|---|---|
| `IDLE_AT_GOAL` | 새 goal 할당 | `READY` | 새 suffix 시작 |
| `READY` | action dispatch | `EXECUTING` | 현재 action 수행 시작 |
| `READY` | dependency 재등장 / patch | `WAITING_DEPENDENCY` | 다음 action 대기 |
| `READY` | hold 요청 | `HELD` | 명시적 정지 |
| `WAITING_DEPENDENCY` | predecessor 완료 | `READY` | 다음 action 활성화 |
| `WAITING_DEPENDENCY` | hold 요청 | `HELD` | 외부 정지 |
| `EXECUTING` | action done + next ready | `READY` | 연속 진행 가능 |
| `EXECUTING` | action done + next pending | `WAITING_DEPENDENCY` | 후속 dependency 대기 |
| `EXECUTING` | goal 도달 | `IDLE_AT_GOAL` | 현재 할당 종료 |
| `EXECUTING` | fault / estop / hold | `HELD` | 안전 정지 진입 |
| `HELD` | hold 해제 + next ready | `READY` | 재개 가능 |
| `HELD` | hold 해제 + next pending | `WAITING_DEPENDENCY` | dependency 복귀 |
| `*` | safe stop 선언 | `SAFE_STOP` | 시스템 정지 |

### 6.8 Feedback 기반 상태 갱신 규칙

SADG는 다음 feedback을 기준으로 상태를 갱신하는 것을 권장한다.

- `POSITION_UPDATE`
  - 현재 robot vertex 갱신
  - path prefix 양립 여부 확인
  - stale / invalid 판정 보조
- `ACTION_DONE`
  - 현재 `DISPATCHED` action을 `DONE`으로 전이
  - 다음 action의 dependency 해제 여부 재평가
- `HOLD_ACK`
  - robot state를 `HELD`로 확정
- `FAULT`
  - robot state를 `HELD` 또는 `SAFE_STOP` 후보로 전이
  - invalid/stale 분류 재평가
- `LOCALIZATION_LOST`
  - 위치 신뢰성 상실로 간주
  - 보수적으로 `invalid` 또는 즉시 replan 후보

### 6.9 권장 SADG Runtime Tick

SADG는 event-driven으로 구현하되, 내부적으로 다음 순서를 반복하는 tick 형태로 생각하면 정리가 쉽다.

```text
1. execution feedback 반영
2. action / robot state 갱신
3. invalid / stale 분류
4. 필요 시 hold 및 affected set 계산
5. replanning 요청 여부 판단
6. READY action dispatch
```

이 순서를 유지하면 “feedback 처리”, “재계획 판단”, “dispatch”가 한 루프 안에서 섞여도 의미가 분명해진다.

---

## 7. Invalid / Stale Plan 판정 규칙

SADG가 감지해야 하는 이상 상황은 최소한 다음과 같다.

중요한 원칙은 다음과 같다.

- 계획 timestep은 `nominal schedule`이다.
- dependency 순서와 resource safety는 `hard constraint`다.
- 단순 지연은 기본적으로 invalid가 아니다.
- 다만 지연이 누적되어 future suffix를 더 신뢰하기 어려우면 stale로 승격할 수 있다.

즉 SADG는 `시간 오차`와 `계획 무효`를 구분해야 한다.

### 7.1 위치 이탈

실제 로봇 위치가 계획 prefix와 일치하지 않으면 invalid다.

예:

- 계획상 `t=15`에 vertex `6`이어야 하는데 실제는 `7`
- 또는 계획에 존재하지 않는 vertex에 있음

이 규칙의 핵심은 “늦었다”가 아니라 “현재 상태가 기존 prefix와 양립 가능한가”다.

### 7.2 진행 지연과 Stale 판정

action이 `expected_end_time + slack`을 넘겨도 완료되지 않았다고 해서 곧바로 invalid는 아니다.

다음 경우는 `timing deviation` 또는 `stale`로 취급한다.

- 로봇이 아직 계획된 경로 위에 있음
- 선행 / 후행 dependency를 어기지 않음
- shared resource 순서를 깨지 않음
- 다른 로봇은 추가 대기로 안전하게 흡수 가능함

즉:

- `late but order-preserving`
  - 계속 실행 가능
- `late beyond slack but still safe`
  - stale warning
  - 필요 시 affected replanning 후보
- `late and order/resource-breaking`
  - invalid

지연은 기본적으로 soft deviation이고, 순서/자원 위반이 hard failure다.

권장 파라미터:

```cpp
struct SadgTimingPolicy {
    int completion_slack_ms;
    int dependency_wait_timeout_ms;
};
```

### 7.3 순서 위반

선행 dependency가 완료되지 않았는데 후행 action이 먼저 실행되면 invalid다.

예:

- release edge가 끝나기 전에 뒤 로봇이 corridor로 진입

### 7.4 외부 강제 정지

다음 상황은 replanning 후보가 된다.

- estop
- manual hold
- localization lost
- hardware fault

이 경우 SADG는 단순히 graph violation만 볼 것이 아니라, 실행 가능성 자체가 깨졌다고 판단해야 한다.

### 7.5 goal 변경

goal assignment가 바뀌면 기존 future suffix는 stale이다.

이건 충돌이 없어도 replan 대상이 된다.

### 7.6 환경 변화

맵 상의 특정 vertex / edge가 일시적으로 사용 불가가 되면, 이를 지나야 하는 suffix는 stale이다.

장애물 감지, 작업 구역 점유, 안전 구역 폐쇄 등이 여기에 해당한다.

### 7.7 Stale에서 Replan으로 승격되는 조건

모든 stale 상황이 즉시 replanning을 요구하는 것은 아니다.

권장 해석은 다음과 같다.

- `timing deviation`
  - 관측만 하고 계속 실행
- `stale`
  - suffix 신뢰도 저하
  - 계속 실행 가능할 수도 있음
- `replan-required stale`
  - 더 이상 기존 suffix를 유지하는 것이 비효율적이거나 위험함
  - affected replanning 요청

다음 조건 중 하나라도 만족하면 stale을 `replan-required`로 승격하는 것을 권장한다.

- 같은 로봇의 stale 상태가 `completion_slack_ms`를 넘긴 뒤에도 추가 관측 window 동안 해소되지 않음
- 해당 로봇 때문에 후행 dependency robot이 `dependency_wait_timeout_ms` 이상 지속적으로 block됨
- stale robot이 corridor / virtual lock / gateway 같은 shared resource를 점유하고 있어 다른 로봇의 진행을 연쇄적으로 막고 있음
- 현재 stale 상태 때문에 기존 future suffix가 reservation 관점에서 더 이상 신뢰되지 않음
- operator 또는 상위 safety 시스템이 replanning을 명시적으로 요구함

반대로 다음 조건이면 stale로 남겨두고 계속 실행하는 것이 더 적절하다.

- 로봇이 여전히 계획 경로 위에 있음
- 선행 / 후행 dependency 위반이 없음
- 다른 로봇은 단순 추가 대기로 안전하게 흡수 가능함
- shared bottleneck 자원을 장시간 점유하지 않음

권장 파라미터는 다음과 같다.

```cpp
struct SadgReplanTriggerPolicy {
    int stale_observation_window_ms;
    int dependency_block_escalation_ms;
    bool replan_on_goal_change = true;
    bool replan_on_resource_unavailable = true;
};
```

---

## 8. Affected Set 계산 규칙

affected set은 너무 작아도, 너무 커도 안 된다.

### 8.1 Seed Set

처음 seed로 들어갈 수 있는 로봇은 다음과 같다.

- invalid 위치 이탈을 일으킨 로봇
- slack 초과로 stale 판정을 받은 로봇
- 새 goal이 할당된 로봇
- 외부 fault/hold를 받은 로봇
- 사용 불가 vertex/edge를 앞으로 지나가야 하는 로봇

### 8.2 1차 확장

seed robot과 직접 dependency로 연결된 로봇을 추가한다.

예:

- seed의 선행 robot
- seed의 후행 robot
- 같은 corridor / virtual lock resource를 공유하는 robot

### 8.3 2차 확장

다음 조건이면 영향을 더 전파한다.

- frozen suffix가 seed와 시간적으로 겹침
- seed가 빠지지 않으면 후행 robot이 영원히 진행 불가
- 같은 gateway / lock exit를 공유

### 8.4 확장 중단 조건

다음이면 확장을 멈출 수 있다.

- 해당 robot의 future suffix가 현재 시각 이후에도 여전히 valid
- seed와 shared resource가 없음
- dependency graph 상 연결이 없음

### 8.5 권장 알고리즘

실용적인 v1 알고리즘은 다음과 같다.

```text
seed robots
-> dependency edge BFS/DFS expansion
-> shared resource overlap expansion
-> time-window closure
-> affected set
```

### 8.6 출력

최종적으로 SADG는 다음을 만들어야 한다.

```cpp
struct SadgReplanIntent {
    FleetSnapshot snapshot;
    std::vector<GoalAssignment> assignments;
    std::vector<int> affected_robot_indices;
    FrozenPlanSet frozen;
    std::string reason;
};
```

---

## 9. MAPF Layer와의 인터페이스

현재 MAPF public API는 `MapfPlanner` 중심이다.

### 9.1 초기 계획

```cpp
PlanResult result = planner.plan_initial(InitialPlanRequest{
    .snapshot = snapshot,
    .assignments = assignments,
    .params_override = std::nullopt,
});
```

SADG는 이 결과를 받아 initial dependency graph를 만든다.

### 9.2 affected replanning

```cpp
PlanResult result = planner.replan_affected(AffectedReplanRequest{
    .snapshot = snapshot,
    .assignments = assignments,
    .affected_robot_indices = affected_set,
    .frozen = frozen,
    .params_override = std::nullopt,
});
```

### 9.3 SADG가 준비해야 하는 입력

SADG는 MAPF 호출 전에 다음을 준비해야 한다.

- 현재 `FleetSnapshot`
- 현재/변경된 `GoalAssignment`
- `affected_robot_indices`
- unaffected robot의 future valid suffix로부터 만든 `FrozenPlanSet`

### 9.4 MAPF 호출 이후 SADG가 할 일

MAPF가 새 valid plan을 반환하면:

1. affected robot의 old suffix를 취소
2. new suffix를 절대 시각 기준으로 patch
3. 관련 node/edge만 다시 생성
4. unaffected robot의 완료 상태는 유지

---

## 10. Execution Layer와의 인터페이스

Execution Layer와는 최소한 다음 계약이 필요하다.

### 10.1 SADG -> Execution

```cpp
enum class ExecutionCommandType {
    MOVE_TO_VERTEX,
    HOLD_POSITION,
    RESUME,
    SAFE_STOP
};
```

```cpp
struct ExecutionCommand {
    std::string robot_id;
    ExecutionCommandType type;
    int target_vertex;
    std::string reason;
};
```

### 10.2 Execution -> SADG

```cpp
enum class ExecutionFeedbackType {
    ACTION_DONE,
    HOLD_ACK,
    POSITION_UPDATE,
    FAULT,
    LOCALIZATION_LOST
};
```

```cpp
struct ExecutionFeedback {
    std::string robot_id;
    ExecutionFeedbackType type;
    int actual_vertex;
    int timestamp;
    std::string detail;
};
```

### 10.3 실행 원칙

- SADG는 `READY` action만 dispatch한다.
- Execution은 action 완료 또는 fault를 반드시 feedback으로 올린다.
- feedback이 늦거나 누락되면 SADG는 stale plan 가능성을 판단해야 한다.

---

## 11. 재계획 반영 절차

권장 절차는 다음과 같다.

### 11.1 invalid / stale 감지와 분류

- 위치 이탈
- fault
- slack 초과 지연
- goal 변경
- 환경 변화

이 단계에서 SADG는 이를 곧바로 동일하게 처리하지 않고 다음으로 분류한다.

- `timing deviation`
  - 계속 실행
- `stale`
  - 모니터링 또는 조건부 replan 후보
- `invalid`
  - 즉시 hold + replan 후보

### 11.2 임시 안전 조치

- seed robot 우선 hold
- 필요 시 직접 충돌 가능성이 있는 neighbor robot도 hold

### 11.3 affected set 계산

- dependency graph 확장
- shared resource 확장
- time-window closure

### 11.4 replan 요청 생성

- snapshot 생성
- affected / unaffected 분리
- frozen suffix 생성
- MAPF runtime 또는 planner에 요청

### 11.5 결과 처리

- 성공: 새 suffix patch 후 dependency graph 갱신
- 실패: runtime status를 확인
  - global replan 결과 반영
  - `SAFE_STOP_REQUIRED`면 모든 robot을 safe stop 상태로 전이

### 11.6 권장 흐름

```text
execution feedback
-> SADG invalid/stale classification
-> seed hold
-> affected set expansion
-> MAPF runtime replan request
-> valid plan patch or safe stop
```

---

## 12. 권장 구현 구조

향후 구현 시 다음 구성으로 나누는 것을 권장한다.

```text
sadg/
├── sadg_types.hpp/.cpp
├── sadg_graph.hpp/.cpp
├── sadg_builder.hpp/.cpp
├── sadg_scheduler.hpp/.cpp
├── sadg_validator.hpp/.cpp
├── affected_set_solver.hpp/.cpp
└── sadg_runtime.hpp/.cpp
```

역할은 다음과 같다.

- `sadg_types`
  - 공용 enum / struct
- `sadg_graph`
  - node / edge / index 자료구조
- `sadg_builder`
  - MAPF solution -> SADG graph 변환
- `sadg_scheduler`
  - READY action 계산 및 dispatch
- `sadg_validator`
  - invalid / stale plan 판정
- `affected_set_solver`
  - seed expansion 및 frozen set 생성
- `sadg_runtime`
  - Execution feedback 처리와 MAPF 재계획 요청 orchestration

---

## 13. 미해결 과제

### 13.1 Continuous-Time 대응

현재 MAPF 출력은 discrete timestep 기준이다. SADG v1에서는 이를 유지하되, 실행 해석만 continuous-time 오차를 흡수하는 방식이 현실적이다.

권장 기본 가정은 다음과 같다.

- SADG의 scheduling quantum은 `dt = 1.0s`
- timestep은 exact trajectory time이 아니라 `nominal relation index`
- dependency 순서와 shared resource 점유 관계가 시간 정밀도보다 우선한다

즉 v1의 핵심은 `continuous-time planner`가 아니라 `continuous-aware SADG`다.

이 선택의 이유는 다음과 같다.

- 너무 작은 `dt`는 relation graph를 quasi-trajectory graph로 만들어 SADG의 장점을 약화시킨다
- 작은 오차에도 stale 판정이 많아져 false replan이 늘어난다
- corridor / virtual lock처럼 relation이 중요한 구간은 정확한 sub-second 추정보다 점유 순서가 더 중요하다

따라서 v1에서는 `dt = 1.0s`를 기준으로 다음처럼 단순하게 시작하는 것이 좋다.

```cpp
struct SadgTimingProfile {
    int duration_ticks;
    int slack_ticks;
};
```

권장 기본 프로파일:

- `WAIT`
  - `duration_ticks = 1`
  - `slack_ticks = 0 또는 1`
- `NORMAL_MOVE`
  - `duration_ticks = 1`
  - `slack_ticks = 1`
- `BOTTLENECK_MOVE`
  - `duration_ticks = 2`
  - `slack_ticks = 1`
- `LOCK_OCCUPANCY`
  - `duration_ticks = 2 ~ 3`
  - `slack_ticks = 1 ~ 2`

의미는 다음과 같다.

- 일반 open-area edge 이동은 1초 단위 relation으로 취급
- corridor / gateway / virtual lock은 더 긴 점유 action으로 취급
- exact completion time은 feedback timestamp로 추적하되, SADG graph 자체는 coarse-grained relation graph로 유지

예를 들면 corridor에서:

- 일반 외곽 이동
  - `NORMAL_MOVE`
- lock 진입 또는 lock 점유
  - `LOCK_OCCUPANCY`
- lock 탈출 직후 gateway 정리
  - `BOTTLENECK_MOVE`

warehouse에서는:

- 선반 사이 open segment
  - `NORMAL_MOVE`
- 교차점 또는 병목 통로
  - `BOTTLENECK_MOVE`

이 접근은 다음 장점이 있다.

- SADG graph 크기를 작게 유지
- robot relation 중심 reasoning 유지
- execution delay를 `slack`과 feedback window로 흡수
- later phase에서 duration profile만 세분화해도 확장 가능

장기적으로는 edge length, nominal speed, turn cost를 사용해 `duration_ticks`를 더 정교하게 산출할 수 있다. 하지만 v1에서는 고정 `dt = 1.0s`와 소수의 timing profile만으로도 충분한 시작점이 된다.

### 13.2 Affected Set 최소성

affected set 최소성 문제의 핵심은 다음 질문이다.

```text
누구까지 같이 replan에 넣어야 충분하고,
누구부터는 frozen suffix로 남겨도 되는가?
```

이 문제는 exact optimum을 찾는 것보다, `충분히 작고 충분히 성공률 높은 집합`을 찾는 heuristic 문제에 가깝다.

너무 작은 affected set은 다음 문제를 만든다.

- 실제로 필요한 shared resource 해제가 안 됨
- frozen robot이 gateway / corridor / lock 출구를 막음
- planner가 `no solution` 또는 timeout으로 끝남

너무 큰 affected set은 다음 문제를 만든다.

- planning 비용이 급격히 커짐
- frozen reservation의 장점이 사라짐
- 결과적으로 global replan과 차이가 거의 없어짐

따라서 SADG는 “절대 최소 집합”보다 “실패하지 않을 만큼 충분히 작은 집합”을 찾는 방향이 현실적이다.

권장 해석은 다음 3개 closure의 조합이다.

#### 13.2.1 Dependency Closure

seed robot과 직접 dependency로 연결된 robot을 포함한다.

- direct predecessor
- direct successor
- release edge로 연결된 robot
- edge-order / swap-order 관계 robot

#### 13.2.2 Resource Closure

seed와 같은 병목 자원을 공유하는 robot을 포함한다.

- corridor / virtual lock
- narrow gateway
- intersection bottleneck
- station approach zone

즉 geometry distance보다 `shared resource overlap`이 더 중요하다.

#### 13.2.3 Time-Window Closure

dependency나 resource가 연결되어 있어도, 현재 replan horizon과 충분히 멀리 떨어져 있으면 이번 affected set에 꼭 넣을 필요는 없다.

따라서:

- 현재 시각 이후 일정 horizon 안에 실제 간섭이 생기는 robot만 포함
- horizon 밖의 robot은 우선 frozen suffix로 유지

이 closure가 없으면 dependency graph를 따라 너무 넓게 퍼져 사실상 global replan이 되기 쉽다.

권장 v1 알고리즘은 다음과 같다.

```text
seed set
-> dependency closure
-> resource closure
-> time-window closure
-> candidate affected set
```

seed set의 예:

- 위치 이탈 / invalid를 일으킨 robot
- stale가 replan-required로 승격된 robot
- goal 변경 robot
- 외부 hold / fault robot
- 사용 불가 vertex/edge를 곧 통과해야 하는 robot

실무적으로는 다음 stop rule이 필요하다.

- 해당 robot의 future suffix가 여전히 valid
- seed와 shared resource가 없음
- 현재 horizon 안에서 dependency 충돌이 없음

그리고 실패 시에는 확장 기반 escalation이 필요하다.

```text
small affected set
-> if failed, expand 1-hop
-> if failed, expand shared bottleneck neighborhood
-> if failed, global replan
```

맵 구조에 따라 heuristic은 달라져야 한다.

- corridor
  - dependency chain이 길고 resource coupling이 강함
  - affected set을 조금 넓게 잡는 편이 안전
- warehouse
  - open area가 많고 우회 가능성이 큼
  - affected set을 더 작게 유지할 여지가 큼

즉 affected set 최소성은 단순 거리 기반 문제가 아니라:

- dependency closure
- shared resource closure
- time-window closure

를 함께 고려하는 runtime heuristic 설계 문제다.

### 13.3 Resource-Aware Trigger

단순 `after_event` 기반 trigger는 구현이 쉽지만, 실제 실행에서는 다음 문제가 있다.

- 어떤 event가 완료되어도 병목 자원이 아직 비지 않았을 수 있음
- 반대로 특정 event 완료를 기다리지 않아도, 자원이 이미 비었으면 다음 action을 열 수 있음
- 결과적으로 event 순서에 과도하게 묶여 불필요한 대기나 replan이 늘어날 수 있음

따라서 장기적으로는 `event-complete trigger`보다 `resource-aware trigger`가 더 중요하다.

핵심 아이디어는 다음과 같다.

```text
"누가 끝났는가?" 보다
"공유 자원이 실제로 사용 가능해졌는가?"를 본다.
```

여기서 resource는 다음을 의미한다.

- corridor / virtual lock
- narrow gateway
- intersection bottleneck
- station approach vertex
- 특정 safety zone 또는 작업 구역

권장 resource 상태 모델은 다음과 같다.

```cpp
enum class ResourceState {
    FREE,
    RESERVED,
    OCCUPIED,
    BLOCKED
};
```

```cpp
struct SadgResource {
    std::string resource_id;
    ResourceState state;
    std::optional<int> owner_robot_index;
    std::optional<std::string> owner_action_id;
};
```

의미:

- `FREE`
  - 즉시 새 action이 사용할 수 있음
- `RESERVED`
  - 아직 점유 전이지만 다음 action에 의해 선점됨
- `OCCUPIED`
  - 현재 실행 중 action이 사용 중
- `BLOCKED`
  - 외부 장애물, safety barrier, manual lock 등으로 사용 불가

### 13.3.1 왜 필요한가

corridor 예를 들면, `after_event`만으로는 다음 문제가 생긴다.

- 어떤 로봇이 corridor 진입 event를 끝냈다고 해서 corridor가 완전히 풀린 것은 아님
- lock 안에 아직 다른 로봇이 남아 있을 수 있음
- gateway가 막혀 있으면 다음 로봇을 열어도 실제 진행은 안 됨

즉 SADG는 “event 완료”보다 “resource free”를 더 직접적으로 봐야 한다.

### 13.3.2 권장 Trigger 종류

장기적으로는 다음 trigger를 함께 쓰는 것이 좋다.

- `on_event_done`
  - 기본 trigger
- `on_resource_free`
  - 특정 resource가 `FREE`가 되었을 때 대기 action 재평가
- `on_resource_blocked`
  - resource가 `BLOCKED`가 되면 관련 suffix stale 여부 재평가
- `on_goal_change`
  - 해당 robot과 연결된 future suffix 재평가
- `on_timeout_window`
  - stale observation window 종료 시점 reevaluate

### 13.3.3 권장 동작

기본 흐름은 다음과 같다.

```text
resource state change
-> waiting action / waiting robot 재평가
-> READY action이 생기면 dispatch
-> 필요 시 stale / invalid 재분류
```

즉 `resource free`는 반드시 replanning으로 이어질 필요가 없고, 먼저 `dispatch 재평가`의 trigger로 쓰는 것이 좋다.

이 점이 중요하다.

- event-aware only
  - 다음 action을 열 기회를 놓치기 쉬움
- resource-aware
  - 기존 plan을 유지한 채 더 자연스럽게 이어갈 수 있음

### 13.3.4 replan 감소 효과

resource-aware trigger가 있으면 다음 상황에서 재계획을 줄일 수 있다.

- 늦어진 로봇이 결국 lock을 비워줌
- 후행 로봇은 단순 wait만 하면 됨
- resource가 free가 되자마자 다음 action이 READY가 됨
- 기존 suffix를 그대로 유지하고 실행 재개 가능

즉 단순 지연은 재계획이 아니라 `resource release 후 resume`으로 끝낼 수 있다.

### 13.3.5 권장 구현 순서

v1에서는 다음처럼 단계적으로 가는 것이 좋다.

1. `event-complete trigger` 유지
2. corridor / virtual lock / gateway에 대해서만 `resource free trigger` 추가
3. stale 판단 전에 `resource free 후 resume 가능 여부`를 먼저 확인
4. 그 뒤에도 진행이 막히면 replan-required stale로 승격

이 순서가 좋은 이유는:

- 구현 난도를 통제할 수 있음
- 병목 자원이 많은 시나리오에서 효과가 큼
- global replan 빈도를 줄일 가능성이 높음

### 13.3.6 SADG와 잘 맞는 이유

SADG의 본질이 robot 간 relation 관리라면, trigger도 “시간”이나 “event id”보다 “relation이 실제로 풀렸는가”를 기준으로 가는 편이 더 자연스럽다.

즉 resource-aware trigger는 SADG의 철학과 잘 맞는다.

### 13.4 Execution Feedback 품질

SADG는 계획 자체보다 `실행 feedback`을 보고 상태를 갱신한다. 따라서 feedback 품질이 낮으면 SADG의 dispatch, hold, replan, safe-stop 판단이 직접 흔들린다.

핵심 위험은 두 가지다.

- `false invalid`
  - 실제로는 정상인데 feedback 지연/노이즈 때문에 stale 또는 invalid로 오판
- `false safe`
  - 실제로는 아직 위험한데 feedback 부족 때문에 안전하다고 오판

후자가 더 위험하므로, shared resource 해제 판단은 기본적으로 보수적으로 가져가는 것이 좋다.

### 13.4.1 SADG가 feedback에 의존하는 판단

SADG는 다음 판단을 feedback으로 수행한다.

- 현재 robot이 어느 vertex에 있는가
- 현재 action이 완료되었는가
- 특정 resource가 비었는가
- dependency가 풀렸는가
- hold / estop / fault가 발생했는가

즉 feedback 품질 문제는 단순 통신 문제가 아니라 SADG 상태 전이의 입력 문제다.

### 13.4.2 대표적인 품질 문제

- feedback 지연
  - `ACTION_DONE` 또는 `POSITION_UPDATE`가 늦게 도착
- feedback 누락
  - hold ack, fault, action done이 유실
- out-of-order delivery
  - 오래된 패킷이 늦게 도착해 최신 상태를 덮으려 함
- localization drift / jitter
  - vertex 경계 부근에서 위치가 흔들림
- action correlation 부재
  - 어떤 완료 feedback이 어떤 command/action에 대응하는지 불명확

### 13.4.3 권장 feedback 계약

실용적인 feedback message는 최소한 다음 정보를 포함하는 것이 좋다.

```cpp
struct ExecutionFeedback {
    std::string robot_id;
    int64_t timestamp_ms;
    uint64_t sequence_no;
    ExecutionFeedbackType type;
    std::optional<std::string> action_id;
    std::optional<int> actual_vertex;
    std::optional<double> confidence;
    std::string detail;
};
```

필드 의미:

- `timestamp_ms`
  - feedback 발생 시각
- `sequence_no`
  - out-of-order packet 무시용
- `action_id`
  - 어떤 dispatch/action에 대한 feedback인지 식별
- `actual_vertex`
  - 현재 또는 완료 시점의 위치
- `confidence`
  - localization 또는 state estimate 신뢰도

### 13.4.4 권장 해석 정책

feedback는 종류별로 다르게 해석해야 한다.

- `POSITION_UPDATE`
  - 관측 정보
  - path prefix 양립 여부 확인에 사용
- `ACTION_DONE`
  - 명령 완료 정보
  - 가능하면 `action_id`와 위치를 함께 확인해 DONE 확정
- `HOLD_ACK`
  - hold 상태 진입 확인
- `FAULT`
  - 즉시 HELD 또는 SAFE_STOP 후보
- `LOCALIZATION_LOST`
  - 위치 기반 판단의 신뢰도 저하

중요한 점은, 위치가 맞는다고 해서 곧바로 action 완료로 단정하지 않는 것이다.

### 13.4.5 보수적 안전 정책

권장 정책은 다음과 같다.

- timestamp가 없거나 sequence가 불명확한 feedback은 약하게 신뢰
- 오래된 feedback은 최신 상태를 되돌리지 않음
- corridor / virtual lock / gateway 해제 판단은 강한 근거가 있을 때만 수행
- 애매하면 `resource occupied` 쪽으로 유지
- feedback 누락은 즉시 invalid가 아니라 우선 stale 후보로 분류
- 일정 window 안에 회복되지 않으면 hold 또는 replan-required stale로 승격

즉 불확실할 때는 “조금 늦게 보내는 것”이 “너무 빨리 보내는 것”보다 낫다.

### 13.4.6 v1 권장 조합

v1에서는 다음 조합이 현실적이다.

- `POSITION_UPDATE`는 주기적으로 수신
- `ACTION_DONE`는 command correlation 가능한 형태로 수신
- `HOLD_ACK`와 `FAULT`는 high priority feedback으로 처리
- resource release는 `ACTION_DONE + 위치 일치` 또는 동등한 강도의 증거가 있을 때만 확정

이 조합이면 feedback 품질이 완벽하지 않아도 SADG가 지나치게 공격적으로 dispatch하지 않게 만들 수 있다.

### 13.5 MAPF Runtime과의 경계

현재 코드에서는 `ScenarioRuntime`이 `affected replan -> global replan -> safe stop` escalation을 담당한다. 이 구조 자체는 괜찮지만, 장기적으로는 SADG와 Runtime의 경계를 더 명확히 고정해야 한다.

핵심 질문은 다음과 같다.

```text
SADG는 어디까지 판단하고,
Runtime은 어디서부터 운영 정책을 집행하는가?
```

이 경계가 흐려지면 다음 문제가 생긴다.

- SADG가 planner policy까지 떠안아 복잡해짐
- Runtime이 dependency / resource semantics를 너무 많이 알아야 함
- replan 실패 시 책임 소재가 불명확해짐

### 13.5.1 권장 책임 분리

권장 분리는 다음과 같다.

- `SADG`
  - invalid / stale 감지
  - affected set 계산
  - frozen suffix 생성
  - replanning intent 생성
  - replanned suffix를 execution graph에 patch

- `MAPF Runtime`
  - SADG가 만든 intent를 planner 호출 형태로 변환
  - affected replan 수행
  - 실패 시 global replan escalation
  - 최종적으로 `SAFE_STOP_REQUIRED` 여부 결정

- `MapfPlanner / Planner Core`
  - 주어진 요청에 대해 valid path 탐색
  - 명시적 status 반환

즉 SADG는 `무엇이 문제인가`와 `누가 영향을 받는가`를 판단하고, Runtime은 `어떤 planning policy를 실행할 것인가`를 담당한다.

### 13.5.2 SADG가 Runtime에 넘겨야 하는 것

SADG는 Runtime에 planner 내부 세부사항이 아니라, 다음 intent를 넘기는 것이 좋다.

```cpp
struct SadgReplanIntent {
    FleetSnapshot snapshot;
    std::vector<GoalAssignment> assignments;
    std::vector<int> affected_robot_indices;
    FrozenPlanSet frozen;
    std::string reason;
};
```

즉 Runtime은:

- 왜 replan이 필요한지
- 누구를 affected로 볼지
- 누구를 frozen reservation으로 남길지

를 이미 전달받고, 그 위에서 운영 정책만 수행한다.

### 13.5.3 Runtime이 SADG에 돌려줘야 하는 것

Runtime은 planner raw result가 아니라, SADG가 바로 반영할 수 있는 결과를 돌려주는 것이 좋다.

예:

```cpp
struct RuntimeReplanResult {
    PlanningStatus status;
    Solution solution;
    bool used_global_replan;
    bool safe_stop_required;
    std::string reason;
};
```

SADG는 이 결과를 받아:

- 성공이면 새 suffix patch
- global replan 사용 여부 기록
- 실패면 hold 유지 또는 safe stop 반영

을 수행한다.

### 13.5.4 무엇을 SADG에 넣지 말아야 하나

다음은 SADG가 직접 가지지 않는 것이 좋다.

- affected 실패 후 global replan 승격 여부 결정
- planner timeout 재시도 정책
- multi-start / worker thread 수 결정
- planner heuristic / objective tuning

이런 것까지 SADG가 알기 시작하면, dependency coordinator가 아니라 planner policy layer가 되어버린다.

### 13.5.5 무엇을 Runtime이 너무 많이 알면 안 되나

반대로 Runtime도 다음까지 깊게 알 필요는 없다.

- action-level dependency graph 전체
- resource release 의미론의 세부 판정
- stale / invalid 분류 로직

Runtime이 이걸 너무 많이 알게 되면 SADG와 책임이 겹친다.

따라서 Runtime은 가능하면:

- `SadgReplanIntent`를 입력으로 받고
- planner를 호출하고
- escalation 결과를 반환하는

얇은 운영 계층으로 유지하는 것이 좋다.

### 13.5.6 권장 호출 흐름

```text
Execution feedback
-> SADG invalid/stale classification
-> SADG affected set calculation
-> SADG replan intent 생성
-> Runtime affected replan
-> if failed, Runtime global replan
-> if failed, Runtime SAFE_STOP_REQUIRED
-> SADG graph patch or safe stop reflect
```

이 흐름의 장점은 다음과 같다.

- SADG는 dependency / resource semantics에 집중
- Runtime은 escalation policy에 집중
- Planner는 pure planning에 집중

### 13.5.7 장기 방향

현재 `ScenarioRuntime`은 시나리오 replay 성격도 함께 가지고 있다. 장기적으로는 다음처럼 더 분리되는 것이 이상적이다.

```text
sadg_runtime
  -> execution feedback 처리
  -> affected set 계산
  -> runtime replan 요청

mapf_runtime
  -> affected/global/safe-stop escalation

planner_core
  -> path search
```

즉 최종 방향은 `SADG orchestration`과 `MAPF runtime policy`를 나누는 것이다.

---

문서 최종 수정: 2026-04-05
