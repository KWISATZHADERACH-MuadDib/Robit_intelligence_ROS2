cmake_minimum_required(VERSION 3.5)
project(ros2_day2_hw3)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC OFF)
set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(Boost 1.71.0 REQUIRED COMPONENTS thread)
find_package(Qt5 COMPONENTS Widgets Core Gui Network REQUIRED)
find_package(std_msgs REQUIRED)
find_package(std_srvs REQUIRED)

# ---------------------------------------------------------------------------
# 다중 토픽 모니터링 대시보드 (Qt GUI)
# ---------------------------------------------------------------------------
file(GLOB PRO_SOURCE_DIR RELATIVE ${CMAKE_SOURCE_DIR} FOLLOW_SYMLINKS  src/*.cpp)
file(GLOB PRO_UI_DIR RELATIVE ${CMAKE_SOURCE_DIR} FOLLOW_SYMLINKS  ui/*.ui)
file(GLOB PRO_INCLUDE_DIR RELATIVE ${CMAKE_SOURCE_DIR} FOLLOW_SYMLINKS  include/${PROJECT_NAME}/*.h include/${PROJECT_NAME}/*.hpp)
file(GLOB PRO_RESOURCE_DIR RELATIVE ${CMAKE_SOURCE_DIR} FOLLOW_SYMLINKS  resource/*.qrc)
qt5_wrap_ui(QT_UI_HPP ${PRO_UI_DIR})
qt5_wrap_cpp(QT_MOC_HPP ${PRO_INCLUDE_DIR})
qt5_add_resources(QT_RESOUCE_HPP ${PRO_RESOURCE_DIR})
include_directories(
	include/
	${CMAKE_CURRENT_BINARY_DIR}
)
add_executable(${PROJECT_NAME}
    ${PRO_SOURCE_DIR}
    ${PRO_INCLUDE_DIR}
    ${PRO_UI_DIR}
    ${QT_MOC_HPP}
    ${QT_RESOUCE_HPP}
    ${QT_UI_HPP}
  )
ament_target_dependencies(${PROJECT_NAME} rclcpp Boost std_msgs std_srvs)
target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Core Qt5::Gui Qt5::Network)
install(TARGETS ${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})

# ---------------------------------------------------------------------------
# 재시작 요청 서비스 서버 (임의 구현 - 로그만 남기고 성공 응답)
# ---------------------------------------------------------------------------
add_executable(restart_server src/nodes/restart_server.cpp)
ament_target_dependencies(restart_server rclcpp std_srvs)
install(TARGETS restart_server DESTINATION lib/${PROJECT_NAME})

# ---------------------------------------------------------------------------
# 대시보드 동작 확인용 테스트 토픽 퍼블리셔
# ---------------------------------------------------------------------------
add_executable(topic_test_publisher src/nodes/topic_test_publisher.cpp)
ament_target_dependencies(topic_test_publisher rclcpp std_msgs)
install(TARGETS topic_test_publisher DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
