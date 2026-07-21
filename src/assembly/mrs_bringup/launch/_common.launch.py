"""_common.launch.py — mrs_bringup 공용 조립 로직 (Composition Root).

pysim.launch.py / isaac.launch.py 가 이 파일을 `sim_backend` 인자만 바꿔 재사용한다 —
"백엔드 선택은 config 한 줄"(architecture §5.1, R-01)의 실현.

## 기동 게이트 (R-A2) — 이 파일의 핵심
`bringup_startup_check` 를 **먼저** 실행하고, 그 프로세스의 **종료코드가 0 일 때에만** 나머지
노드를 띄운다. 검사 노드를 다른 노드와 나란히 실행하기만 하면 검사가 **실패해도 시스템이
그대로 뜬다** — 게이트가 있으나 잠기지 않은 상태다.

R-A2 위반(Δt_h 가 시뮬 스텝 dt 의 정수배가 아님)은 `/clock` 점프를 틱 경계와 어긋나게 만들어
"전 에이전트가 같은 틱에 재계획"을 시뮬에서 성립하지 않게 하고, 그러면 **F5 가 정리 T1 을
검증하지 못한다**. 전제 미성립 상태로 실험을 돌리지 않는다는 것이 이 게이트의 취지다
(N2 통로폭 검사기와 같은 취급).

같은 이유로 `sim_step_s` 는 **검사 노드와 `sim_bridge` 에 같은 값이 주입**된다. 두 곳이
따로 놀면 검사는 통과하는데 세계는 다른 dt 로 도는 상태가 되어 게이트가 거짓 통과한다.

## 두 번째 기동 게이트 (R-05)
`/plan_tick` 발행자 수 검사는 **`mrs_ros_l4` 안에** 있다(architecture §1.7-A, R-05). launch 는
`plan_tick_node` 를 **정확히 1개**만 띄우는 것으로 그 전제를 만든다.

## 네임스페이스 규약 (nav2-reference §2 F3 변형)
전역 토픽(`/clock`·`/plan_tick`·`/sim/*`)은 노드 코드에서 **절대 이름**으로 쓴다. per-robot
노드만 `namespace=robot_{i}` 를 받는다. 어느 것이 절대/상대인지는 계약 §3 정본표가 정한다.

## ⛔ launch 인자를 dict 로 넘기지 않는다 ([0a] 보고서 §4-① 수리)
`Node(parameters=[params_file, {'robot_count': 4}])` 형태는 **작동하지 않는다.** launch_ros 는
그 dict 를 임시 YAML 에 **`/**:` 와일드카드**로 쓰는데, rcl 은 **정확한 노드 이름이 와일드카드를
이긴다**(파일 순서 무관). 따라서 `pysim.params.yaml` 에 `sim_bridge:` 블록의 같은 키가 있으면
launch 인자가 **조용히 진다** — [0a] 는 `robot_count:=4` 로 띄우고도 `sim_bridge.robot_count = 2`
로 돌았고, 그 결과 `initial_x_m` 길이 불일치 기동 거부 가드가 **발동조차 하지 않았다**.

그래서 이 파일은 **파라미터 파일 자체를 생성**한다(@ref _write_resolved_params_file). 기준 YAML 을
읽어 launch 인자를 **정확한 노드 이름 블록**에 써 넣고, 그렇게 만든 **파일 하나만** 전 노드에
넘긴다. 오버라이드가 와일드카드로 내려가는 경로가 구조적으로 사라지므로, 나중에 누가 기준
YAML 에 같은 키를 다시 넣어도 침묵 실패가 재발하지 않는다(같은 파일 안에서는 정확한 이름이
이기고, 그 정확한 이름 값을 launch 가 방금 써 넣었다).

이 파일은 배선(WHO talks to WHOM)만 담당한다 — 알고리즘은 없다(ros2-conventions 규율).
"""
import ast
import copy
import os
import tempfile

import yaml

