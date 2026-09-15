cmake_minimum_required(VERSION 3.8)
project(my_cpp_pkg)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(rosidl_default_generators REQUIRED)

rosidl_generate_interfaces(${PROJECT_NAME}
  "msg/MyMsg.msg"
  "srv/Calculator.srv"
)
rosidl_get_typesupport_target(cpp_typesupport_target ${PROJECT_NAME} "rosidl_typesupport_cpp")

add_executable(my_pub src/my_pub.cpp)
ament_target_dependencies(my_pub rclcpp std_msgs)

add_executable(my_sub src/my_sub.cpp)
ament_target_dependencies(my_sub rclcpp std_msgs)

add_executable(my_msg_pub src/my_msg_pub.cpp)
ament_target_dependencies(my_msg_pub rclcpp)
target_link_libraries(my_msg_pub "${cpp_typesupport_target}")

add_executable(calculator_server src/calculator_server.cpp)
ament_target_dependencies(calculator_server rclcpp)
target_link_libraries(calculator_server "${cpp_typesupport_target}")

add_executable(calculator_client src/calculator_client.cpp)
ament_target_dependencies(calculator_client rclcpp)
target_link_libraries(calculator_client "${cpp_typesupport_target}")

install(TARGETS my_pub my_sub my_msg_pub calculator_server calculator_client
  DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME})

if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()