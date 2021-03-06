#include <iostream>
#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <sensor_msgs/LaserScan.h>
float val;
ros::Publisher dist_pub;
void degCallback(const std_msgs::Float64& msg){
  val = (float)((int)((msg.data+21.5) * 14.884274764));
}
void depthCallback(const sensor_msgs::LaserScan::ConstPtr& msg){
  sensor_msgs::LaserScan laser = *msg;
  int offset = 50;
  int lower = val - offset;
  int upper = val + offset;
  if(lower <= 0)  lower = 0;
  if(upper >= 640) upper = 640; 
  std_msgs::Float64 min;
  min.data = 9999;
  for(int i = lower; i < upper; i++){
    if(min.data > laser.ranges[i])
      min.data = laser.ranges[i];
  }
  dist_pub.publish(min);
  ROS_INFO("human distance:%f",min.data);
}
int main(int argc, char** argv){
  ros::init(argc, argv, "follow_depth");
  ros::NodeHandle nh;
  ros::Subscriber sub=nh.subscribe("/scan",1,depthCallback);
  ros::Subscriber sub2=nh.subscribe("/follow_publisher",1,degCallback);
  dist_pub=nh.advertise<std_msgs::Float64>("/human_distance",1);
  ros::spin();
  return 0;
}
