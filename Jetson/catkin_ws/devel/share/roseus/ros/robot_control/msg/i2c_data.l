;; Auto-generated. Do not edit!


(when (boundp 'robot_control::i2c_data)
  (if (not (find-package "ROBOT_CONTROL"))
    (make-package "ROBOT_CONTROL"))
  (shadow 'i2c_data (find-package "ROBOT_CONTROL")))
(unless (find-package "ROBOT_CONTROL::I2C_DATA")
  (make-package "ROBOT_CONTROL::I2C_DATA"))

(in-package "ROS")
;;//! \htmlinclude i2c_data.msg.html


(defclass robot_control::i2c_data
  :super ros::object
  :slots (_x _y _z _timestamp ))

(defmethod robot_control::i2c_data
  (:init
   (&key
    ((:x __x) 0.0)
    ((:y __y) 0.0)
    ((:z __z) 0.0)
    ((:timestamp __timestamp) 0.0)
    )
   (send-super :init)
   (setq _x (float __x))
   (setq _y (float __y))
   (setq _z (float __z))
   (setq _timestamp (float __timestamp))
   self)
  (:x
   (&optional __x)
   (if __x (setq _x __x)) _x)
  (:y
   (&optional __y)
   (if __y (setq _y __y)) _y)
  (:z
   (&optional __z)
   (if __z (setq _z __z)) _z)
  (:timestamp
   (&optional __timestamp)
   (if __timestamp (setq _timestamp __timestamp)) _timestamp)
  (:serialization-length
   ()
   (+
    ;; float32 _x
    4
    ;; float32 _y
    4
    ;; float32 _z
    4
    ;; float32 _timestamp
    4
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; float32 _x
       (sys::poke _x (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _y
       (sys::poke _y (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _z
       (sys::poke _z (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;; float32 _timestamp
       (sys::poke _timestamp (send s :buffer) (send s :count) :float) (incf (stream-count s) 4)
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; float32 _x
     (setq _x (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _y
     (setq _y (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _z
     (setq _z (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;; float32 _timestamp
     (setq _timestamp (sys::peek buf ptr- :float)) (incf ptr- 4)
   ;;
   self)
  )

(setf (get robot_control::i2c_data :md5sum-) "b89928ff0a35372da5cb4a4faeb57c9a")
(setf (get robot_control::i2c_data :datatype-) "robot_control/i2c_data")
(setf (get robot_control::i2c_data :definition-)
      "# i2c_data.msg
float32 x
float32 y
float32 z 
float32 timestamp
")



(provide :robot_control/i2c_data "b89928ff0a35372da5cb4a4faeb57c9a")


