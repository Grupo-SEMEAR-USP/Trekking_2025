#!/usr/bin/env python3
import rospy
import Jetson.GPIO as GPIO
import time

def main():
    # Inicializa o nó ROS
    rospy.init_node('led_control_node', anonymous=True)

    # Configura o modo da placa
    GPIO.setmode(GPIO.BOARD)  # Usa a numeração física dos pinos

    LED_PIN = 37  # Pino físico 37 (GPIO16)

    # Configura o pino como saída
    GPIO.setup(LED_PIN, GPIO.OUT)

    rospy.loginfo("LED no pino 37 será aceso.")

    # Acende o LED
    GPIO.output(LED_PIN, GPIO.LOW)

    # Mantém o LED aceso até o ROS ser encerrado
    try:
        rospy.spin()
    except KeyboardInterrupt:
        pass
    finally:
        # Limpa os pinos ao encerrar
        GPIO.cleanup()
        rospy.loginfo("Encerrando nó e limpando configuração GPIO.")

if __name__ == '__main__':
    main()
