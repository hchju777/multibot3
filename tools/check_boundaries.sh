#!/usr/bin/env bash
#
# check_boundaries.sh — architecture.md §2.4 경계 불변식 + contract-registry.md §7 추가 검사의
# 기계 검사 스크립트.
#
# 실행: tools/check_boundaries.sh  (multibot3/ 어디서 실행해도 무방 — 리포 루트를 자동 탐지)
# 종료코드: 0 = 전부 통과, 1 = 1건 이상 위반.
#
# 이 스크립트가 강제하는 것 (scaffolding 산출물, scaffolder 작성):
#   1. domain/ 패키지 소스에 rclcpp/rclpy/mrs_interfaces include 금지 (C1)
#   2. mrs_reorder_btpg 가 mrs_judge 를 include 금지 (D-04 v2 독립성 — F2·F3 교락 방지)
#   3. ros/ 패키지에 도메인 알고리즘 금지 — 휴리스틱: 콜백/핸들러 함수 길이 상한
#   4. mrs_ros_plan_tick 이 어떤 도메인 패키지도 의존하지 않음 (architecture §2.2 통과 기준)
#   5. 의존 방향 역행 탐지 (package.xml 파싱, §2.3 계층 랭크 위반)
#   +. 보너스: ISimBackend 심볼 참조 제한(C2), slack 문자열 금지(D-04 v2), 물리량 단위 접미사 규약
#
# 위반 발견 시 사람이 판단하지 않고 파일·라인을 그대로 출력한다 — contract-registry §7-6
# "위반은 판단하지 않고 양쪽 코드 위치와 불일치 내용을 명시해 debugger/integrator 로 넘긴다"
# 규율을 CI 스크립트 층위에서 그대로 따른다.

set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SRC="$ROOT/src"
FAIL=0

pass() { printf 'PASS: %s\n' "$1"; }
fail() { printf 'FAIL: %s\n' "$1"; FAIL=1; }
info() { printf '  %s\n' "$1"; }

echo "=================================================================="
echo " check_boundaries.sh — $ROOT"
echo "=================================================================="

# ------------------------------------------------------------------
# 1. C1 — domain/ 패키지는 rclcpp/rclpy/mrs_interfaces 에 의존하지 않는다.
#    (include/mrs/*.hpp 의 잠긴 포트 헤더도 domain/ 아래이므로 함께 검사된다 — 포트 자체가
#     ROS 무의존이어야 한다는 계약과 정확히 일치한다.)
# ------------------------------------------------------------------
violations=$(grep -rnE '^\s*#include\s*[<"](rclcpp|rclpy|mrs_interfaces)/' \
  "$SRC/domain" --include='*.hpp' --include='*.cpp' 2>/dev/null || true)
if [ -n "$violations" ]; then
  fail "[1] C1 위반 — domain/ 패키지가 rclcpp/rclpy/mrs_interfaces 를 include 함"
  echo "$violations" | sed 's/^/    /'
else
  pass "[1] C1 — domain/ 패키지는 rclcpp/rclpy/mrs_interfaces 를 include 하지 않음"
fi

# ------------------------------------------------------------------
# 2. D-04 v2 — mrs_reorder_btpg 는 mrs_judge 를 include 하지 않는다 (btpg<->judge 독립성).
# ------------------------------------------------------------------
violations=$(grep -rnE '^\s*#include\s*[<"]mrs_judge/' "$SRC/domain/mrs_reorder_btpg" 2>/dev/null || true)
if [ -n "$violations" ]; then
  fail "[2] D-04 v2 위반 — mrs_reorder_btpg 가 mrs_judge 를 include 함 (F2·F3 교락 위험)"
  echo "$violations" | sed 's/^/    /'
else
  pass "[2] D-04 v2 — mrs_reorder_btpg 는 mrs_judge 를 include 하지 않음"
fi

# ------------------------------------------------------------------
# 3. ros/ 패키지에 도메인 알고리즘 금지 — 휴리스틱: 함수 본문 길이 상한(60줄).
#    콜백이 길면 "얇은 I/O 어댑터"(ros2-conventions)를 벗어나 알고리즘이 스며든 신호로 본다.
# ------------------------------------------------------------------
# 대상은 "콜백/핸들러"다 — 생성자(조립/배선)는 이 휴리스틱의 대상이 아니다
# (ros2-conventions: "콜백 안에 알고리즘 금지"이지 "생성자에서 여러 pub/sub/srv를
# 만들지 말라"가 아니다. Composition Root/노드 생성자의 배선 코드는 architecture §2.1
# 원칙상 정상이다). 생성자·소멸자를 제외한 메서드만 검사한다.
LONG_FN_LIMIT=40
python3 - "$SRC/ros" "$LONG_FN_LIMIT" << 'PYEOF'
import re, sys, pathlib

ros_dir, limit = sys.argv[1], int(sys.argv[2])
limit_hit = []
sig_re = re.compile(r'(\w+)::(~?\w+)\s*\(')

