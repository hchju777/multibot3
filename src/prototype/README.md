# 🔴 탐색용 시제 — 증거가 아니다

**C++ 본 구현에 들어가기 전에 알고리즘을 «눈으로 보고 다듬기» 위한 파이썬 시제다.
여기서 나오는 어떤 수치도 논문의 증거가 아니고, 어떤 값도 사전등록 상수가 아니다.**

## 이것이 아닌 것

| 아닌 것 | 왜 |
|--------|----|
| ROS 2 패키지가 아니다 | `COLCON_IGNORE`가 있어 `colcon build`가 이 디렉터리를 건너뛴다 |
| 본 구현이 아니다 | 본 구현은 `src/mrs_*`의 C++이며 계층·어댑터 관례(`src/CLAUDE.md` CN-1~CN-22)를 따른다 |
| 측정 하네스가 아니다 | 계측·링버퍼·분위수·검열이 하나도 없다 |
| 사전등록의 발동이 아니다 | `_workspace/27_spike_preregistration.md` §1의 값은 **동결되지 않았다.** `config/scale.yaml`의 수치가 그 문서의 수치와 우연히 같아도 **동결이 아니다** |
| 실행 계층이 아니다 | 실행 의존 그래프(SADG)·재계획·차단 대응이 없다. 애니메이션 타이밍은 **우리가 지어낸 것**이다 |
| 계약 만족의 판정이 아니다 | 아래 검사는 생성자가 자기 산출물에 돌린 것이다. 판정은 `boundary-verifier`의 몫이다 |

## 이것인 것

1. **로드맵 생성기**(`gen_roadmap.py`) → `mrs.roadmap` 2.0.0
2. **태스크 배정 생성기**(`gen_assignment.py`, 가장 가까운 것부터) → `mrs.assignment` 1.1.0
3. **경로 계획기**(`gen_plan.py`, 우선순위 기반 시공간 A\*) → `mrs.discrete_plan` 2.0.0
4. **로봇별 노드 순서표**(`plan_table.py`) → `out/plan_table.md`
5. **정적 그림**(`viz.py`) → `out/scene.png` · **애니메이션**(`anim.py`) → `out/replay.gif`

---

# 사용법

## 1. 가장 간단한 네 줄 (전부 기본값)

```bash
cd src/prototype
python3 gen_roadmap.py            # out/roadmap.json      (정점 20 · 간선 25 · 통로 2)
python3 gen_assignment.py         # out/assignment.json   (로봇 6 · 목표 16)
python3 gen_plan.py --table       # out/discrete_plan.json + out/plan_table.md (화면에도 나온다)
python3 viz.py && python3 anim.py # out/scene.png · out/replay.gif (GIF 생성에 약 9초)
```

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

### 애니메이션 (`anim.py`)

| 인자 | 무엇이 바뀌는가 |
|------|----------------|
| `--fps` · `--frames-per-tick` | 재생 속도와 부드러움 |
| `--priority` · `--horizon` · `--seed` | **계획기와 같은 값을 줘야 한다**(다르면 경고가 뜬다) |
| `--out` | `.gif`(기본) 또는 `.mp4`. **이 환경에는 `ffmpeg`가 없어 `.mp4`를 주면 GIF로 물러선다** |

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
python3 .claude/skills/multibot3-module-theory/scripts/check_boundary_schema.py \
    --schema-root src/mrs_msgs/schema \
    src/prototype/out/roadmap.json src/prototype/out/assignment.json \
    src/prototype/out/discrete_plan.json
```

| 종료 코드 | 뜻 | 🔴 주의 |
|:--------:|----|--------|
| 0 | 표준 검증 위반 0 + 확장 키워드 위반 0 | **평가된 자리 수와 함께 읽어야 한다**(출력 §2절) |
| 1 | 위반이 있다 | — |
| 2 | **판정 불가** — 대응 스키마가 없거나 묶음이 불완전하다 | **2를 통과로 읽으면 안 된다.** 아무것도 검사되지 않은 상태일 수 있다 |

- **아티팩트 파일 이름은 `{스키마이름}.json`이어야 한다.** `roadmap_big.json`처럼 꾸미면
  *"대응 스키마가 없다"*로 **UNDECIDED(2)**가 된다. 그래서 변형은 이름이 아니라 디렉터리로 가른다.
- 🔴 **디렉터리를 통째로 주지 말고 묶음마다 파일을 명시하라.** 하위 디렉터리에 같은 이름의
  아티팩트가 있으면 **하나만 보고 나머지는 조용히 무시한다** — 일부러 망가뜨린 사본을 하위
  디렉터리에 두고 확인했더니 **종료 코드 0**이 나왔다.
- 순수성(알고리즘 파라미터 유출) 검사:
  `python3 .claude/skills/multibot3-module-theory/scripts/check_schema_purity.py src/prototype/out/`

## 5. 시험

```bash
cd src/prototype
python3 -m pytest tests -q                     # 73항목
python3 -m pytest tests/test_plan_core.py -q   # 계획기만
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
plan_core.py          S2 우선순위 기반 시공간 A*  |
boundary_adapter.py   내부 <-> 경계 변환. **경계 JSON 키가 나오는 유일한 파일**
gen_roadmap.py · gen_assignment.py · gen_plan.py    CLI
plan_table.py         로봇별 노드 순서표
viz.py                정적 그림 (anim.py와 정적 층을 공유)
anim.py               애니메이션 (공칭 속도 재생)
tests/                코어 시험 · 어댑터 왕복 시험 · 경계 산출물 구조 검증 시험
out/                  생성물 (json · md · png · gif)
```

## 검증의 한계 — 읽고 넘어가지 말 것

- `--schema-check`와 `tests/test_schema_conformance.py`가 도는 것은 **표준 JSON Schema 검증기**이며
  확장 키워드(`x-ref`·`x-unique-key`·`x-set-equal`·`x-derived-from` 등)를 **전부 무시한다.**
  그 오류 0은 «구조 검증 통과, 확장 규칙 미검사»를 뜻한다. 확장 규칙은 §4의 층 1 검증기가 본다.
- 층 1까지 돌려도 남는 것: (i) `min_separation_m`의 세 자리 동일성은 `mrs.trajectories`가 없어
  **판정 불가**, (ii) **인접성·비순환·순서 같은 의미 조항(목록 A)은 층 2**이며 층 2 검사기는 아직
  재편되지 않았다, (iii) **생성자가 자기 산출물에 검증기를 돌린 것은 판정이 아니다.**
- 계획기의 «충돌 0»은 `plan_core.validate_plan`이 **다시 계산해** 확인한 것이다. 같은 사람이 쓴
  코드이므로 이것도 판정이 아니다.
