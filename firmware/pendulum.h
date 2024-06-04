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

#ifndef __PENDULUM_H_INCLUDED
#define __PENDULUM_H_INCLUDED

#include "quadratureEncoder.h"

class Pendulum
{
  public:
    Pendulum(unsigned int pulsesPerRotation); //Constructor for the Pendulum
    void Begin();                             //Start the Pendulum library
    void SetHome();                           //Set current position to be the home position 

    float GetCurrentPositionRad();     //Return the current position in pulses between 0 and 2pi
    float GetCurrentPositionDeg();     //Return the current position in pulses between 0 and 360

    float GetCurrentVelocityRad();     //Return the current velocity in radians/s (CCW is + / CW is -)
    float GetCurrentVelocityDeg();     //Return the current velocity in degrees/s (CCW is + / CW is -)

  private:
    QuadratureEncoder encoder;
    unsigned int pulsesPerRotation;
    float pulseAngleRadian;
    float pulseAngleDegree;
};


#endif /* #ifndef __PENDULUM_H_INCLUDED */