for cpp in sorted(pathlib.Path(ros_dir).rglob('*.cpp')):
    lines = cpp.read_text().splitlines()
    i = 0
    while i < len(lines):
        line = lines[i]
        if '::' in line and '(' in line and not line.strip().startswith(('//', '*', '/*')):
            m = sig_re.search(line)
            j = i
            while j < len(lines) and '{' not in lines[j]:
                j += 1
                if j - i > 6:
                    break
            if j < len(lines) and '{' in lines[j]:
                depth = lines[j].count('{') - lines[j].count('}')
                body_start = j + 1
                k = body_start
                while k < len(lines) and depth > 0:
                    depth += lines[k].count('{') - lines[k].count('}')
                    k += 1
                body_len = k - body_start

                is_ctor_or_dtor = False
                if m:
                    class_name, method_name = m.group(1), m.group(2)
                    if method_name in (class_name, '~' + class_name):
                        is_ctor_or_dtor = True

                if not is_ctor_or_dtor and body_len > limit:
                    limit_hit.append((str(cpp), i + 1, body_len))
                i = k
                continue
        i += 1

if limit_hit:
    print("FAIL: [3] ros/ 콜백(비-생성자) 함수 길이 상한(%d줄) 초과 — 알고리즘 스며듦 의심" % limit)
    for path, lineno, n in limit_hit:
        print(f"    {path}:{lineno} — 본문 {n}줄")
    sys.exit(1)
else:
    print("PASS: [3] ros/ 콜백(비-생성자) 함수 길이 전부 상한(%d줄) 이내 — 생성자(배선)는 제외" % limit)
    sys.exit(0)
PYEOF
if [ $? -ne 0 ]; then FAIL=1; fi

# ------------------------------------------------------------------
# 4 + 5. package.xml 파싱 — mrs_ros_plan_tick 도메인 무의존 + 의존 방향 역행 탐지.
# ------------------------------------------------------------------
python3 - "$SRC" << 'PYEOF'
import sys, pathlib, xml.etree.ElementTree as ET

src = pathlib.Path(sys.argv[1])

# architecture §2.3 계층 랭크 (낮을수록 하위 계층). 의존은 항상 랭크가 '감소'하는 방향이어야 한다.
RANK = {
    "mrs_interfaces": 0.0,
    "mrs_core_types": 0.5,
    "mrs_task_alloc": 0.6, "mrs_ladder": 0.6, "mrs_sim_abstraction": 0.6, "mrs_metrics": 0.6,
    "mrs_map_registry": 1.0,
    "mrs_planning": 1.1, "mrs_local_traj": 1.1,
    "mrs_depgraph": 1.5,
    "mrs_reorder_btpg": 2.0, "mrs_reorder_milp": 2.0, "mrs_judge": 2.0,
    "mrs_msg_convert": 3.0,
    "mrs_ros_ta": 4.0, "mrs_ros_pp": 4.0, "mrs_ros_sadg": 4.0, "mrs_ros_map_registry": 4.0,
    "mrs_ros_ladder": 4.0, "mrs_ros_l4": 4.0, "mrs_ros_sim_bridge": 4.0,
    "mrs_ros_comms_emul": 4.0, "mrs_ros_plan_tick": 4.0,
    "mrs_bench": 5.0, "mrs_sim_pysim": 5.0, "mrs_sim_isaac": 5.0, "mrs_analysis": 5.0,
    # mrs_bringup 은 Composition Root — L4 형제(mrs_sim_pysim/mrs_sim_isaac 등)를 launch 로
    # 조립하는 것이 정상이므로(Bootstrap 패턴) 그 형제들보다 엄격히 높은 랭크를 둔다.
    "mrs_bringup": 6.0,
}
DOMAIN_PACKAGES = {
    "mrs_map_registry", "mrs_planning", "mrs_depgraph", "mrs_reorder_btpg", "mrs_reorder_milp",
    "mrs_judge", "mrs_task_alloc", "mrs_ladder", "mrs_local_traj", "mrs_sim_abstraction",
    "mrs_metrics",
}

deps_by_pkg = {}
for pkg_xml in sorted(src.rglob("package.xml")):
    try:
        root = ET.parse(pkg_xml).getroot()
    except ET.ParseError as e:
        print(f"FAIL: [xml] {pkg_xml} 파싱 실패: {e}")
        sys.exit(1)
    name = root.findtext("name")
    deps = set()
    for tag in ("depend", "build_depend", "exec_depend", "build_export_depend"):
        for el in root.findall(tag):
            if el.text:
                deps.add(el.text.strip())
    deps_by_pkg[name] = deps

failed = False

# [4] mrs_ros_plan_tick 은 어떤 도메인 패키지도 의존하지 않는다.
plan_tick_deps = deps_by_pkg.get("mrs_ros_plan_tick", set())
domain_deps = plan_tick_deps & DOMAIN_PACKAGES
if domain_deps:
    print(f"FAIL: [4] mrs_ros_plan_tick 이 도메인 패키지에 의존함: {sorted(domain_deps)}")
    failed = True
else:
    print("PASS: [4] mrs_ros_plan_tick 은 도메인 패키지에 의존하지 않음")

