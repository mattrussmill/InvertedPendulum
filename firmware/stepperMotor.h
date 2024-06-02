/******************************************************//**
 * @file    stepperMotor.h 
 * @version V1.0
 * @date    June 2, 2024
 * @brief   TODO
 * @author  Matthew R. Miller
 *
 * TODO
 **********************************************************/

#ifndef __STEPPER_MOTOR_H_INCLUDED
#define __STEPPER_MOTOR_H_INCLUDED

#include <inttypes.h>
#include "l6474.h"

/// Step mode options for stepper motor
typedef enum {
  STEP_FULL = 1,      //full step
  STEP_HALF = 2,      //1/2 microstep
  STEP_QUARTER = 4,   //1/4 microstep
  STEP_EIGHTH = 8,    //1/8 microstep
  STEP_SIXTEENTH = 16 //1/16 microstep
} stepMode_t;

class StepperMotor
{
  public:
    StepperMotor(float stepAngleDeg, stepMode_t stepMode);         //Start the StepperMotor library
    Begin();


  private:
    L6474 L6474shield;
    stepMode_t stepMode;
    float stepAngleDegree;


};


#endif /* #ifndef __STEPPER_MOTOR_H_INCLUDED */