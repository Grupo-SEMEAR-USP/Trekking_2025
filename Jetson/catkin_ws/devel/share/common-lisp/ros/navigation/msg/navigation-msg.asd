
(cl:in-package :asdf)

(defsystem "navigation-msg"
  :depends-on (:roslisp-msg-protocol :roslisp-utils )
  :components ((:file "_package")
    (:file "encoders" :depends-on ("_package_encoders"))
    (:file "_package_encoders" :depends-on ("_package"))
  ))