# 🔴 탐색용 시제 — 증거가 아니다

**C++ 본 구현에 들어가기 전에 알고리즘을 «눈으로 보고 다듬기» 위한 파이썬 시제다.
여기서 나오는 어떤 수치도 논문의 증거가 아니고, 어떤 값도 사전등록 상수가 아니다.**

## 이것이 아닌 것

| 아닌 것 | 왜 |
|--------|----|
| ROS 2 패키지가 아니다 | `COLCON_IGNORE`가 있어 `colcon build`가 이 디렉터리를 건너뛴다 |
| 본 구현이 아니다 | 본 구현은 `src/mrs_*`의 C++이며 계층·어댑터 관례(`multibot3/CLAUDE.md` CN-1~CN-22)를 따른다 |
| 측정 하네스가 아니다 | 계측·링버퍼·분위수·검열이 하나도 없다 |
| 사전등록의 발동이 아니다 | `_workspace/27_spike_preregistration.md` §1의 값은 **동결되지 않았다.** `config/scale.yaml`의 수치가 그 문서의 수치와 우연히 같아도 **동결이 아니다** |
| **완전한 실행 계층이 아니다** | S3에서 **실행 의존 그래프·스위치 그룹·임계 경로**가, S4에서 **릴리스 판정 루프·커밋 적용 지점·게이트 ①②③④·진입 사건·재발행 발화**가 생겼다. 그러나 **재선택(스위칭) 선택기·승급 사다리·계측(링버퍼·분위수·예산 측정)이 여전히 없다.** 애니메이션 타이밍은 **우리가 지어낸 것**이다 |
| 계약 만족의 판정이 아니다 | 아래 검사는 생성자가 자기 산출물에 돌린 것이다. 판정은 `boundary-verifier`의 몫이다 |

## 이것인 것

1. **로드맵 생성기**(`gen_roadmap.py`) → `mrs.roadmap` 2.0.0
2. **태스크 배정 생성기**(`gen_assignment.py`, 가장 가까운 것부터) → `mrs.assignment` 1.1.0
3. **경로 계획기**(`gen_plan.py`, 우선순위 기반 시공간 A\*) → `mrs.discrete_plan` 2.0.0
4. **로봇별 노드 순서표**(`plan_table.py`) → `out/plan_table.md`
5. **실행 의존 그래프 컴파일러**(`gen_constraints.py`) → `mrs.execution_constraints` 1.1.0
6. **릴리스 판정 루프**(`tick_core.py` + `release_sim.py`) — 매 틱 릴리스를 판정하고
   **놓인 세그먼트만** 물리 계층이 시작한다
7. **정적 그림**(`viz.py`) → `out/scene.png` · **애니메이션 3종**
   (`anim.py` 공칭 속도 재생 → `out/replay.gif` · `anim_exec.py` **의존 그래프 재생 + 차단**
   → `out/replay_exec.gif` · `anim_release.py` **릴리스 판정이 화면에 보이는 재생**
   → `out/replay_release.gif`)

---

# 사용법

## 1. 가장 간단한 네 줄 (전부 기본값)

```bash
cd multibot3/src/prototype
python3 gen_roadmap.py            # out/roadmap.json      (정점 20 · 간선 25 · 통로 2)
python3 gen_assignment.py         # out/assignment.json   (로봇 6 · 목표 16)
python3 gen_plan.py --table       # out/discrete_plan.json + out/plan_table.md (화면에도 나온다)
python3 gen_constraints.py --on-cycle prune --schema-check   # out/execution_constraints.json
python3 viz.py && python3 anim.py # out/scene.png · out/replay.gif (GIF 생성에 약 9초)
python3 anim_exec.py --block-robot r0 --block-at 4 --block-seconds 45   # out/replay_exec.gif
python3 release_sim.py --block-robot r0 --block-at 4 --block-seconds 45 --drop-entry-nth 2
python3 anim_release.py --block-robot r0 --block-at 4 --block-seconds 45 --drop-entry-nth 2
```

**마지막 줄이 「막힘과 회복」이다** — 로봇 하나를 45초 세우고, 나머지가 `visit_order`가 정한
순서를 **지키며** 기다렸다가 회복하는 것을 보인다. 화면에 «누가 누구의 어느 세그먼트를
기다리는가»가 그대로 적힌다.

**`gen_plan.py --table`이 사용자가 보고 싶어 하는 그 표다** — 로봇마다
「시작 → 노드 순서 → 도착」과 어느 단계에서 어느 목표를 덮는지, 그리고 **공유 위치에서 누가 먼저
가는지**(`visit_order`)가 한 곳에 있다. 파일은 `out/plan_table.md`.

