from launch import LaunchDescription
from launch.actions import EmitEvent, RegisterEventHandler
from launch.events import matches_action
from launch_ros.actions import LifecycleNode
from launch_ros.event_handlers import OnStateTransition
from launch_ros.events.lifecycle import ChangeState
from lifecycle_msgs.msg import Transition


def generate_launch_description():
    fake_imu = LifecycleNode(
        package='ros2_day3_hw1',
        executable='FakeImu',
        name='fake_imu',
        namespace='',
        output='screen',
        parameters=[{'rate_hz': 10.0, 'stamp_offset_sec': 0.0}],
    )

    imu_watchdog = LifecycleNode(
        package='ros2_day3_hw1',
        executable='ImuWatchdog',
        name='imu_watchdog',
        namespace='',
        output='screen',
        parameters=[{'timeout_sec': 0.5, 'check_rate_hz': 5.0}],
    )

    configure_fake_imu = EmitEvent(event=ChangeState(
        lifecycle_node_matcher=matches_action(fake_imu),
        transition_id=Transition.TRANSITION_CONFIGURE,
    ))
    configure_watchdog = EmitEvent(event=ChangeState(
        lifecycle_node_matcher=matches_action(imu_watchdog),
        transition_id=Transition.TRANSITION_CONFIGURE,
    ))

    activate_fake_imu_when_inactive = RegisterEventHandler(OnStateTransition(
        target_lifecycle_node=fake_imu,
        goal_state='inactive',
        entities=[EmitEvent(event=ChangeState(
            lifecycle_node_matcher=matches_action(fake_imu),
            transition_id=Transition.TRANSITION_ACTIVATE,
        ))],
    ))
    activate_watchdog_when_inactive = RegisterEventHandler(OnStateTransition(
        target_lifecycle_node=imu_watchdog,
        goal_state='inactive',
        entities=[EmitEvent(event=ChangeState(
            lifecycle_node_matcher=matches_action(imu_watchdog),
            transition_id=Transition.TRANSITION_ACTIVATE,
        ))],
    ))

    return LaunchDescription([
        activate_fake_imu_when_inactive,
        activate_watchdog_when_inactive,
        fake_imu,
        imu_watchdog,
        configure_fake_imu,
        configure_watchdog,
    ])