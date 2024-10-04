/****************************************************************************************************************************************************

Example code for reading a Modbus RTU sensor (connected to the RS-485 port), blinking of the RGB LED in violet color at each reading sample, and 
sending via LoRaWAN technology.

****************************************************************************************************************************************************/

#include "config.h"
#include "wifi_esp.h"
#include "sigfox.h"
#include "timers.h"
#include "LoRa.h"
#include "modbus.h"
#include "TLA2528_C.h"
#include "inputs.h"
#include "communications.h"

Board Ko;                       // Create all Data Struct  (from config.h)
ee_prom_class EEPROM_MEM;       // Create EEPROM Class 
LoRa Lo;                        // Create Lora Class
Sigfox_class Si;                // Create Sigfox Class
wifi_esp_class Wi;              // Create Wifi Class
Timers Ti;                      // Create Timer Class                   
ModbusClass Modbus;             // Create Modbus Class
RTC_clock_class RTC;            // Create RTC Class
comms Comms;                    // Create Communication Task handler
Inputs_Class In;                // Create Inputs class
Outputs_class Outputs;          // Create Outputs Class

void setup() {
  
  Serial.begin(115200);                                     // Serial 0, for debug
  Ko.setup();                                               // Setup KONTROLOG MINI 1.0 Board                                         

  digitalWrite(Buzzer,HIGH);                                // Turn ON Buzzer
  Comms.SET_LED_RGB(led_white);                             // Turn ON RGB LED Indication in White Color 
  delay(1000);
  digitalWrite(Buzzer,LOW);
  Comms.SET_LED_RGB(led_off);
  
  EEPROM_MEM.Extract_Data_EEPROM();                         // Extract all Inputs, Outpus and general config parameters
  Ti.INIT_TIMER();                                          // Init Timer 0    
}

// Main Loop
 void loop()
 {
// Paste the following code into the void loop of your main.cpp file where you are going to program your project
/****************************************************************************************************************************************************/
  Ko.MODBUS.slave_address=1;                        // The slave address is assigned.
  Ko.MODBUS.funtion_code=3;                         // Retention registers are read.
  Ko.MODBUS.start_address=1;                        // The start register to be read is assigned.
  Ko.MODBUS.coils_no=1;                             // The number of coils is assigned.
  Modbus.Modbus_Telegram(Ko.MODBUS);                /* Sends a Modbus telegram, waits for a response from the slave, verifies if it is valid and 
                                                       stores the received registers*/
  
  Serial.print("Modbus read:");                  // The sensor reading is printed by registering it in hexadecimal format.
  for (int x = 0; x < 40; x++)
  {    
    Serial.print(Ko.MODBUS.registers[x],HEX);
    Serial.print(" ");
  }
  Serial.println();
  
  Ti.DELAY_TMR(500);                                // A delay of 500ms is applied.
  Comms.SET_LED_RGB(led_violet);                    // Turns on the RGB LED and activates it in red color.
  Ti.DELAY_TMR(500);                                // A delay of 500ms is applied.
  Comms.SET_LED_RGB(led_off);                       // Turns off the RGB LED.

  Comms.Communication_Task();                       // Communication tasks are performed.
/****************************************************************************************************************************************************/
 }
