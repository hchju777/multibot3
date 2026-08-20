"""S5 — **서브골 열을 실제 궤적으로 실현**한 뒤 검사 여유를 잰다 (탐색용 시제).

🔴 **묻는 것**: 「공표 튜브 ⊆ 검사에 쓴 보수 튜브」(`204b`§3-4c 증명 대상 6호)가 **오늘 서는가.**

시제의 검사는 서브골 사이를 **직선**으로 본다. 그런데 실제 로봇은 헤딩을 갖고 움직이므로
직선을 못 그린다 — 현(弦) 밖으로 부푼다. 그 부풀음이 이격 여유를 먹으면 「위반 0건」은
**실제보다 좁은 모형으로 잰 값**이 된다.

🔴 **이탈량은 궤적 알고리즘마다 다르다.** 그래서 여기서 재는 것은 «어떤 값»이 아니라
**«알고리즘을 바꾸면 얼마나 달라지는가»**다. 달라지는 폭이 곧 **상계가 필요한 이유**이고,
상계가 알고리즘에 의존하면 모듈 교체 가능성(하네스 불변식 1)이 깨진다.

세 계열 — 전부 **같은 끝점 · 같은 시작 헤딩**을 지킨다:

| 계열 | 무엇 | 대표 |
|------|------|------|
| `straight` | 직선 보간 | **오늘의 검사** (기준선) |
| `arc` | 일정 곡률 원호 | Dubins · Reeds–Shepp 계열 |
| `hermite` | 3차 에르미트 (양끝 헤딩 일치) | 다항식·QP 계열 |

🔴 셋 다 **근사**다. 실제 QP도 추종 오차도 여기 없다 — 이 시제가 재는 것은 **기하 성분뿐**이다.
"""
from __future__ import annotations

import math

SAMPLES = 24


def _norm(a: float) -> float:
    while a > math.pi:
        a -= 2 * math.pi
    while a < -math.pi:
        a += 2 * math.pi
    return a


def realize(p0, yaw0, p1, yaw1, kind: str, n: int = SAMPLES):
    """(p0, yaw0) 에서 (p1, yaw1) 로 가는 궤적 표본. 끝점은 세 계열 모두 정확히 같다."""
    x0, y0 = p0
    x1, y1 = p1
    dx, dy = x1 - x0, y1 - y0
    L = math.hypot(dx, dy)
    if L < 1e-9 or kind == "straight":
        return [(x0 + dx * i / n, y0 + dy * i / n) for i in range(n + 1)]

    if kind == "arc":
        # 시작 헤딩을 지키며 끝점을 지나는 유일한 원호. 회전각 = 2α, R = L / (2 sinα)
        alpha = _norm(math.atan2(dy, dx) - yaw0)
        if abs(math.sin(alpha)) < 1e-6:
            return [(x0 + dx * i / n, y0 + dy * i / n) for i in range(n + 1)]
        R = L / (2.0 * math.sin(alpha))
        cx = x0 - R * math.sin(yaw0)
        cy = y0 + R * math.cos(yaw0)
        th0 = math.atan2(y0 - cy, x0 - cx)
        out = []
        for i in range(n + 1):
            th = th0 + (2.0 * alpha) * (i / n)
            out.append((cx + abs(R) * math.cos(th) * (1 if R > 0 else 1),
                        cy + abs(R) * math.sin(th)))
        # 부호 처리를 단순히 하기 위해 반지름 부호를 각도 진행에 실었다 — 끝점으로 검증한다
        ex, ey = out[-1]
        if math.hypot(ex - x1, ey - y1) > 1e-6:
            out = []
            r = abs(R)
            for i in range(n + 1):
                th = th0 + (2.0 * alpha) * (i / n) * (1.0 if R > 0 else -1.0)
                out.append((cx + r * math.cos(th), cy + r * math.sin(th)))
        return out

    if kind == "hermite":
        m0 = (L * math.cos(yaw0), L * math.sin(yaw0))
        m1 = (L * math.cos(yaw1), L * math.sin(yaw1))
        out = []
        for i in range(n + 1):
            s = i / n
            s2, s3 = s * s, s * s * s
            h00 = 2 * s3 - 3 * s2 + 1
            h10 = s3 - 2 * s2 + s
            h01 = -2 * s3 + 3 * s2
            h11 = s3 - s2
            out.append((h00 * x0 + h10 * m0[0] + h01 * x1 + h11 * m1[0],
                        h00 * y0 + h10 * m0[1] + h01 * y1 + h11 * m1[1]))
        return out

    raise ValueError(f"모르는 계열: {kind}")


def deviation(path, p0, p1) -> float:
    """실현된 경로가 **현 밖으로** 얼마나 나가는가 (검사가 쓴 직선 기준)."""
    x0, y0 = p0
    dx, dy = p1[0] - x0, p1[1] - y0
    L2 = dx * dx + dy * dy
    if L2 < 1e-12:
        return max(math.hypot(px - x0, py - y0) for px, py in path)
    worst = 0.0
    for px, py in path:
        s = max(0.0, min(1.0, ((px - x0) * dx + (py - y0) * dy) / L2))
        worst = max(worst, math.hypot(px - (x0 + s * dx), py - (y0 + s * dy)))
    return worst


def max_curvature(path) -> float:
    """표본 셋으로 외접원 곡률을 근사한다. 계열이 곡률 상한을 넘는지 보기 위한 것."""
    k = 0.0
    for i in range(1, len(path) - 1):
        (ax, ay), (bx, by), (cx, cy) = path[i - 1], path[i], path[i + 1]
        a = math.hypot(bx - ax, by - ay)
        b = math.hypot(cx - bx, cy - by)
        c = math.hypot(cx - ax, cy - ay)
        area2 = abs((bx - ax) * (cy - ay) - (by - ay) * (cx - ax))
        if a * b * c < 1e-12:
            continue
        k = max(k, area2 / (a * b * c))
    return k


def pair_min_dist(pa, pb) -> float:
    """같은 시간칸을 함께 지나는 두 실현 경로의 최소 거리. 표본 인덱스가 곧 시각이다."""
    return min(math.hypot(x1 - x2, y1 - y2) for (x1, y1), (x2, y2) in zip(pa, pb))
