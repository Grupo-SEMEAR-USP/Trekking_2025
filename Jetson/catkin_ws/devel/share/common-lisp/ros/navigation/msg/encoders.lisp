; Auto-generated. Do not edit!


(cl:in-package navigation-msg)


;//! \htmlinclude encoders.msg.html

(cl:defclass <encoders> (roslisp-msg-protocol:ros-message)
  ((encoderTicks
    :reader encoderTicks
    :initarg :encoderTicks
    :type (cl:vector cl:fixnum)
   :initform (cl:make-array 2 :element-type 'cl:fixnum :initial-element 0)))
)

(cl:defclass encoders (<encoders>)
  ())

(cl:defmethod cl:initialize-instance :after ((m <encoders>) cl:&rest args)
  (cl:declare (cl:ignorable args))
  (cl:unless (cl:typep m 'encoders)
    (roslisp-msg-protocol:msg-deprecation-warning "using old message class name navigation-msg:<encoders> is deprecated: use navigation-msg:encoders instead.")))

(cl:ensure-generic-function 'encoderTicks-val :lambda-list '(m))
(cl:defmethod encoderTicks-val ((m <encoders>))
  (roslisp-msg-protocol:msg-deprecation-warning "Using old-style slot reader navigation-msg:encoderTicks-val is deprecated.  Use navigation-msg:encoderTicks instead.")
  (encoderTicks m))
(cl:defmethod roslisp-msg-protocol:serialize ((msg <encoders>) ostream)
  "Serializes a message object of type '<encoders>"
  (cl:map cl:nil #'(cl:lambda (ele) (cl:let* ((signed ele) (unsigned (cl:if (cl:< signed 0) (cl:+ signed 256) signed)))
    (cl:write-byte (cl:ldb (cl:byte 8 0) unsigned) ostream)
    ))
   (cl:slot-value msg 'encoderTicks))
)
(cl:defmethod roslisp-msg-protocol:deserialize ((msg <encoders>) istream)
  "Deserializes a message object of type '<encoders>"
  (cl:setf (cl:slot-value msg 'encoderTicks) (cl:make-array 2))
  (cl:let ((vals (cl:slot-value msg 'encoderTicks)))
    (cl:dotimes (i 2)
    (cl:let ((unsigned 0))
      (cl:setf (cl:ldb (cl:byte 8 0) unsigned) (cl:read-byte istream))
      (cl:setf (cl:aref vals i) (cl:if (cl:< unsigned 128) unsigned (cl:- unsigned 256))))))
  msg
)
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql '<encoders>)))
  "Returns string type for a message object of type '<encoders>"
  "navigation/encoders")
(cl:defmethod roslisp-msg-protocol:ros-datatype ((msg (cl:eql 'encoders)))
  "Returns string type for a message object of type 'encoders"
  "navigation/encoders")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql '<encoders>)))
  "Returns md5sum for a message object of type '<encoders>"
  "ab0acfae811ce802d245f0e7acc9b834")
(cl:defmethod roslisp-msg-protocol:md5sum ((type (cl:eql 'encoders)))
  "Returns md5sum for a message object of type 'encoders"
  "ab0acfae811ce802d245f0e7acc9b834")
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql '<encoders>)))
  "Returns full string definition for message of type '<encoders>"
  (cl:format cl:nil "#Mensagem customizada que guarda a quantidade de ticks do encoder de cada roda~%int8[2] encoderTicks #[direita, esquerda]~%~%"))
(cl:defmethod roslisp-msg-protocol:message-definition ((type (cl:eql 'encoders)))
  "Returns full string definition for message of type 'encoders"
  (cl:format cl:nil "#Mensagem customizada que guarda a quantidade de ticks do encoder de cada roda~%int8[2] encoderTicks #[direita, esquerda]~%~%"))
(cl:defmethod roslisp-msg-protocol:serialization-length ((msg <encoders>))
  (cl:+ 0
     0 (cl:reduce #'cl:+ (cl:slot-value msg 'encoderTicks) :key #'(cl:lambda (ele) (cl:declare (cl:ignorable ele)) (cl:+ 1)))
))
(cl:defmethod roslisp-msg-protocol:ros-message-to-list ((msg <encoders>))
  "Converts a ROS message object to a list"
  (cl:list 'encoders
    (cl:cons ':encoderTicks (encoderTicks msg))
))
