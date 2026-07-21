"""unittest discover 가 이 디렉터리를 탐색하려면 패키지여야 한다.

colcon 의 ament_python 경로는 소스 디렉터리에서 `python3 -m unittest` (discover) 를 돌리는데,
discover 는 임포트 가능한 패키지만 재귀한다. 이 파일이 없으면 테스트가 있어도 0건으로 잡히고
`colcon test` 가 EXIT 5 로 실패한다.
"""
