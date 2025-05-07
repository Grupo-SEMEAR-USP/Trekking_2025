#!/usr/bin/env python3
import rospy
import tf
import numpy as np

from nav_msgs.msg import Odometry

class OdometryClass:
    def __init__(self): #Constructor of the class
        self.tf_listener = tf.TransformListener() #Defining a tf listener
        while (True): #Assuring that the tfs are available
            try:
                self.tf_listener.waitForTransform("odom", "base_link_front", rospy.Time(), rospy.Duration(1))
                rospy.loginfo("Transformações disponíveis")
                break
            except tf.Exception as e:
                rospy.loginfo("Transformações ainda não disponíveis")

        self.odom_publisher = rospy.Publisher('/odom_front', Odometry, queue_size=1)
        rospy.Subscriber('odom', Odometry, self.update_odom_msg, queue_size=1) #Gets the odometry info from the base_link (rear)

        self.odom_msg = Odometry()
        self.odom_msg.header.frame_id = "odom"
        self.odom_msg.child_frame_id = "base_link_front"

        self.rate = rospy.Rate(50) #Defining the rate that odom_msg will be published

        self.odom_pub()

    def update_odom_msg(self, msg): #Callback function to the topic /odom
        trans, rot2 = self.tf_listener.lookupTransform("base_link_front", "odom", rospy.Time(0))
        
        x, y, z = trans

        self.odom_msg.pose.pose.position.x = x
        self.odom_msg.pose.pose.position.y = y
        self.odom_msg.pose.pose.position.z = z

        self.odom_msg.pose.pose.orientation = msg.pose.pose.orientation


    
    def odom_pub(self):
        while not rospy.is_shutdown():
            self.odom_msg.header.stamp = rospy.Time.now() #Updating the time stamps of the odom msg

            self.odom_publisher.publish(self.odom_msg) #Publishing the odometry message into the topic /odom_front

            self.rate.sleep()

if __name__ == '__main__':
    rospy.init_node('odom_front_node')
    rospy.loginfo("Odometry_front node initialized")
    
    oc = OdometryClass()

    try:
        rospy.spin()
    except rospy.ROSInterruptException:
        pass
