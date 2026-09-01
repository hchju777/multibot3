# {프로젝트명} — 멀티로봇 통합 시스템

> **이 파일은 로봇 코드 레포(서브모듈)의 루트에 둔다.** 상위 하네스 레포가 아니다 —
> Claude는 작업 중인 디렉토리의 CLAUDE.md를 로드하므로, 코드 작업에 필요한 청사진은
> 코드가 있는 곳에 있어야 한다.
>
> 서브모듈을 만든 뒤 이 파일을 `CLAUDE.md`로 복사하고 `{}` 자리를 채운다.
> `templates/.clang-format`도 함께 루트로 복사한다.
>
> **이 파일은 네 모듈 공통 관례(`CN-1`~`CN-23`)의 정본이다.** 관례가 어긋났을 때 이기는 것은
> 이 파일이고, 모듈 아키텍처 문서(`_workspace/12*_arch_*.md`)가 아니다 — **왜냐하면 구현 단계에
> 아무도 남의 모듈 아키텍처 문서를 열지 않기 때문이다.** 사유와 대안은 `_workspace/46_convention_canon.md`.

## 아키텍처

```
[시나리오·로드맵]
      |
      v
[mrs_mrta]  --mrs.assignment-->  [mrs_mapf]  --mrs.discrete_plan-->  [mrs_sadg]
                                                                          |
                                                          mrs.execution_constraints
                                                                          |
                                                                          v
                                                                    [mrs_trajopt]
                                                                          |
                                                                  mrs.trajectories
                                                                          |
                                                                          v
                                                                     [실행·시뮬]

모든 경계는 mrs_msgs가 정의한다. 모듈 패키지는 서로를 직접 의존하지 않는다.
```

## 파일 구조

> **아래 블록은 패키지 개요다.** 모듈 **내부** 배치의 정본은 이 파일의 「모듈 공통 관례」
> `CN-1`(레이어 5디렉터리)이며, 아래 블록의 `include/mrs_mrta/allocator.hpp` 같은 평면 배치는
> 관례 정본이 서기 전(하네스 1.0.0)의 표기다. **충돌하면 `CN-1`이 이긴다.**
> 패키지 수도 프로젝트에 따라 늘 수 있다(예: `mrs_baselines`·`mrs_sim` 신설 시 10종).

```
src/
  mrs_msgs/                      - 경계 메시지 정의. 유일하게 모든 패키지가 의존하는 패키지
    msg/Assignment.msg           - mrs.assignment
    msg/DiscretePlan.msg         - mrs.discrete_plan (순서 인덱스 + 방문 순서, 타임스텝 없음)
    msg/ExecutionConstraints.msg - mrs.execution_constraints (세그먼트·의존·스위치 그룹)
    msg/Trajectories.msg         - mrs.trajectories (물리 제약 포함)
    msg/Roadmap.msg              - mrs.roadmap

  mrs_mrta/                      - 태스크 할당
    include/mrs_mrta/allocator.hpp   - 알고리즘 인터페이스 (교체 지점)
    src/allocator_node.cpp           - ROS 2 노드 껍데기
    src/adapter.cpp                  - 내부 표현 <-> 경계 스키마
    test/test_allocation_valid.cpp   - 할당 유효성 (모든 태스크 1회, 용량·선행)
    test/test_adapter_roundtrip.cpp  - 어댑터 왕복 무손실

  mrs_mapf/                      - 충돌 없는 이산 경로계획
    include/mrs_mapf/solver.hpp      - 솔버 인터페이스 (교체 지점)
    src/solver_node.cpp
    src/adapter.cpp                  - 내부 시간 인덱스 -> 경계 순서 인덱스 변환
    test/test_conflict_free.cpp      - 정점·간선 충돌 0
    test/test_plan_graph_acyclic.cpp - Type-1 + Type-2 의존 그래프 비순환
    test/test_adapter_roundtrip.cpp

  mrs_sadg/                      - 지연 하 재순서화
    include/mrs_sadg/graph.hpp       - SADG 구성
    include/mrs_sadg/switch_selector.hpp - 스위칭 선택 (교체 지점)
    src/sadg_node.cpp
    src/adapter.cpp
    test/test_sadg_acyclic.cpp       - ★ 자유부 축소 조합의 전수 비순환 (컴파일 층)
    test/test_prop_accept.cpp        - ★ 증분 비순환 수락 게이트 (런타임 층). 불변식 4 참조
    test/test_switch_latency.cpp     - 재선택 지연이 제어 주기 예산 안
    test/test_adapter_roundtrip.cpp

  mrs_trajopt/                   - 궤적 최적화
    include/mrs_trajopt/optimizer.hpp - 최적화기 인터페이스 (교체 지점)
    src/trajopt_node.cpp
    src/adapter.cpp
    test/test_limits.cpp             - 속도·가속도·저크 한계
    test/test_separation.cpp         - 샘플 사이 구간 포함 최소 이격
    test/test_dependency_respect.cpp - 의존 제약 준수

  mrs_bringup/                   - 배선. launch, 파라미터, 네임스페이스, 시뮬레이션 시간
    launch/pipeline.launch.py
    config/qos.yaml
    config/{방법명}.yaml             - 알고리즘 파라미터가 사는 곳. 경계가 아니다
    test/test_pipeline_smoke.py      - end-to-end 스모크

  mrs_eval/                      - 실험 하네스. run 실행, 지표 산출, 결과 덤프
    scripts/run_experiment.py        - 하나의 run_id를 처음부터 끝까지 실행
    scripts/dump_artifacts.py        - 경계 아티팩트를 JSON으로 덤프 (체커 입력)
    test/test_metrics.py

.clang-format                    - templates/.clang-format 사본
```