from ament_index_python.packages import get_package_share_directory

from launch import LaunchDescription
from launch.actions import (
    DeclareLaunchArgument,
    LogInfo,
    OpaqueFunction,
    RegisterEventHandler,
    Shutdown,
)
from launch.event_handlers import OnProcessExit
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node

#: 생성된 파라미터 파일 경로를 `_launch_setup` → `_system_nodes` 로 나르는 컨텍스트 키.
#: 두 함수가 **같은 파일**을 봐야 R-A2 검사 노드와 `sim_bridge` 의 `sim_step_s` 일치가 성립한다.
_RESOLVED_PARAMS_KEY = 'mrs_resolved_params_file'


class _NoAliasDumper(yaml.SafeDumper):
    """YAML 앵커·별칭(`&id001` / `*id001`)을 절대 만들지 않는 덤퍼.

    ⚠ 이 클래스가 없으면 기동이 **깨진다.** PyYAML 은 같은 파이썬 객체가 두 번 실리면 두 번째를
    별칭으로 쓰는데(예: `initial_x_m` 배열을 `sim_bridge` 와 `pysim_backend` 가 공유), rcl 의
    YAML 파서는 별칭을 지원하지 않아 `Will not support aliasing at line N` 으로 파일 전체를
    거부한다. 실제로 `robot_count:=4 initial_x_m:=...` 첫 실행에서 이 형태로 기동이 죽었다.
    """

    def ignore_aliases(self, data) -> bool:
        """모든 노드에 대해 별칭 사용을 거부한다.

        Args:
            data: 직렬화 중인 값. 자료형 임의(PyYAML 이 넘긴다).

        Returns:
            `bool` — 항상 True(= 별칭을 쓰지 않고 값을 그대로 전개한다).
        """
        del data
        return True


def _default_params_file() -> str:
    """설치된 `mrs_bringup` 의 기본 파라미터 파일 경로를 돌려준다.

    Returns:
        `str` — `share/mrs_bringup/config/pysim.params.yaml` 의 절대 경로.
    """
    return os.path.join(
        get_package_share_directory('mrs_bringup'), 'config', 'pysim.params.yaml'
    )


def _parse_float_list(raw: str, argument_name: str) -> list:
    """launch 인자로 들어온 실수 배열 리터럴을 파싱한다.

    빈 문자열은 "덮어쓰지 않음"을 뜻한다 — 기준 YAML 의 값을 그대로 쓴다. 길이가
    `robot_count` 와 맞는지는 **여기서 검사하지 않는다**. 그 검사의 단일 소유자는 `sim_bridge`
    이며(자세를 추측하지 않는다), launch 가 앞질러 막으면 그 기동 거부 가드가 영영 실행되지
    않아 살아 있는지 확인할 수 없다.

    Args:
        raw: launch 인자 원문. 자료형 `str`. 예: `"[0.0, 4.0]"`. 빈 문자열이면 `None` 을 돌려준다.
        argument_name: 오류 메시지에 쓸 인자 이름. 자료형 `str`.

    Returns:
        `list | None` — 파싱된 `float` 목록. 입력이 빈 문자열이면 `None`.

    Raises:
        ValueError: 리터럴이 실수 배열이 아닐 때. 조용히 무시하면 launch 인자가 또 사라진다.
    """
    if not raw.strip():
        return None

    try:
        parsed = ast.literal_eval(raw)
    except (ValueError, SyntaxError) as exc:
        raise ValueError('%s 를 파싱하지 못했습니다: %r (%s)' % (argument_name, raw, exc))

    if not isinstance(parsed, (list, tuple)) or not all(
        isinstance(value, (int, float)) and not isinstance(value, bool) for value in parsed
    ):
        raise ValueError('%s 는 실수 배열이어야 합니다: %r' % (argument_name, raw))
    return [float(value) for value in parsed]


