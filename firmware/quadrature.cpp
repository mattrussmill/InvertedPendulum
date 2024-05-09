/******************************************************//**
 * @file    quadrature.cpp
 * @version V1.0
 * @date    May 6, 2024
 * @brief   AB two-phase quadrature optical encoder library for arduino 
 * @author  Matthew R. Miller
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 **********************************************************/

#include "Quadrature.h"
#include <math.h>

/// static member definitions
volatile int16_t Quadrature::position;
uint16_t Quadrature::pulsesPerRotation;


/******************************************************//**
 * @brief  Sets the ppr for the quadrature encoder device and
 * starts the Quadrature library
 * @param  None
 * @retval None
 **********************************************************/
void Quadrature::Begin(uint16_t ppr)
{
  pulsesPerRotation = ppr;
  pinMode(Quadrature_Lead_Pulse_CW_Pin, INPUT_PULLUP);
  pinMode(Quadrature_Lead_Pulse_CCW_Pin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(Quadrature_Lead_Pulse_CW_Pin), PulseCW, RISING);
  attachInterrupt(digitalPinToInterrupt(Quadrature_Lead_Pulse_CCW_Pin), PulseCCW, RISING);
  SetHomePosition();
}

/******************************************************//**
 * @brief  Sets the home position of the encoder to 0. The
 * home position is equal to the ppr in the same way 360 == 0
 * on a standard position coordinate plane.
 * @param  None
 * @retval None
 **********************************************************/
void Quadrature::SetHomePosition()
{
  position = 0;
}

/******************************************************//**
 * @brief  Returns the current position in pulses away from
 * 0 to ppr-1 in a CCW rotation.
 * @param  None
 * @retval position member variable
 **********************************************************/
int16_t Quadrature::GetCurrentPosition()
{
  return position;
}

/******************************************************//**
 * @brief  The ISR tied to the Quadrature_Lead_Pulse_CW_Pin 
 * encoder pin used to incriment or decrement the position.
 * @param  None
 * @retval None
 **********************************************************/
void Quadrature::PulseCW()
{
  if (digitalRead(Quadrature_Lead_Pulse_CCW_Pin) == LOW)
  {
    UpdatePosition(INCRIMENT_CW);
  }
  else
  {
    UpdatePosition(INCRIMENT_CCW);
  }
}

/******************************************************//**
 * @brief  The ISR tied to the Quadrature_Lead_Pulse_CCW_Pin 
 * encoder pin used to incriment or decrement the position.
 * @param  None
 * @retval None
 **********************************************************/
void Quadrature::PulseCCW()
{
  if (digitalRead(Quadrature_Lead_Pulse_CW_Pin) == LOW)
  {
    UpdatePosition(INCRIMENT_CCW);
  }
  else
  {
    UpdatePosition(INCRIMENT_CW);
  }
} 

/******************************************************//**
 * @brief  Incriment the position of the encoder by one pulse
 * where CCW is + and CW is -, as is for quadrant standard position
 * @param  None
 * @retval None
 **********************************************************/
void Quadrature::UpdatePosition(incrementPosition_t direction)
{
  int16_t newPosition = position + direction;
  if (newPosition < 0)
  {
    position = pulsesPerRotation - 1;
  }
  else if (newPosition > pulsesPerRotation - 1)
  {
    position = 0;
  }
  else
  {
    position = newPosition;
  }
}