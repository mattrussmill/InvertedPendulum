/******************************************************//**
 * @file    stepperMotor.h 
 * @version V1.0
 * @date    June 2, 2024
 * @brief   TODO
 * @author  Matthew R. Miller
 *
 * TODO
 **********************************************************/

#include "stepperMotor.h"

StepperMotor::StepperMotor(float stepAngleDeg, stepMode_t stepMode) : stepAngleDegree(stepAngleDeg / (float)stepMode), stepMode(stepMode) {}

StepperMotor::Begin()
{
  /* Start the library to use one shield. The L6474 registers are set with the predefined
   * values from file l6474_target_config.h. This initialization step occupies the following
   * pins on the Arduino Uno defined in l6474.h: 7, 8, 9 and 2 (pin 2 we will reclaim)*/
  L6474shield.Begin(1);

  /* Detatch the interrupt from the L6474.Begin() init function. Only two interrupts are 
   * available on the Arduino Uno and we will need them both for the encoder. */
  detachInterrupt(2);

  /* As per section 6.17 in hte L6474 datasheet - mask the FLAG conditions to keep the FLAG
   * pin from being pulled to ground through an open drain transistor. This will keep the 
   * interrupt on pin 2 open for use with the encoder and not create a pulse with an error
   * condition. */
  L6474shield.CmdSetParam(0, L6474_ALARM_EN, 0x0);

  /* Select the step mode for the stepper motor */
  switch(stepMode)
  {
    case STEP_HALF:
      L6474shield.SelectStepMode(0, L6474_STEP_SEL_1_2);
      break;

    case STEP_QUARTER:
      L6474shield.SelectStepMode(0, L6474_STEP_SEL_1_4);
      break;

    case STEP_EIGHTH:
      L6474shield.SelectStepMode(0, L6474_STEP_SEL_1_8);
      break;

    case STEP_SIXTEENTH:
      L6474shield.SelectStepMode(0, L6474_STEP_SEL_1_16);
      break;

    case STEP_FULL:
    default:
      L6474shield.SelectStepMode(0, L6474_STEP_SEL_1);
      break;
  }

  /* Set torque output current amplitude to 32mA. This reduces resonance noise level of the
   * selected XY42STH34-0354A stepper motor and provides plenty of power to hold position and
   * move the motor for the selected application */
  L6474shield.CmdSetParam(0, L6474_TVAL, L6474shield.ConvertCurrentToTval(32.0)); //TODO was not using TVAL current to par tester -> give this a shot

  /* Keep power bridge active when stepper motor stops moving. This will have the stepper motor
   * hold its position when inactive. */
  L6474shield.SetHoldPositionOnStop(true);
}