def _write_resolved_params_file(base_params_file: str, overrides: dict) -> str:
    """기준 파라미터 YAML 에 launch 오버라이드를 병합한 파일을 만들어 경로를 돌려준다.

    오버라이드는 **정확한 노드 이름 블록**(`sim_bridge:`, `/robot_0/l4_traj_node:` 등)에 들어간다.
    launch_ros 의 dict 오버라이드가 쓰는 `/**:` 와일드카드는 rcl 에서 정확한 이름에 지므로
    쓰지 않는다([0a] 보고서 §4-①).

    Args:
        base_params_file: 기준 YAML 경로. 자료형 `str`. 주석에 담긴 값의 근거는 이 파일이 갖는다.
        overrides: `{노드 이름: {파라미터 이름: 값}}`. 자료형 `dict`.
            노드 이름은 rcl 이 매칭하는 형태여야 한다(네임스페이스가 있으면 `/ns/node`).

    Returns:
        `str` — 생성된 YAML 파일의 절대 경로. 전 노드가 이 **한 파일만** 받는다.

    Raises:
        ValueError: 기준 파일이 매핑이 아닐 때(빈 파일·목록 등).
    """
    with open(base_params_file, 'r', encoding='utf-8') as stream:
        base = yaml.safe_load(stream)

    if base is None:
        base = {}
    if not isinstance(base, dict):
        raise ValueError('파라미터 파일이 매핑이 아닙니다: %s' % base_params_file)

    merged = copy.deepcopy(base)
    for node_name, parameters in overrides.items():
        block = merged.setdefault(node_name, {})
        block.setdefault('ros__parameters', {}).update(parameters)

    handle, path = tempfile.mkstemp(prefix='mrs_bringup_params_', suffix='.yaml')
    with os.fdopen(handle, 'w', encoding='utf-8') as stream:
        yaml.dump(
            merged, stream, Dumper=_NoAliasDumper, allow_unicode=True, sort_keys=False)
    return path


