# multibot3 — MAPF Layer 설계 문서

> **작성 목적**: multibot3 프로젝트의 MAPF(Multi-Agent Path Finding) 레이어 설계 결정 사항, 동작 원리, 구현 고려사항을 기록한다.  
> **대상 독자**: 프로젝트를 처음 접하는 개발자, 리뷰어

---

## 목차

1. [시스템 전체 구조](#1-시스템-전체-구조)
2. [MAPF Layer 역할](#2-mapf-layer-역할)
3. [알고리즘 선택: LaCAM★](#3-알고리즘-선택-lacam)
4. [Input / Output 명세](#4-input--output-명세)
5. [그래프 모델: 일반 그래프 G=(V,E)](#5-그래프-모델-일반-그래프-gve)
6. [Collision Constraints](#6-collision-constraints)
7. [Anytime 중단 조건](#7-anytime-중단-조건)
8. [SADG Layer와의 인터페이스](#8-sadg-layer와의-인터페이스)
9. [설계 결정 요약](#9-설계-결정-요약)
10. [미해결 과제 및 향후 확장](#10-미해결-과제-및-향후-확장)
11. [참고 문헌](#11-참고-문헌)

---

## 1. 시스템 전체 구조

multibot3는 다수의 로봇(AGV)을 물류 창고 환경에서 운용하기 위한 멀티 로봇 조율 시스템이다. 전체 파이프라인은 세 개의 레이어로 구성된다.

```
┌─────────────────────────────────────────────────┐
│              multibot3 Pipeline                  │
│                                                  │
│  [MAPF Layer]  →  [SADG Layer]  →  [Execution]  │
│                                                  │
│   경로 계획       실행 순서 조율    로봇 제어      │
└─────────────────────────────────────────────────┘
```

| 레이어 | 역할 | 핵심 알고리즘 |
|---|---|---|
| **MAPF Layer** | 모든 로봇의 충돌 없는 경로 계획 | LaCAM★ |
| **SADG Layer** | MAPF 경로 기반 실행 의존성 관리, 지연 흡수 | SE-ADG (Berndt et al., TRO-2024) |
| **Execution Layer** | 실제 로봇 제어 (ROS2) | Local coordination |

> **핵심 관계**: MAPF Layer의 출력이 SADG Layer의 입력이 된다.  
> MAPF가 내놓는 경로의 품질과 제약 조건이 하위 레이어 전체의 안전성을 결정한다.

---

## 2. MAPF Layer 역할

### 2.1 MAPF 문제 정의

MAPF(Multi-Agent Path Finding)란, 그래프 위에서 다수의 에이전트가 각자의 목적지까지 **충돌 없이** 이동하는 경로를 찾는 문제이다.

```
주어진 것:
  G = (V, E)       환경 그래프
  A = {1, ..., n}  로봇 집합
  S = (s₁, ..., sₙ) 시작 위치
  G = (g₁, ..., gₙ) 목표 위치

구하는 것:
  Π = (Q₀, Q₁, ..., Qₖ)  모든 로봇의 시각별 위치 시퀀스
  (단, 어떤 시각 t에서도 충돌이 없어야 함)
```

### 2.2 이 레이어가 보장해야 하는 것

- 모든 로봇이 목적지에 도달하는 경로를 반환한다 (completeness)
- 어떤 두 로봇도 같은 시각에 같은 vertex를 점유하지 않는다 (vertex collision 없음)
- 어떤 두 로봇도 같은 시각에 동일 edge를 반대 방향으로 통과하지 않는다 (non-passing collision 없음)
- 출력 형식이 SADG Layer가 요구하는 형태를 만족한다

---

## 3. 알고리즘 선택: LaCAM★

### 3.1 PIBT vs LaCAM★ 비교

multibot3의 MAPF 알고리즘 후보로 **PIBT**와 **LaCAM★** 을 검토했다.

| 항목 | PIBT | LaCAM★ |
|---|---|---|
| **출력** | 매 timestep의 이동 결정 | **완전한 경로 (full path)** |
| **Completeness** | ❌ (lifelong reachability만) | ✅ |
| **Optimality** | ❌ | eventually optimal (anytime) |
| **Scalability** | ✅ 수만 에이전트 가능 | ✅ 10,000 에이전트 30초 이내 |
| **SADG 입력 호환** | ❌ 경로 자체가 없음 | ✅ 완전한 경로 출력 |
| **Runtime 예측성** | ✅ 매우 예측적 | ⚠️ 인스턴스 의존 |

### 3.2 LaCAM★을 선택한 이유

**SADG는 각 로봇의 전체 경로(vertex 점유 시퀀스)를 입력으로 받아 dependency graph를 컴파일한다.** PIBT는 전체 경로를 출력하지 않으므로 SADG의 입력 요건을 충족하지 못한다.

또한 PIBT는 LaCAM★의 내부 configuration generator로 이미 포함되어 있다.

```
LaCAM★ ⊃ PIBT

LaCAM★ 구조:
  High-Level Search (DFS over configuration space)
      ↓ 매 iteration
  configuration_generator()
      = PIBT with swap  ← PIBT가 여기에 들어감
```

따라서 두 알고리즘은 경쟁 관계가 아니다. **LaCAM★을 선택하면 PIBT의 장점도 함께 가져간다.**

### 3.3 LaCAM★ 동작 원리 요약

LaCAM★은 two-level search 구조를 가진다.

```
High-Level Search
  ├── Open (stack): configuration 노드 관리
  ├── 각 노드에서 LowLevelSearch() → constraint 생성
  ├── configuration_generator(PIBT) → 다음 configuration 후보 생성
  ├── 새 configuration이면 노드 추가
  ├── 알려진 configuration이면 Dijkstra로 g-value 업데이트
  └── goal 발견 후에도 계속 탐색 (anytime: 품질 지속 개선)
```

**Anytime 특성**: 언제 중단해도 그 시점까지의 best solution을 반환한다. 시간이 허락하면 optimal에 수렴한다.

---

## 4. Input / Output 명세

### 4.1 Input

```python
G = (V, E)
# V: vertex 집합 (교차점, 대기 노드, 작업 위치 등)
# E: edge 집합 (통행 가능한 연결선, undirected)
# 조건: biconnected (아래 5.3 참고)

A = {1, ..., n}          # 로봇 집합

S = (s₁, ..., sₙ)        # 각 로봇의 시작 vertex (sᵢ ∈ V, 모두 distinct)
G = (g₁, ..., gₙ)        # 각 로봇의 목표 vertex (gᵢ ∈ V, 모두 distinct)

dist_table[i][v]          # goal gᵢ에서 각 vertex v까지의 거리
                          # 사전에 BFS로 계산, O(|A|·|E|)
```

> ⚠️ **그리드 환경 미사용**: 표준 MAPF 벤치마크의 정사각형 grid가 아닌, 일반 그래프 G=(V,E)를 사용한다. Manhattan distance 대신 BFS distance table을 사용한다.

### 4.2 Output

```python
Π = (Q₀, Q₁, ..., Qₖ)
# Qₜ = (v₁, v₂, ..., vₙ): 시각 t에서 각 로봇의 vertex

# 로봇 i의 경로:
πᵢ = [Qₜ[i] for t in range(k+1)]

# SADG 입력 형식으로 변환:
Pᵢ = [(v, t) for t, v in enumerate(πᵢ)]
# = 로봇 i가 시각 t에 vertex v를 점유함을 나타내는 plan tuple 시퀀스
```

---

## 5. 그래프 모델: 일반 그래프 G=(V,E)

### 5.1 설계 원칙

표준 MAPF 연구에서는 4-connected grid를 주로 사용하지만, 물류 창고의 실제 환경은 grid로 표현하기 어렵다. multibot3는 임의의 G=(V,E)를 입력으로 받는다.

```
vertex = 로봇이 위치할 수 있는 모든 지점
  예: 교차점, 선반 앞 대기 위치, 충전 스테이션, 엘리베이터 앞 등

edge = 두 vertex 간 직접 이동 가능한 연결
  예: 복도 구간, 교차로 연결선
```

### 5.2 Biconnected 요건

**PIBT는 biconnected undirected graph에서만 reachability(모든 에이전트가 유한 시간 내 목적지 도달)를 보장한다.**

Biconnected이 깨지면:
- Bridge edge나 degree-1 vertex에서 PIBT가 livelock에 빠짐
- LaCAM★이 constraint 탐색을 폭발적으로 반복 (최대 287,440회 확인됨)
- 결국 timeout → NO_SOLUTION → SADG에 경로 전달 불가 → 시스템 정지

따라서 **입력 그래프는 반드시 biconnected이어야 하며, 시스템 초기화 시 한 번 검증한다.**

```python
def load_graph(V, E):
    G = build_graph(V, E)
    assert is_biconnected(G), \
        "그래프가 biconnected 조건을 만족하지 않습니다. 맵을 수정하거나 가상 lock vertex를 적용하세요."
    return G
```

### 5.3 좁은 복도 처리: Virtual Lock Vertex

물류 창고에는 로봇 1대만 통과 가능한 좁은 복도가 존재한다. 이 구간은 구조적으로 bridge edge chain이 되어 biconnected 조건을 위반한다.

**해결 방법: 복도 전체를 가상의 single vertex(virtual lock vertex)로 collapse**

```
변환 전 (biconnected 위반):
  [구역A] — a — b — c — d — [구역B]
              (1대만 통과 가능한 복도)

변환 후 (biconnected 복원):
  [구역A] — [V_lock] — [구역B]
              capacity = 1
```

**효과:**
- Bridge edge가 제거되어 biconnected 복원
- 복도 진입 = V_lock vertex 점유 → vertex collision으로 자동 처리
- 반대 방향 동시 진입 자동 차단 (NEXUS 위험1 해결)
- LaCAM★ 코드 수정 불필요, 그래프 전처리만으로 해결

**그래프 구역 분류:**

```
넓은 구역 (Wide Area):
  실제 vertex/edge 그대로 사용
  degree ≥ 3, biconnected 유지
  로봇들이 서로 비켜갈 수 있음

좁은 복도 (Narrow Corridor):
  복도 전체를 virtual lock vertex 하나로 collapse
  capacity = 1 자원으로 모델링
```

---

## 6. Collision Constraints

표준 LaCAM★의 collision 정의에 **non-passing collision을 추가**한다. 이것이 표준 LaCAM★과의 핵심 차이점이다.

### 6.1 표준 LaCAM★의 2가지 collision

```
① Vertex collision
   Qₜ[i] = Qₜ[j]
   같은 시각 t에 두 로봇이 같은 vertex를 점유

② Swap collision (Edge collision)
   Qₜ[i] = Qₜ₊₁[j]  ∧  Qₜ[j] = Qₜ₊₁[i]
   같은 timestep에 두 로봇이 서로의 위치를 교차
```

### 6.2 multibot3에서 추가하는 constraint

```
③ Non-passing collision (신규 추가)
   시각 t에 로봇 i가 edge (u → v)를 사용했고,
   시각 t+1에 로봇 j가 edge (v → u)를 사용하려는 경우 금지

   = 같은 edge를 서로 다른 timestep에 반대 방향으로 통과하는 것 금지

   구현: (Qₜ₋₁, Qₜ, Qₜ₊₁) 3-timestep window로 확인
         edge 사용 이력을 HashSet<(u, v, t)>로 관리
```

### 6.3 Non-passing collision이 필요한 이유

SADG(TRO-2024)의 valid MAPF solution 정의(Definition 1)에는 다음 조건이 명시되어 있다:

> "AGVi and AGVj must never traverse an edge e ∈ E in opposite directions in the same timestep."

그러나 SADG는 **경로를 수정하지 않는다.** 경로 자체에 non-passing 충돌이 있으면 SADG는 이를 "의존성 없음"으로 판단해 두 로봇을 동시에 보낸다. **따라서 MAPF 레이어에서 반드시 이 조건을 보장해야 한다.**

### 6.4 3가지 constraint 전체 요약

| # | 이름 | 조건 | 처리 레이어 |
|---|---|---|---|
| ① | Vertex collision | 같은 t, 같은 vertex | LaCAM★ 기본 |
| ② | Swap collision | 같은 t에 위치 교차 | LaCAM★ 기본 |
| ③ | Non-passing collision | 다른 t에 같은 edge 반대 방향 | **multibot3 추가** |

---

## 7. Anytime 중단 조건

LaCAM★은 anytime 알고리즘이다. **언제 중단하느냐**가 시스템 성능에 직접 영향을 미친다.

### 7.1 피드백 루프 구조

```
[최초 계획]
  LaCAM★ 실행 → 중단 조건 달성 → 경로 반환
      ↓
  SADG 실행 (경로 기반 실행 조율)
      ↓
  로봇 지연 / 고장 발생
      ↓
[재계획 요청] ← SADG가 MAPF에 interrupt 신호 전송
  LaCAM★ 재실행 → 빠른 중단 → 경로 반환
      ↓
  SADG 재실행
      ↓
  안정화 → (선택) 백그라운드 품질 개선
```

### 7.2 상황별 중단 조건

두 상황의 우선순위가 반대이므로 **단일 정책으로 통일하지 않는다.**

**상황 A: 최초 계획**

```
우선순위: 경로 품질 > 속도

primary   → Solution quality threshold
            (optimal 대비 X% 이내에 도달하면 중단)
secondary → Hard timeout
            (품질 미달이라도 이 시간에는 무조건 중단)

이유: 초기 경로 품질이 좋을수록 SADG 재계획 빈도가 낮아지고
      전체 피드백 루프가 안정됨
```

**상황 B: 재계획 (SADG 요청)**

```
우선순위: 속도 > 경로 품질

primary   → First solution 발견 즉시 반환
secondary → Hard timeout
            (first solution도 못 찾으면 강제 종료)

이유: 재계획 요청 시점은 일부 로봇이 정지해 있는 상황
      느린 최적화보다 빠른 sub-optimal 경로가 throughput에 유리
```

### 7.3 파라미터 (튜닝 필요)

```python
class MAPFStopCondition:

    # 최초 계획
    INITIAL_QUALITY_THRESHOLD = 1.2   # optimal 대비 120% 이내
    INITIAL_HARD_TIMEOUT_MS   = 5000  # 5초 hard limit

    # 재계획
    REPLAN_USE_FIRST_SOLUTION = True  # 첫 solution 즉시 반환
    REPLAN_HARD_TIMEOUT_MS    = 500   # 500ms hard limit
```

> 위 수치는 로봇 대수와 그래프 크기에 따라 실험적으로 튜닝이 필요하다.

---

## 8. SADG Layer와의 인터페이스

### 8.1 MAPF Output → SADG Input 변환

```
MAPF Layer output:
  Pᵢ = [(v₀, 0), (v₁, 1), (v₂, 2), ...]  for each robot i
  = 로봇 i가 시각 t에 vertex v를 점유

SADG Algorithm 1 input:
  P = {P₁, P₂, ..., Pₙ}  ← 그대로 전달
```

### 8.2 SADG가 요구하는 조건과 MAPF 보장의 매핑

| SADG Definition 1 조건 | 보장 레이어 |
|---|---|
| 같은 t에 같은 vertex 없음 | constraint ① |
| 같은 t에 반대 방향 edge 없음 | constraint ③ (추가) |
| 모든 로봇이 goal에 도달 | LaCAM★ completeness |
| SE-ADG가 acyclic | \|V\| > N 조건 + constraint ①③ |

### 8.3 재계획 인터페이스

```python
# SADG → MAPF 재계획 요청
def request_replan(affected_robots, current_positions, new_goals):
    stop_condition = MAPFStopCondition.REPLAN
    new_paths = lacam_star(
        G, affected_robots,
        starts=current_positions,
        goals=new_goals,
        stop=stop_condition
    )
    return new_paths
```

---

## 9. 설계 결정 요약

| 결정 사항 | 선택 | 이유 |
|---|---|---|
| MAPF 알고리즘 | LaCAM★ | SADG 입력 요건(완전한 경로) 충족, anytime, PIBT 포함 |
| 환경 모델 | 일반 그래프 G=(V,E) | 물류 창고의 비정형 구조 표현 |
| 거리 계산 | BFS distance table | grid 없으므로 Manhattan distance 불사용 |
| 그래프 조건 | Biconnected (강제) | PIBT livelock 방지, 시스템 시작 시 1회 검증 |
| 좁은 복도 처리 | Virtual lock vertex | Biconnected 복원 + 반대 방향 진입 자동 차단 |
| Collision | 3가지 (non-passing 추가) | SADG Definition 1 충족 |
| 중단 조건 (최초) | Quality threshold + Hard timeout | 경로 품질 우선 |
| 중단 조건 (재계획) | First solution + Hard timeout | 속도 우선 |

---

## 10. 미해결 과제 및 향후 확장

### 10.1 Non-passing constraint 구현 비용
3-timestep window 체크가 PIBT 내부 루프에 들어갈 때의 성능 영향을 측정해야 한다. `HashSet<(u, v, t)>` 기반의 edge 이력 관리가 현실적인 구현 방법이다.

### 10.2 Quality threshold 기준값 결정
`INITIAL_QUALITY_THRESHOLD = 1.2` 는 임시값이다. 로봇 대수(n)와 그래프 밀도에 따라 실험적으로 튜닝이 필요하다.

### 10.3 재계획 시 영향 로봇 집합 결정 (NEXUS 위험3)
SADG에서 재계획 요청 시 어떤 로봇들의 경로를 재계획할지 — affected robot set 의 경계를 정하는 기준이 별도로 필요하다. 현재는 MAPF Layer 외부(SADG)의 책임으로 분리되어 있다.

### 10.4 Non-biconnected 그래프 지원 (장기)
현재는 biconnected 그래프만 지원한다. 향후 더 복잡한 환경을 지원하려면 PIBT generator를 Prioritized Planning 등으로 교체하거나 구역별 hybrid 전략이 필요하다.

---

## 11. 참고 문헌

| 논문 | 내용 |
|---|---|
| Okumura et al. (2022). *PIBT: Priority Inheritance with Backtracking*. AIJ. | PIBT 알고리즘 |
| Okumura (2023). *LaCAM: Search-Based Algorithm for Quick MAPF*. AAAI. | LaCAM 알고리즘 |
| Okumura (2023). *Improving LaCAM for Scalable Eventually Optimal MAPF*. IJCAI. | LaCAM★ 알고리즘 |
| Okumura (2024). *Engineering LaCAM★*. AAMAS. | LaCAM★ 엔지니어링 최적화 |
| Berndt et al. (2024). *Receding Horizon Re-Ordering of Multi-Agent Execution Schedules*. TRO. | SE-ADG, SADG |
| Okumura et al. (2023). *Offline Time-Independent Multiagent Path Planning*. TRO. | Time-Independent MAPF |

---

*문서 최종 수정: 2026-03-22*
