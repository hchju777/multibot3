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
