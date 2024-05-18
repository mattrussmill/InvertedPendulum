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
#include <Arduino.h>

/// static member definitions
volatile int16_t Quadrature::position;
uint16_t Quadrature::pulsesPerRotation;
volatile unsigned long Quadrature::tmp = 0; //todo remove


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
  InitIsrIntervalForTimer2();
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
 * @brief Initialize Timer 2 of the AtMega328P for asynchronous
 * operation by following the steps layed out in section 17.9 
 * (page 126) of the datasheet. It should be noted that Timer 1
 * is being used by l6474 shield 0, and since we are using only
 * one shield there should be no conflict.
 * @param  None
 * @retval None
 **********************************************************/
void Quadrature::InitIsrIntervalForTimer2()
{
  /* Disable Timer2 interrupt by setting the special function register
   * for the Timer/Counter2 Interrupt Enable to 0 with the appropriate mask */
  _SFR_BYTE(TIMSK2) &= ~(_BV(OCIE2B) | _BV(OCIE2A) | _BV(TOIE2)); // 1111 1000

  /* Skip settiing AS2 clock select. Use Arduino default -> I/O clock. */

  /* Turn on CTC mode so that OCR2A defines the TOP value for TCNT2. Leave OC2A disconnected. */
  TCCR2A = 0x02; //sets bit WGM21

  /* Set the prescalar mode to use clk/1024 and don't force OC2B pin or use waveform generation mode. */
  TCCR2B = 0x07; //set bits CS22, CS21, and CS20

  /* Set the timer TOP value for CTC mode based on the prescalar used. Using an 8-bit timer, the prescalar
   * and desired frequency must adhere to this rule: (F_CPU / (prescalar * desired frequency)) - 1 < 255.
   * Solving for 254, the slowest frequency we can achieve is ~61.52 Hz or a poling rate of ~16 ms*/
  OCR2A = 0xFF;

  /* Set the Timer/Counter register to start at 0 */
  TCNT2 = 0;

  /* Clear the Timer/Counter2 interrupt flags */
  TIFR2 = 0;

  /* Enable Timer2 Interrupt Mask Register with the appropriate mask for TIMER2_COMPA_vect*/
  _SFR_BYTE(TIMSK2) |= _BV(OCIE2A);
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
 * @param  direction The direction in which the encoder has turned.
 * @retval None
 **********************************************************/
void Quadrature::UpdatePosition(incrementPosition_t direction)
{
  int16_t newPosition = position + direction;
  if (newPosition < 0)
  {
    position = pulsesPerRotation - 1;
  }
  else if (newPosition >= pulsesPerRotation)
  {
    position = 0;
  }
  else
  {
    position = newPosition;
  }
}

/******************************************************//**
 * @brief Timer2 interrupt handler used by PW2 for shield 1 //TODO
 * and enable the power bridge - only using shield 0 (Timer1) in l6474 library. Timer 1 is open for the encoder. - starts on page 74 in datasheet. Likely have to take this out of waveform generation mode from the motor, if already claimed. Arduino uses timer0 for milis() and other functions.
 * @param  None
 * @retval None
 **********************************************************/
ISR(TIMER2_COMPA_vect)
{
  Quadrature::tmp++;
}
