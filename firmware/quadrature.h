/******************************************************//**
 * @file    quadrature.h 
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

#ifndef __QUADRATURE_H_INCLUDED
#define __QUADRATURE_H_INCLUDED

#include <inttypes.h>

/// Digital pins used for the quadrature pulse signals
#define Quadrature_Lead_Pulse_CW_Pin   (2) // Green wire
#define Quadrature_Lead_Pulse_CCW_Pin  (3) // White wire

/// Value to increment based on direction
typedef enum {
  INCRIMENT_CCW = 1,
  INCRIMENT_CW  = -1
} incrementPosition_t;


/// Quadrature library class
class Quadrature
{
  public:
    void Begin(uint16_t ppr);         //Start the Quadrature library
    void SetHomePosition();           //Set the quadrature position to zero
    uint16_t GetPulsesPerRotation();  //Return the number of pulses in one rotation of the quadrature
    int16_t GetCurrentPosition();     //Return the current position in pulses between 0 and ppr
    int32_t GetCurrentVelocity();     //Return the current velocity in pps (CCW is + / CW is -)

    // these methods are for use in the ISR only
    static class Quadrature *GetInstancePtr();  //pointer to access methods in ISR on a clock step
    void IsrStepClockHandler();                 //function to be called internally, on each step of the ISR clock only

  private:
    void InitIsrIntervalForTimer2();
    void CheckFastCalcStatus();
    void CheckSpeedTimeout();
    void UpdatePosition(incrementPosition_t direction);
    void UpdateDirection(int8_t);
    unsigned long UpdateSpeed(uint32_t samples, unsigned long periodMicros);
    unsigned long UpdateAcceleration(unsigned long periodMicros);
    static void PulseCW();
    static void PulseCCW();

    // member variables
    uint16_t pulsesPerRotation;                 //Number of pulses in one rotation of the quadrature
    volatile int16_t position;                  //Position of the quadrature in pulses from 0 to ppr-1
    volatile uint16_t speed[2];                 //Filtered rotation speed in pulses per second (pps)
    volatile int8_t directionVector;            //The direction of the current and previous rotation (CCW is + / CW is -)
    volatile bool reverseLpfBias;               //Flag to reverse the LPF bias weight from the previous value to the current value
    volatile unsigned long lastPositionTime;    //Timestamp the position was last updated in microseconds
    volatile int32_t pulsesPerSample;           //Pulse counter for when pulse counting is used to determine speed
    volatile bool doFastPulseCalc;              //Flag determines if speed is calculated via pulse counting (fast speeds) or pulse timing (slow speeds)
    static class Quadrature *instancePtr;       //Pointer so the global ISR can call public methods

};

#endif /* #ifndef __QUADRATURE_H_INCLUDED */