#!/usr/bin/env python3
"""lifelong 증분 삽입 시제 + p99 계측 (265d 스파이크 B부).

    python3 insertion_spike.py --out-dir <디렉터리>
    python3 insertion_spike.py --determinism-check
    python3 insertion_spike.py --ns 10,20 --seeds 3        # 축소 실행

🔴 **이 시제는 계산 예산 탐침이지 새 Lifelong MAPF 모듈의 참조 구현이 아니다.**
   비용 모형·합류 규칙·폴백은 예산의 «형태»를 재기 위한 최소 구현이며, 여기서 정한
   어떤 규칙도 설계 결정이 아니다. 수치는 파이썬 벽시계이며 논문 증거가 아니다.
🔴 **판정하지 않는다** — 원자료(CSV/JSON)와 수만 낸다.

무엇을 재나(발주 265d·근거 265b §265b-4-①):
  삽입 1회 = 새 로봇 하나의 경로 탐색(교차로 그래프, 통로 비용에 배치 합류/신설 반영)
           + 방문 순서 갱신(말미 추가 기본, 같은 방향 배치 합류 시 중간 삽입)
           + H-cons(통로 양끝 순서 정합) 유지 + (합류 시) 비순환 검증.
  스윕: 로봇 수 N ∈ {10,20,40,80}, 씨앗 반복, 체제 {배타 h=D, 추종 h=δ_follow}.
  수리 대용: 배경에서 로봇 k ∈ {1,4} 제거 후 재삽입.

수식 정본(265a): n = ⌈βT/h⌉(P-tie) · A-occ(배타)면 h = D, 추종이면 h = δ_follow ·
H-cons = 통로 양끝 방문 순서의 통로 사용자 제한 열이 (통과 단위 대응으로) 동일.
옛 식 n = ⌊βT/h⌋+1은 쓰지 않는다.

재사용: 지도는 gen_corridor_roadmap(A부), 백분위는 corridor_rotation_core._percentile.
`plan_core.py`를 쓰지 않는 이유: 그것은 내부 타임스텝 격자 + 타임스텝 단위 통로 잠금
기반이라(261 §261-6 #1) 무노드·순서-기반 삽입의 대역이 아니다(265b §265b-4-① 표).
"""

from __future__ import annotations

import argparse
import copy
import csv
import hashlib
import heapq
import json
import os
import random
import sys
import time
from dataclasses import dataclass

from corridor_rotation_core import _percentile
from gen_corridor_roadmap import (
    CorridorGenParams,
    generate_corridor_roadmap,
    validate_corridor_spike,
)
from protoscale import load_scale
from roadmap_core import RoadmapModel

SPIKE_VERSION = "prototype-insertion_spike-0.1.0"
HERE = os.path.dirname(os.path.abspath(__file__))

Entry = tuple[str, int]  # (robot, step_idx) — 방문 순서 목록의 원소


@dataclass
class Traversal:
    """통로 c의 통과 하나 — π_c(양끝 공통 제한 열)의 원소.

    i_a·i_b = 끝 정점 a·b에서의 그 로봇 step 인덱스, direction = +1(a→b) / -1(b→a).
    정준 통과 id = (robot, 진입 끝의 step 인덱스) — H-cons 검사가 양끝을 이 id로 대조한다.
    """

    robot: str
    i_a: int
    i_b: int
    direction: int

    def canon(self) -> Entry:
        return (self.robot, self.i_a if self.direction > 0 else self.i_b)


