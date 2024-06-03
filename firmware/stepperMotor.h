/******************************************************//**
 * @file    stepperMotor.h 
 * @version V1.0
 * @date    June 2, 2024
 * @brief   Motor controller driver abstraction layer library
 * @author  Matthew R. Miller
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 **********************************************************/

#ifndef __STEPPER_MOTOR_H_INCLUDED
#define __STEPPER_MOTOR_H_INCLUDED

#include <inttypes.h>
#include "l6474.h"

/// Step mode options for stepper motor
typedef enum {
  STEP_FULL = 1,      //full step
  STEP_HALF = 2,      //1/2 microstep
  STEP_QUARTER = 4,   //1/4 microstep
  STEP_EIGHTH = 8,    //1/8 microstep
  STEP_SIXTEENTH = 16 //1/16 microstep
} stepMode_t;

/// Direction options for stepper motor
typedef enum {
  CCW = FORWARD,      //move counter clockwise
  CW = BACKWARD       //move clockwise
} direction_t;

class StepperMotor
{
  public:
    StepperMotor(float stepAngleDeg, stepMode_t stepMode);  //Start the StepperMotor library
    Begin();

    float GetAccelerationRad();                           //Return the acceleration in radians/s^2
    float GetAccelerationDeg();                           //Return the acceleration in degrees/s^2

    float GetCurrentSpeedRad();                           //Return the current speed in radians/s
    float GetCurrentSpeedDeg();                           //Return the current speed in degrees/s

    float GetMaxSpeedRad();                               //Return the max speed in radians/s
    float GetMaxSpeedDeg();                               //Return the max speed in degrees/s

    float GetMinSpeedRad();                               //Return the min speed in radians/s
    float GetMinSpeedDeg();                               //Return the min speed in degrees/s

    float GetDecelerationRad();                           //Return the deceleration in radians/s^2
    float GetDecelerationDeg();                           //Return the deceleration in degrees/s^2

    bool SetAccelerationRad(float newAcceleration);       //Set the acceleration in radians/s^2
    bool SetAccelerationDeg(float newAcceleration);       //Set the acceleration in degrees/s^2

    bool SetMaxSpeedRad(float newMaxSpeed);               //Set the max speed in radians/s
    bool SetMaxSpeedDeg(float newMaxSpeed);               //Set the max speed in degrees/s

    bool SetMinSpeedRad(float newMinSpeed);               //Set the min speed in radians/s
    bool SetMinSpeedDeg(float newMinSpeed);               //Set the min speed in degrees/s

    bool SetDecelerationRad(float newDeceleration);       //Set the deceleration in radians/s^2
    bool SetDecelerationDeg(float newDeceleration);       //Set the deceleration in degrees/s^2

    void WaitWhileActive();                               //Wait for the shield state becomes Inactive
    void HardStop();                                      //Stop the motor
    bool SoftStop();                                      //Progressively stops the motor
    void Run(direction_t direction);                      //Run the motor continuously 
    
    void SetHome();                                       //Set current position to be the home position
    void GoHome();                                        //Move to the home position

    void GoToRad(float targetPosition);                   //Go to the specified position in radians (CCW is + / CW is -)
    void GoToDeg(float targetPosition);                   //Go to the specified position in degrees (CCW is + / CW is -)

    void MoveRad(float targetDistance);                   //Move the motor the specified number of radians (CCW is + / CW is -)
    void MoveDeg(float targetDistance);                   //Move the motor the specified number of degrees (CCW is + / CW is -)

  private:
    L6474 L6474shield;
    stepMode_t stepMode;
    float stepAngleRadian;
    float stepAngleDegree;


};


#endif /* #ifndef __STEPPER_MOTOR_H_INCLUDED */