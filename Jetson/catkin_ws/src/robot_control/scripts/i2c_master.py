#!/usr/bin/env python3
import rospy
import smbus
import struct
from std_msgs.msg import Int32
from sensor_msgs.msg import JointState
from robot_control.msg import i2c_data, velocity_data
from geometry_msgs.msg import Quaternion
from time import sleep
import threading
import tf

# Constantes
ESP32_ADDRESS = 0x58  # Endereço do dispositivo ESP32 esquerdo no barramento I2C
I2C_BUS = 1  # Número do barramento I2C na Jetson
REG_ADDRESS = 0  # Endereço de registro (offset) a ser usado pelo PID
SERVO_INITIAL_ANGLE = 90.0

i2c_data_global = [0, 0, 0, 0] # # Variável global para armazenar os dados enviados pela esp (x, y, z, time_stamp)

# Classe para comunicação I2C
class I2CCommunication:
    # "Construtor" da classe, definindo seus atributos principais
    def __init__(self, device_address):

        self.ackr_commands = [0, 0, SERVO_INITIAL_ANGLE]  # Inicializa as velocidades das duas rodas como zero

        self.i2c = smbus.SMBus(I2C_BUS)  # Define o barramento que será usado na comunicação
        self.device_address = device_address  # Define o endereço da ESP32 ao qual queremos nos comunicar

        # Cria um objeto de publicação para enviar dados para o tópico ROS
        self.pub_encoder = rospy.Publisher('/i2c_data', i2c_data, queue_size=10)
        self.sub_joints = rospy.Subscriber('/velocity_command', velocity_data, self.nav_callback)

        self.data_msg = i2c_data()

        self.thread = threading.Thread(target=self.update)  # Cria uma nova thread para a função update
        self.thread.start()  # Inicia a execução da thread

    def read_data(self):
        try:
            data = self.i2c.read_i2c_block_data(self.device_address, REG_ADDRESS, 16)  # Faz a leitura da ESP32
            x = struct.unpack('<i', bytes(data[:4]))
            y = struct.unpack('<i', bytes(data[4:8]))
            z = struct.unpack('<i', bytes(data[8:12]))
            timestamp_ms = struct.unpack('<I', bytes(data[12:16]))
            
            rospy.loginfo(f'Dados recebidos: {x[0]}, {y[0]}, {z[0]}, {timestamp_ms[0]}')
            i2c_data_global[0] = x[0]
            i2c_data_global[1] = y[0]
            i2c_data_global[2] = z[0]
            i2c_data_global[3] = timestamp_ms[0]
    
            self.data_msg.x = i2c_data_global[0]
            self.data_msg.y = i2c_data_global[1]
            self.data_msg.z = i2c_data_global[2]
            self.data_msg.timestamp = rospy.Time.from_sec(i2c_data_global[3] / 1000.0)

            self.pub_encoder.publish(self.data_msg)

        except Exception as e:
            rospy.logerr(f"Erro na leitura: {str(e)}")
            return None

    
    def write_data(self):
        try:
            data = struct.pack('<fff', self.ackr_commands[0], self.ackr_commands[1], self.ackr_commands[2])  # Empacota os valores das velocidades das rodas
            self.i2c.write_i2c_block_data(self.device_address, REG_ADDRESS, list(data))  # Escreve valores para a ESP32

            rospy.loginfo(f'Valores enviados: {self.ackr_commands}')

        except Exception as e:
            rospy.logerr(f"Erro na escrita: {str(e)}")
            return None

    def nav_callback(self, msg):
        print(msg)

        self.ackr_commands[0] = msg.angular_speed_left
        self.ackr_commands[1] = msg.angular_speed_right
        self.ackr_commands[2] = msg.servo_angle


    def update(self):
        rate = rospy.Rate(50)  # 50Hz

        while not rospy.is_shutdown():

            self.read_data()
            self.write_data()
            rate.sleep()

if __name__ == "__main__":
    try:
        rospy.init_node('i2c_master', anonymous=True)
        i2c_communication = I2CCommunication(ESP32_ADDRESS)
        rospy.spin()
    except rospy.ROSInterruptException:
        pass