class SpikeState:
    """배경(정점 열 + 정점별 방문 순서) 전체와 통로 파생 표.

    🔴 실현 접두는 이 탐침에서 **빈 열**이다(모든 항목 미실현) — 접두 보존 의무는
    공허하게 성립하며, 접두가 실린 삽입은 재지 않았다(문서 «닫지 못한 것» 참조).
    """

    def __init__(self, model: RoadmapModel, regime: str, delta_follow_s: float,
                 speed_mps: float, allow_mid: bool = True) -> None:
        if regime not in ("exclusive", "following"):
            raise ValueError(f"체제는 exclusive/following 둘뿐이다: {regime}")
        self.model = model
        self.regime = regime
        self.delta_follow_s = float(delta_follow_s)
        self.speed = float(speed_mps)
        self.allow_mid = bool(allow_mid)

        self.steps: dict[str, list[str]] = {}
        self.origin: dict[str, tuple[str, str]] = {}  # robot -> (start, goal) 재삽입용
        self.omega: dict[str, list[Entry]] = {n.key: [] for n in model.nodes}

        self.corr_ends: dict[str, tuple[str, str]] = {}
        self.corr_len: dict[str, float] = {}
        self.edge_corridor: dict[tuple[str, str], str] = {}
        self.corr_seq: dict[str, list[Traversal]] = {}
        edge_by_pair = {(e.u, e.v): e for e in model.edges}
        for c in model.corridors:
            assert len(c.edge_keys) == 1, "이 스파이크의 통로는 간선 하나다 (A부 생성기 보장)"
            u, v = c.edge_keys[0]
            self.corr_ends[c.key] = (u, v)
            self.corr_len[c.key] = edge_by_pair[(u, v)].length_m
            self.edge_corridor[(u, v)] = c.key
            self.edge_corridor[(v, u)] = c.key
            self.corr_seq[c.key] = []

        self.adj: dict[str, list[tuple[str, float, str]]] = {n.key: [] for n in model.nodes}
        for e in model.edges:
            cid = e.corridor_key
            self.adj[e.u].append((e.v, e.length_m, cid))
            self.adj[e.v].append((e.u, e.length_m, cid))
        for k in self.adj:
            self.adj[k].sort()

    # ------------------------------------------------------------- 비용 모형
    def headway_s(self, cid: str) -> float:
        """체제별 진입 간격 h — 배타는 h=D(통로 전체 통과 시간), 추종은 h=δ_follow."""
        if self.regime == "exclusive":
            return self.corr_len[cid] / self.speed
        return self.delta_follow_s

    def transit_s(self, cid: str) -> float:
        return self.corr_len[cid] / self.speed

    def _tail_batch(self, cid: str) -> tuple[int, int]:
        """(말미 배치 방향, 크기). 비어 있으면 (0, 0)."""
        seq = self.corr_seq[cid]
        if not seq:
            return (0, 0)
        d = seq[-1].direction
        k = 0
        for tr in reversed(seq):
            if tr.direction != d:
                break
            k += 1
        return (d, k)

    def entry_penalty(self, cid: str, dirn: int) -> float:
        """통로 진입 구조 비용 [s] — 시각 예측이 아니라 배치 구조의 함수(㉠의 정신).

        말미 배치가 같은 방향: 합류 — 배치 크기 × h.
        말미가 반대 방향: 신설 = 말미 크기 × h + D(배수) 또는, 앞선 같은 방향 배치가
        있고 중간 삽입이 켜져 있으면 그 배치 말석 합류 = h — 둘 중 작은 값.
        🔴 탐침용 최소 모형이다 — 설계 결정이 아니다.
        """
        seq = self.corr_seq[cid]
        if not seq:
            return 0.0
        h = self.headway_s(cid)
        tail_dir, tail_k = self._tail_batch(cid)
        if tail_dir == dirn:
            return tail_k * h
        pen_new = tail_k * h + self.transit_s(cid)
        if self.allow_mid and any(tr.direction == dirn for tr in seq[: len(seq) - tail_k]):
            return min(pen_new, h)
        return pen_new

    # ------------------------------------------------------------- 경로 탐색
    def plan_path(self, start: str, goal: str) -> tuple[list[str], int]:
        """교차로 그래프 위 단일 로봇 최소 비용 경로(다익스트라). 결정론적.

        @return (정점 열, 팝 횟수). 도달 불가면 ValueError — 생성기 검사가 연결성을
                보장하므로 정상 경로에서는 나지 않는다.
        """
        dist: dict[str, float] = {start: 0.0}
        parent: dict[str, str] = {}
        pq: list[tuple[float, str]] = [(0.0, start)]
        pops = 0
        while pq:
            d, u = heapq.heappop(pq)
            if d > dist.get(u, float("inf")):
                continue
            pops += 1
            if u == goal:
                break
            for v, length, cid in self.adj[u]:
                w = length / self.speed
                if cid:
                    a, _b = self.corr_ends[cid]
                    dirn = +1 if u == a else -1
                    w += self.entry_penalty(cid, dirn)
                nd = d + w
                if nd < dist.get(v, float("inf")):
                    dist[v] = nd
                    parent[v] = u
                    heapq.heappush(pq, (nd, v))
        if goal not in dist:
            raise ValueError(f"경로 없음: {start} -> {goal}")
        path = [goal]
        while path[-1] != start:
            path.append(parent[path[-1]])
        path.reverse()
        return path, pops

    # ------------------------------------------------------------- 삽입/제거
    def _tail_place(self, robot: str, path: list[str]) -> None:
        """말미 추가 — 모든 정점 순서의 끝에 잇는다(T-2: 항상 비순환·H-cons 보존)."""
        for i, v in enumerate(path):
            self.omega[v].append((robot, i))
        for i in range(len(path) - 1):
            cid = self.edge_corridor.get((path[i], path[i + 1]), "")
            if cid:
                a, _b = self.corr_ends[cid]
                dirn = +1 if path[i] == a else -1
                i_a = i if dirn > 0 else i + 1
                i_b = i + 1 if dirn > 0 else i
                self.corr_seq[cid].append(Traversal(robot, i_a, i_b, dirn))

    def _successor_map(self) -> dict[Entry, list[Entry]]:
        """기존 항목 의존 그래프의 후속 사상 — 로봇 사슬 + 정점 순서 인접."""
        succ: dict[Entry, list[Entry]] = {}
        for r, s in self.steps.items():
            for i in range(len(s) - 1):
                succ.setdefault((r, i), []).append((r, i + 1))
        for lst in self.omega.values():
            for x, y in zip(lst, lst[1:]):
                succ.setdefault(x, []).append(y)
        return succ

    def _rebuild_corr(self, cid: str) -> None:
        """corr_seq[cid]를 끝 정점 a의 방문 순서 제한에서 다시 만든다(H-cons 정준 쪽)."""
        a, b = self.corr_ends[cid]
        seq: list[Traversal] = []
        for r, i in self.omega[a]:
            s = self.steps[r]
            if i + 1 < len(s) and s[i + 1] == b:
                seq.append(Traversal(r, i, i + 1, +1))  # a에서 진입, b가 i+1
            if i - 1 >= 0 and s[i - 1] == b:
                seq.append(Traversal(r, i, i - 1, -1))  # b(i-1)에서 진입, a 도착이 i
        self.corr_seq[cid] = seq

    def _mid_place(self, robot: str, path: list[str], joins: dict[int, tuple[str, int, int]]) -> bool:
        """폐포 기반 절단 삽입 — 265a §4-3의 «선행자 폐포 계산 한 번» 알고리즘 꼴.

        합류 지점에서 내 뒤가 되는 기존 항목들(S)의 **후속 폐포 D**를 구하고, 경로의
        모든 정점에서 «D의 첫 항목 앞»에 나를 넣는다. D가 후속-닫힘이라 각 정점에서
        D는 접미(suffix)를 이루고, 앞 집합이 하방폐집합이 되어 비순환이 보존된다.
        커밋 전에 비순환·H-cons를 검증하고, 실패하면 False(호출자가 말미 폴백).
        🔴 탐침용 1-패스 휴리스틱이다 — 필요충분 절단 계산은 모듈 몫(참조 구현 아님).
        """
        # 1) S = 합류 자리 뒤가 되는 기존 항목들. 합류 배치 말석(member)도 기억한다.
        after: set[Entry] = set()
        members: list[Entry] = []
        for i in sorted(joins):
            cid, _dirn, seq_idx = joins[i]
            member = self.corr_seq[cid][seq_idx]
            a, b = self.corr_ends[cid]
            pa = self.omega[a].index((member.robot, member.i_a))
            pb = self.omega[b].index((member.robot, member.i_b))
            after.update(self.omega[a][pa + 1:])
            after.update(self.omega[b][pb + 1:])
            members.append((member.robot, member.i_a))
            members.append((member.robot, member.i_b))
        # 2) D = after의 후속 폐포(기존 그래프). member가 D에 들면 합류 자리가 무의미.
        succ = self._successor_map()
        dset: set[Entry] = set(after)
        stack = list(after)
        while stack:
            e = stack.pop()
            for nxt in succ.get(e, ()):
                if nxt not in dset:
                    dset.add(nxt)
                    stack.append(nxt)
        if any(m in dset for m in members):
            return False  # 변이 전 반려 — 호출자가 말미로
        # 3) 각 경로 정점에서 «D의 첫 항목 앞» 위치 계산 후 삽입.
        pos_at: dict[int, int] = {}
        for i, v in enumerate(path):
            lst = self.omega[v]
            p = len(lst)
            for idx, e in enumerate(lst):
                if e in dset:
                    p = idx
                    break
            pos_at[i] = p
        for i, v in enumerate(path):
            self.omega[v].insert(pos_at[i], (robot, i))
        for i in range(len(path) - 1):
            cid = self.edge_corridor.get((path[i], path[i + 1]), "")
            if cid:
                self._rebuild_corr(cid)
        # 4) 커밋 검증 — 알고리즘 자신의 검사(측정 창 안). 실패면 호출자가 되돌린다.
        return self.check_acyclic() and self.check_hcons() == []

    def insert_robot(self, robot: str, start: str, goal: str) -> dict:
        """삽입 1회 — 탐색 + 순서 갱신 + (합류 시) 비순환 검증 + 실패 시 말미 폴백.

        호출자가 벽시계를 잰다. 반환 dict는 원자료 행 재료다.
        """
        path, pops = self.plan_path(start, goal)
        joins: dict[int, tuple[str, int, int]] = {}
        if self.allow_mid:
            for i in range(len(path) - 1):
                cid = self.edge_corridor.get((path[i], path[i + 1]), "")
                if not cid:
                    continue
                seq = self.corr_seq[cid]
                if not seq:
                    continue
                a, _b = self.corr_ends[cid]
                dirn = +1 if path[i] == a else -1
                tail_dir, tail_k = self._tail_batch(cid)
                if tail_dir == dirn:
                    continue  # 말미 합류는 말미 추가로 충분 — 중간 삽입 불요
                for j in range(len(seq) - tail_k - 1, -1, -1):
                    if seq[j].direction == dirn:
                        joins[i] = (cid, dirn, j)
                        break
        self.steps[robot] = list(path)
        self.origin[robot] = (start, goal)
        fell_back = 0
        verified = 0
        if joins:
            verified = 1
            if not self._mid_place(robot, path, joins):
                # 폴백: 내 항목을 걷어내고 전부 말미로 다시 — 항상 성립(T-2).
                self._strip_entries(robot)
                for i in range(len(path) - 1):
                    cid = self.edge_corridor.get((path[i], path[i + 1]), "")
                    if cid:
                        self._rebuild_corr(cid)
                self._tail_place(robot, path)
                fell_back = 1
        else:
            self._tail_place(robot, path)
        return {
            "robot": robot,
            "path_vertices": len(path),
            "search_pops": pops,
            "mid_join_candidates": len(joins),
            "mid_join_applied": len(joins) if (joins and not fell_back) else 0,
            "verify_runs": verified,
            "fell_back": fell_back,
        }

    def _strip_entries(self, robot: str) -> None:
        for v in self.omega:
            self.omega[v] = [e for e in self.omega[v] if e[0] != robot]
        for cid in self.corr_seq:
            self.corr_seq[cid] = [t for t in self.corr_seq[cid] if t.robot != robot]

    def remove_robot(self, robot: str) -> None:
        """배경에서 로봇 하나 제거 — 수리 탐침의 앞부분. 호출자가 벽시계를 잰다."""
        del self.steps[robot]
        self._strip_entries(robot)

    # ------------------------------------------------------------- 자기 검사
    def total_entries(self) -> int:
        return sum(len(s) for s in self.steps.values())

    def check_acyclic(self) -> bool:
        """의존 그래프(로봇 사슬 + 정점 순서 인접) 비순환 여부 — Kahn.

        삽입 알고리즘 자신도(합류 시) 이것을 부른다 — 측정 창 안의 실제 비용이다.
        """
        indeg: dict[Entry, int] = {}
        succ: dict[Entry, list[Entry]] = {}
        total = 0
        for r, s in self.steps.items():
            for i in range(len(s)):
                indeg.setdefault((r, i), 0)
                total += 1
            for i in range(len(s) - 1):
                succ.setdefault((r, i), []).append((r, i + 1))
                indeg[(r, i + 1)] = indeg.get((r, i + 1), 0) + 1
        for lst in self.omega.values():
            for x, y in zip(lst, lst[1:]):
                succ.setdefault(x, []).append(y)
                indeg[y] = indeg.get(y, 0) + 1
        queue = [e for e, d in indeg.items() if d == 0]
        seen = 0
        while queue:
            e = queue.pop()
            seen += 1
            for nxt in succ.get(e, ()):
                indeg[nxt] -= 1
                if indeg[nxt] == 0:
                    queue.append(nxt)
        return seen == total

    def _restricted(self, cid: str, side: str) -> list[Entry]:
        """끝 정점 side의 방문 순서를 통로 c 사용자로 제한한 정준 통과 id 열."""
        a, b = self.corr_ends[cid]
        other = b if side == a else a
        out: list[Entry] = []
        for r, i in self.omega[side]:
            s = self.steps[r]
            if i - 1 >= 0 and s[i - 1] == other:  # other -> side 통과의 도착 끝
                out.append((r, i - 1))
            if i + 1 < len(s) and s[i + 1] == other:  # side -> other 통과의 진입 끝
                out.append((r, i))
        return out

    def check_hcons(self) -> list[str]:
        """H-cons — 양끝 제한 열이 동일하고 corr_seq 캐시와도 같은지. 위반 목록 반환."""
        problems: list[str] = []
        for cid, (a, b) in self.corr_ends.items():
            sa = self._restricted(cid, a)
            sb = self._restricted(cid, b)
            if sa != sb:
                problems.append(f"{cid}: 양끝 제한 열 불일치 a={sa} b={sb}")
            cache = [t.canon() for t in self.corr_seq[cid]]
            if sa != cache:
                problems.append(f"{cid}: corr_seq 캐시 불일치 omega={sa} cache={cache}")
        return problems

    def digest(self) -> str:
        """구조 지문 — 결정론 대조용(벽시계 제외 전부)."""
        payload = {
            "steps": {r: s for r, s in sorted(self.steps.items())},
            "omega": {v: lst for v, lst in sorted(self.omega.items()) if lst},
            "seq": {c: [(t.robot, t.i_a, t.i_b, t.direction) for t in seq]
                    for c, seq in sorted(self.corr_seq.items())},
        }
        blob = json.dumps(payload, ensure_ascii=False, sort_keys=True)
        return hashlib.sha256(blob.encode("utf-8")).hexdigest()


