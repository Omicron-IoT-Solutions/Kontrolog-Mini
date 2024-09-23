/*
KONTROLOG-MINI LIBRARIES FOR ARDUINO
By Omicron IoT Solutions
	

Copyright (c) 2024 Omicron IoT Solutions

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

version 1.0 September 2024
*/

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
 
}

// Main Loop
 void loop()
 {
	 
  //************************************************************************************************************************************************************
  //Example for the reading of a 10k NTC temperature sensor 

  Comms.SET_LED_RGB(led_red);
  In.READ_ANALOG_AND_STORING();                             // Reads The  A/D channels  IN1/IN2/VDC
  Ko.IN1.value=In.NTC_TEMP_INPUT_R(analog_input1);          // A 10k NTC is connected to input 1 channel, so the A/D value reading is converted to temperature
  Serial.printf("Temperature from NTC 10K Sensor : %.1f °C\r\n", float(Ko.IN1.value)/10.0);
  delay(500);
  Comms.SET_LED_RGB(led_off);
  delay(500);
  //************************************************************************************************************************************************************
	 
 }
