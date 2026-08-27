# mrs_trajopt — 궤적 생성 모듈

세 시계(제어 50 ms ⊃ 궤적 재계산 n^traj ⊃ 서브골+공표 n^re)로 궤적을 생성한다. 코어는 **ROS 비의존**.
정본: `_workspace/322_pseudocode_r43*.md`(수도코드) · `321a/b/c`(요구·아키텍처·정식화) · 구현 노트
`_workspace/20d_impl_trajopt*.md`.

## 레이어 (CN-1)

- `core/` — 순수 알고리즘 (ROS·JSON·mrs_msgs 미링크). 세 시계 게이트·속도 프로파일러·라운드 원장·
  안전 감시·궤적 버퍼·기동 검사·선언 판별식·확장점 인터페이스(`i_*.hpp`)·SSIPP 자료구조(`core/search/`).
- `plugins/` — 교체 대상 구현 2종씩: 탐색(`SearchHybridAstar`↔`SearchFixedPathGating`, AB-SWAP-CTRL)·
  서브골 후보·팽창·가로 채널.
- `adapter/` — 내부 ↔ 경계 스키마 변환 (`from_boundary`/`to_boundary`, JSON 미포함).
- `io/` — 경계 아티팩트 JSON (de)serialization (core/adapter 밖 별도 타깃).
- `node/` — ROS 노드층 **골격만** (빌드 제외, 통합 라운드에 ROS 머신으로).

## 빌드·시험 (순수 CMake)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
ctest --test-dir build --output-on-failure   # 단위·어댑터 왕복·경계 산출물 시험
./build/mrs_trajopt_spike                     # 측정 스파이크 (p50/p95/p99, 세 거리 유도)
```

## 알려진 한계

`mrs_core` 부재로 `Status`/`Result`/`ContractViolation`을 로컬 정의(`core/status.hpp`). 탐색기는 축약판
(완전 SSIPP+RS 미구현). 로봇 간 라운드 루프·커밋 게이트는 단위 구현·시험만 되고 서비스 미배선. 자세히는
`_workspace/20d_impl_trajopt.md` §4.
