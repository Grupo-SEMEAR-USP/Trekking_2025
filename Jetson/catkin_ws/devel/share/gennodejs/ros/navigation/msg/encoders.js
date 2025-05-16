// Auto-generated. Do not edit!

// (in-package navigation.msg)


"use strict";

const _serializer = _ros_msg_utils.Serialize;
const _arraySerializer = _serializer.Array;
const _deserializer = _ros_msg_utils.Deserialize;
const _arrayDeserializer = _deserializer.Array;
const _finder = _ros_msg_utils.Find;
const _getByteLength = _ros_msg_utils.getByteLength;

//-----------------------------------------------------------

class encoders {
  constructor(initObj={}) {
    if (initObj === null) {
      // initObj === null is a special case for deserialization where we don't initialize fields
      this.encoderTicks = null;
    }
    else {
      if (initObj.hasOwnProperty('encoderTicks')) {
        this.encoderTicks = initObj.encoderTicks
      }
      else {
        this.encoderTicks = new Array(2).fill(0);
      }
    }
  }

  static serialize(obj, buffer, bufferOffset) {
    // Serializes a message object of type encoders
    // Check that the constant length array field [encoderTicks] has the right length
    if (obj.encoderTicks.length !== 2) {
      throw new Error('Unable to serialize array field encoderTicks - length must be 2')
    }
    // Serialize message field [encoderTicks]
    bufferOffset = _arraySerializer.int8(obj.encoderTicks, buffer, bufferOffset, 2);
    return bufferOffset;
  }

  static deserialize(buffer, bufferOffset=[0]) {
    //deserializes a message object of type encoders
    let len;
    let data = new encoders(null);
    // Deserialize message field [encoderTicks]
    data.encoderTicks = _arrayDeserializer.int8(buffer, bufferOffset, 2)
    return data;
  }

  static getMessageSize(object) {
    return 2;
  }

  static datatype() {
    // Returns string type for a message object
    return 'navigation/encoders';
  }

  static md5sum() {
    //Returns md5sum for a message object
    return 'ab0acfae811ce802d245f0e7acc9b834';
  }

  static messageDefinition() {
    // Returns full string definition for message
    return `
    #Mensagem customizada que guarda a quantidade de ticks do encoder de cada roda
    int8[2] encoderTicks #[direita, esquerda]
    `;
  }

  static Resolve(msg) {
    // deep-construct a valid message object instance of whatever was passed in
    if (typeof msg !== 'object' || msg === null) {
      msg = {};
    }
    const resolved = new encoders(null);
    if (msg.encoderTicks !== undefined) {
      resolved.encoderTicks = msg.encoderTicks;
    }
    else {
      resolved.encoderTicks = new Array(2).fill(0)
    }

    return resolved;
    }
};

module.exports = encoders;
