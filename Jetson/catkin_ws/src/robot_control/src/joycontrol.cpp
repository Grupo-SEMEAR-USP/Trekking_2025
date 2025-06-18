/*#include <ros/ros.h>
//#include <geometry_msgs/Twist.h>
#include <sensor_msgs/Joy.h>
#include <ackermann_msgs/AckermannDrive.h>


class TeleopJoy
{
public:
  TeleopJoy();

private:
  void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);

  ros::NodeHandle nh_;

  //int linear_, angular_;
  double joy_scale, left_scale_trigger,right_scale_trigger;
  ros::Publisher vel_pub_;
  ros::Subscriber joy_sub_;

};


TeleopJoy::TeleopJoy()//:
  //linear_(1),
  //angular_(2)
{

  joy_scale = 0.349;
  left_scale_trigger = 1.5;
  right_scale_trigger = 1.5;

  nh_.param("joy_params/joy_scale", joy_scale);
  nh_.param("joy_params/left_scale_trigger", left_scale_trigger);
  nh_.param("joy_params/right_scale_trigger", right_scale_trigger);



  vel_pub_ = nh_.advertise<std_msgs::Twist>("cmd_vel", 1);


  joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy", 10, &TeleopJoy::joyCallback, this);

}

void TeleopJoy::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
  static ackermann_msgs::AckermannDrive acker_drive;
  acker_drive.steering_angle = joy_scale*joy->axes[0];
  acker_drive.speed = (-right_scale_trigger*((joy->axes[5] + 1)/2) )+(left_scale_trigger*((joy->axes[2] + 1)/2));
  vel_pub_.publish(acker_drive);
}


int main(int argc, char** argv)
{
  ros::init(argc, argv, "joy_teleop");
  TeleopJoy teleop;

  ros::spin();
}*/

//Para testes com Twist
#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <geometry_msgs/Twist.h>

class TeleopJoy
{
public:
  TeleopJoy();

private:
  void joyCallback(const sensor_msgs::Joy::ConstPtr& joy);

  ros::NodeHandle nh_;

  double joy_scale, left_scale_trigger, right_scale_trigger;
  ros::Publisher vel_pub_;
  ros::Subscriber joy_sub_;
};

TeleopJoy::TeleopJoy()
{
  joy_scale = 0.349;
  left_scale_trigger = 1.5;
  right_scale_trigger = 1.5;

  nh_.param("joy_params/joy_scale", joy_scale, joy_scale);
  nh_.param("joy_params/left_scale_trigger", left_scale_trigger, left_scale_trigger);
  nh_.param("joy_params/right_scale_trigger", right_scale_trigger, right_scale_trigger);

  vel_pub_ = nh_.advertise<geometry_msgs::Twist>("cmd_vel", 1);
  joy_sub_ = nh_.subscribe<sensor_msgs::Joy>("joy", 10, &TeleopJoy::joyCallback, this);
}

void TeleopJoy::joyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
  geometry_msgs::Twist cmd_vel_msg;

  // Conversão: eixo do joystick para angular.z
  cmd_vel_msg.angular.z = joy_scale * joy->axes[0];

  // Conversão: triggers para linear.x (mesma lógica que você usava para o speed)
  double speed = (-right_scale_trigger * ((joy->axes[5] + 1) / 2)) + (left_scale_trigger * ((joy->axes[2] + 1) / 2));
  cmd_vel_msg.linear.x = speed;

  vel_pub_.publish(cmd_vel_msg);
}

int main(int argc, char** argv)
{
  ros::init(argc, argv, "joy_teleop");
  TeleopJoy teleop;
  ros::spin();
}
