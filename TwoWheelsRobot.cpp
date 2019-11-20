/*
 *    TwoWheelsRobot.cpp - A library for controling Two Wheels Robot with Arduino and motor shield
 *    Copyright (c) 2019 Ronaldo Barbieri
 *
 *    Permission is hereby granted, free of charge, to any person
 *    obtaining a copy of this software and associated documentation
 *    files (the "Software"), to deal in the Software without
 *    restriction, including without limitation the rights to use,
 *    copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following
 *    conditions:
 *
 *    This permission notice shall be included in all copies or
 *    substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *    OTHER DEALINGS IN THE SOFTWARE.
 */

#include <TwoWheelsRobot.h>
#include <Arduino.h>

// constructor
RobotMotors::RobotMotors(uint8_t leftMotorNum,
                         uint8_t rightMotorNum,
                         uint8_t maxSpeed,
                         uint8_t minSpeed) {

  float angleRange, minAngle;

  // sets min running speed
  this->minSpeed = minSpeed;

  // sets max running speed
  if (maxSpeed < minSpeed) {
    this->maxSpeed = minSpeed;
  } else {
    this->maxSpeed = maxSpeed;
  }

  leftMotor = new AF_DCMotor(leftMotorNum);
  rightMotor = new AF_DCMotor(rightMotorNum);

  // turns on motors
  leftMotor->setSpeed(maxSpeed);
  leftMotor->run(RELEASE);
  rightMotor->setSpeed(maxSpeed);
  rightMotor->run(RELEASE);

  // calculates max running speed when spinning
  maxSpinSpeed = (uint8_t)(minSpeed + ((maxSpeed - minSpeed) / 2));

  // calculates initial angle to start spinnig robot on its axis
  angleRange = 90 * (float)(MAX_MOTOR_SPEED - minSpeed) / MAX_MOTOR_SPEED;
  minAngle = 45 - (float)(angleRange / 2);
  startSpinAngle = (uint8_t)(minAngle + angleRange * (1 - ((float)(maxSpeed - minSpeed) / (MAX_MOTOR_SPEED - minSpeed))));
}

// calculates inner/outer wheels speed
void RobotMotors::calcWheelsSpeed(uint8_t angle, float speedRate) {
  // calculates outer wheel speed
  outerWheel = (uint16_t)round(speedRate * (maxSpinSpeed + (angle * (maxSpeed - maxSpinSpeed) / (90 - startSpinAngle))));

  // calculates inner wheel speed
  if (angle < (startSpinAngle - WHEEL_STOP_ANGLE_DIFF)) {  // calculates inner wheel reverse speed to spin robot on its axis
    is_innerWheelReverse = true;
    innerWheel = (uint16_t)round(speedRate * (minSpeed + (((startSpinAngle - WHEEL_STOP_ANGLE_DIFF) - angle) * (maxSpinSpeed - minSpeed) / (startSpinAngle - WHEEL_STOP_ANGLE_DIFF))));
  } else if (angle < startSpinAngle) {   // stops inner wheel
    innerWheel = 0;
  } else {    // calculates inner wheel normal speed
    innerWheel = (uint16_t)round(speedRate * (minSpeed + ((angle - startSpinAngle) * (maxSpeed - minSpeed) / (90 - startSpinAngle))));
  }
}

// sets inner wheel speed and direction
void RobotMotors::setInnerWheel(AF_DCMotor* motor) {
  if (innerWheel >= minSpeed) {
    motor->setSpeed(innerWheel);   // sets inner wheel speed
    if (is_backward) {
      if (is_innerWheelReverse) {
        motor->run(FORWARD);    // reverses inner wheel forward to spin robot on its axis
      } else {
        motor->run(BACKWARD);   // rotates inner wheel backward
      }
    } else {
      if (is_innerWheelReverse) {
        motor->run(BACKWARD);   // reverses inner wheel backward to spin robot on its axis
      } else {
        motor->run(FORWARD);    // rotates inner wheel forward
      }
    }
  } else {
    motor->run(RELEASE);  // stops inner wheel if below minimum effective speed
  }
}

// sets outer wheel speed and direction
void RobotMotors::setOuterWheel(AF_DCMotor* motor) {
  if (outerWheel >= minSpeed) {
    motor->setSpeed(outerWheel);  // sets outer wheel speed
    if (is_backward) {
      motor->run(BACKWARD);   // rotates outer wheel backward
    } else {
      motor->run(FORWARD);    // rotates outer wheel forward
    }
  } else {
    motor->run(RELEASE);  // stops outer wheel if below minimum effective speed
  }
}

// sets motors speed based on direction and speed rate
void RobotMotors::setWheelsSpeed(int direction, float speedRate) {
  uint8_t angle;

  is_backward = false;
  is_left = false;
  is_innerWheelReverse = false;

  // checks if going backward
  if (direction < 0) {
    is_backward = true;
    direction = abs(direction);
  }

  // checks if turning left
  if (direction > 90) {
    is_left = true;
    angle = (uint8_t)(90 - direction % 90);
  } else {
    angle = (uint8_t)direction;
  }

  // checks if direction reversed
  if (is_backward == was_backward) {
    // calculates wheels speed
    calcWheelsSpeed(angle, speedRate);

    // sets inner/outer motors
    if (is_left) {  // turns left
      setInnerWheel(leftMotor);   // sets left motor as inner
      setOuterWheel(rightMotor);  // sets right motor as outer
    } else {  // turns right
      setInnerWheel(rightMotor);  // sets right motor as inner
      setOuterWheel(leftMotor);   // sets left motor as outer
    }
  } else {  // direction reversed
    was_backward = is_backward;
    // first stop wheels before reversing
    leftMotor->run(RELEASE);
    rightMotor->run(RELEASE);
    delay(500);
  }
}
