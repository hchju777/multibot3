# mrs_bringup/config

파라미터 YAML 자리. **수치 창작 금지**(CLAUDE.md, mrs-implement 절대규칙)를 파일 안 표기로
강제한다 — `pysim.params.yaml` 의 모든 값에 다음 세 표기 중 하나가 붙어 있다.

| 표기 | 뜻 |
|---|---|
| `[확정]` | theory·계약이 값을 고정했다. 바꾸려면 계약 변경 게이트를 거친다 |
| `[구성]` | 시나리오 구성값. 측정 대상이 아니라 실험자가 고르는 값이다 |
| `[0a 실측]` | **placeholder.** [0a] tracer bullet 실측 전까지 근거가 없다 |

`[0a 실측]` 로 표기된 값은 지금 근거가 없다. 실측 후 이 파일에서 교체하되, **그 수치로 이후
실험의 N·반복 수를 확정하지 말 것**(오케스트레이터 R-01 조건 2 — [0a] 통과는 [0] 통과가
아니며 N·반복 수는 `TBD-pending-[0b]` 다).

## 현재 파일

- `pysim.params.yaml` — pysim 충실도 티어 전 노드 파라미터. `pysim.launch.py` /
  `isaac.launch.py` 가 `params_file` 인자 기본값으로 참조한다.

## launch 인자와의 관계

`robot_count` · `sim_backend` · `replan_period_s` · `sim_step_s` · `step_wall_period_s` 는
**launch 인자가 정본**이며 YAML 값을 덮어쓴다. YAML 의 같은 키는 `ros2 run` 으로 노드 하나만
띄울 때의 기본값이다.

특히 `sim_step_s` 는 `bringup_startup_check` 와 `sim_bridge` **양쪽에 같은 값**이 주입된다.
두 곳이 따로 놀면 R-A2 검사는 통과하는데 세계는 다른 dt 로 도는 상태가 되어 게이트가 거짓
통과한다.
