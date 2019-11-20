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

#ifndef TwoWheelsRobot_h
#define TwoWheelsRobot_h

#include <AFMotor.h>

#define MAX_MOTOR_SPEED         255   // max motor speed
#define WHEEL_STOP_ANGLE_DIFF   10    // angle difference to keep wheel stopped

class RobotMotors
{
  // private data and function member
  private:
    AF_DCMotor* leftMotor;
    AF_DCMotor* rightMotor;

    uint8_t maxSpeed;
    uint8_t minSpeed;
    uint8_t maxSpinSpeed;
    uint8_t startSpinAngle;

    bool is_left, is_backward, was_backward, is_innerWheelReverse;
    uint16_t innerWheel, outerWheel;

    // calculates inner/outer wheels speed
    void calcWheelsSpeed(uint8_t angle, float speedRate);

    // sets inner wheel speed and direction
    void setInnerWheel(AF_DCMotor* motor);

    // sets outer wheel speed and direction
    void setOuterWheel(AF_DCMotor* motor);

  // public member function
  public:
    // constructor
    RobotMotors(uint8_t leftMotorNum,   // left motor driver number
                uint8_t rightMotorNum,  // right motor driver number
                uint8_t maxSpeed = MAX_MOTOR_SPEED,         // max running speed: from 0 to 255
                uint8_t minSpeed = (MAX_MOTOR_SPEED / 2));  // min running speed: from 0 to 255 ( < max running speed )

    // sets motors speed based on direction and speed rate
    // direction: forward:  right: from 0 to 90
    //                      left:  from 90 to 180
    //            backward: right: from 0 to -90
    //                      left:  from -90 to -180
    // speed rate: from 0.0 to 1.0
    void setWheelsSpeed(int direction, float speedRate);
};

#endif
