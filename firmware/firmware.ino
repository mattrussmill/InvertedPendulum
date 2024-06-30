#include <Arduino.h>
#include <SPI.h>
#include "stepperMotor.h"
#include "pendulum.h"

#define ENCODER_CW_PIN 2  // Green wire
#define ENCODER_CCW_PIN 3 // White wire

StepperMotor stepperMotor(1.8f, STEP_QUARTER);
Pendulum pendulum(360);

int32_t position = 0;
int32_t velocity;
int32_t acceleration;
unsigned long tmp = millis();
  
void setup()
{

//----- Init


  stepperMotor.Begin();




  /* Start the library to use the quadrature encoder. This library is configured to only support
   * one encoder per application. The quadrature initialization step occupies the following pins
   * on the Arduino Uno defined in quadratureEncoder.h: 2 and 3. */
  pendulum.Begin();
  position = pendulum.GetCurrentPositionDeg();
  velocity = pendulum.GetCurrentVelocityDeg();

  
  // // set speed after moving starts on Run? Weird...
  // //L6474shield.Run(0, BACKWARD);
  // //L6474shield.SetMaxSpeed(0,1600);

  // L6474shield.GoTo(0,0);
  //L6474shield.CmdEnable(0); //how to hold position -> add functions to hold position on stop
  //L6474shield.GoTo(0,0);
  Serial.begin(9600); 

  //TODO list:
  // modify the shield code to have AndStop methods or toggle 
  // write HAL objects to abstract away the BSPs
  // CAD enclosure / stand with power button
  stepperMotor.SetHome();
  stepperMotor.SetAccelerationDeg(800.0);
  stepperMotor.SetDecelerationDeg(800.0);
  Serial.println(stepperMotor.SetMinSpeedDeg(45.0));
  Serial.println(stepperMotor.SetMaxSpeedDeg(720.0));

  //stepperMotor.Run(CCW);
}

void loop()
{

  
  // stepperMotor.GoToDeg(60.0);
  // stepperMotor.WaitWhileActive();

  // stepperMotor.GoHome();
  // stepperMotor.WaitWhileActive();

  // stepperMotor.GoToDeg(-60.0);
  // stepperMotor.WaitWhileActive();

  // stepperMotor.GoHome();
  // stepperMotor.WaitWhileActive();

  // delay(2000);


  Serial.println(pendulum.GetCurrentPositionDeg());
  //Serial.println(pendulum.GetCurrentVelocityDeg());


}
