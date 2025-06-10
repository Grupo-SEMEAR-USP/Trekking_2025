#include "hw_interface.hpp"

RobotHWInterface::RobotHWInterface(ros::NodeHandle& nh) : nh(nh), command_timeout_(nh.createTimer(ros::Duration(0.1), &RobotHWInterface::commandTimeoutCallback, this, true, false)) {
    cmd_vel_sub = nh.subscribe("cmd_vel", 10, &RobotHWInterface::cmdVelCallback, this);

    velocity_command_pub = nh.advertise<robot_control::velocity_data>("velocity_command", 10);

    encoder_sub = nh.subscribe("/encoder_data", 10, &RobotHWInterface::encoderCallback, this);
    odom_pub = nh.advertise<nav_msgs::Odometry>("odom", 50);

    // Initialize parameters
    nh.param("wheel_control/wheel_radius", wheel_radius, 0.05f); // Default value: 0.05
    nh.param("wheel_control/wheel_separation_width", wheel_separation_width, 0.2f); // Default value: 0.2
    nh.param("wheel_control/wheel_separation_length", wheel_separation_length, 0.2f); // Default value: 0.2
    nh.param("wheel_control/max_speed", max_speed, 1.0f); // Default value: 1.0
    nh.param("wheel_control/min_speed", min_speed, -1.0f); // Default value: -1.0

    // Validate parameters
    if (wheel_radius <= 0 || wheel_separation_width <= 0 || wheel_separation_length <= 0) {
        ROS_FATAL("Invalid geometry parameters. Please check the configuration.");
        ros::shutdown();
    }

    x = 0;
    y = 0;
    th = 0;

    vel_linearx = 0;
    vel_lineary = 0;
    vel_angular_z = 0;

    auto current_time = ros::Time::now();

}

void RobotHWInterface::cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg) {
    const float vx = msg->linear.x;
    const float vy = msg->linear.y;
    const float omega = msg->angular.z;

    // Ensure wheel_radius is not zero to avoid division by zero error
    if (wheel_radius == 0) {
        ROS_ERROR("wheel_radius is zero, cannot calculate wheel speeds");
        return;
    }

    const float speed_multiplier = 1 / wheel_radius;
    const float omega_effect = omega * base_geometry;

    rear_left_wheel_speed =  //depende do modelo dinâmico
    rear_right_wheel_speed = //depende do modelo dinâmico

    // Reset the command timeout with auto-restart
    command_timeout_.stop();
    command_timeout_.setPeriod(ros::Duration(0.05), true);
    command_timeout_.start();
}


void RobotHWInterface::publishWheelSpeeds() {
    robot_control::velocity_data msg;

    msg.angular_speed_left = rear_left_wheel_speed;
    msg.angular_speed_right = rear_right_wheel_speed;
    msg.servo_angle = 0;// definir depois

    velocity_command_pub.publish(msg);
}


void RobotHWInterface::encoderCallback(const robot_control::i2c_data::ConstPtr& msg) {

    current_time = ros::Time::now();

    vel_linearx = // depende do modelo dinâmico
    vel_lineary = // depende do modelo dinâmico
    vel_angular_z = // depende do modelo dinâmico
    //std::cout << "Velocidade Linear X: " << vel_linearx << ", Velocidade Linear Y: " << vel_lineary << ", Velocidade Angular Z: " << vel_angular_z << std::endl;

    //std::cout << "Teste Encoder: " << teste << "\n";

}

// EU ACHO Q O VOID ABAIXO SE MANTÉM

void RobotHWInterface::updateOdometry() {

    current_time = ros::Time::now();

    x += delta_x;
    y += delta_y;
    th += delta_th;

    geometry_msgs::Quaternion odom_quat = tf::createQuaternionMsgFromYaw(th);

    geometry_msgs::TransformStamped odom_trans;
    odom_trans.header.stamp = current_time;
    odom_trans.header.frame_id = "odom";
    odom_trans.child_frame_id = "base_link";

    odom_trans.transform.translation.x = x;
    odom_trans.transform.translation.y = y;
    odom_trans.transform.translation.z = 0.0;
    odom_trans.transform.rotation = odom_quat;

    odom_broadcaster.sendTransform(odom_trans);

    nav_msgs::Odometry odom;
    odom.header.stamp = current_time;
    odom.header.frame_id = "odom";

    odom.pose.pose.position.x = x;
    odom.pose.pose.position.y = y;
    odom.pose.pose.position.z = 0.0;
    odom.pose.pose.orientation = odom_quat;

    odom.child_frame_id = "base_link";
    odom.twist.twist.linear.x = vel_linearx;
    odom.twist.twist.linear.y = vel_lineary;
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