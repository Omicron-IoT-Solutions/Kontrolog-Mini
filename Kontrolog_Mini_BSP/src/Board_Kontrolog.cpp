/*
KONTROLOG-MINI LIBRARIES FOR ARDUINO

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
#include "inputs.h"

void  Board::setup(void)
{
  pinMode(LED_RED, OUTPUT);                                 // Configure IO LEDs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(Buzzer,OUTPUT);
  digitalWrite(LED2,HIGH);                                  // Turn OFF LED 2 ( (Negative Logic LOW = LED ON / HIGH =LED OFF))                     
 
  Serial1.begin(9600,SERIAL_8N1,RX_COM,TX_COM);             // Serial 1, for Communication Module Sigfox / LoRa
  pinMode(EN_COM_MOD,OUTPUT);                               // Pin for Enable/Disable COM Module
  digitalWrite(EN_COM_MOD,HIGH);                            // Activated the Comm. Module LSM110A
  
  Serial2.begin(9600, SERIAL_8N1, RX_RS485, TX_RS485);      //Serial 2,  for RS-485 Communication   
  pinMode(EN_RS485,OUTPUT);                                 //Rx/Tx Pin for RS485
  digitalWrite(EN_RS485,HIGH);                              //Set Tx in RS-485 Port

  In.INPUTS_INI();                                          //Init The Analog Inputs Channels:  IN1/IN2/VDC

}