# ------------------------------------------------------------------- 스윕 실행


def _pick_pair(rng: random.Random, nodes: list[str]) -> tuple[str, str]:
    s, g = rng.sample(nodes, 2)
    return s, g


def run_sweep(ns: list[int], regimes: list[str], seeds: list[int], *,
              delta_follow_s: float, allow_mid: bool, map_params_base: CorridorGenParams,
              repair_ks: list[int]) -> tuple[list[dict], list[dict], dict]:
    """스윕 본체. 반환 = (삽입 행들, 수리 행들, 구조 지문 모음).

    각 (체제, N, 씨앗)마다: 지도 생성(씨앗 = 지도 씨앗 = 로봇 스트림 씨앗) 후
    로봇 N대를 하나씩 순차 삽입(배경이 자란다). 삽입마다 perf_counter_ns로 벽시계.
    N 완성 후 수리 탐침: 사본에서 로봇 k대 제거 → 재삽입(같은 시작/목표).
    매 삽입·수리 뒤 **측정 창 밖에서** 비순환·H-cons 자기 검사를 돌린다.
    """
    cfg = load_scale()
    ins_rows: list[dict] = []
    rep_rows: list[dict] = []
    digests: dict[str, str] = {}
    for regime in regimes:
        for n in ns:
            for seed in seeds:
                params = CorridorGenParams(
                    cols=map_params_base.cols, rows=map_params_base.rows,
                    endpoints=map_params_base.endpoints,
                    narrow_ratio=map_params_base.narrow_ratio,
                    corridor_lengths_m=map_params_base.corridor_lengths_m,
                    wait_spurs=map_params_base.wait_spurs,
                    spur_length_m=map_params_base.spur_length_m, seed=seed,
                )
                model = generate_corridor_roadmap(cfg, params)
                assert validate_corridor_spike(model, params) == [], "지도 구조 자기 검사 실패"
                st = SpikeState(model, regime, delta_follow_s, cfg.nominal_speed_mps,
                                allow_mid=allow_mid)
                nodes = sorted(st.omega)
                rng = random.Random(1_000_003 * seed + 7)
                for k in range(n):
                    robot = f"r{k:03d}"
                    s, g = _pick_pair(rng, nodes)
                    t0 = time.perf_counter_ns()
                    info = st.insert_robot(robot, s, g)
                    dt_ms = (time.perf_counter_ns() - t0) / 1e6
                    assert st.check_acyclic(), f"삽입 후 순환: {regime} N={n} seed={seed} k={k}"
                    hc = st.check_hcons()
                    assert hc == [], f"삽입 후 H-cons 위반: {hc[:2]}"
                    row = {
                        "regime": regime, "N": n, "seed": seed, "k_before": k,
                        "insert_ms": round(dt_ms, 6),
                        "entries_total": st.total_entries(),
                    }
                    row.update(info)
                    ins_rows.append(row)
                digests[f"{regime}_N{n}_s{seed}"] = st.digest()

                # ---- 수리 탐침: 제거 k대 + 재삽입 (사본에서 — 측정 창 밖 deepcopy)
                for k_rm in repair_ks:
                    if k_rm > n:
                        continue
                    st2 = copy.deepcopy(st)
                    rng_r = random.Random(9_000_017 * seed + 13 * k_rm)
                    victims = rng_r.sample(sorted(st2.steps), k_rm)
                    origins = {v: st2.origin[v] for v in victims}
                    t0 = time.perf_counter_ns()
                    for v in victims:
                        st2.remove_robot(v)
                    remove_ms = (time.perf_counter_ns() - t0) / 1e6
                    t1 = time.perf_counter_ns()
                    fell = 0
                    for v in victims:
                        s, g = origins[v]
                        info = st2.insert_robot(v, s, g)
                        fell += info["fell_back"]
                    reinsert_ms = (time.perf_counter_ns() - t1) / 1e6
                    assert st2.check_acyclic(), "수리 후 순환"
                    assert st2.check_hcons() == [], "수리 후 H-cons 위반"
                    rep_rows.append({
                        "regime": regime, "N": n, "seed": seed, "k_removed": k_rm,
                        "remove_ms": round(remove_ms, 6),
                        "reinsert_ms": round(reinsert_ms, 6),
                        "total_ms": round(remove_ms + reinsert_ms, 6),
                        "reinsert_fell_back": fell,
                        "entries_total": st2.total_entries(),
                    })
    return ins_rows, rep_rows, digests


