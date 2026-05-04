#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
 
class TeleopTurtle
{
public:
    TeleopTurtle();
 
private:
    // 处理手柄发送过来的信息
    void callback(const sensor_msgs::Joy::ConstPtr &joy);
    // 实例化ROS句柄
    ros::NodeHandle nh;
    // 定义订阅者对象，用来订阅手柄发送的数据
    ros::Subscriber sub;
    // 定义发布者对象，用来将手柄数据发布到乌龟控制话题上
    ros::Publisher pub;
    // 用来接收launch文件中设置的参数，绑定手柄摇杆、轴的映射
    int axis_linear, axis_angular;
    int sticks_left, sticks_right;
};
 
TeleopTurtle::TeleopTurtle()
{
    // 从参数服务器读取的参数
    // 按手柄摇杆分配
    nh.param<int>("sticks_left", sticks_left, 1);
    nh.param<int>("sticks_right", sticks_right, 3);
    // 按键分配
    nh.param<int>("axis_linear", axis_linear, 1);
    nh.param<int>("axis_angular", axis_angular, 2);
  // Axes:  0:     0  1:     0  2:　0　  3:     0  4:     0  5:　0　  6:     0  7:     0 
    // 对应：0: 左摇杆轴左右
    // 　　　1: 左摇杆轴前后
    // 　　　2: 默认全不开放
    // 　　　3: 右摇杆轴左右
    // 　　　4: 右摇杆轴前后
    // 　　　5: 默认全不开放
    //　　　 6: 方向按键左+右
    // 　　　7: 方向按键前+后 
 
    pub = nh.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 10);
    sub = nh.subscribe<sensor_msgs::Joy>("joy", 10, &TeleopTurtle::callback, this);
}
 
void TeleopTurtle::callback(const sensor_msgs::Joy::ConstPtr &joy)
{
    geometry_msgs::Twist vel;
    // 将手柄摇杆轴拨动时值的输出赋值给乌龟的线速度和角速度
   if(joy->axes[axis_linear] || joy->axes[axis_angular])
    {
        vel.linear.x = joy->axes[axis_linear];
        vel.angular.z = joy->axes[axis_angular];
    }
    else if(joy->axes[sticks_left] ||  joy->axes[sticks_right])
    {
        vel.linear.x = joy->axes[sticks_left];
        vel.angular.z = joy->axes[sticks_right];
    }
    // // 将手柄摇杆轴拨动时值的输出赋值给乌龟的线速度和角速度
    // vel.linear.x = joy->axes[axis_linear];
    // vel.angular.z = joy->axes[axis_angular];
    // // 按键按动时也可以输出乌龟的线速度和角速度
    // vel.linear.x = joy->axes[sticks_left];
    // vel.angular.z = joy->axes[sticks_right];
    ROS_INFO("当前线速度为:%.3lf ; 角速度为:%.3lf", vel.linear.x, vel.angular.z);
    pub.publish(vel);
}
 
int main(int argc, char **argv)
{
    // 设置编码
    setlocale(LC_ALL, "");
    // 初始化ROS节点
    ros::init(argc, argv, "teleop_joy");
    TeleopTurtle teleopTurtle;
    ros::spin();
    return 0;
}