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
#ifndef eeprom_esp_h
#define	eeprom_esp_h

#include <Arduino.h>
#include <EEPROM.h>
#include "Config.h"
#include "RTC_CLOCK.h"

// Data Version in EEPROM
const int EEPROM_VERSION = 0x0001;

// Reserved addresses in the EEPROM for stored parameters for inputs, outputs and general parameters
const unsigned char
// Inputs
IN1_EEPROM = 0,
IN2_EEPROM = 0x10,
BAT_EEPROM = 0x50,
// Outputs
OUT1_DIR = 0x80,
OUT2_DIR = 0x90,
// RTC       
RTC_DIR  = 0xE0,
// General configuration Parameters
EEPROM_CONFIG_INI=0xF0,    
EEPROM_ID      = 0xF0,
DIR_TPU        = 0xF2,      // TPU
DIR_BOOT       = 0xF3,      // Boot Loader
EEPROM_EEINI   = 0xF5,      // Programación inicial de parámetros de fábrica.
DIR_COM_MODULE = 0xF6,      // Dirección módulo de comunicación
DIR_DOWNLINK   = 0xF8,      // Dirección de Downlink
DIR_RF_ZONE    = 0xF9,      // Dirección de zona RF para Sigfox y LoRa
DIR_STATUS     = 0XFA;      // Operational Status Address  0:off, 1: On

// Sensor Factory settings
 
/* Estructura de Datos
        0:  address;						// UID of the sensor
        1:  value;							// Current reading value
        2:  type;							// Type of the sensor, temperature,  humidity,  diferent from sensor
        3:  resolution;	            		// Resolution, 0, 1, 2 or 3 digits					
        4:  enable;							// Enable/Disable the sensor
        5:  offset;							// Offset to correct the reading value
        6:  L_limit;				    	// Lower alarm limit
        7:  H_limit;						// Higher alarm limit
        8:  alarm_delay;					// delay for triggering the alarm [in seconds]
        9:  slope;                          // Slope Scaled for 1000
        10: modbus_address_register;        // Modbus Start Adrress Register
        11: modbus_coils;                   // Modbus number of reading registers
        12: modbus_type;                    // Modbus type of data

 */

/* Types of sensors
    0: off,
    1: digital,
    2: i4to20mA,
    3: voltDC,
    4: temp,
    5: temp_hume,
    9: Modbus*/

const int Sens_Ini0[16]={1,0,4,1,1,0,0,200,300,1000,1,1,0,0};          // Sensor S1 
const int Sens_Ini1[16]={2,0,4,1,1,0,150,300,300,1000,1,1,1,0};        // Sensor S2 
const int Sens_Ini5[16]={6,0,3,1,1,0,0,1000,600,1000,1,1,1,0};         // DC Voltage

/*  Outputs factory settings
    0: value;
    1: Control Funtion;
    2: Control Singal;
    3: Set_point;
    4: Hysteresis
    5: Control Type
    6: Set_Time_ON;
    7: Set_Time_OFF;
    */
/* Outputs Types
    0: Output Off,
    1: Remote Commands
    2: Input follower
    3: Control ON/ OFF  Heating and Cooling
    4: Timer
    5: External Alarm
 */
const int Out1_Init[16]={1,0,0,1,40,20,0,60,60,10,5,3,12,2};          // Output1
const int Out2_Init[16]={2,0,0,2,40,20,0,60,60,10,5,3,12,2};          // Output2

const int Initial_Params [16] = {                                 // General Configurations
0,221,                 // initial ID
5,                     // TPU: Publication Time
1,                     // Enter to Service Mode
0,                     // Not used,  Reserved
1,                     // Version of factory parameters (Reset to factory settings Stored in EEPROM)
2,                     // Type of communication module programmed (0: none, 1: Sigfox, 2: WiFi, 3: Lora)
0,                     // Not Used, Reserved
3,                     // Number of messages for downlink
2,                     // RF zone  (2: USA-US915, 4: Latam-Australia-AU915 / 1: EU-EU868)
1,                     // Current status of the device (off, on, test mode)
0,                     // Password for configuration
0,                     // Sub task mode 
0,                     // Real communication module detected (0: none, 1: Sigfox, 2: WiFi, 3: Lora)
0,                     // Not Used, Reserved
0,                     // Not Used, Reserved
};                     


class ee_prom_class {

public:
    void Setup_EEPROM();                            // Begin EEPROM
    void Extract_Data_EEPROM(void);                 // Extract all parameter for sensor, outpus and general configuration stored into EEPROM 
    uint8_t Read_1Byte_EEPROM(unsigned int);        // Read one Byte Stored into EEPROM
    void Write_1Byte_EEPROM(int, unsigned char);    // Write one Byte into EEPROM
    int16_t Read_Int_EEPROM(unsigned int);          // Read one Integer (2bytes) stored into EEPROM
    void Write_Int_EEPROM(unsigned int, int16_t);   // Write one Integer (2bytes) into EEPROM
    String Read_String_EEPROM (unsigned int);       // Read one String Stored into EEPROM
    void Write_String_EEPROM (unsigned int, String);// Write one String into EEPROM


private:
    void Init_Sensor_Data (int,const int *, int16_t *,char);
    void Read_Sensor_Data (int, int16_t *, char);

};

extern ee_prom_class EEPROM_MEM;
#endif
