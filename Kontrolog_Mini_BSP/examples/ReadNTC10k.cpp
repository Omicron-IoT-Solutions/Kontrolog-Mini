/****************************************************************************************************************************************************

Example code for reading a 10k NTC sensor and blinking the RGB LED red on each data sample.

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
  //Paste the following code into the void loop of your main.cpp file where you are going to program your project
/****************************************************************************************************************************************************/
  In.READ_ANALOG_AND_STORING();                             // Reads The  A/D channels  IN1/IN2/VDC
  Ko.IN1.value=In.NTC_TEMP_INPUT_R(analog_input1);          // A 10k NTC is connected to input 1 channel, so the A/D value reading is converted to temperature
  In.SUPPLY_VDC_READER();
  Serial.printf("Temperature from NTC 10K Sensor 1 : %.1f °C\r\n", float(Ko.IN1.value)/10.0);
  Comms.SET_LED_RGB(led_red);
  delay(500);
  Comms.SET_LED_RGB(led_off);
  delay(500);
/****************************************************************************************************************************************************/
 }