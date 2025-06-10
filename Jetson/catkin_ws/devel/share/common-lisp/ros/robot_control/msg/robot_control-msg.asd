
(cl:in-package :asdf)

(defsystem "robot_control-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "i2c_data" :depends-on ("_package_i2c_data"))
    (:file "_package_i2c_data" :depends-on ("_package"))
    (:file "velocity_data" :depends-on ("_package_velocity_data"))
    (:file "_package_velocity_data" :depends-on ("_package"))
  ))