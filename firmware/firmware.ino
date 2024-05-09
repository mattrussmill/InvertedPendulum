#include <Arduino.h>
#include <SPI.h>
#include "l6474.h"
#include "quadrature.h"

#define ENCODER_CW_PIN 2  // Green wire
#define ENCODER_CCW_PIN 3 // White wire

L6474 L6474shield;
Quadrature quadrature;

uint16_t position;
  
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


//----- Change step mode to full step mode
  /* Select decap mode for shield 0 */
  //L6474shield.CmdSetParam(0, L6474_Registers_t param, uint32_t value)

  /* Select full step mode for shield 0 */
  L6474shield.SelectStepMode(0, L6474_STEP_SEL_1_4);  

  /* Update speed, acceleration, deceleration for 1/16 microstepping mode*/
  L6474shield.SetMaxSpeed(0, 1000);
  L6474shield.SetMinSpeed(0, 1000);
  L6474shield.SetAcceleration(0, 5000);
  L6474shield.SetDeceleration(0, 5000);
  
  Serial.begin(9600);
}

void loop()
{

  if (quadrature.GetCurrentPosition() != position)
  {
    position = quadrature.GetCurrentPosition();
    Serial.println(position);
  }
  

  // if (positionDiff != 0)
  // {
  //   if (positionDiff > 0)
  //   {
  //     L6474shield.Move(0, BACKWARD, positionDiff); //ccw
  //   }
  //   else 
  //   {
  //     L6474shield.Move(0, FORWARD, abs(positionDiff));
  //   }
  //   positionDiff = 0;
  // }

  

}
