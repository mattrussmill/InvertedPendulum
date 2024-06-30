/******************************************************//**
 * @file    quadratureEncoder.cpp
 * @version V1.0
 * @date    May 21, 2024
 * @brief   AB two-phase quadrature optical encoder library for arduino 
 * @author  Matthew R. Miller
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 **********************************************************/

#include "quadratureEncoder.h"
#include <Arduino.h>

#define Fast_Calc_Threshold   130
#define Isr_Sample_Period_Us  16225

/// static member definitions
class QuadratureEncoder* QuadratureEncoder::instancePtr = NULL;

/******************************************************//**
 * @brief  Constructor for the encoder object. Sets the pulses per
 * rotation of the device and initializes necessary member variables
 * to their appropriate starting values.
 * @param  None
 * @retval None
 **********************************************************/
QuadratureEncoder::QuadratureEncoder()
{
  pulsesPerSample = 0;
  directionVector = 0;
  speed[0] = 0;
  speed[1] = 0;
  doFastPulseCalc = false;
  instancePtr = this;
}

/******************************************************//**
 * @brief  Initializes I/O and any interrupts needed for the
 * encoder and starts the library. Cannot be done in constructor
 * as ISR must be setup in an init function call.
 * @param  ppr Pulses per full rotation of the quadrature encoder
 * @retval None
 **********************************************************/
void QuadratureEncoder::Begin(uint16_t ppr)
{
  pulsesPerRotation = ppr;

  //LDP3806 encoder uses an open-collector output. Enable internal input pullup resistors as they are required.
  pinMode(Quadrature_Lead_Pulse_CW_Pin, INPUT_PULLUP);
  pinMode(Quadrature_Lead_Pulse_CCW_Pin, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(Quadrature_Lead_Pulse_CW_Pin), PulseCW, RISING);
  attachInterrupt(digitalPinToInterrupt(Quadrature_Lead_Pulse_CCW_Pin), PulseCCW, RISING);
  InitIsrIntervalForTimer2();
  SetHomePosition();
  lastPositionTime = micros();
}

/******************************************************//**
 * @brief  Sets the home position of the encoder to 0. The
 * home position is equal to the ppr in the same way 360 == 0
 * on a standard position coordinate plane.
 * @param  None
 * @retval None
 **********************************************************/
void QuadratureEncoder::SetHomePosition()
{
  position = 0;
}

/******************************************************//**
 * @brief  Return the number of pulses in one rotation of 
 * the quadrature
 * @param  None
 * @retval pulses per rotation member variable
 **********************************************************/
uint16_t QuadratureEncoder::GetPulsesPerRotation()
{
  return pulsesPerRotation;
}

/******************************************************//**
 * @brief  Returns the current position in pulses away from
 * 0 to ppr-1 in a CCW rotation.
 * @param  None
 * @retval position member variable
 **********************************************************/
int16_t QuadratureEncoder::GetCurrentPosition()
{
  return position;
}

/******************************************************//**
 * @brief  Returns the current rotational velocity of the
 * device in pulses per second (pps). Direction is indicated
 * by sign where CCW is + and CW is -
 * @param  None
 * @retval the current rotational velocity in pps
 **********************************************************/
