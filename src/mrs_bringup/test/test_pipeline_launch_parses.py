# SPDX-License-Identifier: Apache-2.0
"""test_pipeline_launch_parses.py — F48-4(48차 코드 게이트).

`launch/pipeline.launch.py`의 `generate_launch_description()`가 예외 없이
파싱되고, D16 스모크 최소가 요구하는 노드·인자 개수를 실제로 담고 있는지
확인한다. `get_package_share_directory("mrs_bringup")`는 설치 상태에
의존하므로 이 시험이 그것을 임시 디렉터리로 monkeypatch한다 — 순수 구조
검사이고 `colcon test`가 `mrs_bringup`을 설치하기 전(빌드 트리)에 돌아도
성립한다.

🔴 이 시험은 "런치가 파싱된다"만 본다 — 실제 노드 기동·토픽 배선은
스모크(361_integration_p3p4.md §6)가 실행으로 확인했고 이 시험의 범위가
아니다.

🔴 이 파일은 `_workspace/CLAUDE.md`가 요구하는 `python3 -m pytest . .claude/ -q`
(레포 루트, ROS 미소싱)에도 수집된다 — `multibot3/src/prototype/tests/`가
33차부터 이미 그렇게 세어지고 있다(하네스 CLAUDE.md 변경 이력). `launch`·
`launch_ros`·`ament_index_python`은 ROS 환경에서만 있으므로, 그 환경이 없으면
**건너뛴다**(pytest `skip`, `failed`가 아니다) — 레포 루트 445건이 이 파일
때문에 흔들리지 않게 한다. `colcon test`(ROS 환경, F48-4가 요구한 곳)에서는
정상적으로 전부 돈다.
"""
import importlib.util
import os
import tempfile
from unittest import mock

import pytest

launch = pytest.importorskip("launch", reason="ROS 2 launch 미설치/미소싱 — 레포 루트 pytest 환경")
launch_ros = pytest.importorskip("launch_ros")
ament_index_python = pytest.importorskip("ament_index_python")


def _load_launch_module():
    here = os.path.dirname(os.path.abspath(__file__))
    launch_path = os.path.join(here, "..", "launch", "pipeline.launch.py")
    spec = importlib.util.spec_from_file_location("mrs_bringup_pipeline_launch", launch_path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def _generate_with_fake_share_dir():
    module = _load_launch_module()
    with tempfile.TemporaryDirectory() as tmp:
        with mock.patch.object(module, "get_package_share_directory", return_value=tmp):
            return module.generate_launch_description()


def test_generate_launch_description_does_not_raise():
    ld = _generate_with_fake_share_dir()
    assert ld is not None


def test_generate_launch_description_has_expected_actions():
    from launch.actions import DeclareLaunchArgument
    from launch_ros.actions import Node

    ld = _generate_with_fake_share_dir()
    entities = ld.entities

    node_count = sum(1 for e in entities if isinstance(e, Node))
    arg_count = sum(1 for e in entities if isinstance(e, DeclareLaunchArgument))

    # D16 스모크 최소 = 다섯 실행 파일(sadg 둘 포함: sadg_t0_node·switch_selector_node)
    # + mrs_sim 넷(clock_node·state_integrator·roadmap_publisher·
    # task_release_publisher, 367_pipeline_inputs.md 웨이브 1-A) = 9.
    assert node_count == 9, f"expected 9 Node actions, got {node_count}"
    # use_sim_time · roadmap_path · robot_specs_path · task_release_path ·
    # observation(자리 예약) = 5.
    assert arg_count == 5, f"expected 5 DeclareLaunchArgument actions, got {arg_count}"
