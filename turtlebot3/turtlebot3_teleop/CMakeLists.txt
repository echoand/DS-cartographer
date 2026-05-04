################################################################################
# Set minimum required version of cmake, project name and compile options
################################################################################
cmake_minimum_required(VERSION 3.0.2)
project(turtlebot3_teleop)

################################################################################
# Find catkin packages and libraries for catkin and system dependencies
################################################################################
find_package(catkin REQUIRED COMPONENTS
  rospy
  roscpp
  geometry_msgs
  sensor_msgs
)

################################################################################
# Setup for python modules and scripts
################################################################################
catkin_python_setup()

################################################################################
# Declare ROS messages, services and actions
################################################################################

################################################################################
# Declare ROS dynamic reconfigure parameters
################################################################################

################################################################################
# Declare catkin specific configuration to be passed to dependent projects
################################################################################
catkin_package(
  CATKIN_DEPENDS  rospy roscpp geometry_msgs sensor_msgs
)

include_directories(
  ${catkin_INCLUDE_DIRS}
)

add_executable(teleop_joy src/turtlebot3_teleop/teleop_joy.cpp)

target_link_libraries(teleop_joy
  ${catkin_LIBRARIES}
)

install(TARGETS teleop_joy
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
################################################################################
# Build
################################################################################

################################################################################
# Install
################################################################################
catkin_install_python(PROGRAMS
  nodes/turtlebot3_teleop_key
  DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY launch
  DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
)

################################################################################
# Test
################################################################################
