#pragma once

#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
//#include "robot_control/velocity_data.h"
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Quaternion.h>
#include <geometry_msgs/Point.h>
//#include "robot_control/encoder_data.h" 
#include <tf/transform_broadcaster.h>
#include <tf/tf.h>
#include <cmath>
#include <geometry_msgs/TransformStamped.h>

#include "initializers.hpp"



class RobotHWInterface {
public:

    // Ajustado para receber NodeHandle por referência
    RobotHWInterface(ros::NodeHandle& nh, i2c_config_t& i2c_configuration, i2c_slave_device_config_t& slave_configuration); 
    void init_i2c(i2c_config_t& i2c_configuration, i2c_slave_device_config_t& slave_configuration);
    void write_data();
    void read_data();
    void ackermann_inverse();
    void calculate_speed();
    void cmdVelCallback(const geometry_msgs::Twist::ConstPtr& msg);
    void commandTimeoutCallback(const ros::TimerEvent&); // Callback para o timeout
    void updateWheelSpeedForDeceleration(); // Desaceleração
    double mapSpeed(double v_input); // Normalização da velocidade
    double mapTangent(double tangent_input);
    void updateOdometry();

    bsc_i2c_handle i2c_master;

private:

    ros::NodeHandle nh;

    // Hw Interface 
    ros::Subscriber cmd_vel_sub;

    // Odometria
    ros::Publisher odom_pub;

    // Última orientação do robô (quaternion)
    geometry_msgs::Quaternion last_orientation;

    // Posição atual do robô
    geometry_msgs::Point current_position;

    // Temporizador para o timeout de comandos
    ros::Timer command_timeout_; 

    // Variables to send
    float rear_left_wheel_speed = 0.0;
    float rear_right_wheel_speed= 0.0;
    float servo_angle = 45.0; //degrees

    // Parâmetros carregados do arquivo .yaml
    // used for calculating  the inverse ackermann kinematics
    double wheel_radius; // Raio das rodas
    double wheel_separation_width;
    double wheel_separation_lenght;
    double front_four_bar_separation;
    double deceleration_rate; // Taxa de desaceleração
    double vcenter_max_speed; // Velocidade máxima
    double vcenter_min_speed; // Velocidade mínima


    // Parâmetros para a odometria 

    int total_x_displacement_intermed = 0.0;
    int total_y_displacement_intermed  = 0.0;
    int total_theta_displacement_intermed = 0.0;
    uint32_t time_stamp_intermed = 0;

    double total_x_displacement_old = 0.0;
    double total_y_displacement_old  = 0.0;
    double total_theta_displacement_old = 0.0;
    uint32_t time_stamp_old = 0;

    double total_x_displacement = 0.0;
    double total_y_displacement  = 0.0;
    double total_theta_displacement = 0.0;
    uint32_t time_stamp = 0;

    double vel_linear_x = 0.0;
    double vel_linear_y = 0.0;
    double vel_angular_z = 0.0;

    double vel_linear_x_to_esp = 0.0;
    double vel_linear_y_to_esp = 0.0;
    double vel_angular_z_to_esp = 0.0;

    ros::Time current_time;
    tf::TransformBroadcaster odom_broadcaster;


    //four bar parameters
    double link_a2;
    double link_b2;
    double link_c;
    double link_d2;
    double phi_2_o;
    double phi_tangent_lower_limit;
    double phi_tangent_upper_limit;
    //double q2_lower_limit;
    //double q2_upper_limit;

};