def _system_nodes(context) -> list:
    """R-A2 게이트를 통과한 뒤에 띄울 노드 전량을 만든다.

    Args:
        context: launch 실행 컨텍스트. `LaunchConfiguration` 값을 해석하는 데 쓴다.
            자료형 `launch.LaunchContext`.

    Returns:
        `list` — 서버 노드군 + `plan_tick_node` + `sim_bridge` + per-robot `l4_traj_node` xN
        (+ 선택적으로 pysim 백엔드 프로세스) 액션 목록.
    """
    robot_count = int(LaunchConfiguration('robot_count').perform(context))
    sim_backend = LaunchConfiguration('sim_backend').perform(context)
    # ⚠ 파라미터 파일은 `_launch_setup` 이 이미 생성했다. 여기서 다시 만들면 R-A2 검사 노드가
    #   본 값과 `sim_bridge` 가 받는 값이 갈라질 수 있고, 그러면 게이트가 거짓 통과한다.
    params_file = context.launch_configurations[_RESOLVED_PARAMS_KEY]

    actions = [
        Node(
            package='mrs_ros_map_registry',
            executable='map_registry_node_exe',
            name='map_registry',
            output='screen',
            parameters=[params_file],
        ),
        Node(
            package='mrs_ros_ta',
            executable='ta_service_node_exe',
            name='ta_service',
            output='screen',
            parameters=[params_file],
        ),
        # robot_count 는 생성된 파라미터 파일의 `pp_service:` 블록에 이미 들어 있다. 주입이
        # 빠지면 `robot_count:=4` 로 띄워도 pp 만 자기 기본값(2)으로 계획을 내고, 로봇 2·3 은
        # 창을 영영 못 받는다 — 에러가 아니라 침묵 실패다.
        Node(
            package='mrs_ros_pp',
            executable='pp_service_node_exe',
            name='pp_service',
            output='screen',
            parameters=[params_file],
        ),
        Node(
            package='mrs_ros_sadg',
            executable='sadg_service_node_exe',
            name='sadg_service',
            output='screen',
            parameters=[params_file],
        ),
        Node(
            package='mrs_ros_ladder',
            executable='ladder_orchestrator_node_exe',
            name='ladder_orchestrator',
            output='screen',
            parameters=[params_file],
        ),
        # 정확히 1개만 띄운다 — 두 개의 틱 소스는 로봇마다 다른 스텝 지수 h 를 배포해
        # (A1) 지수 합의를 조용히 깨뜨린다(계약 L-15, R-05). l4 가 기동 시 이를 검사한다.
        Node(
            package='mrs_ros_plan_tick',
            executable='plan_tick_node_exe',
            name='plan_tick_node',
            output='screen',
            parameters=[params_file],
        ),
        # 시간의 소유자(`/clock` 유일 발행자) 겸 sim 추상화 통로.
        # sim_step_s 가 검사 노드와 **같은 값**인 것이 R-A2 게이트의 전제이며, 두 블록이 같은
        # 생성 파일에서 같은 launch 인자를 받으므로 그 일치가 구조적으로 보장된다.
        Node(
            package='mrs_ros_sim_bridge',
            executable='sim_bridge_node_exe',
            name='sim_bridge',
            output='screen',
            parameters=[params_file],
            # 백엔드 구성 실패 시 이 노드는 0 이 아닌 종료코드로 빠져나간다. 그 상태로 나머지가
            # 계속 도는 것은 "시뮬이 도는 것처럼 보이지만 세계가 멈춰 있는" 거짓 실행이다.
            # `initial_x_m` 길이 ≠ `robot_count` 기동 거부도 이 경로로 전체를 중단시킨다.
            on_exit=Shutdown(reason='sim_bridge exited — 세계가 전진하지 않습니다.'),
        ),
    ]

    # pysim 백엔드 프로세스: [0a] 는 인프로세스 fake 로 통과하므로 기본적으로 띄우지 않는다.
    # 프로세스 형태를 확인하거나 sim_backend:=pysim 을 시도할 때만 함께 띄운다.
    launch_pysim = LaunchConfiguration('launch_pysim_process').perform(context)
    if launch_pysim.lower() in ('true', '1', 'yes') or sim_backend == 'pysim':
        actions.append(
            Node(
                package='mrs_sim_pysim',
                executable='pysim_backend_node',
                name='pysim_backend',
                output='screen',
                parameters=[params_file],
            )
        )

    # per-robot 값(`robot_id`·`neighbor_robot_ids`)도 생성 파일의 `/robot_{i}/l4_traj_node:`
    # 블록에 들어 있다 — 로봇마다 값이 다르므로 와일드카드로는 애초에 표현할 수 없다.
    for robot_id in range(robot_count):
        actions.append(
            Node(
                package='mrs_ros_l4',
                executable='l4_traj_node_exe',
                name='l4_traj_node',
                namespace='robot_%d' % robot_id,
                output='screen',
                parameters=[params_file],
            )
        )

    return actions


def _on_startup_check_exit(event, context) -> list:
    """R-A2 기동 검사 프로세스의 종료를 받아 시스템 기동 여부를 결정한다.

    Args:
        event: 검사 프로세스 종료 이벤트. 자료형 `launch.events.process.ProcessExited`.
            `returncode`(`int`)가 0 이면 통과다.
        context: launch 실행 컨텍스트. 자료형 `launch.LaunchContext`.

    Returns:
        `list` — 종료코드 0 이면 시스템 노드 액션 목록, 그 밖에는 `LogInfo` + `Shutdown`
        (전체 기동 중단). **검사 실패를 경고로 낮추지 않는다** — 전제가 깨진 상태의 실행은
        F5 결과를 무효화한다.
    """
    if event.returncode == 0:
        return _system_nodes(context)

    return [
        LogInfo(
            msg=(
                'R-A2 기동 거부 (종료코드 %d) — Δt_h 가 시뮬 스텝 dt 의 정수배가 아닙니다. '
                '전체 기동을 중단합니다. replan_period_s / sim_step_s 를 확인하십시오.'
                % event.returncode
            )
        ),
        Shutdown(reason='R-A2 startup check failed'),
    ]


