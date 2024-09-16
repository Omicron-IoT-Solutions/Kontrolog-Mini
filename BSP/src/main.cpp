#include "config.h"
#include "wifi_esp.h"
#include "sigfox.h"
#include "timers.h"
#include "LoRa.h"
#include "modbus.h"
#include "TLA2528_C.h"
#include "inputs.h"


Config Ko;                      // Create all Data Struct  (from config.h)
ee_prom_class EEPROM_MEM;       // Create EEPROM Class 
Sigfox_class Si;                // Create Sigfox Class
wifi_esp_class Wi;              // Create Wifi Class
LoRa Lo;                        // Create Lora Class
Timers TIM;                     // Create Timer Class                   
ModbusClass Mod;                // Create Modbus Class
RTC_clock_class RTC;            // Create RTC Class
comms Comms;                    // Communication Task handler
Inputs In;                      // Inputs class

IN_t INPUT1,INPUT2;
 
void setup() {
  
  Serial.begin(115200);                                     // Serial 0, for debug

  pinMode(LED_RED, OUTPUT);                                 // Configure IO LEDs
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(Buzzer,OUTPUT);
  digitalWrite(LED2,HIGH);                                  // Turn OFF ( (Negative Logic LOW = LED ON / HIGH =LED OFF))                     
 
  digitalWrite(Buzzer,HIGH);                                // Turn ON Buzzer
  Comms.SET_LED_RGB(led_white);                             // Turn ON LED Indication 
  delay(1000);
  digitalWrite(Buzzer,LOW);
  Comms.SET_LED_RGB(led_off);

  
  Serial1.begin(9600,SERIAL_8N1,RX_COM,TX_COM);             // Serial 1, for Communication Module Sigfox / LoRa
  pinMode(EN_COM_MOD,OUTPUT);                               // Pin for Enable/Disable COM Module
  
  Serial2.begin(9600, SERIAL_8N1, RX_RS485, TX_RS485);      //Serial 2,  for RS-485 Communication   
  pinMode(EN_RS485,OUTPUT);                                 //Rx / Tx Pin for RS485
  digitalWrite(EN_RS485,HIGH);                              // Set Tx RS-485

  In.INPUTS_INI();

}

// Main Loop
 void loop()
 {
  Comms.SET_LED_RGB(led_blue);
  delay(100);
  Comms.SET_LED_RGB(led_orange);
  delay(100);
 }
