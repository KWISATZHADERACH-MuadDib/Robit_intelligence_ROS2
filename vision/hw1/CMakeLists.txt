cmake_minimum_required(VERSION 3.8)
project(vision_day1_hw1)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

find_package(OpenCV REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(sensor_msgs REQUIRED)

include_directories(include)

add_executable(vision_node src/vision_node.cpp)

target_include_directories(vision_node PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

ament_target_dependencies(vision_node
  rclcpp
  sensor_msgs
  cv_bridge
  OpenCV)

target_link_libraries(vision_node ${OpenCV_LIBS})

install(TARGETS
  vision_node
  DESTINATION lib/${PROJECT_NAME})

  add_executable(hw1_color_mask
  src/main.cpp
  src/color_range.cpp
  src/color_masker.cpp
  src/image_processor.cpp)

target_include_directories(hw1_color_mask PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)

ament_target_dependencies(hw1_color_mask OpenCV)

target_link_libraries(hw1_color_mask ${OpenCV_LIBS})

install(TARGETS
  hw1_color_mask
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
