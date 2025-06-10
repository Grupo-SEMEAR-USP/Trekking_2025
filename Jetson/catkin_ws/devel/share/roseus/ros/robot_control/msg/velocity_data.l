;; Auto-generated. Do not edit!


(when (boundp 'robot_control::velocity_data)
  (if (not (find-package "ROBOT_CONTROL"))
    (make-package "ROBOT_CONTROL"))
  (shadow 'velocity_data (find-package "ROBOT_CONTROL")))
(unless (find-package "ROBOT_CONTROL::VELOCITY_DATA")
  (make-package "ROBOT_CONTROL::VELOCITY_DATA"))

(in-package "ROS")
;;//! \htmlinclude velocity_data.msg.html


(defclass robot_control::velocity_data
  :super ros::object
  :slots (_angular_speed_left _angular_speed_right _servo_angle ))

(defmethod robot_control::velocity_data
  (:init
   (&key
    ((:angular_speed_left __angular_speed_left) 0.0)
    ((:angular_speed_right __angular_speed_right) 0.0)
    ((:servo_angle __servo_angle) 0.0)
    )
   (send-super :init)
   (setq _angular_speed_left (float __angular_speed_left))
   (setq _angular_speed_right (float __angular_speed_right))
   (setq _servo_angle (float __servo_angle))
   self)
  (:angular_speed_left
   (&optional __angular_speed_left)
   (if __angular_speed_left (setq _angular_speed_left __angular_speed_left)) _angular_speed_left)
  (:angular_speed_right
   (&optional __angular_speed_right)
   (if __angular_speed_right (setq _angular_speed_right __angular_speed_right)) _angular_speed_right)
  (:servo_angle
   (&optional __servo_angle)
   (if __servo_angle (setq _servo_angle __servo_angle)) _servo_angle)
  (:serialization-length
   ()
   (+
    ;; float64 _angular_speed_left
    8
    ;; float64 _angular_speed_right
    8
    ;; float64 _servo_angle
    8
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float64 _angular_speed_left
       (sys::poke _angular_speed_left (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _angular_speed_right
       (sys::poke _angular_speed_right (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;; float64 _servo_angle
       (sys::poke _servo_angle (send s :buffer) (send s :count) :double) (incf (stream-count s) 8)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float64 _angular_speed_left
     (setq _angular_speed_left (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _angular_speed_right
     (setq _angular_speed_right (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;; float64 _servo_angle
     (setq _servo_angle (sys::peek buf ptr- :double)) (incf ptr- 8)
   ;;
   self)
  )

(setf (get robot_control::velocity_data :md5sum-) "826270096bebd36aa4c516facc8a6a69")
(setf (get robot_control::velocity_data :datatype-) "robot_control/velocity_data")
(setf (get robot_control::velocity_data :definition-)
      "# velocity_data.msg
float64 angular_speed_left
float64 angular_speed_right
float64 servo_angle
")



(provide :robot_control/velocity_data "826270096bebd36aa4c516facc8a6a69")


