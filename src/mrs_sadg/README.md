# mrs_sadg — 실행 계층(SADG) 모듈

> 지연 하 재순서화(안3 순서탐색) · 봉투(안2 안전) · 후보 생성(안1) · 통로 복귀(EVAC).
> 입력 `mrs.discrete_plan` 4.0.0 → 출력 `mrs.execution_constraints` 3.0.0 (+ 가부 비트,
> `mrs.segment_release` 2.0.0) / 상향 `mrs.replan_request` 7.0.0. **코어는 ROS 비의존.**
> 정본: `_workspace/298_sadg_req_r39` · `337_arch_sadg_r2` · `340_sadg_pseudocode`.

## 레이어 (CN-1 / CN-23)

| Clean Architecture | 디렉터리 | 내용 |
|---|---|---|
| Domain | `core/` | ExecutionGraph · ConstraintCompiler(IN##) · SafetyEnvelope(SE##) · CommitGate(AG1–4) · ReleaseChecker(ET-REL) · StandbyResolver · 확장점 `i_*.hpp` |
| Application | `service/` | ExecutionTickService(ET##) · OrderSearchService(OS##) |
| Infrastructure | `adapter/` `io/` (+`plugins/`) | 경계↔내부 변환 · JSON 코덱 · 교체 구현 6종 |
| Presentation | `node/` | `sadg_t0_node`·`switch_selector_node` (ROS 골격, 통합 라운드에 활성) |

의존은 바깥→안: `service→core` · `adapter→core` · `io→adapter` · `plugins→core`.
`core/`는 ROS·JSON·pluginlib를 모른다(CN-2/CN-3).

## 확장점 3종 (각 구현 ≥2, `plugins/`)

- **IExecutionLayer** (순서탐색, `method.modules.sadg` 어블레이션 축): `LayerBranchBound`(정본) · `LayerFixedOrder`(하한).
- **IAlternativeGenerator** (후보, F37-1 ［D5］ 대향 판정): `AltHeadLift`(정본) · `AltAdjacentSwap`(구판).
- **IEscalationPolicy** (승급 사다리 0/1/2/3, 규칙 다섯): `PolicyDeficitLadder`(정본) · `PolicyConstantRung`(하한).

## 빌드·시험 (순수 CMake — ROS 없음)

```bash
cmake -S . -B build && cmake --build build && ctest --test-dir build
```

`SafetyEnvelope`는 확장점이 아니라 core 고정(어느 방법에서도 사는 안전 자리, T-DIV).
알고리즘 파라미터(theta_gain·D3·노드 예산·탐색 주기)는 경계에 없다 — `mrs_bringup/config/sadg.yaml`
(CN-6)에서 주입되는 `*Config` 필드 자리만 있다.
