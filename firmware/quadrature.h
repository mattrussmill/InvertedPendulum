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
    void Begin(uint16_t ppr);       //Start the Quadrature library
    void SetHomePosition();         //Set the quadrature position to zero

    int16_t GetCurrentPosition();   //Return the current position in pulses between 0 and ppr
    int16_t GetAcceleration();      //Return the acceleration in pps^2
    uint16_t GetCurrentSpeed();     //Return the current speed in pps

    static volatile unsigned long tmp; //todo remove
    //static volatile unsigned long tmpt; //todo remove

  private:
    void InitIsrIntervalForTimer2();
    static void PulseCW();
    static void PulseCCW();
    static void UpdatePosition(incrementPosition_t direction);

    // member variables
    static volatile int16_t position;         //Position of the quadrature in pulses from 0 to ppr-1 
    static uint16_t pulsesPerRotation;   //Number of pulses per rotation of the quadrature

    
};

#endif /* #ifndef __QUADRATURE_H_INCLUDED */