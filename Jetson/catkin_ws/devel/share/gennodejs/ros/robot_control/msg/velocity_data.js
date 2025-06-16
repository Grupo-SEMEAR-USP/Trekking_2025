// Auto-generated. Do not edit!

// (in-package robot_control.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------

class velocity_data {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.angular_speed_left = null;
      this.angular_speed_right = null;
      this.servo_angle = null;
    }
    else {
      if (initObj.hasOwnProperty('angular_speed_left')) {
        this.angular_speed_left = initObj.angular_speed_left
      }
      else {
        this.angular_speed_left = 0.0;
      }
      if (initObj.hasOwnProperty('angular_speed_right')) {
        this.angular_speed_right = initObj.angular_speed_right
      }
      else {
        this.angular_speed_right = 0.0;
      }
      if (initObj.hasOwnProperty('servo_angle')) {
        this.servo_angle = initObj.servo_angle
      }
      else {
        this.servo_angle = 0.0;
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type velocity_data
    // Serialize message field [angular_speed_left]
    bufferOffset = _serializer.float32(obj.angular_speed_left, buffer, bufferOffset);
    // Serialize message field [angular_speed_right]
    bufferOffset = _serializer.float32(obj.angular_speed_right, buffer, bufferOffset);
    // Serialize message field [servo_angle]
    bufferOffset = _serializer.float32(obj.servo_angle, buffer, bufferOffset);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type velocity_data
    let len;
    let data = new velocity_data(null);
    // Deserialize message field [angular_speed_left]
    data.angular_speed_left = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [angular_speed_right]
    data.angular_speed_right = _deserializer.float32(buffer, bufferOffset);
    // Deserialize message field [servo_angle]
    data.servo_angle = _deserializer.float32(buffer, bufferOffset);
    return data;
  }

  static getMessageSize(object) {
    return 12;
  }

  static datatype() {
    // Returns string type for a message object
    return 'robot_control/velocity_data';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'e294170f2d297bc948c772795931b47a';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    # velocity_data.msg
    float32 angular_speed_left
    float32 angular_speed_right
    float32 servo_angle
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new velocity_data(null);
    if (msg.angular_speed_left !== undefined) {
      resolved.angular_speed_left = msg.angular_speed_left;
    }
    else {
      resolved.angular_speed_left = 0.0
    }

    if (msg.angular_speed_right !== undefined) {
      resolved.angular_speed_right = msg.angular_speed_right;
    }
    else {
      resolved.angular_speed_right = 0.0
    }

    if (msg.servo_angle !== undefined) {
      resolved.servo_angle = msg.servo_angle;
    }
    else {
      resolved.servo_angle = 0.0
    }

    return resolved;
    }
};

module.exports = velocity_data;
