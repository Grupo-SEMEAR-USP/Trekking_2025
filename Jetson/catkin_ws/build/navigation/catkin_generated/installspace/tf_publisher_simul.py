#!/usr/bin/env python3
import rospy

import tf2_ros
import geometry_msgs.msg
from nav_msgs.msg import Odometry
from tf.transformations import quaternion_from_euler


class   TFClass:
    def __init__(self): #Constructor of the class
        self.global_frame_id = rospy.get_param('~global_frame_id', 'odom')
        self.odom_publisher = rospy.Subscriber('/odom', Odometry, self.update_tf, queue_size=10)

        self.br = tf2_ros.TransformBroadcaster()

        #Defining the map frame and creating the transform between it and the odom frame
        self.t_map_odom = geometry_msgs.msg.TransformStamped()
        self.t_map_odom.header.stamp = rospy.Time.now()
        self.t_map_odom.header.frame_id = "/map"
        self.t_map_odom.child_frame_id = "/odom"
        self.t_map_odom.transform.translation.x = 3.1769 #-2.5836, 
        self.t_map_odom.transform.translation.y = 1.8869 #-1.7246, 
        self.t_map_odom.transform.translation.z = 0
        
        self.quartenion = quaternion_from_euler(0.0, 0.0, 3.14159)

        self.t_map_odom.transform.rotation.x = self.quartenion[0]
        self.t_map_odom.transform.rotation.y = self.quartenion[1]
        self.t_map_odom.transform.rotation.z = self.quartenion[2]
        self.t_map_odom.transform.rotation.w = self.quartenion[3]

        #Defining the initial conditions of the transform between the odom and the base_link frames
        self.t = geometry_msgs.msg.TransformStamped()
        self.t.header.stamp = rospy.Time.now()
        self.t.header.frame_id = "/odom"
        self.t.child_frame_id = "base_link"
        self.t.transform.translation.x = 0.0
        self.t.transform.translation.y = 0.0
        self.t.transform.translation.z = 0.0
        self.t.transform.rotation.x = 0.0
        self.t.transform.rotation.y = 0.0
        self.t.transform.rotation.z = 0.0
        self.t.transform.rotation.w = 1

        #Defining virtual frames to help in some geometry calculations in the ackermann_controller node
        #Left virtual frame
        self.t_left = geometry_msgs.msg.TransformStamped()
        self.t_left.header.stamp = rospy.Time.now()
        self.t_left.header.frame_id = "base_link"
        self.t_left.child_frame_id = "left_virtual_frame"
        self.t_left.transform.translation.x = 0.10378 
        self.t_left.transform.translation.y = -0.21397
        self.t_left.transform.translation.z = -0.0371
        self.t_left.transform.rotation.x = 0.0
        self.t_left.transform.rotation.y = 0.0
        self.t_left.transform.rotation.z = 0.0
        self.t_left.transform.rotation.w = 1.0

        #Right virtual frame
        self.t_right = geometry_msgs.msg.TransformStamped()
        self.t_right.header.stamp = rospy.Time.now()
        self.t_right.header.frame_id = "base_link"
        self.t_right.child_frame_id = "right_virtual_frame"
        self.t_right.transform.translation.x = -0.10624 
        self.t_right.transform.translation.y = -0.21396 
        self.t_right.transform.translation.z = -0.0371
        self.t_right.transform.rotation.x = 0.0
        self.t_right.transform.rotation.y = 0.0
        self.t_right.transform.rotation.z = 0.0
        self.t_right.transform.rotation.w = 1.0

    def update_time_stamps(self): #Updates the time stamps of all frames
        self.t_map_odom.header.stamp = rospy.Time.now()
        self.t.header.stamp = rospy.Time.now()
        self.t_left.header.stamp = rospy.Time.now()
        self.t_right.header.stamp = rospy.Time.now()
        

    def update_tf(self, msg): #Callback function to the topic /odom
        #Filling the transform message with the information from odometry (tf between base_link and odom)
        self.t.transform.translation.x = msg.pose.pose.position.x
        self.t.transform.translation.y = msg.pose.pose.position.y
        self.t.transform.translation.z = 0.0
        self.t.transform.rotation = msg.pose.pose.orientation

        self.update_time_stamps() #Updating the time stamps of all frames

        #Broadcasting the transforms of all frames
        self.br.sendTransform(self.t_map_odom) #Updating the transform between the map and the odom frames
        self.br.sendTransform(self.t) #Updating the transform between odom and base_link
        self.br.sendTransform(self.t_left) #Updating the transform between the base_link and the left virtual frame
        self.br.sendTransform(self.t_right) #Updating the transform between the base_link and the right virtual frame

if __name__ == '__main__':
    rospy.init_node('tf_pub_node')
    rospy.loginfo("TF node initialized")
    
    oc = TFClass()

    try:
        rospy.spin()
    except rospy.ROSInterruptException:
        pass
