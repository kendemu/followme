#include <iostream>
#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <std_msgs/Float64.h>
#define PI 3.1415926
ros::Publisher vel_pub;

float distance = 9999;
static float last_deg = 0;
static float last_distance = distance;
static float last_rad = 0;
void distanceCallback(const std_msgs::Float64& msg){
  last_distance = distance;
  distance = msg.data * 100;
}

void detectorCallback(const std_msgs::Float64& msg){
  geometry_msgs::Twist base_cmd;
  float kp_angular=8;
  float kp_linear = 0.6;
  float period = 1/15;
  float kd_angular=kp_angular * 0.125 * period;
  float kd_linear = kp_linear * 0.125 * period;
  if(last_distance != 9999 && distance == 9999) distance = 50;
  float objective = 150.0;
  float vel_x = (distance - objective) * 0.01;
  float last_vel_x = (last_distance -objective) * 0.01;
  float deg = msg.data;
  float rad = msg.data/360.0 * 2 * PI;
  base_cmd.angular.z=-(rad * kp_angular);
  base_cmd.linear.x= vel_x * kp_linear; + (last_vel_x - vel_x) * kd_linear;
  base_cmd.linear.x=0.2;
  if(distance <= objective) base_cmd.linear.x = 0;
  if(distance == 9999 && last_distance == 9999) base_cmd.linear.x = 0;
  vel_pub.publish(base_cmd);
  ROS_INFO("moving at angular:%f[m], linear:%f[m]",base_cmd.angular.z,base_cmd.linear.x);
  if(base_cmd.linear.x == 0){
    ROS_INFO("state stop");
  }
  else ROS_INFO("state start");
  last_distance=distance;
  last_deg=deg;
  last_rad=rad;
}

int main(int argc, char** argv){
  ros::init(argc, argv,"human_follower");
  ros::NodeHandle nh;
  ros::Subscriber sub= nh.subscribe("follow_publisher",1,detectorCallback);
  ros::Subscriber sub2=nh.subscribe("/human_distance",1,distanceCallback);
  vel_pub=nh.advertise<geometry_msgs::Twist>("/cmd_vel_mux/input/teleop",1);
  ros::spin();
  return 0;
}
