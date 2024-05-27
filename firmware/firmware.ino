#include <Arduino.h>
#include <SPI.h>
#include "l6474.h"
#include "quadrature.h"

#define ENCODER_CW_PIN 2  // Green wire
#define ENCODER_CCW_PIN 3 // White wire

L6474 L6474shield;
Quadrature quadrature;

uint16_t position;
int32_t velocity;
int32_t acceleration;
unsigned long tmp = millis();
  
void setup()
{

//----- Init
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

  /* Start the library to use the quadrature encoder. This library is configured to only support
   * one encoder per application. The quadrature initialization step occupies the following pins
   * on the Arduino Uno defined in quadrature.h: 2 and 3. */
  quadrature.Begin(360);

  position = quadrature.GetCurrentPosition();
  velocity = quadrature.GetCurrentVelocity();
  acceleration = quadrature.GetCurrentAcceleration();


// TODO check if polarity is backwards - Change step mode to full step mode

  /* Select full step mode for shield 0 */
  L6474shield.SelectStepMode(0, L6474_STEP_SEL_1);  

  /* Update speed, acceleration, deceleration for 1/16 microstepping mode*/
  // L6474shield.SetMaxSpeed(0, 100);
  // L6474shield.SetMinSpeed(0, 99);
  // L6474shield.SetAcceleration(0, 10000);
  // L6474shield.SetDeceleration(0, 10000);
  
  //L6474shield.Run(0, BACKWARD);
  Serial.begin(9600);

  
}

void loop()
{

  if (velocity != quadrature.GetCurrentVelocity())
  {
    position = quadrature.GetCurrentPosition();
    velocity = quadrature.GetCurrentVelocity();
    acceleration = quadrature.GetCurrentAcceleration();
    // Serial.print("pos: ");
    // Serial.print(position);
    Serial.print("vel:");
    Serial.print(velocity);
    Serial.print("\t\tacc:");
    Serial.println(acceleration);
    
  }

//test the frequency
  // if (millis() - tmp >= 1000) {
  //   Serial.println(Quadrature::tmp);
  //   Quadrature::tmp = 0;
  //   tmp = millis();
  // }
  
// TODO figure out why motor not working right - check wires are right configuration

  // L6474shield.Move(0, BACKWARD, 200); //ccw
  // Serial.print(L6474shield.GetCurrentSpeed(0));
  // Serial.print("\t");
  // Serial.println(quadrature.GetCurrentVelocity());
  // L6474shield.WaitWhileActive(0);


  //delay(1000);
  // L6474shield.Move(0, FORWARD, 200);
  // L6474shield.WaitWhileActive(0);
  // delay(1000);


}