**테스트 파일은 1급 시민이다.** 새 모듈을 만들 때 테스트 파일을 이 구조에 먼저 적고
시작한다 — **왜냐하면** 구조에 명시되지 않은 테스트는 거의 작성되지 않고,
테스트 없는 코드로 만든 실험 숫자는 논문의 증거가 될 수 없기 때문이다.

## 핵심 불변식

1. **모듈 패키지는 형제 모듈 패키지를 의존하지 않는다.** `mrs_msgs`만 의존한다.
   `package.xml`에 형제 모듈이 등장하면 그 자체로 결함이다.
2. **경계 스키마에 알고리즘 파라미터를 넣지 않는다.** k-robustness 윈도, 서브옵티멀 계수,
   지평 길이, 솔버 종류, 스레드 수, 타임아웃, 시드는 `mrs_bringup/config/`에 산다.
   물리·문제 파라미터(`v_max`, `a_max`, `j_max`, `min_separation_m`)만 경계에 실린다.
3. **상류 경계에 절대 시각이 없다.** `DiscretePlan`은 0부터 시작하는 순서 인덱스와
   공유 위치의 방문 순서만 싣는다. 연속 시간은 `Trajectories`에서만 등장한다.
4. **SADG는 «공칭 순서가 비순환이고 런타임 불변식이 지켜지는 한» 비순환이다** (가정 A4-W).
   두 부분으로 나뉜다 — **(W1)** 공칭 순서 하나가 비순환일 것(상류 경로계획이 보장한다),
   **(W2)** 런타임 불변식 셋이 성립할 것: 지정 갱신이 증분 비순환 검사를 통과할 때만 반영되고
   (PROP-ACCEPT), 릴리스 층이 현재 지정을 정확히 강제하며, 이미 개시된 위치를 재인증이
   탈인증하지 않는다(RECERT-LOCK).
   🔴 **「임의의 스위칭 조합이 비순환」이라는 옛 가정 A4는 철회됐다** — 반례가 실측으로
   20/20 나왔다. 그것을 인스턴스 요구로 되살리지 않는다.
   검사는 두 층이다: **컴파일 시** 자유부의 축소된 조합 집합에 대한 전수 비순환,
   **런타임** 증분 검사(PROP-ACCEPT). 현재 선택만 검사하는 테스트는 앞의 층을 검증하지 못하고,
   컴파일 전수 검사만으로는 뒤의 층을 검증하지 못한다.
   정본: `_workspace/68_formulation_r7.md` §R7-2 · `_workspace/11c_req_sadg_r4.md` §V-3.
