#include "hw_interface.hpp"
#include <cmath>

RobotHWInterface::RobotHWInterface(ros::NodeHandle& nh) : nh(nh), command_timeout_(nh.createTimer(ros::Duration(0.1), &RobotHWInterface::commandTimeoutCallback, this, true, false)) {
     cmd_vel_sub = nh.subscribe("cmd_vel", 10, &RobotHWInterface::cmdVelCallback, this);

    velocity_command_pub = nh.advertise<robot_control::velocity_data>("velocity_command", 10);

    encoder_sub = nh.subscribe("/encoder_data", 10, &RobotHWInterface::encoderCallback, this);
    odom_pub = nh.advertise<nav_msgs::Odometry>("odom", 50);

    // Initialize parameters
    nh.param("wheel_control/wheel_separation_width", wheel_separation_width, 0.2f); 
    nh.param("wheel_control/wheel_separation_length", wheel_separation_length, 0.2f); 
    nh.param("ewheel_control/wheel_radius", wheel_radius, 0.2f)

    // Validate parameters
    if (wheel_separation_width <= 0 || wheel_separation_length <= 0) {
        ROS_FATAL("Invalid geometry parameters. Please check the configuration.");
        ros::shutdown();
    }

    x = 0;
    y = 0;
    th = 0;

    delta = false;
}

void RobotHWInterface::cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg) {
    const float vx = msg->linear.x;
    const float vy = msg->linear.y;
    const float omega = msg->angular.z;

    double phi;
    double theta_2;
    double v;

    double 

    double R = std::abs(v/omega);

    if (std::abs(omega) < 1e-5) {

        phi = 0;
        v = std::hypot(vx, vy);
        double w = v / wheel_radius;
        right_wheel_angular_speed = w;
        left_wheel_angular_speed = w;

    
    }
    else if (omega < 0){
    
        phi = - std::atan(wheel_separation_length / R);
        left_wheel_speed = omega * (R + wheel_separation_width/2);
        left_wheel_angular_speed = left_wheel_speed / wheel_radius;

        right_wheel_speed = omega * (R - wheel_separation_width/2);
        right_wheel_angular_speed = right_wheel_speed / wheel_radius;

    
    }
    else if (omega > 0){
    
        phi = std::atan(wheel_separation_length / R);
        left_wheel_speed = omega * (R - wheel_separation_width/2);
        left_wheel_angular_speed = left_wheel_speed / wheel_radius;

        right_wheel_speed = omega * (R + wheel_separation_width/2);
        right_wheel_angular_speed = right_wheel_speed / wheel_radius;
    
    }
    
    double phi_L = std::atan((2*wheel_separation_length*std::sin(phi))/(2*wheel_separation_length*std::cos(phi)-wheel_separation_width*std::sin(phi)));

    phi_L += 1.1969747762; // Somando diferença com a barra comum
    
    long double phi_L4 = phi_L*phi_L*phi_L*phi_L;
    long double phi_L3 = phi_L*phi_L*phi_L;
    long double phi_L2 = phi_L*phi_L;

    theta_2 = 0.00249637 * phi_L4 - 0.10561985 * phi_L3 + 0.57738126 * phi_L2 -0.49075692 * phi_L + 1.68385704;

    servo_angle = 180 - theta_2 * 180/M_PI;     // Converte para angulo do servo e passa para graus

    // Reset the command timeout with auto-restart
    command_timeout_.stop();
    command_timeout_.setPeriod(ros::Duration(0.05), true);
    command_timeout_.start();
}


void RobotHWInterface::publishWheelSpeeds() {
    robot_control::velocity_data msg;

    msg.angular_speed_left = left_wheel_angular_speed;
    msg.angular_speed_right = right_wheel_angular_speed;
    msg.servo_angle = servo_angle;

    velocity_command_pub.publish(msg);
}


void RobotHWInterface::encoderCallback(const robot_control::i2c_data::ConstPtr& msg) {

    x = msg->x / 1000;
    y = msg->y / 1000;
    th = msg->z / 1000;
    timestamp = msg->timestamp;
    
    if (!delta){
        x_old = x;
        y_old = y;
        th_old = th;
        timestamp_old = timestamp;
        delta = true;
    } else if (delta && timestamp != timestamp_old){
        vel_linear_x = (x - x_old)/(timestamp - timestamp_old);
        vel_linear_y = (y - y_old)/(timestamp - timestamp_old);
        vel_angular_z = (th - th_old)/(timestamp - timestamp_old);

        base_vel_linear = std::hypot(vel_linear_x, vel_linear_y);
        base_vel_angular = vel_angular_z;

        delta = false;
    }

    //std::cout << "Velocidade Linear X: " << vel_linearx << ", Velocidade Linear Y: " << vel_lineary << ", Velocidade Angular Z: " << vel_angular_z << std::endl;

    //std::cout << "Teste Encoder: " << teste << "\n";

}

void RobotHWInterface::commandTimeoutCallback(const ros::TimerEvent&) {
    updateWheelSpeedForDeceleration();
}

void RobotHWInterface::updateWheelSpeedForDeceleration() {
    // Desacelera cada roda gradualmente até zero

    if (std::abs(rear_left_wheel_speed) > DECELERATION_RATE) rear_left_wheel_speed -= DECELERATION_RATE * (rear_left_wheel_speed > 0 ? 1 : -1);
    else rear_left_wheel_speed = 0;

    if (std::abs(rear_right_wheel_speed) > DECELERATION_RATE) rear_right_wheel_speed -= DECELERATION_RATE * (rear_right_wheel_speed > 0 ? 1 : -1);
    else rear_right_wheel_speed = 0;

    // Verifica se todas as velocidades chegaram a zero, se não, continua desacelerando
    if (rear_left_wheel_speed != 0 || rear_right_wheel_speed != 0) {
        command_timeout_.stop();
        command_timeout_.setPeriod(ros::Duration(CMD_VEL_TIMEOUT_DEACELERATION_PERIOD), true); // Use um intervalo mais curto para desaceleração suave
        command_timeout_.start();
    }
}

void RobotHWInterface::updateOdometry() {

    current_time = ros::Time::now();

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
    odom.twist.twist.linear.x = base_vel_linear;
    odom.twist.twist.linear.y = 0;
    odom.twist.twist.angular.z = base_vel_angular;

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