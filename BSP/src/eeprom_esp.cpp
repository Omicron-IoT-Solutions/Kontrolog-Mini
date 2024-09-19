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

#include "eeprom_esp.h"


void ee_prom_class::Setup_EEPROM() {
    // Initialize EEPROM
    EEPROM.begin(0x1200); // Adjust size as needed based on your ESP32's EEPROM size
}

void ee_prom_class::Extract_Data_EEPROM(void)
{
unsigned int i,j;
    Setup_EEPROM();           // Adjust size as needed based on your ESP32's EEPROM size
    
    if (Read_Int_EEPROM(EEPROM_EEINI)!=EEPROM_VERSION)      // Change the Parameters Stored in EEPROM ?
        {
        Serial.printf("Factory Settings %d\r\n",EEPROM_VERSION );           // Debug
        Init_Sensor_Data (IN1_EEPROM,Sens_Ini0,Ko.IN1.Array,16);            // Init factory settings from sensors
        Init_Sensor_Data (IN2_EEPROM,Sens_Ini1,Ko.IN2.Array,16);
        Init_Sensor_Data (BAT_EEPROM,Sens_Ini5,Ko.VDC.Array,16);
        
        Init_Sensor_Data (OUT1_DIR,Out1_Init,Ko.OUT1.Array,16);             // Init factory settings from Outputs
        Init_Sensor_Data (OUT2_DIR,Out2_Init,Ko.OUT2.Array,16);

        for (i=0;i<16;i++)               // General Parameters
            {
            Write_Int_EEPROM(EEPROM_CONFIG_INI+i,Initial_Params[i]);           
            Ko.CONFIG.Array[i]=Initial_Params[i];                              
            }
 
        //Factory Reset for Comm. Module Parameters
        Ko.SIGFOX_MOD.zone=2;                                                      // Sigfox   RC2: USA
        Ko.LORA_MOD.band=US915;                                                    // Lora Band USA
        Ko.CONFIG.DT.RF_zone=2;                                                    // RF Zone 2: USA
        Write_Int_EEPROM(RTC_DIR+2,-5);                                            // Defaul Time Zone
        Ko.RTC.time_zone=-5;  
        
        /* Write  The last EEPROM version in memory */
        Write_Int_EEPROM(EEPROM_EEINI, EEPROM_VERSION);
        
        }
    
    else                                                                        // Read the Cofiguration Parameters stored in EEPROM
        {
        // Inputs
        Read_Sensor_Data (IN1_EEPROM,Ko.IN1.Array,16);                  
        Read_Sensor_Data (IN2_EEPROM,Ko.IN2.Array,16);
        Read_Sensor_Data (BAT_EEPROM,Ko.VDC.Array,16);
        // Outputs
        Read_Sensor_Data (OUT1_DIR,Ko.OUT1.Array,16);
        Read_Sensor_Data (OUT2_DIR,Ko.OUT2.Array,16);
        // General Configuration
        for (i=0;i<16;i++)
            {
            Ko.CONFIG.Array[i]=Read_Int_EEPROM(i+EEPROM_CONFIG_INI); 
            }
        
        // RTC (User can saved the RTC: Epoch, Date, Hour...)
        for(i=0;i<12;i++)
            Ko.RTC.Array[i]=Read_Int_EEPROM(RTC_DIR+i);  
        
        if(Ko.RTC.RTC_is_synchronized)
            RTC.SET_TIME(Ko.RTC.time_stamp);
        else;
        }
    
    // Init Timers for Alarms
    Ko.TIMERS.timer_Sil_IN1=Ko.IN1.alarm_delay;
    Ko.TIMERS.timer_Sil_IN2=Ko.IN2.alarm_delay;
    Ko.TIMERS.timer_Sil_VDC=Ko.VDC.alarm_delay;
    // Init timers for delay in activation of outputs
    Ko.OUT1.start_delay=10;
    Ko.OUT2.start_delay=10;
}

// Init Factory sensor data in EEPROM
void ee_prom_class::Init_Sensor_Data (int pos,const int *Dat_ini, int16_t *Array, char Size)
{
    unsigned char i;
    for (i=0;i<Size;i++)
        {
        Write_Int_EEPROM(pos+i,*(Dat_ini+i));                                      // Inicialización Sensor TCamara
        *(Array+i)=*(Dat_ini+i);
    } 
}

// Read Sensor Data Stored in EEPROM
void ee_prom_class::Read_Sensor_Data (int pos, int16_t *Array, char Size)
{
    unsigned char i;
    for (i=0;i<Size;i++)
        {
        *(Array+i)=Read_Int_EEPROM(pos+i);                                      // Inicialización Sensor TCamara
        } 
}

// Read and Integer(2 Bytes) parameter stored in EEPROM 
int16_t ee_prom_class::Read_Int_EEPROM(unsigned int index) {
    index *= 2; // The index is multiplied by 2 since the EEPROM is organized in bytes and the Parameters are stored in integers (2 Bytes).
    int16_t data_eeprom = EEPROM.readShort(index);
    return data_eeprom;
}

//Write and Integer(2 Bytes) parameter in EEPROM
void ee_prom_class::Write_Int_EEPROM(unsigned int index, int16_t newData) {
    index *= 2; // The index is multiplied by 2 since the EEPROM is organized in bytes and the Parameters are stored in integers (2 Bytes).
    int16_t data_eeprom = EEPROM.readShort(index);
    EEPROM.writeShort(index,newData);
    EEPROM.commit();  // Ensure data is written to flash (required for ESP32)
}

void ee_prom_class::Write_1Byte_EEPROM(int index, unsigned char newData) {
    EEPROM.write(index, newData); // Write a single byte to EEPROM at 'index'
    EEPROM.commit(); // Ensure data is written to flash (required for ESP32)
}

uint8_t ee_prom_class::Read_1Byte_EEPROM(unsigned int index)
{
    uint8_t data=EEPROM.read(index);
    return data;
}

// Write String in EEPROM
void ee_prom_class::Write_String_EEPROM (unsigned int addr, String a)
{ 
    EEPROM.writeString(addr,a);
    EEPROM.commit();  // Ensure data is written to flash (required for ESP32)
}

// Read String Stored in EEPROM
String ee_prom_class::Read_String_EEPROM (unsigned int addr){ 
    String newString=EEPROM.readString(addr);
return newString; 
}