5. **제어 주기 {50} ms.** 스위칭 결정은 이 안에 완료한다. p50이 아니라 **p99** 기준이다.
6. **내부 표현은 어댑터를 통해서만 경계로 나간다.** 알고리즘 자료구조가 메시지에
   그대로 실리면 모듈 교체 가능성이 깨진다.

## 모듈 공통 관례 (`CN-1`~`CN-23`) — **이 절이 정본이다**

네 모듈(`mrs_mrta`·`mrs_mapf`·`mrs_sadg`·`mrs_trajopt`)에 **같은 어휘**를 쓴다.
어기려면 사유를 남기고 이 절을 개정한다. **조용히 다르게 쓰지 않는다** — 어휘가 갈리면
그 차이는 통합 시점에야 드러나고 그때는 전부 뜯어야 하기 때문이다.

각 관례에 **강제 수단**을 적는다. **문서에만 적힌 관례는 안 읽으면 그만이기 때문이다.**
`빌드`=어기면 링크·컴파일이 깨진다 / `CI`=`check_layer_layout.py` 또는 `git diff` 검사 /
`사람`=**기계 강제 수단이 없다. 리뷰로 잡는다.**

### 배치·의존

| # | 관례 | 강제 |
|:-:|------|:----:|
| **CN-1** | **레이어 디렉터리는 최대 6개**: `node/`(ROS 의존) · `service/`(유스케이스 1회) · `core/`(순수 알고리즘) · `adapter/`(경계↔내부) **필수 넷** · `ports/`(시임 — 모듈 고유 시임 있는 모듈만) · **`io/`(경계 JSON 텍스트 ↔ 경계 표현 구조체를 다루는 모듈만)**. `io/`는 `adapter/boundary_types`에만 의존하고 **`core`를 모른다**(JSON을 core·adapter 어디에도 넣지 않는다 — CN-2/CN-3의 확장). ROS wire만 쓰고 파일 코덱이 없는 모듈은 `io/`를 두지 않는다(가법적 차이 CN-4). `include/{pkg}/`와 `src/`가 같은 이름으로 미러링. **여섯(+`plugins/`, CN-5) 밖의 이름 금지**(`core/` 하위 세분은 허용). `plugins/`는 CN-5가 요구하는 교체 구현 자리라 화이트리스트에 포함된다 — `check_layer_layout.py`도 7종을 허용한다. 45차 FIX-1 처분(가) — `_workspace/337b_fix1_disposition.md` | CI |
| **CN-2** | **`core/`·`ports/`는 별도 타깃 `mrs_{모듈}_core`이고 `rclcpp`·`rosidl`·`mrs_msgs`·JSON·`pluginlib`에 하나도 링크하지 않는다.** 플러그인 `.so`도 이 타깃 + 등록 매크로만. **어기면 링크가 깨진다 — 이것이 육각형의 전부다** | **빌드** |
| **CN-3** | **`mrs_core`는 두 타깃**: `mrs_core_pure`(시임·링버퍼·히스토그램·그래프 술어 — ROS·JSON 미링크) / `mrs_core_msgs`(경계 표현·JSON). `core/`·`ports/`·`plugins/`는 **pure에만**. 한 타깃이면 `core/`가 `mrs_msgs`를 전이로 얻어 `CN-2`가 이름뿐이 된다 | **빌드** |
| **CN-4** | **`ports/`는 «모듈 고유» 시임이 있을 때만 둔다.** 공용 시임(`ISteadyClock`·`IInstrSink`)은 `mrs_core`에 하나만 있고 **모듈이 재정의하지 않는다.** `ports/`가 없는 것은 위반이 아니다 | CI |
| **CN-5** | **교체 대상 구현은 `src/{pkg}/plugins/**`에만** 산다. 어블레이션의 통과 기준이 «플러그인 밖 소스 수정 0줄»이고 그것을 **경로로** 검사하기 때문이다 | CI(`git diff`) |
| **CN-6** | **알고리즘 파라미터는 `src/mrs_bringup/config/{모듈}.yaml` 하나에만.** 모듈 패키지에 `config/`를 두지 않는다. QoS는 `mrs_bringup/config/qos.yaml` | CI |
| **CN-7** | **config 키 경로는 `method.modules.{모듈}.*`.** 알고리즘 교체는 이 트리의 **한 줄** | CI |
| **CN-21** | **형제 의존 0건.** `package.xml`·`CMakeLists.txt`에 `mrs_msgs`·`mrs_core`·자기 자신 밖의 `mrs_*`가 없다 | CI(grep) |
| **CN-22** | **`test/core/`·`test/adapter/`가 각각 파일 1개 이상** — 노드를 띄우지 않고 도는 시험 | CI |
| **CN-23** | **Clean Architecture 대응**(U46-1): Domain=`core/` · Application=`service/`+`ports/` · Infrastructure=`adapter/`+`io/`(+swap `plugins/`) · Presentation=`node/`. **의존은 항상 바깥→안**; `core/`는 ROS·JSON·pluginlib 무지(CN-2/CN-3의 재진술, 새 강제 없음). 확장점 `I{역할}`은 **core/(Domain) 소유**, plugins가 구현(의존 역전). 매핑표는 「Clean Architecture 대응」절 | CI |

