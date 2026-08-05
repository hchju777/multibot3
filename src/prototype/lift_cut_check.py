#!/usr/bin/env python3
"""LIFT-CUT 조합 확인 — `68b_orchestrator_recheck.md`의 전수 계산을 **독립으로 다시 돌린다**.

🔴 **탐색용 시제이며 증거가 아니다. 어떤 값도 사전등록 상수가 아니다.**

무엇을 다시 계산하는가 (68b §68b-2):

1. $\\mathrm{tc}(J_g)\\subsetneq I_g$ 비율 — «커버 관계 교집합»과 «전순서 교집합»이 다른 답을
   내는 대안족의 비율. 68b는 정본 생성기(읽기 A) 부분족에서 $n{=}3$ 33 % → $n{=}6$ 84 %,
   대조(임의 순열쌍)에서 $n{=}3$ 40 % · $n{=}5$ 87 %를 얻었다.
2. 조건 **LC-4** $\\mathrm{Min}(I_g)=\\mathrm{Heads}(A_g)$ — 68b는 읽기 A($n\\le7$)·읽기 B
   ($n\\le7$ × 고정 구간 전 조합) 모두 **위반 0건**, 대조(임의 순열쌍)에서 27 %/49 % 위반.

**나는 68b의 스크립트를 보지 않았다** — 정의만 읽고 `sadg_core`의 `total_order_pairs`·
`cover_pairs`·`forced_pairs`·`minimal_items`·`head_items`로 다시 짰다. 값이 어긋나면 그것이
발견이고, 어긋난 채로 보고한다. **판정하지 않는다.**

읽기 두 가지 (68b §68b-1의 모호성):

- **읽기 A** — «앞» = 그룹 선두. `sadg_core.gen_head_lift`가 실제로 하는 것과 같다.
- **읽기 B** — «앞» = 자유 구간 선두. 고정 구간 길이 $f$ 뒤로 옮긴다.
"""

from __future__ import annotations

import argparse
import json
import os
from itertools import combinations, permutations

from sadg_core import (
    COVER_RELATION,
    TOTAL_ORDER,
    cover_pairs,
    total_order_pairs,
    transitive_closure,
)

HERE = os.path.dirname(os.path.abspath(__file__))

Order = tuple[str, ...]


# ---------------------------------------------------------------------------
# 순열 층위 도구 — `sadg_core`는 `Alternative`를 받으므로 여기서 순열 직행판을 둔다
# ---------------------------------------------------------------------------


def intersect(orders: list[Order], representation: str) -> set[tuple[str, str]]:
    """대안족의 강제 선행 관계. `sadg_core.forced_pairs`와 같은 규칙, 입력만 순열이다."""
    if representation == TOTAL_ORDER:
        return set.intersection(*[total_order_pairs(o) for o in orders])
    if representation == COVER_RELATION:
        return transitive_closure(set.intersection(*[cover_pairs(o) for o in orders]))
    raise ValueError(f"알 수 없는 표현: {representation}")


def minima(pairs: set[tuple[str, str]], items: Order) -> set[str]:
    """Min(I_g) — 선행자가 없는 항목."""
    has_pred = {y for (_, y) in pairs}
    return {x for x in items if x not in has_pred}


def heads(orders: list[Order]) -> set[str]:
    """Heads(A_g) — 어떤 대안의 선두인 항목 (항목 층위)."""
    return {o[0] for o in orders}


def lift_a(order: Order, i: int) -> Order:
    """읽기 A — $x_i$를 **열 맨 앞**으로. `sadg_core.gen_head_lift`의 식과 같다."""
    return (order[i],) + order[:i] + order[i + 1 :]


def lift_b(order: Order, i: int, fixed: int) -> Order:
    """읽기 B — $x_i$를 **고정 구간(길이 `fixed`) 바로 뒤**로."""
    rest = order[fixed:]
    j = i - fixed
    return order[:fixed] + (rest[j],) + rest[:j] + rest[j + 1 :]


def canonical_subfamilies(n: int, reading: str, fixed: int = 0) -> list[list[Order]]:
    """정본 생성기가 만들 수 있는 **부분족** 전부.

    원 순서는 언제나 들어간다(E3 불변). 나머지는 올릴 위치의 부분집합이며, 부분집합이
    비면 대안이 하나뿐이라 그룹이 발행되지 않으므로 제외한다.
    가지치기(`prune_cyclic_alternatives`)가 대안을 지우면 실제로 남는 것이 이 부분족들이다 —
    그래서 «전 위치를 다 올린 하나»만 보는 것으로는 부족하다.
    """
    base: Order = tuple(f"x{i + 1}" for i in range(n))
    movable = range(1, n) if reading == "A" else range(fixed + 1, n)
    out: list[list[Order]] = []
    positions = list(movable)
    for size in range(1, len(positions) + 1):
        for subset in combinations(positions, size):
            if reading == "A":
                out.append([base] + [lift_a(base, i) for i in subset])
            else:
                out.append([base] + [lift_b(base, i, fixed) for i in subset])
    return out


def arbitrary_pairs(n: int) -> list[list[Order]]:
    """대조군 — 생성기 제약 없는 **임의 순열쌍**. 68b의 «대조» 행과 같은 모집단이다."""
    base = [tuple(p) for p in permutations(f"x{i + 1}" for i in range(n))]
    return [[a, b] for a, b in combinations(base, 2)]


