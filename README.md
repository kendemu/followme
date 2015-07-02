## followme (indigo) - 0.0.1-0

The packages in the `followme` repository were released into the `indigo` distro by running `/usr/bin/bloom-release --rosdistro indigo --track indigo followme` on `Thu, 02 Jul 2015 05:33:48 -0000`

The `followme` package was released.

Version of package(s) in repository `followme`:
- upstream repository: https://github.com/kendemu/followme
- release repository: unknown
- rosdistro version: `null`
- old version: `null`
- new version: `0.0.1-0`

Versions of tools used:
- bloom version: `0.5.20`
- catkin_pkg version: `0.2.8`
- rosdep version: `0.11.2`
- rosdistro version: `0.4.2`
- vcstools version: `0.1.36`


Author:Kensei Demura(kendemu)

This ros software can detect and track people, and can distinguish people.
I am using CamShift to track people, and using the kinect to calculate the distance of the people.
I'm using Propotional Navigation to track people. Also you can use the PD distance control by modificating the code a little. I have made the PD controller, so you just have to tune the parameters and have to make it publish to the motor topic.

Benchmarked in RoboCup Japan Open@home SPL league, scored 181 points.  

ROS Distribution Supported
-ROS Indigo(Ubuntu 14.04)

ROS Package Dependency
-Turtlebot ROS Packages
-std_msgs
-sensor_msgs
-geometry_msgs
-image_transport
-cv_bridge

Other dependencies
-libfreenect/OpenNI/SensorKincet

Hardware Requirements
-Turtlebot 
-Kinect v1/v2/ASUS Xtion Pro live

How to start the software
$roslaunch turtlebot_bringup minimal.launch

$roslaunch turtlebot_bringup 3dsensor.launch

$rosrun followme followme_node

$rosrun followme distance_node

configure what object to track with the followme_node GUI interface
(you can choose the object to track using mouse selection)

$rosrun followme trackme_node 

Now the turtlebot is going to track you!