### Clean Architecture 대응 (CN-23 상세)

| CA 레이어 | 디렉터리 | 소유 |
|---|---|---|
| Domain | `core/`(+세분, `I{역할}` 포함) | 순수 알고리즘·엔티티·확장점 인터페이스 |
| Application | `service/`+`ports/` | 팩토리(CN-11)·2차 시임(CN-4) |
| Infrastructure | `adapter/`+`io/`(+`plugins/`) | 경계 변환·JSON 코덱·교체 구현 |
| Presentation | `node/` | ROS 노드 |

의존은 바깥→안(`node→service→core`, `adapter→core`, `io→adapter`, `plugins→core`).
`ports/`는 Application의 2차(driven) 시임 자리다. **알고리즘 교체 인터페이스 `I{역할}`은
그보다 안쪽인 `core/`(Domain)가 소유한다(CN-10)** — 이것이 어블레이션 교체가 `core/` 아래로
국한되는 근거다. 물리 디렉터리 재편은 하지 않는다(U46-1: 대응 명문화, 재편 기각).

### 명명

| # | 관례 | 강제 |
|:-:|------|:----:|
| **CN-8** | 어댑터 클래스 **`{경계아티팩트}Adapter`**, 파일 `adapter/{snake}_adapter.{hpp,cpp}` | CI |
| **CN-9** | 어댑터 함수는 **`from_boundary(const Msg&, Internal& out)` / `to_boundary(const Internal&, Msg& out)` 두 이름뿐.** 출력 인자로 쓰고 반환은 상태다(어댑터가 할당하지 않는다). **필요한 방향만 구현해도 된다** — 소비만 하는 이음매에 반대 방향을 만들면 아무도 부르지 않는 코드가 남는다. **`to_msg`·`parse`·`serialize` 같은 다른 이름의 경계 변환 함수 금지** | CI |
| **CN-10** | 확장점 인터페이스 **`I{역할}`**, 헤더 **`core/i_{역할}.hpp`**(snake). 순수 가상이며 **`pluginlib` 헤더를 포함하지 않는다** | CI |
| **CN-11** | 팩토리 **`{역할}Factory`**, **`service/`에 둔다**(`core/`에 두면 `pluginlib` 의존이 `core` 타깃에 붙어 `CN-2`가 깨진다). **`ClassLoader`를 인스턴스보다 오래 소유한다** — 로더가 먼저 죽으면 코드 섹션이 언로드된 객체를 부른다 | CI(이름·위치) / **사람**(수명) |
| **CN-12** | **플러그인 키는 `{패키지}/{ClassName}`**(예: `mrs_mapf/LacamStar`). `plugins.xml`에 맨키 `name` 속성을 붙이지 않고 config 값도 같은 형식으로 적는다. **맨키를 쓰면 맨키↔정식 키 매핑 표가 소스에 생기고, 계열을 더할 때 그 표를 고쳐야 해 «플러그인 밖 소스 수정 0줄»이 정의상 깨진다** | **빌드**(`pluginlib`) + CI |