def aggregate_insertions(ins_rows: list[dict]) -> list[dict]:
    """(체제, N)별 삽입 1회 벽시계 통계 — 평균·p50·p99·최대 + 분모(표본 수) 동반."""
    cells: dict[tuple, list[dict]] = {}
    for row in ins_rows:
        cells.setdefault((row["regime"], row["N"]), []).append(row)
    out: list[dict] = []
    for (regime, n), group in sorted(cells.items()):
        times = sorted(r["insert_ms"] for r in group)
        late = [r["insert_ms"] for r in group if r["k_before"] >= n // 2]
        early = [r["insert_ms"] for r in group if r["k_before"] < n // 2]
        out.append({
            "regime": regime, "N": n, "samples": len(times),
            "mean_ms": round(sum(times) / len(times), 6),
            "p50_ms": round(_percentile(times, 0.50), 6),
            "p99_ms": round(_percentile(times, 0.99), 6),
            "max_ms": round(times[-1], 6),
            "mean_ms_first_half": round(sum(early) / len(early), 6) if early else float("nan"),
            "mean_ms_last_half": round(sum(late) / len(late), 6) if late else float("nan"),
            "mid_join_applied_sum": sum(r["mid_join_applied"] for r in group),
            "fell_back_sum": sum(r["fell_back"] for r in group),
            "path_vertices_mean": round(sum(r["path_vertices"] for r in group) / len(group), 3),
            "entries_total_max": max(r["entries_total"] for r in group),
        })
    return out


def aggregate_repairs(rep_rows: list[dict]) -> list[dict]:
    cells: dict[tuple, list[dict]] = {}
    for row in rep_rows:
        cells.setdefault((row["regime"], row["N"], row["k_removed"]), []).append(row)
    out: list[dict] = []
    for (regime, n, k), group in sorted(cells.items()):
        tot = sorted(r["total_ms"] for r in group)
        out.append({
            "regime": regime, "N": n, "k_removed": k, "samples": len(tot),
            "remove_ms_mean": round(sum(r["remove_ms"] for r in group) / len(group), 6),
            "reinsert_ms_mean": round(sum(r["reinsert_ms"] for r in group) / len(group), 6),
            "total_ms_mean": round(sum(tot) / len(tot), 6),
            "total_ms_max": round(tot[-1], 6),
            "reinsert_fell_back_sum": sum(r["reinsert_fell_back"] for r in group),
        })
    return out


def write_csv(path: str, rows: list[dict]) -> None:
    if not rows:
        return
    with open(path, "w", encoding="utf-8", newline="") as handle:
        writer = csv.DictWriter(handle, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        for row in rows:
            writer.writerow(row)


def main(argv: list[str] | None = None) -> int:
    ap = argparse.ArgumentParser(description="증분 삽입 계산 예산 탐침 (증거 아님·판정 없음)")
    ap.add_argument("--ns", default="10,20,40,80")
    ap.add_argument("--seeds", type=int, default=10, help="씨앗 수 (0..k-1)")
    ap.add_argument("--delta-follow", type=float, default=2.0)
    ap.add_argument("--no-mid", action="store_true", help="중간 삽입 없이 말미 추가만")
    ap.add_argument("--repair-ks", default="1,4")
    ap.add_argument("--out-dir", default=os.path.join(HERE, "out", "insertion_spike"))
    ap.add_argument("--determinism-check", action="store_true")
    args = ap.parse_args(argv)

    ns = [int(x) for x in args.ns.split(",")]
    seeds = list(range(args.seeds))
    repair_ks = [int(x) for x in args.repair_ks.split(",")]
    base = CorridorGenParams()  # cols=8 rows=6 ep=16 narrow 0.5 lengths (15,30,60) spur 끔
    regimes = ["exclusive", "following"]

    t0 = time.time()
    ins_rows, rep_rows, digests = run_sweep(
        ns, regimes, seeds, delta_follow_s=args.delta_follow,
        allow_mid=not args.no_mid, map_params_base=base, repair_ks=repair_ks)
    elapsed = time.time() - t0
    print(f"# 삽입 {len(ins_rows)}건 · 수리 {len(rep_rows)}건 · {elapsed:.1f} s", file=sys.stderr)

    if args.determinism_check:
        _, _, digests2 = run_sweep(
            ns, regimes, seeds, delta_follow_s=args.delta_follow,
            allow_mid=not args.no_mid, map_params_base=base, repair_ks=repair_ks)
        same = digests == digests2
        print(f"# determinism_identical={same} states={len(digests)}")
        if not same:
            diff = [k for k in digests if digests.get(k) != digests2.get(k)]
            print(f"# first_diff={diff[:3]}", file=sys.stderr)
            return 1

    os.makedirs(args.out_dir, exist_ok=True)
    runs_path = os.path.join(args.out_dir, "insertion_runs.csv")
    stats_path = os.path.join(args.out_dir, "insertion_stats.csv")
    rep_path = os.path.join(args.out_dir, "repair_runs.csv")
    rep_stats_path = os.path.join(args.out_dir, "repair_stats.csv")
    meta_path = os.path.join(args.out_dir, "insertion_meta.json")
    write_csv(runs_path, ins_rows)
    write_csv(stats_path, aggregate_insertions(ins_rows))
    write_csv(rep_path, rep_rows)
    write_csv(rep_stats_path, aggregate_repairs(rep_rows))
    meta = {
        "spike_version": SPIKE_VERSION,
        "not_evidence": "계산 예산 탐침. 파이썬 벽시계. 논문 증거·사전등록 상수 아님. "
                        "참조 구현 아님.",
        "ns": ns, "seeds": seeds, "regimes": regimes,
        "delta_follow_s": args.delta_follow, "allow_mid": not args.no_mid,
        "repair_ks": repair_ks,
        "map_params": {
            "cols": base.cols, "rows": base.rows, "endpoints": base.endpoints,
            "narrow_ratio": base.narrow_ratio,
            "corridor_lengths_m": list(base.corridor_lengths_m),
            "wait_spurs": base.wait_spurs,
        },
        "timer": "time.perf_counter_ns (단조 시계)",
        "measured_window": "경로 탐색 + 순서 갱신(합류 시: 후속 폐포 계산 + corr_seq "
                           "재구축 + 비순환·H-cons 커밋 검증) + 실패 시 말미 폴백. "
                           "자기 검사(비순환·H-cons 재확인)는 측정 창 밖.",
        "elapsed_s": round(elapsed, 2),
        "state_digests": digests,
    }
    with open(meta_path, "w", encoding="utf-8") as handle:
        json.dump(meta, handle, ensure_ascii=False, indent=2)
    for p in (runs_path, stats_path, rep_path, rep_stats_path, meta_path):
        print(f"# wrote {p}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
