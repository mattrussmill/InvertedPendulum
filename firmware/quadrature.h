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
#include <Arduino.h>

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
    void Begin(uint16_t ppr);       //Start the Quadrature library
    void SetHomePosition();         //Set the quadrature position to zero

    int16_t GetCurrentPosition();   //Return the current position in pulses between 0 and ppr
    int32_t GetCurrentVelocity();   //Return the current velocity in pps (CCW is + / CW is -)
    int16_t GetAcceleration();      //Return the acceleration in pps^2

    // these methods are for use in the ISR only
    static class Quadrature *GetInstancePtr(void); //todo
    static unsigned long GetLastPositionTime();
    static bool GetFastCalcStatus();
    static uint16_t GetPulsesPerSample();
    static void ClearPulsesPerSample();
    static void UpdateSpeed(uint32_t samples, unsigned long periodMicros);
    static void CheckSpeedTimeout();


    static volatile unsigned long tmp; //todo remove
    //static volatile unsigned long tmpt; //todo remove

    static volatile unsigned long compTime; //todo do I need this?

  private:
    void InitIsrIntervalForTimer2();
    static void PulseCW();
    static void PulseCCW();
    static void UpdatePosition(incrementPosition_t direction);


    // member variables
    static volatile int16_t position;               //Position of the quadrature in pulses from 0 to ppr-1
    static volatile uint16_t speed[2];              //Filtered rotation speed in pulses per second (pps)
    static volatile uint16_t speedSample;           //Raw rotation speed sample in pulses per second (pps)
    static volatile int8_t directionVector;         //The direction of the current rotation (CCW is + / CW is -)
    static class Quadrature *instancePtr;           //Pointer so the global ISR can call public methods
    static volatile unsigned long lastPositionTime; //Timestamp the position was last updated in microseconds
    static volatile uint16_t pulsesPerSample;
    static volatile bool calcFastPulse;             //Flag determines if speed is calculated via pulse counting (fast speeds) or pulse timing (slow speeds)


    static uint16_t pulsesPerRotation;              //Number of pulses in one rotation of the quadrature

    
};

#endif /* #ifndef __QUADRATURE_H_INCLUDED */