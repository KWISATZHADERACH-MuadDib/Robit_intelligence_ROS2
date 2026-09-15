from launch import LaunchDescription
from launch_ros.actions import Node

# generate_launch_description: ros2 launch가 이 함수를 호출해서
# 실행할 노드 목록을 가져온다.
def generate_launch_description():
    return LaunchDescription([
        Node(
            package='my_cpp_pkg', executable='my_pub',
        ),
        Node(
            package='my_cpp_pkg', executable='my_sub',
        ),
    ])