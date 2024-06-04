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

#include "stepperMotor.h"
#include <Arduino.h>

/******************************************************//**
 * @brief  Constructor for the StepperMotor object. Sets the
 * step mode for the stepper motor and the number of degrees
 * per step based on the degrees per full step of the motor
 * and the step mode the motor is in.
 * @param  stepAngleDeg Degrees per full step of the physical stepper motor
 * @param  stepMode     Microstepping mode of the stepper motor controller
 * @retval None
 **********************************************************/
StepperMotor::StepperMotor(float stepAngleDeg, stepMode_t stepMode) : stepMode(stepMode),
stepAngleDegree(stepAngleDeg / (float)stepMode), stepAngleRadian( (stepAngleDeg / (float)stepMode) * PI / 180.0) {}

/******************************************************//**
 * @brief  Initializes the L6474 BSP library and any initial
 * states the stepper motor driver chip should be in.
 * @param  None
 * @retval None
 **********************************************************/
StepperMotor::Begin()
{
  /* Start the library to use one shield. The L6474 registers are set with the predefined
   * values from file l6474_target_config.h. This initialization step occupies the following
   * pins on the Arduino Uno defined in l6474.h: 7, 8, 9 and 2 (pin 2 we will reclaim)*/
  L6474shield.Begin(1);

  /* Detatch the interrupt from the L6474.Begin() init function. Only two interrupts are 
   * available on the Arduino Uno and we will need them both for the encoder. */
  detachInterrupt(2);

  /* As per section 6.17 in hte L6474 datasheet - mask the FLAG conditions to keep the FLAG
   * pin from being pulled to ground through an open drain transistor. This will keep the 
   * interrupt on pin 2 open for use with the encoder and not create a pulse with an error
   * condition. */
  L6474shield.CmdSetParam(0, L6474_ALARM_EN, 0x0);

  /* Select the step mode for the stepper motor */
  switch(stepMode)
  {
    case STEP_HALF:
      L6474shield.SelectStepMode(0, L6474_STEP_SEL_1_2);
      break;

    case STEP_QUARTER:
      L6474shield.SelectStepMode(0, L6474_STEP_SEL_1_4);
      break;

    case STEP_EIGHTH:
      L6474shield.SelectStepMode(0, L6474_STEP_SEL_1_8);
      break;

    case STEP_SIXTEENTH:
      L6474shield.SelectStepMode(0, L6474_STEP_SEL_1_16);
      break;

    case STEP_FULL:
    default:
      L6474shield.SelectStepMode(0, L6474_STEP_SEL_1);
      break;
  }

  /* Set torque output current amplitude to 150mA. This reduces resonance noise level of the
   * selected XY42STH34-0354A stepper motor and provides plenty of power to hold position and
   * move the motor for the selected application */
  L6474shield.CmdSetParam(0, L6474_TVAL, L6474shield.ConvertCurrentToTval(150.0)); //TODO was not using TVAL current to par tester -> give this a shot

  /* Keep power bridge active when stepper motor stops moving. This will have the stepper motor
   * hold its position when inactive. */
  L6474shield.SetHoldPositionOnStop(true);
}

/******************************************************//**
 * @brief Returns the acceleration of the stepper motor
 * @param None
 * @retval Acceleration in radians/s^2
 **********************************************************/
float StepperMotor::GetAccelerationRad()
{
  return (float)L6474shield.GetAcceleration(0) * stepAngleRadian;
}

/******************************************************//**
 * @brief Returns the acceleration of the stepper motor
 * @param None
 * @retval Acceleration in degrees/s^2
 **********************************************************/
float StepperMotor::GetAccelerationDeg()
{
  return (float)L6474shield.GetAcceleration(0) * stepAngleDegree;
}

/******************************************************//**
 * @brief Returns the current speed of the stepper motor
 * @param None
 * @retval Acceleration in radians/s
 **********************************************************/
float StepperMotor::GetCurrentSpeedRad()
{
  return (float)L6474shield.GetCurrentSpeed(0) * stepAngleRadian;
}

/******************************************************//**
 * @brief Returns the current speed of the stepper motor
 * @param None
 * @retval Acceleration in degrees/s
 **********************************************************/
float StepperMotor::GetCurrentSpeedDeg()
{
  return (float)L6474shield.GetCurrentSpeed(0) * stepAngleDegree;
}

/******************************************************//**
 * @brief Returns the max speed of the stepper motor
 * @param None
 * @retval Max speed in radians/s
 **********************************************************/
float StepperMotor::GetMaxSpeedRad()
{
  return (float)L6474shield.GetMaxSpeed(0) * stepAngleRadian;
}

/******************************************************//**
 * @brief Returns the max speed of the stepper motor
 * @param None
 * @retval Max speed in degrees/s
 **********************************************************/
