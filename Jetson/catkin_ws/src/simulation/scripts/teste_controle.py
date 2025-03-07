#!/usr/bin/env python3
#Nó de teste dos controladores das juntas do robô
import rospy
from std_msgs.msg import Float64

class ManualController:
    def __init__(self):
        #Criando os publishers nos tópicos dos controladores de velocidade
        self.pub_rear_left = rospy.Publisher("left_rear_axle_ctrlr/command", Float64, queue_size=10)
        self.pub_rear_right = rospy.Publisher("right_rear_axle_ctrlr/command", Float64, queue_size=10)
        self.pub_front_left = rospy.Publisher("left_front_axle_ctrlr/command", Float64, queue_size=10)
        self.pub_front_right = rospy.Publisher("right_front_axle_ctrlr/command", Float64, queue_size=10)

        #Criando os publishers nos tópicos dos controladores de direção (ackermann)
        self.pub_steer_left = rospy.Publisher("left_steering_ctrlr/command", Float64, queue_size=10)
        self.pub_steer_right = rospy.Publisher("right_steering_ctrlr/command", Float64, queue_size=10)

        #Definindo os parâmetros que serão publicados nos tópicos dos controladores de cada junta
        self.vel_rear_left = 10.0
        self.vel_rear_right = 10.0
        self.vel_front_left = 10.0
        self.vel_front_right = 10.0

        self.ang_steer_right = 0.0
        self.ang_steer_left = 0.0

        self.pub()

    def pub(self):
        while not rospy.is_shutdown():
            self.pub_rear_left.publish(self.vel_rear_left)
            self.pub_rear_right.publish(self.vel_rear_right)
            self.pub_front_left.publish(self.vel_front_left)
            self.pub_front_right.publish(self.vel_front_right)

            self.pub_steer_left.publish(self.ang_steer_left)
            self.pub_steer_right.publish(self.ang_steer_right)


if __name__ == "__main__":
    #Iniciando o nó
    rospy.init_node("manual_controller_node")
    rospy.loginfo("Iniciado nó de controle manual")

    mc = ManualController()

    try:
        rospy.spin()
    except rospy.ROSInterruptException:
        pass
    