### 오류·통지

| # | 관례 | 강제 |
|:-:|------|:----:|
| **CN-13** | **값이 없는 성공/실패는 `mrs_core::Status` 반환.** 어댑터·검사·적용 함수가 여기 든다 | **사람** |
| **CN-14** | **값을 동반하는 결과는 `mrs_core::Result<T,E>` 반환**(예: `solve`가 통계를 낸다). **`Result<void,E>` 금지** — `Status`와 같은 것을 두 이름으로 표현하면 구현자가 물을 어휘가 사라진다. `Result` 정의에 `static_assert(!std::is_void_v<T>)`를 둔다(이것이 막는 것은 **절반뿐**이다) | **사람**(절반은 빌드) |
| **CN-15** | **계약·불변식 위반은 `mrs_core::ContractViolation` 예외.** 던지는 곳은 **기동 경로뿐** — 플러그인 로드 실패·필수 키 부재·기동 검사 실패. `main`이 잡아 **종료 코드 1**, **폴백 없음** | 기동 검사 |
| **CN-16** | **제어 주기 콜백·틱 경로에서 예외를 던지지 않는다.** 틱 안의 위반은 `fatal_latch` + 치명 카운터 + 안전 정지 선언으로 바꾼다. **콜백에서 예외가 실행기를 빠져나가면 프로세스가 죽고 run이 조용히 멈춘다 — 조용한 중단은 위반보다 나쁘다** | CI(얕은 grep) |
| **CN-17** | **상류 통지**: 액션이 있는 모듈은 **액션 result의 닫힌 어휘 `reason`**, 액션이 없는 모듈은 `/diagnostics` + `result.json` 키 | **사람** |

### 시임·계측·재현성

| # | 관례 | 강제 |
|:-:|------|:----:|
| **CN-18** | **시계는 `mrs_core::ISteadyClock` 하나**, 계측 싱크는 `mrs_core::IInstrSink`. 모듈 고유 시임만 `ports/I*`. **`core/`·`plugins/`에 `std::random_device`·`rand()`·시계 직접 호출 0건**, 시드는 config로 주입한다. **재현성이 시계·난수·경계 입력 셋에만 걸려 있다** | CI(grep) |
| **CN-19** | **계측 3종 세트**: 고정 크기 링버퍼 + **고정 버킷** 히스토그램 + 드롭 카운터, **1 Hz `/diagnostics`**. 틱 안에서 파일·로그·문자열 조립 0건 | **사람** |
| **CN-20** | **`result.json`의 같은 키를 두 모듈이 생산하지 않는다.** 생산자가 갈리면 집계기가 어느 쪽을 썼는지 사후에 알 수 없다 | **사람** |

### 이 관례를 검사하는 것

```bash
# 빌드/CI 검사다. 기동 검사(C-#)가 아니다 — 소스 트리의 배치를 본다.
python3 check_layer_layout.py --src-root src/
#   0 = 통과 / 1 = 위반 / 2 = 판정 불가(서브모듈 미초기화). 2를 통과로도 실패로도 읽지 않는다.
```

**강제 수단이 없는 넷은 사람이 잡는다**: `CN-13`·`CN-14`의 «자리»(값 없으면 `Status`,
값 있으면 `Result`) · `CN-17` 통지 어휘 · `CN-19` 계측 3종 · `CN-20` 키 생산자 유일성.
**리뷰 체크리스트에 이 네 줄을 그대로 넣는다.**

## 규칙

- **빌드·테스트**: `colcon build --symlink-install` / `colcon test --event-handlers console_direct+`
- **포맷**: `clang-format -i` (Allman 중괄호, 4-space, 열 폭 100).
  검사만: `clang-format --dry-run --Werror`
- **모든 공개 함수에 Doxygen 블록** (`@brief`, `@param`, `@return`).
  clangd가 이것을 hover 툴팁으로 렌더하므로 형식을 지킨다.
