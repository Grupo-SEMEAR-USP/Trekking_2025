#include "hw_interface.hpp"
#include <cmath>

RobotHWInterface::RobotHWInterface(ros::NodeHandle& nh) : nh(nh), command_timeout_(nh.createTimer(ros::Duration(0.1), &RobotHWInterface::commandTimeoutCallback, this, true, false)) {
    cmd_vel_sub = nh.subscribe("cmd_vel", 10, &RobotHWInterface::cmdVelCallback, this);

    velocity_command_pub = nh.advertise<robot_control::VelocityData>("velocity_command", 10);

    //encoder_sub = nh.subscribe("/i2c_data", 10, &RobotHWInterface::encoderCallback, this);
    odom_pub = nh.advertise<nav_msgs::Odometry>("odom", 50);

    // Initialize parameters
    nh.param("wheel_control/wheel_separation_width", wheel_separation_width, 0.2f); 
    nh.param("wheel_control/wheel_separation_length", wheel_separation_length, 0.2f); 
    nh.param("ewheel_control/wheel_radius", wheel_radius, 0.2f);

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
    const float v = msg->linear.x;
    const float omega = msg->angular.z;

    double phi;
    double theta_2;

    double R;

    if (std::abs(omega) < 1e-5) {
        // Movimento reto
        phi = 0.0;

        double w = v / wheel_radius;
        left_wheel_angular_speed = w;
        right_wheel_angular_speed = w;
    }
    else {
        // Movimento curvo
        R = v / omega;  // Pode ser negativo se for movimento de ré

        // Calcular o ângulo de esterçamento (Ackermann)
        phi = std::atan(wheel_separation_length / R);

        // Calcular velocidades lineares de cada roda traseira
        double v_left = v - (omega * wheel_separation_width / 2.0);
        double v_right = v + (omega * wheel_separation_width / 2.0);

        // Converter para velocidades angulares das rodas
        left_wheel_angular_speed = v_left / wheel_radius;
        right_wheel_angular_speed = v_right / wheel_radius;
    }
    
    double phi_L = std::atan((2*wheel_separation_length*std::sin(phi))/(2*wheel_separation_length*std::cos(phi)-wheel_separation_width*std::sin(phi)));

    phi_L += 1.944617877; // Somando diferença com a barra comum
    
    long double phi_L4 = phi_L*phi_L*phi_L*phi_L;
    long double phi_L3 = phi_L*phi_L*phi_L;
    long double phi_L2 = phi_L*phi_L;

    theta_2 = 1.67913859 * phi_L4 - 12.93393178 * phi_L3 + 35.44166937 * phi_L2 -38.12573195 * phi_L + 12.76117928;

    servo_angle = 180 - theta_2 * 180/M_PI;     // Converte para angulo do servo e passa para graus

    //ROS_INFO("Angulo do servo: %f", servo_angle);

    // Reset the command timeout with auto-restart
    command_timeout_.stop();
    command_timeout_.setPeriod(ros::Duration(0.05), true);
    command_timeout_.start();
}


void RobotHWInterface::publishWheelSpeeds() {
    robot_control::VelocityData msg;

    msg.angular_speed_left = left_wheel_angular_speed;
    msg.angular_speed_right = right_wheel_angular_speed;
    msg.servo_angle = servo_angle;

    velocity_command_pub.publish(msg);
}


/*void RobotHWInterface::encoderCallback(const robot_control::I2cData::ConstPtr& msg) {

    ROS_INFO("Callback do encoder ativado!");
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

    ROS_INFO("x: %f, y: %f, vel_linear_x: %f, vel_linear_y %f", x, y, vel_linear_x, vel_linear_y);
}*/

void RobotHWInterface::commandTimeoutCallback(const ros::TimerEvent&) {
    updateWheelSpeedForDeceleration();
}

void RobotHWInterface::updateWheelSpeedForDeceleration() {
    // Desacelera cada roda gradualmente até zero

    if (std::abs(left_wheel_speed) > DECELERATION_RATE) left_wheel_speed -= DECELERATION_RATE * (left_wheel_speed > 0 ? 1 : -1);
    else left_wheel_speed = 0;

    if (std::abs(right_wheel_speed) > DECELERATION_RATE) right_wheel_speed -= DECELERATION_RATE * (right_wheel_speed > 0 ? 1 : -1);
    else right_wheel_speed = 0;

    // Verifica se todas as velocidades chegaram a zero, se não, continua desacelerando
    if (left_wheel_speed != 0 || right_wheel_speed != 0) {
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
    odom.twist.twist.linear.x = 0;
    odom.twist.twist.linear.y = base_vel_linear;
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