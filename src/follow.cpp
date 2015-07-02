#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <opencv2/opencv.hpp>
#include <std_msgs/Float64.h>
struct MOUSE_CALLBACK_DATA {
  int trackObject;
  bool selectObject;
  cv::Point origin;
  cv::Rect selection;
};
MOUSE_CALLBACK_DATA data = { 0 };
int vmin = 10, vmax = 256, smin = 30;
cv::Mat hist;
cv::Mat histimg = cv::Mat::zeros(200, 320, CV_8UC3);
cv::Rect trackWindow;
int hsize = 16;
float hranges[] = { 0, 180 };
const float* phranges = hranges;
std_msgs::Float64 deg;
bool deg_flag = false;
ros::Publisher deg_pub;
static void onMouse(int event, int x, int y, int flag, void *arg)
{
  MOUSE_CALLBACK_DATA *data = (MOUSE_CALLBACK_DATA*)arg;
 
  if (data->selectObject) {
    data->selection.x = MIN(x, data->origin.x);
    data->selection.y = MIN(y, data->origin.y);
    data->selection.width = std::abs(x - data->origin.x);
    data->selection.height = std::abs(y - data->origin.y);
  }
 
  switch (event) {
  case cv::EVENT_LBUTTONDOWN:
    data->origin = cv::Point(x, y);
    data->selection = cv::Rect(x, y, 0, 0);
    data->selectObject = true;
    break;
  case cv::EVENT_LBUTTONUP:
    data->selectObject = false;
    if (data->selection.width > 0 && data->selection.height > 0) data->trackObject = -1;
    break;
  }
}

void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  char c = cv::waitKey(10);
  if (c == 0x1b) return;
  cv::Mat image;
  try
    {
      image = cv_bridge::toCvShare(msg, "bgr8")->image;
      cv::imshow("window check",image);
    }
  catch (cv_bridge::Exception& e)
    {
      ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
  cv::Mat hsv;
  cv::cvtColor(image, hsv, cv::COLOR_BGR2HSV);
  if(data.trackObject){
    cv::Mat mask;
    cv::Scalar lower(0, smin, MIN(vmin,vmax));
    cv::Scalar upper(180,256,MAX(vmin,vmax));
    cv::inRange(hsv,lower,upper,mask);
    int ch[]={0,0};
    cv::Mat hue(hsv.size(),hsv.depth());
    mixChannels(&hsv,1,&hue,1,ch,1);
    if(data.trackObject < 0){
      trackWindow=data.selection;
      trackWindow&=cv::Rect(0,0,image.cols,image.rows);
      cv::Mat roi(hue,trackWindow),maskroi(mask,trackWindow);
      cv::calcHist(&roi, 1, 0, maskroi, hist, 1, &hsize, &phranges);
      cv::normalize(hist, hist, 0, 255, cv::NORM_MINMAX);
      histimg = cv::Scalar::all(0);
      int binW = histimg.cols / hsize;
      cv::Mat buf(1, hsize, CV_8UC3);
      for (int i = 0; i < hsize; i++) buf.at<cv::Vec3b>(i) = cv::Vec3b(cv::saturate_cast<uchar>(i*180. / hsize), 255, 255);
      cv::cvtColor(buf, buf, cv::COLOR_HSV2BGR);
      for (int i = 0; i < hsize; i++) {
	int val = cv::saturate_cast<int>(hist.at<float>(i)*histimg.rows / 255);
	cv::rectangle(histimg, cv::Point(i*binW, histimg.rows), cv::Point((i + 1)*binW, histimg.rows - val), cv::Scalar(buf.at<cv::Vec3b>(i)), -1, 8);
      }
      data.trackObject = 1;
    }
    cv::Mat backproj;
    cv::calcBackProject(&hue,1,0,hist,backproj,&phranges);
    backproj&=mask;
    cv::RotatedRect trackBox=cv::CamShift(backproj,trackWindow,cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 1));
    deg.data = 0.0671875 * trackBox.center.x - 21.5;
    ROS_INFO("centroid angle  : %f", deg.data);
    deg_pub.publish(deg);
    deg_flag = true;
    ellipse(image, trackBox, cv::Scalar(0, 0, 255), 3, 16); // cv::LINE_AA=16
  }
  if (data.selectObject && data.selection.width > 0 && data.selection.height > 0) {
    cv::Rect roi = data.selection & cv::Rect(0, 0, image.cols, image.rows);
    cv::Mat tmp(image, roi);
    cv::bitwise_not(tmp, tmp);
  }
  cv::imshow("CamShift", image);
  cv::imshow("Histogram", histimg);
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "human_detector");
  ros::NodeHandle nh;
  cv::namedWindow("Histogram");
  cv::namedWindow("CamShift");
  cv::namedWindow("window check");
  cv::setMouseCallback("CamShift", onMouse, &data);
  cv::createTrackbar("Vmin", "CamShift", &vmin, 256);
  cv::createTrackbar("Vmax", "CamShift", &vmax, 256);
  cv::createTrackbar("Smin", "CamShift", &smin, 256);
  image_transport::ImageTransport it(nh);
  image_transport::Subscriber sub = it.subscribe("camera/rgb/image_raw", 1, imageCallback);
  deg_pub=nh.advertise<std_msgs::Float64>("follow_publisher",1000);
  ROS_INFO("centroid angle in main  : %f", deg.data);
  ros::spin();
}