int32_t QuadratureEncoder::GetCurrentVelocity()
{
  return (int32_t)speed[0] * directionVector;
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
void QuadratureEncoder::InitIsrIntervalForTimer2()
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
void QuadratureEncoder::PulseCW()
{
  class QuadratureEncoder* instancePtr = QuadratureEncoder::GetInstancePtr();
  if (instancePtr)
  {
    // if both pins CW LOW and CCW LOW, complete a step
    if (!(digitalRead(Quadrature_Lead_Pulse_CW_Pin) && digitalRead(Quadrature_Lead_Pulse_CCW_Pin)))
    {
      instancePtr->UpdatePosition(INCRIMENT_CW);
    }
    // if both pins CW HIGH and CCW HIGH, complete a step
    else if(!digitalRead(Quadrature_Lead_Pulse_CW_Pin) && !digitalRead(Quadrature_Lead_Pulse_CCW_Pin))
    {
      instancePtr->UpdatePosition(INCRIMENT_CCW);
    }
  }
}

/******************************************************//**
 * @brief  The ISR tied to the Quadrature_Lead_Pulse_CCW_Pin 
 * encoder pin used to incriment or decrement the position.
 * @param  None
 * @retval None
 **********************************************************/
void QuadratureEncoder::PulseCCW()
{
  class QuadratureEncoder* instancePtr = QuadratureEncoder::GetInstancePtr();
  if (instancePtr)
  {
    // if both pins CW LOW and CCW LOW, complete a step
    if (!(digitalRead(Quadrature_Lead_Pulse_CW_Pin) && digitalRead(Quadrature_Lead_Pulse_CCW_Pin)))
    {
      instancePtr->UpdatePosition(INCRIMENT_CCW);
    }
    // if both pins CW HIGH and CCW HIGH, complete a step
    else if(!digitalRead(Quadrature_Lead_Pulse_CW_Pin) && !digitalRead(Quadrature_Lead_Pulse_CCW_Pin))
    {
      instancePtr->UpdatePosition(INCRIMENT_CW);
    }
  }
} 

/******************************************************//**
 * @brief  Incriment the position of the encoder by one pulse
 * where CCW is + and CW is -, as is for quadrant standard position
 * when measuring rotation from a starting position
 * @param  direction The direction in which the encoder has turned.
 * @retval None
 **********************************************************/
void QuadratureEncoder::UpdatePosition(incrementPosition_t direction)
{
  /* update the position based on the most recent pulse direction 
   * and account for rollover of the number of pulses from the home
   * position of 0 to ppr - 1 */
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

  /* If pulse rate is below 125 pps, calculate the speed based on the time between pulses
   * as counting the pulses in a fixed period becomes less accurate as slower speeds. See
   * CheckFastCalcStatus comment for more detail */
  if (!doFastPulseCalc)
  {
    UpdateDirection(direction);

    /* since only 1 pulse is counted, accounting for the computation time of the LPF which aids 
     * in accuracy of the speed calculation*/
    lastPositionTime = micros() - UpdateSpeed(1L, micros() - lastPositionTime);
    
  }
  else
  {
    pulsesPerSample += direction;
    lastPositionTime = micros();
  }
}

/******************************************************//**
 * @brief  Update the direction member variable for if the most
 * recent input is CCW (+) or CW (-). Sets a flag, reverseLpfBias,
 * if the direction was reversed on the most recent update
 * @param  newDirection The direction in which the encoder has turned.
 * @retval None
 **********************************************************/
void QuadratureEncoder::UpdateDirection(int8_t newDirection)
{
  newDirection != directionVector? reverseLpfBias = true : reverseLpfBias = false;
  directionVector = newDirection;
}

/******************************************************//**
 * @brief  Gets the pointer to the Quadrature instance
 * @param  None
 * @retval Pointer to the instance of Quadrature
 **********************************************************/
class QuadratureEncoder* QuadratureEncoder::GetInstancePtr(void)
{
  return (class QuadratureEncoder*)instancePtr;
}

/******************************************************//**
 * @brief Checks and updates the flag which determines whether or not 
 * to use pulse counting or pulse timing to determine the rate of rotation.
 * Pulse counting is used for speeds exceeding 125pps and pulse timing is
 * used for speeds lower or equal to 125pps. This was based on the ISR clock
 * period of 16.25ms. If the pulse timing, which has a period of ~50us when
 * including filter calculations, exceeds 40% of the ISR clock period.
 * So, 16.25ms * 40% = 6.5ms. 6.5ms / 50us = 130 pulses.
 * @param  None
 * @retval None
  **********************************************************/
void QuadratureEncoder::CheckFastCalcStatus()
{
  doFastPulseCalc = speed[0] > Fast_Calc_Threshold;
}

/******************************************************//**
 * @brief Calculates the current rotational speed of the device
 * in pulses per second (pps) using a range of samples over a microsecond
 * sampling period. The output of this calculation is smoothed with
 * a low pass filter to reduce jitter between pulses.
 * @param samples The number of samples, or pulses, in the sample period.
 * @param periodMicros The sample period in microseconds.
 * @retval The total computation time of the method in microseconds (us).
  **********************************************************/
unsigned long QuadratureEncoder::UpdateSpeed(uint32_t samples, unsigned long periodMicros)
{
  unsigned long startComputationTime = micros();

  /* Using a sample period of microseconds for relatively slow pulse speed, the resolution
   * remains high enough to use integer math instead of floating point operations to reduce
   * calculation time. Since speed is measured pps, it is necessary to scale the number of
   * samples in the same way we scale microseconds to get an integer. So samples * 1^6. */
  uint16_t speedSample = 1000000L * samples / periodMicros;
  speed[1] = speed[0];

  /* The formula used for the filter is basic a formula for a discrete Infinite Impulse Response (IIR)
   * low-pass fil ter (LPF), also known as a weighted moving average, being: y[i]= B∗x[i]+(1-B)∗y[i-1].
   * By using a fraction, B=3/8, we are able to simplify the equation and avoid using floating point arithmetic: 
   * y[i]= 3/8∗x[i]+(1-3/8)∗y[i-1] = 3/8∗x[i]+(5/8)∗y[i-1] = (3∗x[i]+5∗y[i-1])/8
   * This reduced computation time from ~82us to ~48us from the floating point solution */
  if (!reverseLpfBias)
  {
    speed[0] = (3 * speedSample + 5 * speed[1]) / 8;
  }
  /* On direction changes, reverse the LPF bias to favor the new sample so the speed change is more accurate @ B=3/4 */
  else
  {
    speed[0] = (3 * speedSample + speed[1]) / 4;
  }

  /* Note: Tried a slightly improved low pass that averages the last two inputs instead of using only
   * the current input: y[i]=B(x[i]+x[i−1])/2 + (1−B)y[i−1], but this proved to smooth the results
   * more than desired, even when adjusting B to weight the samples more. */

   return micros() - startComputationTime;
}

/******************************************************//**
 * @brief  To be called by the IsrStepClockHandler, this method
 * performs the timeout check for when the quadrature has stopped
 * moving and sets the current speed to zero. The timeout period
 * scales from half of the Isr_Sample_Period_Us period half the
 * period times the Fast_Calc_Threshold. It scales so that the
 * slower the rotational velocity, the longer the timeout duration.
 * @param  None
 * @retval None
 **********************************************************/
void QuadratureEncoder::CheckSpeedTimeout()
{
  if (micros() - lastPositionTime > (Fast_Calc_Threshold - (uint32_t)speed[0]) * Isr_Sample_Period_Us / 2 ) {
    speed[1] = speed[0];
    speed[0] = 0;
  }
}

/******************************************************//**
 * @brief  The ISR handling function. When the pulse rate exceeds
 * the Fast_Calc_Threshold in pps, this method is used to calculate
 * the rotational velocity of the quadrature. The sample rate is
 * set in InitIsrIntervalForTimer2. This handler also periodically
 * checks the timeout for setting the velocity to zero if the velocity
 * is below the Fast_Calc_Threshold. It also controls switching between
 * pulse counting and pulse timing to determine the current speed.
 * @param  None
 * @retval None
 **********************************************************/
void QuadratureEncoder::IsrStepClockHandler()
{
    if (doFastPulseCalc)
    {
      UpdateDirection(pulsesPerSample > 0 ? 1 : -1);
      UpdateSpeed((uint32_t) abs(pulsesPerSample), Isr_Sample_Period_Us); // 16255us is the timer frequency set for TIMER2_COMPA_vect in InitIsrIntervalForTimer2
    }
    else
    {
      CheckSpeedTimeout();
    }

    pulsesPerSample = 0;
    CheckFastCalcStatus();
}

/******************************************************//**
 * @brief  The Interrupt Service Routine (ISR) which occurs periodically
 * based on the configuration of Timer 2 of the AtMega328P if the
 * non-static members of the class are initialized. See
 * InitIsrIntervalForTimer2 for configuration of the timer.
 * @param  TIMER2_COMPA_vect interrupt handler which is called
 * when the timer/counter2(TCNT2) and compare register(OCR2A) becomes 
 * the same value
 * @retval None
 **********************************************************/
ISR(TIMER2_COMPA_vect)
{
  class QuadratureEncoder* instancePtr = QuadratureEncoder::GetInstancePtr();
  if (instancePtr)
  {
    instancePtr->IsrStepClockHandler();
  }
}