# ---------------------------------------------------------------------------
# 집계
# ---------------------------------------------------------------------------


def tally(families: list[list[Order]], items_of) -> dict:
    """한 모집단에 대해 두 물음을 한 번에 센다. 분모(가족 수)를 언제나 함께 돌려준다."""
    strict = 0
    lc4_violations = 0
    examples: list[dict] = []
    for family in families:
        items = items_of(family)
        i_g = intersect(family, TOTAL_ORDER)
        j_g = intersect(family, COVER_RELATION)
        if j_g < i_g:
            strict += 1
            if len(examples) < 3:
                examples.append(
                    {
                        "family": ["->".join(o) for o in family],
                        "I_g": sorted("<".join(p) for p in i_g),
                        "tc_J_g": sorted("<".join(p) for p in j_g),
                    }
                )
        if minima(i_g, items) != heads(family):
            lc4_violations += 1
    return {
        "families": len(families),
        "tc_J_strictly_smaller_than_I": strict,
        "tc_J_strict_ratio": round(strict / len(families), 4) if families else None,
        "lc4_violations": lc4_violations,
        "lc4_violation_ratio": round(lc4_violations / len(families), 4) if families else None,
        "examples_of_strict_gap": examples,
    }


def run(max_n: int = 7) -> dict:
    """전 모집단 집계. `max_n`이 전수 범위의 상한이며 **그 위는 보지 않았다**."""
    out: dict = {
        "note": "탐색용 시제. 조합적 열거이며 인스턴스 측정이 아니다. 증거가 아니다.",
        "max_n_exhaustive": max_n,
        "reading_A": {},
        "reading_B": {},
        "control_arbitrary_pairs": {},
        "w1_witness": {},
    }
    items_of = lambda family: family[0]  # noqa: E731 — 모든 대안이 같은 항목 집합이다

    for n in range(3, max_n + 1):
        out["reading_A"][f"n={n}"] = tally(canonical_subfamilies(n, "A"), items_of)
    for n in range(4, max_n + 1):
        for fixed in range(1, n - 1):
            fams = canonical_subfamilies(n, "B", fixed)
            out["reading_B"][f"n={n},f={fixed}"] = tally(fams, items_of)
    for n in (3, 4, 5):
        out["control_arbitrary_pairs"][f"n={n}"] = tally(arbitrary_pairs(n), items_of)

    w1 = [("x1", "x2", "x3"), ("x2", "x1", "x3")]
    out["w1_witness"] = {
        "family": ["->".join(o) for o in w1],
        "I_g": sorted("<".join(p) for p in intersect(w1, TOTAL_ORDER)),
        "tc_J_g": sorted("<".join(p) for p in intersect(w1, COVER_RELATION)),
        "Min_I_g": sorted(minima(intersect(w1, TOTAL_ORDER), w1[0])),
        "Heads": sorted(heads(w1)),
    }
    w3 = [("x1", "x2", "x3"), ("x3", "x2", "x1")]
    out["w3_witness"] = {
        "family": ["->".join(o) for o in w3],
        "I_g": sorted("<".join(p) for p in intersect(w3, TOTAL_ORDER)),
        "Min_I_g": sorted(minima(intersect(w3, TOTAL_ORDER), w3[0])),
        "Heads": sorted(heads(w3)),
    }
    return out


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="LIFT-CUT 조합 재계산 (탐색용 시제)")
    ap.add_argument("--max-n", type=int, default=7)
    ap.add_argument("--out", default=os.path.join(HERE, "stats", "lift_cut_check.json"))
    args = ap.parse_args(argv)

    data = run(args.max_n)
    os.makedirs(os.path.dirname(os.path.abspath(args.out)), exist_ok=True)
    with open(args.out, "w", encoding="utf-8") as handle:
        json.dump(data, handle, indent=2, ensure_ascii=False)
        handle.write("\n")

    print("[읽기 A — 정본 생성기 부분족]")
    for key, row in data["reading_A"].items():
        print(
            f"  {key}: 가족 {row['families']} / tc(J)⊊I {row['tc_J_strictly_smaller_than_I']}"
            f" ({row['tc_J_strict_ratio']}) / LC-4 위반 {row['lc4_violations']}"
        )
    print("[읽기 B — 자유 구간 선두]")
    viol_b = sum(r["lc4_violations"] for r in data["reading_B"].values())
    fams_b = sum(r["families"] for r in data["reading_B"].values())
    print(f"  전 조합 합계: 가족 {fams_b} / LC-4 위반 {viol_b}")
    print("[대조 — 임의 순열쌍]")
    for key, row in data["control_arbitrary_pairs"].items():
        print(
            f"  {key}: 가족 {row['families']} / tc(J)⊊I {row['tc_J_strictly_smaller_than_I']}"
            f" ({row['tc_J_strict_ratio']}) / LC-4 위반 {row['lc4_violations']}"
            f" ({row['lc4_violation_ratio']})"
        )
    print(f"[W1] I_g={data['w1_witness']['I_g']} / tc(J_g)={data['w1_witness']['tc_J_g']}")
    print(
        f"[W3] Min(I_g)={data['w3_witness']['Min_I_g']} / Heads={data['w3_witness']['Heads']}"
    )
    print(f"wrote: {os.path.abspath(args.out)}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
