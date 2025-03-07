#!/usr/bin/env python3

import rospy
from nav_msgs.msg import Path
from std_msgs.msg import Bool
import pickle
import os

class PathPublisher:
    def __init__(self):
        self.path_pub = rospy.Publisher('/plan', Path, queue_size=1)
        self.move_status_sub = rospy.Subscriber("path_follower/move_status", Bool, self.update_move_status, queue_size=1)
        self.move_status_pub = rospy.Publisher("path_follower/move_status", Bool, queue_size=1)
        self.path_data = []

        self.rate = rospy.Rate(10)

        self.current_path_index = 0
        self.move_status = Bool()
        self.move_status.data = False

        self.files_names = ["path_1.pkl", "path_2.pkl"]#, "path_3.pkl"]
        self.directory = os.path.abspath(rospy.get_param("~path", 'src/navigation/paths')) #Getting the directory in which we will save the path

        #Loading the paths from the disk and storing them in the path_data list
        for i in range (len(self.files_names)):
            self.file_path = os.path.join(self.directory, self.files_names[i])
            self.read_path_from_disk()
        
        #Running the node's routine
        self.run_node()

    def update_move_status(self, msg): #Callback function to the move_status topic
        self.move_status.data = msg.data

    def read_path_from_disk(self):
        if os.path.exists(self.file_path):
            with open(self.file_path, 'rb') as file:
                self.path_data.append(pickle.load(file))
                rospy.loginfo("Path read")
        else:
            rospy.logerr(f"File {self.file_path} does not exist")

    
    def run_node(self):
        while not rospy.is_shutdown():
            #Updating the path we are following according to move_status flag
            if (self.move_status.data and self.current_path_index < len(self.path_data)): 
                self.current_path_index += 1
                self.move_status.data = False 
                self.move_status_pub.publish(self.move_status) #Updating the move status so the controller can see and resume its work

            if (self.current_path_index <= len(self.path_data)):
                self.path_pub.publish(self.path_data[self.current_path_index]) #Publishing the current path

            self.rate.sleep()


if __name__ == '__main__':
    rospy.init_node('path_publisher_node')

    path_publisher = PathPublisher()

    try:
        rospy.spin()
    except rospy.ROSInterruptException:
        pass
