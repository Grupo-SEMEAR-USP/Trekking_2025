
(cl:in-package :asdf)

(defsystem "robot_control-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "I2cData" :depends-on ("_package_I2cData"))
    (:file "_package_I2cData" :depends-on ("_package"))
    (:file "VelocityData" :depends-on ("_package_VelocityData"))
    (:file "_package_VelocityData" :depends-on ("_package"))
  ))