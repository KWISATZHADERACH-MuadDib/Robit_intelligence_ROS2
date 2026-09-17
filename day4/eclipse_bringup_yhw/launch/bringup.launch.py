from launch import LaunchDescription
from launch.substitutions import Command, FindExecutable, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    robot_description_content = ParameterValue(
        Command([
            FindExecutable(name='xacro'), ' ',
            PathJoinSubstitution(
                [FindPackageShare('robot_description'), 'urdf', 'eclipse.xacro']),
            ' with_base:=true with_arm:=true with_camera_tower:=true',
            ' use_mock_base:=true use_mock_arm:=true use_mock_ct:=true',
        ]),
        value_type=str,
    )

    controllers_yaml = PathJoinSubstitution(
        [FindPackageShare('eclipse_bringup_yhw'), 'config', 'controllers.yaml'])

    rviz_config_file = PathJoinSubstitution(
        [FindPackageShare('robot_description'), 'config', 'eclipse.rviz'])

    return LaunchDescription([
        Node(
            package='robot_state_publisher',
            executable='robot_state_publisher',
            parameters=[{'robot_description': robot_description_content}],
            output='screen',
        ),
        # controller_manager는 robot_state_publisher가 발행하는 /robot_description 토픽으로 URDF를 받음
        Node(
            package='controller_manager',
            executable='ros2_control_node',
            parameters=[controllers_yaml],
            output='screen',
        ),
        Node(
            package='controller_manager',
            executable='spawner',
            arguments=['joint_state_broadcaster'],
        ),
        Node(
            package='controller_manager',
            executable='spawner',
            arguments=['camera_tower_controller'],
        ),
        Node(
            package='controller_manager',
            executable='spawner',
            arguments=['arm_controller'],
        ),
        Node(
            package='controller_manager',
            executable='spawner',
            arguments=['base_velocity_controller'],
        ),
        Node(
            package='controller_manager',
            executable='spawner',
            arguments=['flipper_controller'],
        ),
        Node(
            package='rviz2',
            executable='rviz2',
            arguments=['-d', rviz_config_file],
            output='screen',
        ),
    ])