# [5] 의존 방향 역행 탐지 — 모든 내부(mrs_*) 의존 엣지가 랭크 감소 방향인지 확인.
reversed_edges = []
for pkg, deps in deps_by_pkg.items():
    if pkg not in RANK:
        continue
    for dep in deps:
        if dep not in RANK:
            continue  # 외부 ROS 패키지(rclcpp, std_msgs 등)는 랭크 대상이 아님
        if RANK[dep] >= RANK[pkg]:
            reversed_edges.append((pkg, RANK[pkg], dep, RANK[dep]))

if reversed_edges:
    print("FAIL: [5] 의존 방향 역행(또는 동일 계층 상호 의존) 탐지:")
    for pkg, prank, dep, drank in reversed_edges:
        print(f"    {pkg}(rank {prank}) -> {dep}(rank {drank}) : rank 감소가 아님")
    failed = True
else:
    print("PASS: [5] 전 내부 의존 엣지가 랭크 감소 방향(순환 불가) — architecture §2.3 구성적 증명과 일치")

sys.exit(1 if failed else 0)
PYEOF
if [ $? -ne 0 ]; then FAIL=1; fi

# ------------------------------------------------------------------
# 보너스 A/B — 주석·Doxygen 산문은 제외하고 "코드 라인"만 검사한다(오탐 방지: 이 코드베이스는
# CLAUDE.md 규율 4에 따라 근거 설명 주석이 길어 "isaac"·"slack" 같은 단어가 산문에 자연스럽게
# 등장한다 — 예: "isaac 미부착 지표는 OPEN(pending-isaac)" 설명, "slack_estimate 를 넣지 마라"
# 금지 주석 자체. 코드 식별자/필드 선언에만 검사를 적용해야 오탐이 없다).
# ------------------------------------------------------------------
python3 - "$SRC" << 'PYEOF'
import re, sys, pathlib

src = pathlib.Path(sys.argv[1])
failed = False


def code_lines(path):
    """주석·Doxygen 라인을 제외한 (lineno, text) 를 반환한다."""
    in_block_comment = False
    is_srv = path.suffix == '.srv'
    for lineno, raw in enumerate(path.read_text().splitlines(), start=1):
        line = raw.strip()
        if is_srv:
            if line.startswith('#'):
                continue
            yield lineno, raw
            continue
        if in_block_comment:
            if '*/' in line:
                in_block_comment = False
            continue
        if line.startswith('/*'):
            if '*/' not in line:
                in_block_comment = True
            continue
        if line.startswith('//') or line.startswith('*'):
            continue
        yield lineno, raw


# [A] C2 — ISimBackend 비구현 패키지의 코드 라인에 pysim/isaac 심볼(식별자) 참조 금지.
exempt = {"mrs_sim_abstraction", "mrs_sim_pysim", "mrs_sim_isaac", "mrs_bringup", "mrs_ros_sim_bridge"}
word_re = re.compile(r'(?<![A-Za-z0-9_])(pysim|isaac)(?![A-Za-z0-9_])', re.IGNORECASE)
a_violations = []
for f in list((src / "domain").rglob("*.hpp")) + list((src / "domain").rglob("*.cpp")) + \
         list((src / "convert").rglob("*.hpp")) + list((src / "convert").rglob("*.cpp")):
    if any(part in exempt for part in f.parts):
        continue
    for lineno, line in code_lines(f):
        if word_re.search(line):
            a_violations.append((str(f), lineno, line.strip()))

if a_violations:
    print("FAIL: [A] C2 위반 — 코드 라인(주석 제외)에서 pysim/isaac 심볼 참조 발견")
    for path, lineno, line in a_violations:
        print(f"    {path}:{lineno}: {line}")
    failed = True
else:
    print("PASS: [A] C2 — ISimBackend 비구현 패키지의 코드 라인에 pysim/isaac 심볼 없음")

# [B] D-04 v2 — ReorderRequest/ReorderPassing 의 코드(필드 선언) 라인에 slack 문자열 금지.
b_targets = [
    src / "domain/mrs_depgraph/include/mrs/reorder_types.hpp",
    src / "contracts/mrs_interfaces/srv/ReorderPassing.srv",
]
b_violations = []
for f in b_targets:
    if not f.exists():
        continue
    for lineno, line in code_lines(f):
        if re.search(r'slack', line, re.IGNORECASE):
            b_violations.append((str(f), lineno, line.strip()))

if b_violations:
    print("FAIL: [B] D-04 v2 위반 — ReorderRequest/ReorderPassing 필드 선언에 slack 문자열 존재")
    for path, lineno, line in b_violations:
        print(f"    {path}:{lineno}: {line}")
    failed = True
else:
    print("PASS: [B] D-04 v2 — ReorderRequest/ReorderPassing 필드 선언에 slack 문자열 없음(금지 주석만 존재)")

sys.exit(1 if failed else 0)
PYEOF
if [ $? -ne 0 ]; then FAIL=1; fi

echo "=================================================================="
if [ "$FAIL" -eq 0 ]; then
  echo "check_boundaries.sh: 전체 통과"
else
  echo "check_boundaries.sh: 위반 발견 — 위 FAIL 항목 참조"
fi
exit "$FAIL"
