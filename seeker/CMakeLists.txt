cmake_minimum_required(VERSION 2.8.3)
project(seeker)

find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  roscpp
  sensor_msgs
  std_msgs
  std_srvs
  nav_msgs
  message_generation
)

add_message_files(
  FILES
  displacement.msg
)

generate_messages(
  DEPENDENCIES
  geometry_msgs
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES seeker
  CATKIN_DEPENDS geometry_msgs roscpp sensor_msgs std_msgs std_srvs nav_msgs message_runtime
#  DEPENDS system_lib
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(seeker src/seeker.cpp)
add_dependencies(seeker ${seeker_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})
target_link_libraries(seeker ${catkin_LIBRARIES})

