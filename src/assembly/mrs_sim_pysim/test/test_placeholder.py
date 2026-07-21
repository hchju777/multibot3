"""mrs_sim_pysim 의 unit 티어 자리표시 테스트.

이 패키지는 아직 스텁이다(로드맵 [0a]~[2] 대상). 테스트가 0건이면 pytest 가 EXIT 5 를,
unittest 가 "NO TESTS RAN" 을 내어 `colcon test` 전체가 실패하고, 그러면 **CI 게이트가
진짜 실패와 '테스트 없음'을 구별하지 못한다**. 실질 테스트가 들어올 때까지 이 파일이
그 구멍을 막는다.

`unittest.TestCase` 로 쓴 이유: colcon 의 ament_python 경로가 unittest 수집기를 쓰므로
pytest 스타일 함수만 두면 수집되지 않는다. 이 형태는 pytest·unittest 양쪽에서 수집된다.

⚠ **이 파일이 통과한다고 해서 mrs_sim_pysim 가 검증된 것이 아니다.**
"""

import importlib
import unittest


class PlaceholderTest(unittest.TestCase):
    """스텁 패키지의 최소 방어선 — 구문 오류·의존 누락을 잡는다."""

    def test_package_importable(self) -> None:
        """패키지가 import 가능한지 확인한다.

        Returns:
            None. 실패 시 assertion 이 발생한다.
        """
        module = importlib.import_module("mrs_sim_pysim")
        self.assertIsNotNone(module)
