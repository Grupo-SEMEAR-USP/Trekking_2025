#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <geometry_msgs/PoseArray.h>
#include <geometry_msgs/Pose.h>


typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  ros::init(argc, argv, "simple_navigation_goals");
  ros::NodeHandle nh;

  ros::Publisher via_points_pub = nh.advertise<geometry_msgs::PoseArray>("move_base/TebLocalPlannerROS/via_points", 10);

  //tell the action client that we want to spin a thread by default
  MoveBaseClient ac("move_base", true);

  //wait for the action server to come up
  while(!ac.waitForServer(ros::Duration(5.0))){
    ROS_INFO("Waiting for the move_base action server to come up");
  }

  // Cria mensagem dos via points
  geometry_msgs::PoseArray via_points;

  via_points.header.frame_id = "map";
  via_points.header.stamp = ros::Time::now();

  geometry_msgs::Pose p;
  p.position.x = 6.0;
  p.position.y = 3.0;
  p.position.z = 0.0;

  via_points.poses.push_back(p);

  ROS_INFO("Via points published!");
  via_points_pub.publish(via_points);

  move_base_msgs::MoveBaseGoal goal;

  //we'll send a goal to the robot to move 1 meter forward
  goal.target_pose.header.frame_id = "map";
  goal.target_pose.header.stamp = ros::Time::now();

  goal.target_pose.pose.position.x = 9.0;
  goal.target_pose.pose.position.y = 1.0;
  goal.target_pose.pose.orientation.w = 1.0;

  ROS_INFO("Sending goal");
  ac.sendGoal(goal);

  ac.waitForResult();

  if(ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    ROS_INFO("Hooray, the base moved 15 meter forward");
  else
    ROS_INFO("The base failed to move forward 1 meter for some reason");

  return 0;
  }