def _collect_overrides(context) -> dict:
    """launch 인자를 **노드 이름별** 파라미터 오버라이드 표로 모은다.

    이 표가 "어느 launch 인자가 어느 노드의 어느 파라미터로 가는가"의 정본이다. 여기 없는
    노드·키는 기준 YAML 값이 그대로 쓰인다.

    Args:
        context: launch 실행 컨텍스트. 자료형 `launch.LaunchContext`.

    Returns:
        `dict` — `{노드 이름: {파라미터 이름: 값}}`. per-robot `l4_traj_node` 는 네임스페이스를
        포함한 정규화 이름(`/robot_0/l4_traj_node`)을 키로 갖는다.
    """
    robot_count = int(LaunchConfiguration('robot_count').perform(context))
    sim_backend = LaunchConfiguration('sim_backend').perform(context)
    replan_period_s = float(LaunchConfiguration('replan_period_s').perform(context))
    sim_step_s = float(LaunchConfiguration('sim_step_s').perform(context))
    step_wall_period_s = float(LaunchConfiguration('step_wall_period_s').perform(context))

    sim_bridge = {
        'robot_count': robot_count,
        'sim_backend': sim_backend,
        'sim_step_s': sim_step_s,
        'step_wall_period_s': step_wall_period_s,
    }
    pysim_backend = {'sim_step_s': sim_step_s}

    # 초기 배치는 **선택 인자**다. 비워 두면 기준 YAML 값을 쓴다(= 로봇 2대 구성).
    # ⚠ 길이와 robot_count 의 일치는 여기서 검사하지 않는다 — 그 가드의 소유자는 sim_bridge 다.
    for argument_name, parameter_name in (
        ('initial_x_m', 'initial_x_m'),
        ('initial_y_m', 'initial_y_m'),
        ('initial_theta_rad', 'initial_theta_rad'),
    ):
        values = _parse_float_list(
            LaunchConfiguration(argument_name).perform(context), argument_name)
        if values is not None:
            sim_bridge[parameter_name] = values
            pysim_backend[parameter_name] = values

    overrides = {
        'bringup_startup_check': {
            'replan_period_s': replan_period_s,
            'sim_step_s': sim_step_s,
        },
        'pp_service': {'robot_count': robot_count},
        'sadg_service': {'robot_count': robot_count},
        'ladder_orchestrator': {'robot_count': robot_count},
        'plan_tick_node': {'replan_period_s': replan_period_s},
        'sim_bridge': sim_bridge,
        'pysim_backend': pysim_backend,
    }

    for robot_id in range(robot_count):
        overrides['/robot_%d/l4_traj_node' % robot_id] = {
            'robot_id': robot_id,
            'neighbor_robot_ids': [i for i in range(robot_count) if i != robot_id],
        }
    return overrides


def _launch_setup(context, *args, **kwargs) -> list:
    """파라미터 파일을 생성하고, 검사 노드와 그 종료 이벤트 핸들러를 배치한다.

    핸들러를 검사 노드보다 **먼저** 등록한다 — 반대 순서면 검사가 빨리 끝났을 때 이벤트를
    놓쳐 시스템이 영원히 뜨지 않는다.

    Args:
        context: launch 실행 컨텍스트. 자료형 `launch.LaunchContext`.
        *args: launch 가 넘기는 가변 인자(사용하지 않는다).
        **kwargs: launch 가 넘기는 키워드 인자(사용하지 않는다).

    Returns:
        `list` — `[LogInfo(생성 파일 경로), RegisterEventHandler, Node(bringup_startup_check)]`.
        경로를 로그로 남기는 이유는 "어떤 값으로 떴는가"를 사후에 대조할 수 있어야 하기 때문이다.
    """
    del args, kwargs
    base_params_file = LaunchConfiguration('params_file').perform(context)
    resolved = _write_resolved_params_file(base_params_file, _collect_overrides(context))
    context.launch_configurations[_RESOLVED_PARAMS_KEY] = resolved

    startup_check = Node(
        package='mrs_bringup',
        executable='startup_check_exe',
        name='bringup_startup_check',
        output='screen',
        parameters=[resolved],
    )

    gate = RegisterEventHandler(
        OnProcessExit(target_action=startup_check, on_exit=_on_startup_check_exit)
    )

    return [
        LogInfo(msg='해석된 파라미터 파일: %s (기준: %s)' % (resolved, base_params_file)),
        gate,
        startup_check,
    ]


