
#include <TwoWheelsRobot.h>

#define LEFT_MOTOR_NUM    2   // left motor driver number
#define RIGHT_MOTOR_NUM   3   // right motor driver number

#define REC_DELIM '\|'    // record delimiter
#define DATA_DELIM '~'    // data delimiter

// initializes robot motors
RobotMotors robotMotors(LEFT_MOTOR_NUM, RIGHT_MOTOR_NUM, 180);

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
}

void loop() {
  uint8_t buf[1024];
  size_t len;
  uint8_t *pos;
  float xPercent, yPercent, speedRate;
  int direction;

  // reads bytes from serial port into buffer
  len = Serial2.readBytesUntil(REC_DELIM, buf, sizeof(buf));
  if (len > 0) {
    pos = buf;
    // reads joystick xy coordinates
    if ((*pos == (uint8_t)DATA_DELIM) &&
        (len == 2 * sizeof(float) + 1)) {
      // gets joystick x coordinate
      pos++;
      xPercent = *(float*)pos;

      // gets joystick y coordinate
      pos += sizeof(float);
      yPercent = *(float*)pos;

      // calculates direction from joystick xy coordinates
      direction = (int)round(atan2(yPercent, xPercent) * 180 / PI);

      // calculates speed rate from joystick xy coordinates
      speedRate = hypot(xPercent, yPercent);
      if (speedRate > 1.0) {
        speedRate = 1.0;
      }

      // sets robot motors speed based on direction and speed rate
      robotMotors.setWheelsSpeed(direction, speedRate);

    } else {  // forwards anything else to output
      Serial.write(buf, len);
    }
  }
}

