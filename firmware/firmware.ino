#include <Arduino.h>
#include <SPI.h>

#include "l6474.h"

L6474 L6474shield;  
  
void setup()
{
  int32_t pos;
  uint16_t mySpeed;

//----- Init
  /* Start the library to use one shield. The L6474 registers are set with the predefined
   * values from file l6474_target_config.h. This initialization step occupies the following
   * pins on the Arduino Uno defined in l6474.h: 7, 8, 9 and 2 (which we will reclaim)*/
  L6474shield.Begin(1);

  /* Detatch the interrupt from the L6474.Begin() init function. Only two interrupts are 
   * available on the Arduino Uno and we will need them both for the encoder. */
  detachInterrupt(2);

  /* As per section 6.17 in hte L6474 datasheet - mask the FLAG conditions to keep the FLAG
   * pin from being pulled to ground through an open drain transistor. This will keep the 
   * interrupt on pin 2 open for use with the encoder and not create a pulse with an error
   * condition. */
  L6474shield.CmdSetParam(0, L6474_ALARM_EN, 0x0);



//----- Change step mode to full step mode
  /* Select decap mode for shield 0 */
  //L6474shield.CmdSetParam(0, L6474_Registers_t param, uint32_t value)

  /* Select full step mode for shield 0 */
  L6474shield.SelectStepMode(0,L6474_STEP_SEL_1_4);  

  /* Update speed, acceleration, deceleration for 1/16 microstepping mode*/
  L6474shield.SetMaxSpeed(0,4000);
  L6474shield.SetMinSpeed(0,40);
  L6474shield.SetAcceleration(0,2000);
  L6474shield.SetDeceleration(0,2000);
  
}

void loop()
{
  L6474shield.Move(0,FORWARD, 800); 


  // /* Request shield 0 to go position -6400 */
  // L6474shield.GoTo(0,-6400);

  // /* Wait for the motor of shield 0 ends moving */
  // L6474shield.WaitWhileActive(0);

  // /* Request shield 0 to go position 6400 */
  // L6474shield.GoTo(0,6400);

  // /* Wait for the motor of shield 0 ends moving */
  L6474shield.WaitWhileActive(0);
  delay(2000)  ;
}


























void MyFlagInterruptHandler(void)
{
  /* Get the value of the status register via the L6474 command GET_STATUS */
  uint16_t statusRegister = L6474shield.CmdGetStatus(0);

  /* Check HIZ flag: if set, power brigdes are disabled */
  if ((statusRegister & L6474_STATUS_HIZ) == L6474_STATUS_HIZ)
  {
    // HIZ state
  }

  /* Check direction bit */
  if ((statusRegister & L6474_STATUS_DIR) == L6474_STATUS_DIR)
  {
    // Forward direction is set
  }  
  else
  {
    // Backward direction is set
  }  

  /* Check NOTPERF_CMD flag: if set, the command received by SPI can't be performed */
  /* This often occures when a command is sent to the L6474 */
  /* while it is in HIZ state */
  if ((statusRegister & L6474_STATUS_NOTPERF_CMD) == L6474_STATUS_NOTPERF_CMD)
  {
       // Command received by SPI can't be performed
  }  

  /* Check WRONG_CMD flag: if set, the command does not exist */
  if ((statusRegister & L6474_STATUS_WRONG_CMD) == L6474_STATUS_WRONG_CMD)
  {
     //command received by SPI does not exist 
  }  

  /* Check UVLO flag: if not set, there is an undervoltage lock-out */
  if ((statusRegister & L6474_STATUS_UVLO) == 0)
  {
     //undervoltage lock-out 
  }  

  /* Check TH_WRN flag: if not set, the thermal warning threshold is reached */
  if ((statusRegister & L6474_STATUS_TH_WRN) == 0)
  {
    //thermal warning threshold is reached
  }    

  /* Check TH_SHD flag: if not set, the thermal shut down threshold is reached */
  if ((statusRegister & L6474_STATUS_TH_SD) == 0)
  {
    //thermal shut down threshold is reached * 
  }    

  /* Check OCD  flag: if not set, there is an overcurrent detection */
  if ((statusRegister & L6474_STATUS_OCD) == 0)
  {
    //overcurrent detection 
  }      
}
