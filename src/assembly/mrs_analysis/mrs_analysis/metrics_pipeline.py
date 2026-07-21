"""metrics_pipeline — 로그->지표 오프라인 통계 파이프라인 (architecture §2.5-b, D-09).

왜 Python 인가: 24~50회 반복·95% CI·paired-t 통계는 pandas/scipy 가 압도적으로 우월하고,
런타임 임계 경로가 아니다(CLAUDE.md 규율 1의 "파이썬은 꼭 필요한 경우"에 해당).

입력은 `mrs_bench`(D-09)가 산출한 로그(`SimMetricSample`·`RungEvent` 계열, mrs_metrics 가
집계)이며, 출력은 `baseline-repro` SKILL 규격의 반복·CI·paired-t 리포트다. 이 모듈은
알고리즘 없이 함수 시그니처만 고정한다 — 통계 계산 본문은 Phase 6(sim-runner) 이후 대상.
"""
import argparse
from typing import Dict, List, Sequence, Tuple


def load_logs(log_dir: str) -> List[Dict[str, object]]:
    """로그 디렉터리에서 시나리오 반복 실행 결과를 로드한다.

    Args:
        log_dir: `mrs_bench` 가 기록한 로그 파일들이 있는 디렉터리 경로.

    Returns:
        반복 실행 1건당 1개 딕셔너리로 이루어진 목록(지표 키 -> 값).

    Raises:
        NotImplementedError: 항상 — 파싱 본문은 Phase 6(sim-runner/sim-scenarios) 대상.
    """
    raise NotImplementedError(f'not implemented: load_logs({log_dir!r}) — Phase 6 대상')


def compute_confidence_interval(
    samples: Sequence[float], confidence: float = 0.95
) -> Tuple[float, float, float]:
    """표본의 평균과 신뢰구간을 계산한다 (baseline-repro 규격).

    Args:
        samples: 반복 실행에서 얻은 표본값 목록.
        confidence: 신뢰수준. 기본 0.95.

    Returns:
        (평균, 하한, 상한) 튜플.

    Raises:
        NotImplementedError: 항상 — scipy.stats 기반 계산은 Phase 6 대상.
    """
    raise NotImplementedError(
        f'not implemented: compute_confidence_interval(n={len(samples)}, confidence={confidence})'
        ' — Phase 6 대상')


def paired_t_test(baseline_samples: Sequence[float], treatment_samples: Sequence[float]) -> Dict[str, float]:
    """두 조건(baseline vs treatment)의 대응표본 t-검정을 수행한다 (D-09 비교 프로토콜).

    Args:
        baseline_samples: baseline 조건의 반복별 표본값.
        treatment_samples: 비교 대상 조건의 반복별 표본값. baseline_samples 와 길이가 같아야 한다.

    Returns:
        검정 통계량·p-value·효과크기를 담은 딕셔너리.

    Raises:
        NotImplementedError: 항상 — scipy.stats.ttest_rel 기반 계산은 Phase 6 대상.
    """
    raise NotImplementedError(
        f'not implemented: paired_t_test(n_baseline={len(baseline_samples)}, '
        f'n_treatment={len(treatment_samples)}) — Phase 6 대상')


def generate_report(results: Dict[str, object], output_path: str) -> None:
    """집계 결과로 impl-reporting 규격 리포트를 생성한다.

    Args:
        results: `compute_confidence_interval`/`paired_t_test` 산출물을 모은 딕셔너리.
        output_path: 리포트 출력 경로.

    Returns:
        None.

    Raises:
        NotImplementedError: 항상 — 리포트 렌더링은 Phase 6 대상.
    """
    raise NotImplementedError(f'not implemented: generate_report(-> {output_path!r}) — Phase 6 대상')


def main(argv: List[str] = None) -> int:
    """run_analysis 실행 파일 진입점 — CLI 인자만 파싱하고 파이프라인 호출은 스텁이다.

    Args:
        argv: CLI 인자 목록. None 이면 sys.argv 사용.

    Returns:
        프로세스 종료 코드.
    """
    parser = argparse.ArgumentParser(description='mrs_analysis — 로그->지표 오프라인 파이프라인')
    parser.add_argument('--log-dir', required=True, help='mrs_bench 로그 디렉터리')
    parser.add_argument('--output', default='report.md', help='리포트 출력 경로')
    args = parser.parse_args(argv)

    try:
        logs = load_logs(args.log_dir)
        generate_report({'logs': logs}, args.output)
        return 0
    except NotImplementedError as e:
        print(f'[mrs_analysis] stub, not yet implemented: {e}')
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
