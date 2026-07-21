"""time_guard — `builtin_interfaces/Time` <-> `float` 초 변환의 가드 (파이썬 미러).

⚠ **이 모듈은 C++ `mrs::convert::seconds_to_time` / `time_to_seconds` 의 미러다.**
정본은 `convert/mrs_msg_convert/include/mrs_msg_convert/common_convert.hpp` 이며,
파이썬에서 그 함수를 호출할 통로(rclpy 바인딩)가 없기 때문에 규약만 복제한다.

복제한 규약(정본 주석 그대로):
  * 음수·NaN·무한대·`int32` 초 범위 초과는 **실패**다. 0 으로 자르거나 포화시키지 않는다 —
    조용한 치환은 창 만료 판정(불변식 W5)을 엉뚱한 시점으로 옮긴다.
  * 나노초는 반올림이 아니라 **버림**이다(같은 값이 왕복해도 커지지 않게).
  * 수신 방향은 음수 `sec` 과 `nanosec >= 1e9` 를 거부한다(rosidl 이 막지 않는다).

⛔ `1e9` 로 직접 나누는 산술을 이 파일 **밖에** 두지 말 것. 가드가 흩어지면 그 순간 무의미해진다.
⚠ 미결(contract-keeper 판단 대상): 두 벌의 구현이 드리프트할 위험이 있다. 파이썬 백엔드가
  계약 시각을 다루는 한 이 위험은 남으며, 근본 해소는 (a) 파이썬 convert 바인딩 제공 또는
  (b) 백엔드 프로세스가 시각을 전혀 다루지 않는 프로토콜 중 하나다.
"""
import math
from typing import Optional, Tuple

#: `builtin_interfaces/Time.sec` 의 표현 범위 상한 (`int32`).
INT32_MAX_SEC: int = 2147483647

#: 1 초의 나노초 수. 이 상수를 쓰는 산술은 이 파일 안에만 존재해야 한다.
NANOSECONDS_PER_SECOND: int = 1000000000


def seconds_to_time(seconds: float) -> Optional[Tuple[int, int]]:
    """도메인 시각(초)을 와이어 `Time` 의 (sec, nanosec) 쌍으로 바꾼다.

    Args:
        seconds: 시뮬 시계 기준 절대시각 [s]. 자료형 `float`. 음수·NaN·무한대는 거부된다.

    Returns:
        `Optional[Tuple[int, int]]` — 성공하면 `(sec, nanosec)`, 가드 위반이면 `None`.
        호출자는 `None` 을 받으면 **메시지를 발행하지 않고** 폐기 카운터를 올려야 한다
        (C++ 쪽 `ConvertStatus.TIME_CONVERSION_GUARD` 와 같은 취급).
    """
    if not isinstance(seconds, (int, float)):
        return None
    value = float(seconds)
    if math.isnan(value) or math.isinf(value) or value < 0.0:
        return None

    sec = int(math.floor(value))
    if sec > INT32_MAX_SEC:
        return None

    nanosec = int((value - float(sec)) * float(NANOSECONDS_PER_SECOND))
    if nanosec < 0:
        nanosec = 0
    if nanosec >= NANOSECONDS_PER_SECOND:
        # 부동소수 오차로 경계를 넘은 경우만 발생한다. 초를 올리고 나노초를 0 으로 되돌린다.
        sec += 1
        nanosec = 0
        if sec > INT32_MAX_SEC:
            return None
    return (sec, nanosec)


def time_to_seconds(sec: int, nanosec: int) -> Optional[float]:
    """와이어 `Time` 을 도메인 시각(초)으로 바꾼다.

    Args:
        sec: 와이어 초. 자료형 `int`. 음수는 거부된다.
        nanosec: 와이어 나노초. 자료형 `int`. `[0, 1e9)` 밖은 거부된다.

    Returns:
        `Optional[float]` — 성공하면 시뮬 시계 기준 절대시각 [s], 가드 위반이면 `None`.
    """
    if sec < 0 or nanosec < 0 or nanosec >= NANOSECONDS_PER_SECOND:
        return None
    return float(sec) + float(nanosec) / float(NANOSECONDS_PER_SECOND)