- **시뮬레이션 시간**을 쓴다 (`use_sim_time: true`). 벽시계 시각에 의존하는 로직은
  그 자체로 재현성 결함이다.
- **시드를 명시적으로 주입**한다. 난수 스트림은 소비 지점마다 독립이다.
- **커밋 메시지는 `[Add]` / `[Update]` / `[Fix]` / `[Del]` / `[Hotfix]` 중 하나로 시작**한다.
  예: `[Fix] Fix SADG cycle detection on empty switch group`
- **버전은 `MAJOR.MINOR.PATCH` git tag.** MAJOR는 공개 API 변경, MINOR는 기능 추가,
  PATCH는 버그 수정. 경계 메시지의 하위 호환 파괴는 반드시 MAJOR다.
- 상위 하네스 레포의 `_workspace/13_interface_contract.md`가 계약의 원본이다.
  코드가 계약과 어긋나면 코드를 고치거나 계약 개정 절차를 밟는다. 조용히 벌리지 않는다.

## 변경 이력

| 날짜 | 버전 | 변경 내용 | 대상 | 사유 |
|------|------|----------|------|------|
| 2026-08-27 | - | **FIX-1 처분 확정(45차, `_workspace/337b_fix1_disposition.md`)**: `io/`를 **여섯째 레이어로 승인**(모듈 선택적). `mrs_trajopt`의 `io/json_io`는 JSON 텍스트↔경계 표현 구조체 변환이고 `adapter/`(경계 표현↔내부 표현)와 **다른 관심사**임을 실물로 확인(`io/json_io.hpp` 의존이 `boundary_types`뿐, core 미의존). 재배치(나)를 기각한 이유: JSON을 adapter에 들이면 wire-format churn과 표현 매핑 churn이 결합되고 adapter 순수성이 약해진다. U45 리플레이·원클릭 재현이 JSON 경계 아티팩트를 첫급으로 요구. 다른 모듈도 같은 배치 허용. 아래 편차 기록 행은 발견의 기록으로 보존한다 | `mrs_trajopt`·`mrs_sadg` 배치 · 이 표 · CN-1 | 편차→정본 승격 |
| 2026-08-27 | - | **편차 기록(처분 미정)**: `mrs_trajopt`가 `include/·src/` 아래 **`io/` 디렉터리**를 둔다 — `CN-1` 5레이어(node/service/core/adapter/ports) 밖이다. 경계 JSON 파일 입출력을 어댑터와 갈라 두려는 배치이며, **코어 순수성(`CN-2`/`CN-3`)은 유지됨을 44차 코드 게이트가 실물로 확인했다**(`_workspace/22_code_review.md` FIX-1). 🔴 **처분은 다음 라운드 결정이다** — (가) `CN-1`을 개정해 `io/`를 여섯째 레이어로 승인(다른 모듈도 같은 배치 허용) (나) `adapter/` 아래로 재배치(5레이어 유지). 그때까지 다른 모듈이 이 편차를 **선례로 복제하지 않는다.** `check_layer_layout.py`는 이 레포에 아직 없어 `CN-1`은 수기 판정이다 | `mrs_trajopt` 배치 · 이 표 | 관례 정본의 원칙 — *어기려면 사유를 남기고 이 절을 개정한다. 조용히 다르게 쓰지 않는다* |
| {YYYY-MM-DD} | 0.1.0 | 초기 구성 | 전체 | - |
| 2026-08-02 | 0.2.0 | **「모듈 공통 관례」 절 신설**(`CN-1`~`CN-22`) — 정본을 `12a_arch_mrta_p2.md` §10-3에서 이 파일로 이동. 어긋난 여섯 항목 해소(계층 디렉터리 수·오류 반환 타입·플러그인 키 형식·config 위치·단방향 어댑터·`mrs_core` 타깃 분할) + 관례마다 **강제 수단** 명시 | 이 파일 | 모듈 아키텍처 문서 안에 있으면 **구현 단계에 아무도 열지 않는다.** 그리고 **강제 수단이 없는 관례는 안 읽으면 그만이므로** 있는 것과 없는 것을 갈라 적어야 한다. 근거는 `_workspace/46_convention_canon.md` |
