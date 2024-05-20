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


/// static member definitions
volatile int16_t Quadrature::position;
volatile uint16_t Quadrature::speed[2];
volatile uint16_t Quadrature::speedSample;
volatile int8_t Quadrature::directionVector; 
uint16_t Quadrature::pulsesPerRotation;
volatile bool Quadrature::calcFastPulse;

//test values
class Quadrature* Quadrature::instancePtr = NULL;
volatile unsigned long Quadrature::lastPositionTime;
volatile uint16_t Quadrature::pulsesPerSample;
volatile unsigned long Quadrature::compTime;

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
  SetHomePosition();
  pulsesPerSample = 0;
  directionVector = 0;
  speedSample = 0;
  speed[0] = 0;
  speed[1] = 0;
  calcFastPulse = true; //todo usually false, true to test until mechanism is in place to auto switch
  lastPositionTime = micros();
  instancePtr = this;
  InitIsrIntervalForTimer2();
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
 * @brief  TODO
 * @param  None
 * @retval TODO
 **********************************************************/
int32_t Quadrature::GetCurrentVelocity()
{
  return (uint32_t)speed[0] * directionVector;
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
  // update the position
  directionVector = direction;
  int16_t newPosition = position + directionVector;
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

  // calculate speed if at slow speed
  if (!calcFastPulse)
  {
    unsigned long start = micros();
    UpdateSpeed(1000000L, micros() - lastPositionTime);
    compTime = micros() - start;
    lastPositionTime = micros() - compTime; //todo don't forget to handle overflow for micros()
  }
  else
  {
    pulsesPerSample++;
    lastPositionTime = micros();
  }
}

/******************************************************//**
 * @brief  Gets the pointer to the Quadrature instance
 * @param  None
 * @retval Pointer to the instance of Quadrature
  **********************************************************/
class Quadrature* Quadrature::GetInstancePtr(void)
{
  return (class Quadrature*)instancePtr;
}

//todo write comment header
bool Quadrature::GetFastCalcStatus()
{
  return calcFastPulse;
}

//todo write comment header
uint16_t Quadrature::GetPulsesPerSample()
{
  return pulsesPerSample;
}

//todo write comment header
void Quadrature::ClearPulsesPerSample()
{
  pulsesPerSample = 0;
}

/******************************************************//**
 * @brief  Calculates the current speed of the TODO
 * @param  None
 * @retval Pointer to the instance of Quadrature
  **********************************************************/
// fast pulse and slow pulse condition. Slow pulse condition is < 62 pps since a period of ~0.016s as discussed in InitIsrIntervalForTimer2 comments.
// The basic formula for a discrete Infinite Impulse Response (IIR) low-pass filter (LPF) being: y(i)= β∗x(i)+(1-β)∗y(i-1)  note == this discrete-time implementation of a simple RC low-pass filter is the exponentially weighted moving average
// slightly improved low pass averages the last two inputs instead of using only the current input: y[n]=a⋅(x[n]+x[n−1])/2 + (1−a)⋅y[n−1]
void Quadrature::UpdateSpeed(uint32_t sample, unsigned long periodMicros)
{
  speedSample = sample / periodMicros;
  speed[1] = speed[0];
  speed[0] = 0.625f * speedSample + 0.375f * speed[1];
}

/******************************************************//**
 * @brief  Getter method for the last time the position changed
 * in microseconds
 * @param  None
 * @retval The last time the position changed in microseconds
  **********************************************************/
unsigned long Quadrature::GetLastPositionTime()
{
  return lastPositionTime;
}

//todo write function header or get rid of function call -> prob don't need it anymore since update speed is exposed now
void Quadrature::CheckSpeedTimeout()
{
  if (micros() - instancePtr->GetLastPositionTime() > 500000) //todo don't forget to handle overflow for micros()
  {
    //UpdateSpeed(0, 250000); ??
    speed[0] = 0;
  }
}

/******************************************************//**
 * @brief Timer2 interrupt handler used by PW2 for shield 1 //TODO used for speed and accel cleanup?
 * and enable the power bridge - only using shield 0 (Timer1) in l6474 library. Timer 1 is open for the encoder. - starts on page 74 in datasheet. Likely have to take this out of waveform generation mode from the motor, if already claimed. Arduino uses timer0 for milis() and other functions.
 * @param  None
 * @retval None
 **********************************************************/
ISR(TIMER2_COMPA_vect)
{
  class Quadrature* instancePtr = Quadrature::GetInstancePtr();
  if (instancePtr)
  {
    //instancePtr->CheckSpeedTimeout();
    if (instancePtr->GetFastCalcStatus())
    {
      instancePtr->UpdateSpeed(1000000 * (uint32_t)(instancePtr->GetPulsesPerSample()), 16255); // 16255us is the timer frequency set for TIMER2_COMPA_vect in InitIsrIntervalForTimer2
      instancePtr->ClearPulsesPerSample();
    } //this works much better with the filter -> maybe set a mode to select between fast, slow, or auto speed calculation?
    // also must figure out the change of direction when calculating speed with this method. Also see if I can speed up the calc any faster? Can do this later... might fix 
  }

  //Quadrature::tmp++;
}

