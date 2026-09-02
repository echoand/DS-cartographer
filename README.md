cd carto_ws
source devel_isolated/setup.bash
roslaunch turtlebot3_slam turtlebot3_slam.launch slam_methods:=cartographer 
roslaunch turtlebot3_teleop turtlebot3_teleop_key.launch



## Datasets
The evaluation datasets recorded in degraded indoor corridors and indoor park environments can be downloaded from GitHub Releases:

- [Download Evaluation Datasets (ROS Bags)](https://github.com/echoand/DS-cartographer/releases/download/v1.0.0/Evaluation.Datasets.zip)

