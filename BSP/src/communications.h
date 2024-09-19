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

#ifndef comms_h
#define	comms_h

#include <cstdint>
#include <Arduino.h>
#include "config.h"
#include "eeprom_esp.h"
#include "RTC_clock.h"
#include "sigfox.h"
#include "LoRa.h"
#include "WiFi.h"
#include "timers.h"


class comms {

public:
    // Functions
    
    void Communication_Task(void);          // Communication task manager
    void Found_Com_Module(void);            // Detect communication module

    int  Process_Rx_Data(char *);           // Procces Downlink Command                 
    uint8_t User_downlink_data(void);       // Funtion for user procces commands 
    void Prepare_Command_to_Send(char *);   // Prepare one Messages to send
    int  sendMessage(char *);               // Send the Messages to Com Module
    int  get_answer_Msg(void);              // Get the answer form Com Module
    void ASCII2Hex(unsigned char* input, int length, char* buf_str);   // Convert data into HEX ASCII

    // RGB LED funtions
    void SET_LED_RGB(char color);           // Set RGB in different colors (See Config.h for admit colors)
    void LED_SEND_MSG(void);                // RGB LED confirm the send data
    void LED_ERR_MSG(void);                 // RGB Indicates transmision Error

    // Public Variables
    char BufferRx[200];                     // Buffer Data Rx from Comm. Module

    
private:
    // Functions
    void Serial_Tx (char *buffer);
    void Serial_Rx(void);
    void BufferRx_Init(void);
    void Turn_Off_Com_Module(void);
    void Prepare_Data_To_Send(void);
    char Activate_Com_Module (void);
    void Transmit_Data(unsigned char *, char);

    // Internal variables
    bool Dato_RxFlag;                                        // FLag indicates Rx Command
    char DataRx;                                             // Last Data Received
    uint16_t Count_Rx;                                        // Counter for Data Rx                          
    uint16_t Cont_Tx;                                        // Counter for Data Tx                        
    unsigned char buf_str[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};    // Auxiliar for transmision
}; 

extern comms Comms;
#endif