def generate_launch_description() -> LaunchDescription:
    """공용 launch 기술을 만든다.

    Returns:
        `LaunchDescription` — launch 인자 선언 + R-A2 게이트가 걸린 조립 로직.
    """
    return LaunchDescription(
        [
            DeclareLaunchArgument(
                'robot_count', default_value='2',
                description='로봇 수. [0a] tracer bullet 은 2 대다.'),
            DeclareLaunchArgument(
                'sim_backend', default_value='fake',
                description=(
                    '시뮬 백엔드 (fake|pysim|isaac). [0a] 는 인프로세스 FakeSimBackend("fake")로 '
                    '통과한다. pysim/isaac 은 C++ 프로세스 어댑터가 없어 sim_bridge 가 기동을 '
                    '거부한다 — 조용히 fake 로 대체하지 않는다.')),
            DeclareLaunchArgument(
                'replan_period_s', default_value='0.1',
                description='Δt_h [s]. theory T1 §6.1 확정값(계약 L-15).'),
            DeclareLaunchArgument(
                'sim_step_s', default_value='0.05',
                description='시뮬 스텝 dt [s]. [0a] 실측 대상 placeholder — 근거 없음.'),
            DeclareLaunchArgument(
                'step_wall_period_s', default_value='0.05',
                description=(
                    '자동 스텝의 벽시계 주기 [s]. [0a] 실측 대상 — sim_step_s 와 같게 두면 '
                    '공칭 배속 1.0(정의값이지 측정치가 아니다).')),
            DeclareLaunchArgument(
                'launch_pysim_process', default_value='false',
                description=(
                    'pysim 백엔드 프로세스를 함께 띄울지. sim_backend:=pysim 이면 자동으로 '
                    '띄운다. [0a] 통과 경로가 아니며 프로세스 형태 확인용이다.')),
            DeclareLaunchArgument(
                'initial_x_m', default_value='',
                description=(
                    '초기 배치 x [m] 배열 리터럴. 예: "[0.0, 4.0, 0.0, 4.0]". 비우면 '
                    'params_file 값을 쓴다. 길이 ≠ robot_count 이면 sim_bridge 가 기동을 '
                    '거부한다(자세를 추측하지 않는다) — launch 는 앞질러 막지 않는다.')),
            DeclareLaunchArgument(
                'initial_y_m', default_value='',
                description='초기 배치 y [m] 배열 리터럴. initial_x_m 과 같은 규약.'),
            DeclareLaunchArgument(
                'initial_theta_rad', default_value='',
                description='초기 방위각 [rad] 배열 리터럴. initial_x_m 과 같은 규약.'),
            DeclareLaunchArgument(
                'params_file', default_value=_default_params_file(),
                description=(
                    '**기준** 노드 파라미터 YAML. 값의 근거는 파일 안 주석이 갖고 있다. '
                    'launch 는 이 파일을 읽어 인자를 정확한 노드 이름 블록에 병합한 새 파일을 '
                    '만들어 노드에 넘긴다 — 이 파일 자체가 노드에 전달되지는 않는다.')),
            OpaqueFunction(function=_launch_setup),
        ]
    )
