#!/usr/bin/env python3
import rospy

from nav_msgs.msg import Odometry
from geometry_msgs.msg import Twist, Pose
from std_msgs.msg import Header
from tf.transformations import quaternion_from_euler

class OdometryClass:
    def __init__(self): #Constructor of the class
        self.odom_pub = rospy.Publisher('/odom', Odometry, queue_size=10)

        self.odom_msg = Odometry()

        # Inicializar variáveis
        self.x = 0.0  # Posição inicial no eixo x
        self.y = 0.0  # Posição inicial no eixo y
        self.theta = -1.570796  # Ângulo inicial (sem rotação)
        self.linear_velocity = 0.5  # Velocidade linear constante ao longo do eixo y (m/s)
        self.angular_velocity = 0.0  # Sem rotação

        self.rate = rospy.Rate(10) #Defining the rate that odom_msg will be published and the transform information will be sent

        self.odom_pub_func()
    
    def odom_pub_func(self):
        while not rospy.is_shutdown():
            # Atualizar o tempo e calcular o deslocamento em y
            current_time = rospy.Time.now()
            dt = 1.0 / 10.0  # Intervalo de tempo entre cada ciclo (em segundos)

            # Atualizar a posição (y) com base na velocidade linear
            self.x += self.linear_velocity * dt
            
            # Preencher o cabeçalho
            self.odom_msg.header = Header()
            self.odom_msg.header.stamp = current_time
            self.odom_msg.header.frame_id = "odom"  # Nome do frame de referência

            # Posição (pose) do robô
            self.odom_msg.pose.pose.position.x = self.x
            self.odom_msg.pose.pose.position.y = self.y
            self.odom_msg.pose.pose.position.z = 0.0

            quaternion = quaternion_from_euler(0, 0, self.theta)
            self.odom_msg.pose.pose.orientation.x = quaternion[0]
            self.odom_msg.pose.pose.orientation.y = quaternion[1]
            self.odom_msg.pose.pose.orientation.z = quaternion[2]
            self.odom_msg.pose.pose.orientation.w = quaternion[3]

            # Velocidade (linear e angular)
            self.odom_msg.twist.twist.linear.x = self.linear_velocity
            self.odom_msg.twist.twist.linear.y = 0.0
            self.odom_msg.twist.twist.linear.z = 0.0
            self.odom_msg.twist.twist.angular.x = 0.0
            self.odom_msg.twist.twist.angular.y = 0.0
            self.odom_msg.twist.twist.angular.z = self.angular_velocity

            # Publicar a mensagem Odometry
            self.odom_pub.publish(self.odom_msg)

            self.rate.sleep()

if __name__ == "__main__":
    rospy.init_node("Odom_esp_node")
    rospy.loginfo("Odom node initialized")

    oc = OdometryClass()

    try:
        rospy.spin()
    except rospy.ROSInterruptException:
        pass