## 2. 바꿔 볼 수 있는 인자

### 로드맵 (`gen_roadmap.py`)

| 인자 | 무엇이 바뀌는가 | 예 |
|------|----------------|-----|
| `--seed` | 통로 배치·엔드포인트 위치. **같은 시드 → 같은 로드맵** | `--seed 7` |
| `--aisles` / `--cross` | 격자 크기(세로 통로 / 가로 교차통로 개수) | `--aisles 6 --cross 4` |
| `--endpoints` | 엔드포인트(적치·픽 지점) 개수 | `--endpoints 14` |
| `--narrow-ratio` | 🔴 **교행 불가 통로의 비율. 올리면 막히는 상황이 빨리 나온다** | `--narrow-ratio 1.0` |
| `--out` | 출력 경로 | `--out out/big/roadmap.json` |

### 배정 (`gen_assignment.py`)

| 인자 | 무엇이 바뀌는가 | 예 |
|------|----------------|-----|
| `--robots` | 🔴 **로봇 수. 지도 크기 대비 올리면 가장 빨리 막힌다**(아래 3절) | `--robots 8` |
| `--tasks` | 태스크 수. 로봇 수 × `--max-tasks-per-robot`을 넘으면 미배정이 남는다 | `--tasks 14` |
| `--max-tasks-per-robot` | 로봇당 태스크 상한(태스크 하나 = 픽업·배달 목표 두 개) | `--max-tasks-per-robot 1` |
| `--seed` | 시작 위치·태스크 지점 | `--seed 3` |

### 계획 (`gen_plan.py`)

| 인자 | 무엇이 바뀌는가 | 예 |
|------|----------------|-----|
| `--priority` | **무엇을 먼저 계획하는가.** `most_goals`(기본) · `longest_route` · `fewest_goals` · `robot_id` · `random` | `--priority longest_route` |
| `--horizon` | 🔴 **내부 타임스텝 상한. 줄이면 `plan_truncated`가 바로 나온다**(경계로 나가지 않는다) | `--horizon 8` |
| `--table` / `--table-out` | 노드 순서표를 함께 낸다 | `--table` |
| `--schema-check` | 표준 JSON Schema 구조 검증을 함께 돌린다 | `--schema-check` |

### 실행 의존 그래프 (`gen_constraints.py`)

| 인자 | 무엇이 바뀌는가 | 예 |
|------|----------------|-----|
| `--alt-gen` | 🔴 **확장점 E3 — 대안 생성기.** `lift`(선두 올리기, 정본) / `swap`(인접 쌍 교환, 구판) | `--alt-gen swap` |
| `--on-cycle` | 어떤 조합이 순환할 때. `report`(기본, 그대로 내고 종료 코드 4) / `prune`(순환을 만드는 대안을 지운다) | `--on-cycle prune` |
| `--out-dir` | 🔴 **파일 이름은 언제나 `execution_constraints.json`이다.** 변형은 디렉터리로 가른다 | `--out-dir out/swap` |
| `--max-groups` · `--max-combinations` | 전수 비순환 검사의 상한. 넘으면 **표본으로 강등**하고 그 사실을 표기한다 | `--max-combinations 64` |
| `--samples` · `--sample-seed` | 표본 강등 시 검사할 조합 수와 시드 | `--samples 5000` |
| `--inject-cycle` | 🔴 **음성 시험** — 일부러 순환을 만든다. 검사기가 잡는지 보는 용도 | |
| `--commit-seq` | 판본 번호. **시각이 아니다** | `--commit-seq 3` |

#### `gen_constraints.py`의 종료 코드

| 코드 | 뜻 |
|:----:|----|
| 0 | 산출했고 **전수** 비순환 검사 위반 0 |
| 1 | 산출했으나 **표본으로 강등**했다 — 위반 0이지만 **증명이 아니다** |
| 2 | 산출물 자체가 깨졌다 — 자체 점검·스키마·어댑터 왕복 위반 |
| **4** | **어떤 스위칭 조합이 순환한다.** 아티팩트는 **쓴다**(하류 검사기에 먹여야 한다) |

**왜 4를 새로 만들었나** — `gen_plan.py`의 넷으로 부족하기 때문이다. 순환은 (i) 구조가 깨진 것이
아니라 **의미 불변식**이 깨진 것이라 2와 다르고, (ii) `gen_plan.py`의 3(*"산출물을 쓰지 않았다"*)과
달리 **아티팩트를 써야 한다** — 그것을 층 1·`check_sadg_acyclic.py`에 먹여야 하기 때문이다.

