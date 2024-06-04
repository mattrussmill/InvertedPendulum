/******************************************************//**
 * @file    pendulum.h 
 * @version V1.0
 * @date    June 3, 2024
 * @brief   pendulum driver abstraction layer library
 * @author  Matthew R. Miller
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 **********************************************************/

#include "pendulum.h"
#include <Arduino.h>

/******************************************************//**
 * @brief  Constructor for the Pendulum object. Sets the
 * pulse angle constants for converting from pulses to 
 * degrees and radians
 * @param  pulsesPerRotation the number of pulses in one
 * rotation of the position sensor
 * @retval None
 **********************************************************/
Pendulum::Pendulum(unsigned int pulsesPerRotation) : pulsesPerRotation(pulsesPerRotation),
pulseAngleRadian(TWO_PI / (float)pulsesPerRotation), pulseAngleDegree(360.0 / (float)pulsesPerRotation) {}


/******************************************************//**
 * @brief  Initializes the quadrature encoder library.
 * @param  None
 * @retval None
 **********************************************************/
void Pendulum::Begin()
{
  encoder.Begin(pulsesPerRotation);
}

/******************************************************//**
 * @brief  Sets the home position of the pendulum to 0. The
 * home position is equal 0 in the same way as 2pi rad == 0
 * and 360 deg == 0 on a standard position coordinate plane.
 * @param  None
 * @retval None
 **********************************************************/
void Pendulum::SetHome()
{
  encoder.SetHomePosition();
}

/******************************************************//**
 * @brief  Returns the current position in pulses away from
 * 0 to 2pi radians where 0 == 2pi in a CCW rotation.
 * @param  None
 * @retval position in radians
 **********************************************************/
float Pendulum::GetCurrentPositionRad()
{
  return (float)encoder.GetCurrentPosition() * pulseAngleRadian;
}

/******************************************************//**
 * @brief  Returns the current position in pulses away from
 * 0 to 360 degrees where 0 == 360 in a CCW rotation.
 * @param  None
 * @retval position in degrees
 **********************************************************/
float Pendulum::GetCurrentPositionDeg()
{
  return (float)encoder.GetCurrentPosition() * pulseAngleDegree;
}

/******************************************************//**
 * @brief  Returns the current rotational velocity of the
 * device in radians/s. Direction is indicated  by sign where
 * CCW is + and CW is -
 * @param  None
 * @retval the current rotational velocity in radians
 **********************************************************/
float Pendulum::GetCurrentVelocityRad()
{
  return (float)encoder.GetCurrentVelocity() * pulseAngleRadian;
}

/******************************************************//**
 * @brief  Returns the current rotational velocity of the
 * device in degrees/s. Direction is indicated  by sign where
 * CCW is + and CW is -
 * @param  None
 * @retval the current rotational velocity in degrees
 **********************************************************/
float Pendulum::GetCurrentVelocityDeg()
{
  return (float)encoder.GetCurrentVelocity() * pulseAngleDegree;
}