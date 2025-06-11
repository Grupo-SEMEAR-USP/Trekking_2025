#include "hw_interface.hpp"
#include <cmath>

RobotHWInterface::RobotHWInterface(ros::NodeHandle& nh) : nh(nh), command_timeout_(nh.createTimer(ros::Duration(0.1), &RobotHWInterface::commandTimeoutCallback, this, true, false)) {
     cmd_vel_sub = nh.subscribe("cmd_vel", 10, &RobotHWInterface::cmdVelCallback, this);

    velocity_command_pub = nh.advertise<robot_control::velocity_data>("velocity_command", 10);

    encoder_sub = nh.subscribe("/encoder_data", 10, &RobotHWInterface::encoderCallback, this);
    odom_pub = nh.advertise<nav_msgs::Odometry>("odom", 50);

    // Initialize parameters
    nh.param("wheel_control/wheel_separation_width", wheel_separation_width, 0.2f); // Default value: 0.2
    nh.param("wheel_control/wheel_separation_length", wheel_separation_length, 0.2f); // Default value: 0.2

    // Validate parameters
    if (wheel_separation_width <= 0 || wheel_separation_length <= 0) {
        ROS_FATAL("Invalid geometry parameters. Please check the configuration.");
        ros::shutdown();
    }

    x = 0;
    y = 0;
    th = 0;

    timestamp = ros::Time::now();
}

void RobotHWInterface::cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg) {
    const float vx = msg->linear.x;
    const float vy = msg->linear.y;
    const float omega = msg->angular.z;

    double v = std::hypot(vx, vy);

    double R = v/omega;

    if (std::abs(omega) < 1e-5) {
        rear_left_wheel_speed = v;
        rear_right_wheel_speed = v;
    }
    else if (omega < 0){
        rear_left_wheel_speed = omega * (R + wheel_separation_width/2);
        rear_right_wheel_speed = omega * (R - wheel_separation_width/2);
    }
    else if (omega > 0){
        rear_left_wheel_speed = omega * (R - wheel_separation_width/2);
        rear_right_wheel_speed = omega * (R + wheel_separation_width/2);
    }
    
    double phi = std::atan(wheel_separation_length / R);
    
    double phi_L = std::atan((2*wheel_separation_length*std::sin(phi))/(2*wheel_separation_length*std::cos(phi)-wheel_separation_width*std::sin(phi)));

    phi_L += 1.1969747762; // cheacar se está certo
    
    long double phi_L4 = phi_L*phi_L*phi_L*phi_L;
    long double phi_L3 = phi_L*phi_L*phi_L;
    long double phi_L2 = phi_L*phi_L;

    theta_2 = 0.00249637 * phi_L4 - 0.10561985 * phi_L3 + 0.57738126 * phi_L2 -0.49075692 * phi_L + 1.68385704;

    theta_2 += 1.57049091488;

    // Reset the command timeout with auto-restart
    command_timeout_.stop();
    command_timeout_.setPeriod(ros::Duration(0.05), true);
    command_timeout_.start();
}


void RobotHWInterface::publishWheelSpeeds() {
    robot_control::velocity_data msg;

    msg.angular_speed_left = rear_left_wheel_speed;
    msg.angular_speed_right = rear_right_wheel_speed;
    msg.servo_angle = theta_2;

    velocity_command_pub.publish(msg);
}


void RobotHWInterface::encoderCallback(const robot_control::i2c_data::ConstPtr& msg) {

    x = msg->x;
    y = msg->y;
    th = msg->z;
    timestamp = ros::Time::now();
    
    //std::cout << "Velocidade Linear X: " << vel_linearx << ", Velocidade Linear Y: " << vel_lineary << ", Velocidade Angular Z: " << vel_angular_z << std::endl;

    //std::cout << "Teste Encoder: " << teste << "\n";

}


void RobotHWInterface::updateOdometry() {

    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);

    geometry_msgs::TransformStamped odom_trans;
    odom_trans.header.stamp = timestamp;
    odom_trans.header.frame_id = "odom";
    odom_trans.child_frame_id = "base_link";

    odom_trans.transform.translation.x = x;
    odom_trans.transform.translation.y = y;
    odom_trans.transform.translation.z = 0.0;
    odom_trans.transform.rotation = odom_quat;

    odom_broadcaster.sendTransform(odom_trans);

    nav_msgs::Odometry odom;
    odom.header.stamp = timestamp;
    odom.header.frame_id = "odom";

    odom.pose.pose.position.x = x;
    odom.pose.pose.position.y = y;
    odom.pose.pose.position.z = 0.0;
    odom.pose.pose.orientation = odom_quat;

    odom.child_frame_id = "base_link";
    odom.twist.twist.linear.x = 0;
    odom.twist.twist.linear.y = 0;
    odom.twist.twist.angular.z = 0;

    odom_pub.publish(odom);

    //std::cout << "Teste Odometry: " << teste << "\n";

}

int main(int argc, char** argv) {
    ros::init(argc, argv, "hw_interface");
    ros::NodeHandle nh; // Cria um NodeHandle

    RobotHWInterface controller(nh); // Passa o NodeHandle como argumento

    ros::Rate rate(HW_IF_UPDATE_FREQ);
    ros::AsyncSpinner spinner(4);
    spinner.start();

    while (ros::ok())
    {
        //controller.teste = controller.teste + 1;
        controller.publishWheelSpeeds();
        controller.updateOdometry();
        
        rate.sleep();
    }

    return 0;
}