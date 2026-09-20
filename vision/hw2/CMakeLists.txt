cmake_minimum_required(VERSION 3.8)
project(vision_day1_hw2)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(OpenCV REQUIRED)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOUIC_SEARCH_PATHS ui)

# 영상처리 라이브러리: OpenCV만 사용 (Qt/ROS 의존 없음)
add_library(vision_core STATIC
  src/feature_extractor.cpp
  src/position_classifier.cpp
  src/cone_detector.cpp
  src/result_renderer.cpp)

target_include_directories(vision_core PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  ${OpenCV_INCLUDE_DIRS})

target_link_libraries(vision_core PUBLIC ${OpenCV_LIBS})
set_target_properties(vision_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

# GUI 노드: Qt UI + ROS2 구독
add_executable(gui_node
  src/main.cpp
  src/main_window.cpp
  src/qt_image.cpp
  include/main_window.hpp)   # AUTOMOC이 Q_OBJECT를 처리하도록 헤더도 등록

target_include_directories(gui_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

ament_target_dependencies(gui_node
  rclcpp
  sensor_msgs
  cv_bridge
  OpenCV)

target_link_libraries(gui_node
  vision_core
  Qt5::Widgets
  ${OpenCV_LIBS})

install(TARGETS
  gui_node
  DESTINATION lib/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
