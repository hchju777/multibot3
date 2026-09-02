# `config/scenario/` — 시나리오 소스 데이터

이 디렉터리의 파일은 `mrs_sim`이 기동 시 그대로 재발행하는 **경계 아티팩트 소스**다
(`roadmap_publisher`가 `roadmap.json`·`robot_specs.json`을, `task_release_publisher`가
`task_release.json`을 읽는다). `mrs_bringup` 소스 0줄 규율 아래 이 디렉터리는 **데이터**만
갖는다 — 알고리즘·기본값 코드는 여기 없다.

## `roadmap.json`

`prototype/gen_roadmap.py` 산출물을 그대로 복사한 것(`347§4-4`). instance_id
`proto_w4x3_ep8_s0`.

## `robot_specs.json` — 🔴 가정 데이터다. 확정값이 아니다

**사용자 결정 U48-5**(`_workspace/362_u48_robot_specs_assumption.md`): 실제 로봇 사양을
아직 등록·측정하지 않았으므로, **파이프라인이 흐르게 하기 위한 가정값**을 이 파일에
채웠다.

| | |
|---|---|
| 출처 | **가정**. 실제 로봇 하드웨어·등록 스펙에서 온 값이 아니다 |
| 근거 | 소형 실내 AMR(Automated Mobile Robot) 부류의 상식적 규모 — 순항 0.5 m/s,
  하드웨어 상한 1.0 m/s, 최소 제동 감속 1.0 m/s², 최소 회전 반경 2 m(곡률 상한
  0.5 · m⁻¹), 후진 가능, 외접 반경 0.35 m. **사전등록되지 않았고 어느 계측도
  뒷받침하지 않는다.** |
| 로봇별 차이 | 6대(`r0`~`r5`) 전부 같은 값 — 로봇마다 다른 사양을 가정할 근거가 없어
  가장 단순한 가정(균일 편대)을 택했다 |
| 언제 바뀌는가 | 시뮬레이터/실기 연동으로 실제 스펙이 생기는 시점. 그때 하는 일은
  **이 파일을 바꾸거나 `robot_specs_path` 런치 인자가 다른 파일을 가리키게 하는 것**뿐이어야
  한다 — 코드 변경이 필요하다면 U48-5가 안 지켜진 것이다 |
| 실험 증거 자격 | 🔴 **이 값으로 돌린 run은 `SC-*`의 증거로 계상될 수 없다** — 물리
  한계가 실제 로봇의 것이 아니면 "이 시스템이 이 로봇들에서 무엇을 달성했는가"가
  성립하지 않는다(U48-5). run 기록에 robot_specs 출처(가정 파일 경로)를 반드시 남긴다 |

## `task_release.json` — 🔴 가정 데이터(파생)다

`prototype/out/assignment.json`(같은 instance_id `proto_w4x3_ep8_s0`)에서 **역산**했다 —
그 파일의 `assignments[].goals[]`를 `task` 필드로 재그룹하면 `t000`~`t007` 여덟 태스크
각각 목표 두 개(픽업·배달)로 정확히 나뉘고, `goal_id` 오름차순이 태스크 오름차순과
일치한다(`prototype/assignment_core.py`의 `_make_tasks`/`build_assignment`가 만드는
순서 그대로). **새 값을 지어내지 않았다** — 이미 있는 산출물의 역변환이다.

`release_index`는 0부터(schema 2.0.0 §B-19 기점 규약) `t000`→0, `t001`→1, …, `t007`→7
순서로 매겼다. `task_release_publisher`(`mrs_sim`)가 이 배열의 각 원소를 **개별 `mrs.
task_release` 봉투로 그대로 재발행**한다 — 배열 자체는 계약 스키마가 아니라 이 launch가
정한 소스 파일 형식(각 원소가 `task_release.schema.json` 2.0.0에 개별적으로 유효하다).

## `blocked_edges.json` — 🔴 계약 스키마가 아니다(도구 층 내부 데이터)

375_observation_publisher.md(48차 웨이브 4-A) — `observation_node`(`mrs_sim`)가 읽는
"시각별 차단 이벤트" 시나리오. U46-3이 확정한 두 출처(런타임 토글 + 시나리오 파일) 중
**시나리오 파일 절반만** 이 라운드가 연다 — 런타임 토글(대시보드 클릭)은 소비 측
(`mrs_trajopt` 온보드 제어 틱 루프)이 선 다음 라운드다.

`schema`가 `mrs.`가 아니라 `mrs_sim.`으로 시작한다 — 일부러다. `mrs_msgs/schema/`에
등록된 경계 스키마의 이름 공간이 `mrs.*`이고, 이 파일은 `observation_node` 하나만
읽고 어떤 경계 토픽으로도 그대로 재직렬화되지 않는 도구 층 내부 데이터라서 그 이름
공간을 쓰면 안 된다(357§Q1 "도구 층" 판정, `blocked_edges_json.hpp` 파일 머리 주석).

`events[]`의 각 원소는 `{time_s, from, to, action}` — `action`은 닫힌 어휘
`{"block", "unblock"}`이다. `from`/`to`는 `/roadmap`의 간선 정점 id 쌍이며 **방향을
따지지 않는다**(양방향 간선의 물리적 차단은 방향이 없다 — `NormalizeEdgeKey`가
정규화한다). `time_s`는 절대 시각이 아니라 **`observation_node` 자신이 기동해 첫
관측 틱을 낸 뒤로 몇 번째 관측 틱이 지났는가**(`scan_publish_period_ms` 배수, 관측
틱 카운트 기반 — `observation_node.cpp::OnTick`)를 초 단위로 환산한 것이다. 파일
안 원소 순서는 의미가 없다 — 파서가 `time_s` 오름차순으로 다시 정렬한다.

**이 예시 파일이 무엇을 하는가**: `J0100`↔`J0101`(폭 1.2 m, `capacity_robots:1`인
좁은 통로, `q_v01` 통로 소속)을 5초 시점에 막고 20초 시점에 푼다. `r1`이 `J0100`에서,
`r2`가 `J0101`에서 시작하므로(`mrta.yaml` `initial_vertices`) 두 로봇 모두 기동 직후
이 간선 위에 있다 — 별도 이동 없이도 `/r1/scan`·`/r2/scan`이 차단 전후로 달라지는 것을
관찰할 수 있다.

🔴 **가정 데이터다** — `robot_specs.json`·`task_release.json`과 같은 규율: 이 시각·
간선 선택은 데모를 만들기 위한 것이지 사전등록된 시나리오가 아니다. 이 파일로 만든
run은 `SC-*` 증거로 계상되지 않는다.
