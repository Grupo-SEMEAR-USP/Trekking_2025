import yaml
import math
import rospy
import actionlib
#import Jetson.GPIO as GPIO
from move_base_msgs.msg import MoveBaseAction, MoveBaseGoal
from geometry_msgs.msg import PoseArray, Pose, Twist
from actionlib_msgs.msg import GoalStatus
from tf.transformations import quaternion_from_euler

# GPIO
LED_PIN = 11


# def setup_gpio():
#     GPIO.setmode(GPIO.BOARD)
#     GPIO.setup(LED_PIN, GPIO.OUT)
#     GPIO.output(LED_PIN, GPIO.LOW)

# def cleanup_gpio():
#     GPIO.output(LED_PIN, GPIO.LOW)
#     GPIO.cleanup()

def publish_via_points(pub, points):
    via_points_msg = PoseArray()
    via_points_msg.header.stamp = rospy.Time.now()
    via_points_msg.header.frame_id = "map"
    via_points_msg.poses = [p['pose'] for p in points if not p['rampa']]
    pub.publish(via_points_msg)

def load_goals_from_yaml(path):
    with open(path, 'r') as file:
        data = yaml.safe_load(file)

    goals = []
    for entry in data['goals']:
        pose = Pose()
        pose.position.x = entry['x']
        pose.position.y = entry['y']

        theta = entry['theta']
        q = quaternion_from_euler(0, 0, theta)
        pose.orientation.x = q[0]
        pose.orientation.y = q[1]
        pose.orientation.z = q[2]
        pose.orientation.w = q[3]

        goals.append({'pose': pose, 'rampa': entry.get('rampa', False)})

    return goals

def main():
    rospy.init_node('set_trajectory')

    #setup_gpio()

    via_points_pub = rospy.Publisher('/move_base/TebLocalPlannerROS/via_points', PoseArray, queue_size=1)
    cmd_vel_pub = rospy.Publisher('/cmd_vel', Twist, queue_size=1)

    client = actionlib.SimpleActionClient('move_base', MoveBaseAction)
    rospy.loginfo("Esperando pelo servidor move_base...")
    client.wait_for_server()
    rospy.loginfo("Conectado ao move_base!")

    # Lista de goals: cada um é um dicionário com pose + tipo
    # path_to_yaml = rospy.get_param('~goal_file', 'goals.yaml')
    # TODO: generalizar o caminho para o YAML depois
    path_to_yaml = "/home/fernando/Documentos/Trekking_2025/Jetson/catkin_ws/src/navigation/config/path.yaml"
    goals = load_goals_from_yaml(path_to_yaml)

    publish_via_points(via_points_pub, goals)

    for idx, item in enumerate(goals):
        pose = item['pose']
        is_rampa = item['rampa']

        rospy.loginfo(f"Processando goal {idx + 1} ({'RAMPA' if is_rampa else 'NORMAL'})")

        if is_rampa:
            # Acende LED e segue reto por tempo fixo
            # GPIO.output(LED_PIN, GPIO.HIGH)

            twist = Twist()
            twist.linear.x = 0.2  # Avanço constante
            cmd_vel_pub.publish(twist)

            rospy.sleep(3.0)

            twist.linear.x = 0.0
            cmd_vel_pub.publish(twist)

            # GPIO.output(LED_PIN, GPIO.LOW)
            rospy.loginfo("Rampa atravessada!")
        else:
            # Goal normal com move_base
            goal_msg = MoveBaseGoal()
            goal_msg.target_pose.header.frame_id = "map"
            goal_msg.target_pose.header.stamp = rospy.Time.now()
            goal_msg.target_pose.pose = pose

            rospy.loginfo(f"Enviando goal {idx + 1}")
            client.send_goal(goal_msg)
            finished = client.wait_for_result(rospy.Duration(60))

            state = client.get_state()

            if finished and state == GoalStatus.SUCCEEDED:
                rospy.loginfo(f"Goal {idx + 1} alcançado!")
                # GPIO.output(LED_PIN, GPIO.HIGH)
                rospy.sleep(2.0)
                # GPIO.output(LED_PIN, GPIO.LOW)
            else:
                rospy.logwarn(f"Falha ao alcançar goal {idx + 1} (state={state})")

    # cleanup_gpio()
    rospy.loginfo("Trajetória completa!")

if __name__ == '__main__':
    try:
        main()
    except rospy.ROSInterruptException:
        pass
        # cleanup_gpio()