### 릴리스 판정 루프 (`release_sim.py` · `anim_release.py`)

| 인자 | 무엇이 바뀌는가 |
|------|----------------|
| `--block-robot` · `--block-at` · `--block-seconds` | 차단 주입 |
| `--drop-entry-nth` | 🔴 **n번째 진입 사건을 유실시킨다** — D7(진입 간주) 경로와 «유도 불가 커밋» 재발행 발화를 밟는다 |
| `--entry-fraction` | 세그먼트의 몇 %에서 진입 사건을 내는가. 🔴 **N(v) 기하가 값 부재라 쓴 임시값이며 사전등록 상수가 아니다** |
| `--constraints` | 재생할 `mrs.execution_constraints`. 스위치 그룹이 있는 아티팩트를 주면 **진입 커밋**이 발동한다 |

🔴 **S3(`anim_exec.py`)와 다른 점**: S3는 max-plus로 시각을 **한 번에 풀었고**, S4는 **매 틱
릴리스를 판정하고 놓인 세그먼트만 시작한다.** 즉 *"언제 놓아 주는가"*가 코드의 결정이다.
차단이 없으면 둘의 완료 시각이 일치하며 **그것이 서로의 교차검증**이다(시험으로 고정).

### 애니메이션 · 공칭 속도 재생 (`anim.py`)

| 인자 | 무엇이 바뀌는가 |
|------|----------------|
| `--fps` · `--frames-per-tick` | 재생 속도와 부드러움 |
| `--priority` · `--horizon` · `--seed` | **계획기와 같은 값을 줘야 한다**(다르면 경고가 뜬다) |
| `--out` | `.gif`(기본) 또는 `.mp4`. **이 환경에는 `ffmpeg`가 없어 `.mp4`를 주면 GIF로 물러선다** |

### 애니메이션 · **의존 그래프 재생 + 차단** (`anim_exec.py`)

| 인자 | 무엇이 바뀌는가 |
|------|----------------|
| `--block-robot` · `--block-at` · `--block-seconds` | 🔴 **차단 주입** — 그 로봇의 그 세그먼트 위에서 몇 초 세운다 |
| `--constraints` | 재생할 `mrs.execution_constraints` 경로 |
| `--frames` · `--fps` | 프레임 수와 재생 속도 |

🔴 **`anim.py`와 다른 점 둘.** (1) **계획기를 다시 돌리지 않는다** — 경계 아티팩트만으로 재생한다.
S2가 *"경계만으로는 재생할 수 없다"*고 적은 것은 옳았고, **잃어버린 것은 대기가 아니라 순서였으며
그 순서를 실행 의존 그래프가 되돌려 준다.** (2) 세그먼트는 **들어오는 모든 의존이 끝나야** 시작한다.
🔴 **그래도 릴리스 판정의 완전한 형태가 아니다 — 그것은 S4다**(재선택·커밋 게이트·진입 사건·
신선도·계측·승급 사다리가 하나도 없다). 그림 안에 빨간 글씨로 적혀 있다.

## 3. 🔴 막히는 것을 보고 싶다면 — 인자 두 개면 된다

```bash
# (가) 지도 대비 로봇을 늘린다  ->  «인스턴스 미해결». 산출물을 쓰지 않고 종료 코드 3
python3 gen_roadmap.py    --seed 2 --out out/tight/roadmap.json
python3 gen_assignment.py --seed 2 --robots 8 --tasks 12 \
        --roadmap out/tight/roadmap.json --out out/tight/assignment.json
python3 gen_plan.py --roadmap out/tight/roadmap.json --assignment out/tight/assignment.json \
        --out out/tight/discrete_plan.json
#   -> 🔴 인스턴스 미해결 — 계획을 주지 못한 로봇이 있다: ['r3']

# (나) 지평을 줄인다  ->  절단(plan_truncated)이 늘고 종료 코드 1
python3 gen_plan.py --horizon 8
#   -> terminal 분포 {'goal_reached': 2, 'plan_truncated': 4}
```

**둘은 다른 실패다.** (가)는 «계약을 만족하는 산출물을 만들 수 없다»(로봇 전부에게 계획을 줘야
한다는 §17-3-1(8)을 못 지킨다)이고, (나)는 **계약이 허용하는 절단**이다(§1-2(5)).
**우선순위 기반 계획은 완전하지 않다. 막히는 것이 정상이며, 그것이 실행 계층이 필요한 이유다.**

