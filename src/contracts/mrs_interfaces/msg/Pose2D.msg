# Pose2D — 평면 자세 (계약 공통 값 타입)
#
# 좌표계: REP-105 `map` 프레임 (오른손, x 전방, y 좌측, ENU). 원점·축은 MapRegistry가 소유한다.
# geometry_msgs/Pose2D 는 ROS2에서 deprecated 이므로 계약 전용 타입을 둔다.

float64 x_m      # 위치 x [m], map 프레임
float64 y_m      # 위치 y [m], map 프레임
float64 theta_rad  # 방위각 [rad], map 프레임 +x축 기준 CCW, 정규화 구간 [-pi, pi]
