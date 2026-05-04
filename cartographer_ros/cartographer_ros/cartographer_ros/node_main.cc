/*
 * Copyright 2016 The Cartographer Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "absl/memory/memory.h"
#include "cartographer/mapping/map_builder.h"
#include "cartographer_ros/node.h"
#include "cartographer_ros/node_options.h"
#include "cartographer_ros/ros_log_sink.h"
#include "gflags/gflags.h"
#include "tf2_ros/transform_listener.h"
//定义参数通过DEFINE_type宏实现，该宏的三个参数含义分别为命令行参数名，参数默认值，以及参数的帮助信息
// 当参数被定义后，通过FLAGS_name就可以访问到对应的参数
DEFINE_bool(collect_metrics, false,
            "Activates the collection of runtime metrics. If activated, the "
            "metrics can be accessed via a ROS service.");
DEFINE_string(configuration_directory, "",
              "First directory in which configuration files are searched, "
              "second is always the Cartographer installation to allow "
              "including files from there.");
DEFINE_string(configuration_basename, "",
              "Basename, i.e. not containing any directory prefix, of the "
              "configuration file.");
DEFINE_string(load_state_filename, "",
              "If non-empty, filename of a .pbstream file to load, containing "
              "a saved SLAM state.");
DEFINE_bool(load_frozen_state, true,
            "Load the saved state as frozen (non-optimized) trajectories.");
DEFINE_bool(
    start_trajectory_with_default_topics, true,
    "Enable to immediately start the first trajectory with default topics.");
DEFINE_string(
    save_state_filename, "",
    "If non-empty, serialize state and write it to disk before shutting down.");

namespace cartographer_ros {
namespace {

void Run() {
  constexpr double kTfBufferCacheTimeInSeconds = 10.;
  tf2_ros::Buffer tf_buffer{::ros::Duration(kTfBufferCacheTimeInSeconds)};
  tf2_ros::TransformListener tf(tf_buffer);
  NodeOptions node_options;   //从配置文件中读取参数
  TrajectoryOptions trajectory_options;
  // 根据lua配置文件中的内容，为node_options，trajectory_options赋值
  std::tie(node_options, trajectory_options) =
      LoadOptions(FLAGS_configuration_directory, FLAGS_configuration_basename);
  
  //MapBuilder类是完整的SLAM算法类
  auto map_builder =
      cartographer::mapping::CreateMapBuilder(node_options.map_builder_options);
  //创建一个节点，把地图构建器给到这个节点，接下来就可以用地图构建器进行构建地图了
  // Node类的初始化，将ROS的topic传入SLAM，也就是MapBuilder
  Node node(node_options, std::move(map_builder), &tf_buffer,
            FLAGS_collect_metrics);
  if (!FLAGS_load_state_filename.empty()) {
    node.LoadState(FLAGS_load_state_filename, FLAGS_load_frozen_state);
  }
  //不需要用户调用，直接启动一条轨迹FLAGS_start_trajectory_with_default_topics
  //是从配置参数中读出来的。机器人行走的时候会形成轨迹。程序启动起来，不需要订阅就直接启动一条轨迹
  if (FLAGS_start_trajectory_with_default_topics) {
    node.StartTrajectoryWithDefaultTopics(trajectory_options);
  }

  ::ros::spin();
  //停止所有的轨迹
  node.FinishAllTrajectories();
  //进行最后的优化
  node.RunFinalOptimization();

  if (!FLAGS_save_state_filename.empty()) {
    node.SerializeState(FLAGS_save_state_filename,
                        true /* include_unfinished_submaps */);
  }
}

}  // namespace
}  // namespace cartographer_ros

int main(int argc, char** argv) {
  // 初始化glog库
  google::InitGoogleLogging(argv[0]);
  // 使用gflags进行参数的初始化，其中第三个参数为remove_flag
  // 如果为true，gflags会移除parse过的参数，否则gflags就会保留这些参数
  google::ParseCommandLineFlags(&argc, &argv, true);

  CHECK(!FLAGS_configuration_directory.empty())
      << "-configuration_directory is missing.";
  CHECK(!FLAGS_configuration_basename.empty())
      << "-configuration_basename is missing.";

  ::ros::init(argc, argv, "cartographer_node");
  ::ros::start();

  cartographer_ros::ScopedRosLogSink ros_log_sink;
  cartographer_ros::Run();
  ::ros::shutdown();
}