### `gen_plan.py`의 종료 코드

| 코드 | 뜻 |
|:----:|----|
| 0 | 전 로봇 `goal_reached` |
| 1 | 계획은 냈으나 절단·정체가 있다(계약상 합법) |
| 2 | **산출물 자체가 깨졌다** — 자체 점검 또는 스키마 위반 |
| 3 | **인스턴스 미해결** — 로봇 하나 이상에게 계획을 못 줬다. 산출물을 쓰지 않았다 |

## 4. 계약 검사 돌리는 법 (레포 루트에서)

```bash
cd /path/to/repo
T=.claude/skills/multibot3-module-theory/scripts
# 층 1 — 묶음마다 파일을 명시한다 (한 스키마에 파일이 둘이면 종료 코드 2)
python3 $T/check_boundary_schema.py --schema-root multibot3/src/mrs_msgs/schema \
    multibot3/src/prototype/out/{roadmap,assignment,discrete_plan,execution_constraints}.json
# SADG 전수 비순환
python3 $T/check_sadg_acyclic.py multibot3/src/prototype/out/execution_constraints.json
# 순수성 (경계 디렉터리와 내부 회계 디렉터리를 따로)
python3 $T/check_schema_purity.py multibot3/src/prototype/out/
python3 $T/check_schema_purity.py multibot3/src/prototype/stats/
```

| 종료 코드 | 뜻 | 🔴 주의 |
|:--------:|----|--------|
| 0 | 표준 검증 위반 0 + 확장 키워드 위반 0 | **평가된 자리 수와 함께 읽어야 한다**(출력 §2절) |
| 1 | 위반이 있다 | — |
| 2 | **판정 불가** — 대응 스키마가 없거나 묶음이 불완전하다 | **2를 통과로 읽으면 안 된다.** 아무것도 검사되지 않은 상태일 수 있다 |

- **아티팩트 파일 이름은 `{스키마이름}.json`이어야 한다.** `roadmap_big.json`처럼 꾸미면
  *"대응 스키마가 없다"*로 **UNDECIDED(2)**가 된다. 그래서 변형은 이름이 아니라 디렉터리로 가른다.
- 🔴 **디렉터리를 통째로 주지 말고 묶음마다 파일을 명시하라.** 하위 디렉터리에 같은 이름의
  아티팩트가 있으면 **모호로 판정해 종료 코드 2**를 낸다. **3단계에서 다시 재어 고쳤다** —
  2단계 README는 *"하나만 보고 나머지는 조용히 무시한다"*로 적었으나 **현재 검증기는 그렇지
  않다.** `check_boundary_schema.py multibot3/src/prototype/out/`를 그대로 주면
  `[UNDECIDED] 스키마 execution_constraints.schema.json 에 아티팩트가 5개 걸렸다 … `와 함께
  **종료 코드 2**가 나온다(실측, 3단계).
- 🔴 **경계 아티팩트 디렉터리에 비경계 JSON을 두지 마라.** 검증기는 파일명 줄기로 스키마를
  찾으므로 `sadg_stats.json`을 `out/`에 두면 *"스키마에 대응된 아티팩트가 없다"*로
  **종료 코드 2**가 된다(실측). 그래서 S3의 내부 회계는 **`stats/` 디렉터리**에 쓴다.
- 🔴 **층 1 통과는 데드락 부재를 뜻하지 않는다.** 일부러 순환을 심은
  `out/negative/execution_constraints.json`을 층 1에 먹이면 **종료 코드 0**이고,
  같은 파일이 `check_sadg_acyclic.py`에서 **종료 코드 1**이다(실측). 비순환은 **층 2**다
  (`execution_constraints.schema.json`의 `x-layer2-note` A7).
- 순수성(알고리즘 파라미터 유출) 검사:
  `python3 .claude/skills/multibot3-module-theory/scripts/check_schema_purity.py multibot3/src/prototype/out/`

## 5. 시험

```bash
cd multibot3/src/prototype
python3 -m pytest tests -q                     # 152항목
python3 -m pytest tests/test_plan_core.py -q   # 계획기만
python3 -m pytest tests/test_sadg_core.py -q   # 실행 의존 그래프만 (53항목)
python3 -m pytest tests/test_tick_core.py -q   # 릴리스 판정 루프만 (26항목)
```

## 6. 물리 규모를 바꾸려면 — `config/scale.yaml` **하나만** 고친다

