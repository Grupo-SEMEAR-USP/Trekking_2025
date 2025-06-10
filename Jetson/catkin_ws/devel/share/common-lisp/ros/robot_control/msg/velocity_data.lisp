; Auto-generated. Do not edit!


(cl:in-package robot_control-msg)


;//! \htmlinclude velocity_data.msg.html

(cl:defclass <velocity_data> (roslisp-msg-protocol:ros-message)
  ((angular_speed_left
    :reader angular_speed_left
    :initarg :angular_speed_left
    :type cl:float
    :initform 0.0)
   (angular_speed_right
    :reader angular_speed_right
    :initarg :angular_speed_right
    :type cl:float
    :initform 0.0)
   (servo_angle
    :reader servo_angle
    :initarg :servo_angle
    :type cl:float
    :initform 0.0))
)

(cl:defclass velocity_data (<velocity_data>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <velocity_data>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'velocity_data)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name robot_control-msg:<velocity_data> is deprecated: use robot_control-msg:velocity_data instead.")))

(cl:ensure-generic-function 'angular_speed_left-val :lambda-list '(m))
(cl:defmethod angular_speed_left-val ((m <velocity_data>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_control-msg:angular_speed_left-val is deprecated.  Use robot_control-msg:angular_speed_left instead.")
  (angular_speed_left m))

(cl:ensure-generic-function 'angular_speed_right-val :lambda-list '(m))
(cl:defmethod angular_speed_right-val ((m <velocity_data>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_control-msg:angular_speed_right-val is deprecated.  Use robot_control-msg:angular_speed_right instead.")
  (angular_speed_right m))

(cl:ensure-generic-function 'servo_angle-val :lambda-list '(m))
(cl:defmethod servo_angle-val ((m <velocity_data>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader robot_control-msg:servo_angle-val is deprecated.  Use robot_control-msg:servo_angle instead.")
  (servo_angle m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <velocity_data>) ostream)
  "Serializes a message object of type '<velocity_data>"
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'angular_speed_left))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'angular_speed_right))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
  (cl:let ((bits (roslisp-utils:encode-double-float-bits (cl:slot-value msg 'servo_angle))))
    (cl:write-byte (cl:ldb (cl:byte 8 0) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 8) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 16) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 24) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 32) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 40) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 48) bits) ostream)
    (cl:write-byte (cl:ldb (cl:byte 8 56) bits) ostream))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <velocity_data>) istream)
  "Deserializes a message object of type '<velocity_data>"
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'angular_speed_left) (roslisp-utils:decode-double-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'angular_speed_right) (roslisp-utils:decode-double-float-bits bits)))
    (cl:let ((bits 0))
      (cl:setf (cl:ldb (cl:byte 8 0) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 8) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 16) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 24) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 32) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 40) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 48) bits) (cl:read-byte istream))
      (cl:setf (cl:ldb (cl:byte 8 56) bits) (cl:read-byte istream))
    (cl:setf (cl:slot-value msg 'servo_angle) (roslisp-utils:decode-double-float-bits bits)))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<velocity_data>)))
  "Returns string type for a message object of type '<velocity_data>"
  "robot_control/velocity_data")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'velocity_data)))
  "Returns string type for a message object of type 'velocity_data"
  "robot_control/velocity_data")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<velocity_data>)))
  "Returns md5sum for a message object of type '<velocity_data>"
  "826270096bebd36aa4c516facc8a6a69")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'velocity_data)))
  "Returns md5sum for a message object of type 'velocity_data"
  "826270096bebd36aa4c516facc8a6a69")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<velocity_data>)))
  "Returns full string definition for message of type '<velocity_data>"
  (cl:format cl:nil "# velocity_data.msg~%float64 angular_speed_left~%float64 angular_speed_right~%float64 servo_angle~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'velocity_data)))
  "Returns full string definition for message of type 'velocity_data"
  (cl:format cl:nil "# velocity_data.msg~%float64 angular_speed_left~%float64 angular_speed_right~%float64 servo_angle~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <velocity_data>))
  (cl:+ 0
     8
     8
     8
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <velocity_data>))
  "Converts a ROS message object to a list"
  (cl:list 'velocity_data
    (cl:cons ':angular_speed_left (angular_speed_left msg))
    (cl:cons ':angular_speed_right (angular_speed_right msg))
    (cl:cons ':servo_angle (servo_angle msg))
))