float StepperMotor::GetMaxSpeedDeg()
{
  return (float)L6474shield.GetMaxSpeed(0) * stepAngleDegree;
}

/******************************************************//**
 * @brief Returns the min speed of the stepper motor
 * @param None
 * @retval Min speed in radians/s
 **********************************************************/
float StepperMotor::GetMinSpeedRad()
{
  return (float)L6474shield.GetMinSpeed(0) * stepAngleRadian;
}

/******************************************************//**
 * @brief Returns the min speed of the stepper motor
 * @param None
 * @retval Min speed in degrees/s
 **********************************************************/
float StepperMotor::GetMinSpeedDeg()
{
  return (float)L6474shield.GetMinSpeed(0) * stepAngleDegree;
}

/******************************************************//**
 * @brief Returns the deceleration of the stepper motor
 * @param None
 * @retval Deceleration in radians/s^2
 **********************************************************/
float StepperMotor::GetDecelerationRad()
{
  return (float)L6474shield.GetDeceleration(0) * stepAngleRadian;
}

/******************************************************//**
 * @brief Returns the deceleration of the stepper motor
 * @param None
 * @retval Deceleration in degrees/s^2
 **********************************************************/
float StepperMotor::GetDecelerationDeg()
{
  return (float)L6474shield.GetDeceleration(0) * stepAngleDegree;
}

/******************************************************//**
 * @brief  Returns the absolute position of the stepper motor
 * in radians
 * @param  None
 * @retval Absolute position from home in radians
 **********************************************************/
float StepperMotor::GetAbsolutePositionRad()
{
  return (float)L6474shield.GetPosition(0) * stepAngleRadian;
}

/******************************************************//**
 * @brief  Returns the absolute position of the stepper motor
 * in degrees
 * @param  None
 * @retval Absolute position from home in degrees
 **********************************************************/
float StepperMotor::GetAbsolutePositionDeg()
{
  return (float)L6474shield.GetPosition(0) * stepAngleDegree;
}

/******************************************************//**
 * @brief  Changes the acceleration of the stepper motor
 * @param newAcceleration New acceleration to apply in radians/s^2
 * @retval true if the command is successfully executed, else false
 * @note The command is not performed is the shield is executing 
 * a MOVE or GOTO command (but it can be used during a RUN command)
 **********************************************************/
bool StepperMotor::SetAccelerationRad(float newAcceleration)
{
  return newAcceleration > 0 ? L6474shield.SetAcceleration(0, (uint16_t)(newAcceleration / stepAngleRadian)) : false;
}

/******************************************************//**
 * @brief  Changes the acceleration of the stepper motor
 * @param newAcceleration New acceleration to apply in degrees/s^2
 * @retval true if the command is successfully executed, else false
 * @note The command is not performed is the shield is executing 
 * a MOVE or GOTO command (but it can be used during a RUN command)
 **********************************************************/
bool StepperMotor::SetAccelerationDeg(float newAcceleration)
{
  return newAcceleration > 0 ? L6474shield.SetAcceleration(0, (uint16_t)(newAcceleration / stepAngleDegree)) : false;
}

/******************************************************//**
 * @brief  Changes the maximum speed of the stepper motor
 * @param newDeceleration New maximum speed to apply in radians/s^2
 * @retval true if the command is successfully executed, else false
 * @note The command is not performed is the shield is executing 
 * a MOVE or GOTO command (but it can be used during a RUN command)
 **********************************************************/
bool StepperMotor::SetMaxSpeedRad(float newMaxSpeed)
{
  return newMaxSpeed > 0 ? L6474shield.SetMaxSpeed(0, (uint16_t)(newMaxSpeed / stepAngleRadian)) : false;
}

/******************************************************//**
 * @brief  Changes the maximum speed of the stepper motor
 * @param newDeceleration New maximum speed to apply in degrees/s^2
 * @retval true if the command is successfully executed, else false
 * @note The command is not performed is the shield is executing 
 * a MOVE or GOTO command (but it can be used during a RUN command)
 **********************************************************/
bool StepperMotor::SetMaxSpeedDeg(float newMaxSpeed)
{
  return newMaxSpeed > 0 ? L6474shield.SetMaxSpeed(0, (uint16_t)(newMaxSpeed / stepAngleDegree)) : false;
}

/******************************************************//**
 * @brief  Changes the minimum speed of the stepper motor
 * @param newDeceleration New minimum speed to apply in radians/s^2
 * @retval true if the command is successfully executed, else false
 * @note The command is not performed is the shield is executing 
 * a MOVE or GOTO command (but it can be used during a RUN command)
 **********************************************************/
bool StepperMotor::SetMinSpeedRad(float newMinSpeed)
{
  return newMinSpeed > 0 ? L6474shield.SetMinSpeed(0, (uint16_t)(newMinSpeed / stepAngleRadian)) : false;
}

