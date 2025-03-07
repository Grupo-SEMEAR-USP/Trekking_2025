#!/usr/bin/env python3
import rospy

from gazebo_msgs.msg import ModelStates
from nav_msgs.msg import Odometry

class OdometryClass:
    def __init__(self): #Constructor of the class
        self.vehicle_name = rospy.get_param('~vehicle_name', 'ackermann_vehicle')
        self.global_frame_id = rospy.get_param('~global_frame_id', 'odom')
        self.odom_publisher = rospy.Publisher('/odom', Odometry, queue_size=1)
        rospy.Subscriber('/gazebo/model_states', ModelStates, self.update_odom_msg, self.vehicle_name) #Gets the info of Pose and Twist from this topic, directly from gazebo

        self.vehicle_index = 0 #It's the correct index of the robot in the ModelStates msg

        self.odom_msg = Odometry()

        self.rate = rospy.Rate(50) #Defining the rate that odom_msg will be published and the transform information will be sent

        self.odom_pub()

    def update_odom_msg(self, msg, vehicle_name): #Callback function to the topic gazebo/model_states
        self.vehicle_index = msg.name.index(vehicle_name)

        #Filling the odometry message with the information (Pose and Twist) from gazebo
        self.odom_msg.header.frame_id = self.global_frame_id
        self.odom_msg.child_frame_id = vehicle_name
        self.odom_msg.pose.pose = msg.pose[self.vehicle_index]
        self.odom_msg.twist.twist = msg.twist[self.vehicle_index]
    
    def odom_pub(self):
        while not rospy.is_shutdown():
            self.odom_msg.header.stamp = rospy.Time.now() #Updating the time stamps of the odom msg

            self.odom_publisher.publish(self.odom_msg) #Publishing the odometry message into the topic /odom

            self.rate.sleep()

if __name__ == '__main__':
    rospy.init_node('odom_pub_simul')
    rospy.loginfo("Odometry node initialized")
    
    oc = OdometryClass()

    try:
        rospy.spin()
    except rospy.ROSInterruptException:
        pass