간선 길이·통로 폭·로봇 속도·차체 치수가 **코드에 하나도 박혀 있지 않다.** 아이작 심 에셋 치수가
정해지면 그 파일만 바꾼다. 시험 `test_roadmap_core.py::test_geometry_comes_from_config_only`가
이것을 지킨다(간선 5 m → 8 m로 바꿔 위상은 그대로이고 기하만 바뀌는지 본다).
애니메이션의 재생 속도도 같은 파일의 `robot.nominal_speed_mps`에서 온다.

---

## 파일 배치

```
protoscale.py         물리 규모 설정 로더 (수치가 나오는 유일한 곳)
roadmap_core.py       로드맵 생성 알고리즘        | 셋 다 경계 스키마를 모른다
assignment_core.py    태스크 배정 알고리즘        |
plan_core.py          S2 우선순위 기반 시공간 A*  | 넷 다 경계 스키마를 모른다
sadg_core.py          S3 실행 의존 그래프·스위치 그룹·전수 비순환 |
tick_core.py          S4 릴리스 판정·게이트 1234·커밋 적용 지점·원자성 감시 |
boundary_adapter.py   내부 <-> 경계 변환. **경계 JSON 키가 나오는 유일한 파일**
gen_roadmap.py · gen_assignment.py · gen_plan.py · gen_constraints.py    CLI
plan_table.py         로봇별 노드 순서표
exec_sim.py           S3 의존 그래프 max-plus 재생 (차단 주입 · 정점 점유 자체 점검)
release_sim.py        S4 닫힌 고리 — 릴리스 판정이 물리 계층을 움직인다 + CLI
viz.py                정적 그림 (두 애니메이션과 정적 층을 공유)
anim.py               애니메이션 — 공칭 속도 재생 (계획기를 다시 돌린다)
anim_exec.py          애니메이션 — **의존 그래프 재생 + 막힘과 회복** (경계만으로 재생한다)
anim_release.py       애니메이션 — **릴리스 판정이 화면에 보인다** (무엇이 왜 아직 안 놓였는가)
tests/                코어 시험 · 어댑터 왕복 시험 · 경계 산출물 구조 검증 시험 · 음성 시험
out/                  경계 아티팩트와 그림 (json · md · png · gif)
out/{swap,raw,negative,big,tight}/   변형. **이름이 아니라 디렉터리로 가른다**
stats/                내부 회계 (lambda* 등). 🔴 **경계가 아니며 out/ 밖에 둔다**
```

### 🔴 `out/` 하위 디렉터리가 무엇인가

| 디렉터리 | 무엇 | `check_sadg_acyclic.py` |
|----------|------|:----------------------:|
| `out/` | 정본 생성기 `lift` + `--on-cycle prune` | **0** |
| `out/swap/` | 구판 생성기 `swap` + `--on-cycle prune` | **0** |
| `out/raw/` | `lift` + `--on-cycle report` — **가지치기하지 않은 날것.** A4가 실제로 깨진 모습 | **1** |
| `out/negative/` | 🔴 **음성 시험** — 일부러 순환을 심었다 | **1** |
| `out/big/` | 로봇 8 묶음(`--seed 2 --robots 8 --tasks 14`) | **0** |
| `out/bigraw/` | 로봇 8 묶음의 날것(`--on-cycle report`) | **1** |
| `out/tight/` | S2의 미해결 인스턴스(계획 산출물 없음) | — |

## 검증의 한계 — 읽고 넘어가지 말 것

- `--schema-check`와 `tests/test_schema_conformance.py`가 도는 것은 **표준 JSON Schema 검증기**이며
  확장 키워드(`x-ref`·`x-unique-key`·`x-set-equal`·`x-derived-from` 등)를 **전부 무시한다.**
  그 오류 0은 «구조 검증 통과, 확장 규칙 미검사»를 뜻한다. 확장 규칙은 §4의 층 1 검증기가 본다.
- 층 1까지 돌려도 남는 것: (i) `min_separation_m`의 세 자리 동일성은 `mrs.trajectories`가 없어
  **판정 불가**, (ii) **인접성·비순환·순서 같은 의미 조항(목록 A)은 층 2**이며 층 2 검사기는 아직
  재편되지 않았다, (iii) **생성자가 자기 산출물에 검증기를 돌린 것은 판정이 아니다.**
- 계획기의 «충돌 0»은 `plan_core.validate_plan`이 **다시 계산해** 확인한 것이다. 같은 사람이 쓴
  코드이므로 이것도 판정이 아니다.