/******************************************************//**
 * @brief  Changes the minimum speed of the stepper motor
 * @param newDeceleration New minimum speed to apply in degrees/s^2
 * @retval true if the command is successfully executed, else false
 * @note The command is not performed is the shield is executing 
 * a MOVE or GOTO command (but it can be used during a RUN command)
 **********************************************************/
bool StepperMotor::SetMinSpeedDeg(float newMinSpeed)
{
  return newMinSpeed > 0 ? L6474shield.SetMinSpeed(0, (uint16_t)(newMinSpeed / stepAngleDegree)) : false;
}

/******************************************************//**
 * @brief  Changes the acceleration of the stepper motor
 * @param newDeceleration New acceleration to apply in radians/s^2
 * @retval true if the command is successfully executed, else false
 * @note The command is not performed is the shield is executing 
 * a MOVE or GOTO command (but it can be used during a RUN command)
 **********************************************************/
bool StepperMotor::SetDecelerationRad(float newDeceleration)
{
  return newDeceleration > 0 ? L6474shield.SetDeceleration(0, (uint16_t)(newDeceleration / stepAngleRadian)) : false;
}

/******************************************************//**
 * @brief  Changes the acceleration of the stepper motor
 * @param newDeceleration New acceleration to apply in degrees/s^2
 * @retval true if the command is successfully executed, else false
 * @note The command is not performed is the shield is executing 
 * a MOVE or GOTO command (but it can be used during a RUN command)
 **********************************************************/
bool StepperMotor::SetDecelerationDeg(float newDeceleration)
{
  return newDeceleration > 0 ? L6474shield.SetDeceleration(0, (uint16_t)(newDeceleration / stepAngleDegree)) : false;
}

/******************************************************//**
 * @brief  Stops program execution until the shield state becomes Inactive
 * @param  None
 * @retval None
 **********************************************************/
void StepperMotor::WaitWhileActive()
{
  L6474shield.WaitWhileActive(0);
}

/******************************************************//**
 * @brief  Immediatly stops the motor and disable the power bridge
 * @param  None
 * @retval None
 **********************************************************/
void StepperMotor::HardStop()
{
  L6474shield.HardStop(0);
}

/******************************************************//**
 * @brief  Stops the motor by using the shield deceleration
 * @param  None
 * @retval true if the command is successfully executed, else false
 * @note The command is not performed is the shield is in INACTIVE state.
 **********************************************************/
bool StepperMotor::SoftStop()
{
  return L6474shield.SoftStop(0);
}

/******************************************************//**
 * @brief  Runs the motor. It will accelerate from the min 
 * speed up to the max speed by using the shield acceleration.
 * @param  direction counter clockwise (CCW) or clockwise (CW)
 * @retval None
 **********************************************************/
void StepperMotor::Run(direction_t direction)
{
  L6474shield.Run(0, direction == CCW ? FORWARD : BACKWARD);
}

/******************************************************//**
 * @brief  Set current position to be the home, or absolute, position
 * @param  None
 * @retval None
 **********************************************************/
void StepperMotor::SetHome()
{
  L6474shield.SetHome(0);
}

/******************************************************//**
 * @brief  Requests the motor to move to the home, or absolute, position
 * @param  None
 * @retval None
 **********************************************************/
void StepperMotor::GoHome()
{
  L6474shield.GoHome(0);
}

/******************************************************//**
 * @brief  Requests the motor to move to the specified position 
 * @param  targetPosition absolute position in radians (CCW is + / CW is -)
 * @retval None
 **********************************************************/
void StepperMotor::GoToRad(float targetPosition)
{
  L6474shield.GoTo(0, (int32_t)(targetPosition / stepAngleRadian));
}

/******************************************************//**
 * @brief  Requests the motor to move to the specified position 
 * @param  targetPosition absolute position in degrees (CCW is + / CW is -)
 * @retval None
 **********************************************************/
void StepperMotor::GoToDeg(float targetPosition)
{
  L6474shield.GoTo(0, (int32_t)(targetPosition / stepAngleDegree));
}

/******************************************************//**
 * @brief  Moves the motor of the specified number of radians
 * @param  targetDistance Target distance in radians
 * @retval None
 **********************************************************/
void StepperMotor::MoveRad(float targetDistance)
{
  L6474shield.Move(0, targetDistance > 0 ? FORWARD : BACKWARD, (uint32_t)(abs(targetDistance) / stepAngleRadian));
}

/******************************************************//**
 * @brief  Moves the motor of the specified number of degrees
 * @param  targetDistance Target distance in degrees
 * @retval None
 **********************************************************/
void StepperMotor::MoveDeg(float targetDistance)
{
  L6474shield.Move(0, targetDistance > 0 ? FORWARD : BACKWARD, (uint32_t)(abs(targetDistance) / stepAngleDegree));
}
