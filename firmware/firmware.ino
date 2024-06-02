#include <Arduino.h>
#include <SPI.h>
#include "stepperMotor.h"
#include "quadratureEncoder.h"

#define ENCODER_CW_PIN 2  // Green wire
#define ENCODER_CCW_PIN 3 // White wire

StepperMotor stepperMotor(1.8f, STEP_HALF);
QuadratureEncoder quadrature(360);

uint16_t position;
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
  quadrature.Begin();

  // position = quadrature.GetCurrentPosition();
  // velocity = quadrature.GetCurrentVelocity();


// TODO check if polarity is backwards - Change step mode to full step mode


  // L6474shield.SetMaxSpeed(0,31);
  // L6474shield.SetMinSpeed(0,31);
  // L6474shield.SetAcceleration(0,20000);
  // L6474shield.SetDeceleration(0,20000);  
  
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
  
}

void loop()
{

  // /* Request shield 0 to go position -6400 */
  // L6474shield.GoTo(0,-6400);

  // /* Wait for the motor of shield 0 ends moving */
  // L6474shield.WaitWhileActive(0);
  // //L6474shield.CmdEnable(0); //how to hold position
  // delay(2000);

  // /* Request shield 0 to go position 6400 */
  // L6474shield.GoTo(0,6399);

  // /* Wait for the motor of shield 0 ends moving */
  // L6474shield.WaitWhileActive(0); 

  // delay(2000);

}
