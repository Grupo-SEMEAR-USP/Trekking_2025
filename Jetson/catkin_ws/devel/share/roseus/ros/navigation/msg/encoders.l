;; Auto-generated. Do not edit!


(when (boundp 'navigation::encoders)
  (if (not (find-package "NAVIGATION"))
    (make-package "NAVIGATION"))
  (shadow 'encoders (find-package "NAVIGATION")))
(unless (find-package "NAVIGATION::ENCODERS")
  (make-package "NAVIGATION::ENCODERS"))

(in-package "ROS")
;;//! \htmlinclude encoders.msg.html


(defclass navigation::encoders
  :super ros::object
  :slots (_encoderTicks ))

(defmethod navigation::encoders
  (:init
   (&key
    ((:encoderTicks __encoderTicks) (make-array 2 :initial-element 0 :element-type :integer))
    )
   (send-super :init)
   (setq _encoderTicks __encoderTicks)
   self)
  (:encoderTicks
   (&optional __encoderTicks)
   (if __encoderTicks (setq _encoderTicks __encoderTicks)) _encoderTicks)
  (:serialization-length
   ()
   (+
    ;; int8[2] _encoderTicks
    (* 1    2)
    ))
  (:serialize
   (&optional strm)
   (let ((s (if strm strm
              (make-string-output-stream (send self :serialization-length)))))
     ;; int8[2] _encoderTicks
     (dotimes (i 2)
       (write-byte (elt _encoderTicks i) s)
       )
     ;;
     (if (null strm) (get-output-stream-string s))))
  (:deserialize
   (buf &optional (ptr- 0))
   ;; int8[2] _encoderTicks
   (dotimes (i (length _encoderTicks))
     (setf (elt _encoderTicks i) (sys::peek buf ptr- :char)) (incf ptr- 1)
     (if (> (elt _encoderTicks i) 127) (setf (elt _encoderTicks i) (- (elt _encoderTicks i) 256)))
     )
   ;;
   self)
  )

(setf (get navigation::encoders :md5sum-) "ab0acfae811ce802d245f0e7acc9b834")
(setf (get navigation::encoders :datatype-) "navigation/encoders")
(setf (get navigation::encoders :definition-)
      "#Mensagem customizada que guarda a quantidade de ticks do encoder de cada roda
int8[2] encoderTicks #[direita, esquerda]
")



(provide :navigation/encoders "ab0acfae811ce802d245f0e7acc9b834")


