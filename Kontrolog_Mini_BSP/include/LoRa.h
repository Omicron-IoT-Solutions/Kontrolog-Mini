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

#ifndef lora_h
#define lora_h

#include "sigfox.h"
#include "config.h"
#include "RTC_clock.h"
#include "communications.h"
#include "timers.h"


// BASED ON MODULE SJI LSM110A
// https://github.com/Support-SJI/LSM110A
class LoRa {

public:
    // Functions
    int  Active_LoRaMod(void);
    int  Set_Sleep_LoRaMod(void);
    int  Set_LoRaMod_OTAA(void);
    int  GetLoRaDeviceID(void);
    int  joinNetwork(int);
    int  wait_for_join(void);
    int  sendCMSGHEX(unsigned char *, int, int);
    int  wait_Rx_LoRa(void);
    void Read_Rx_data_LoRa(void);

private:
    // Functions
    int Detect_LoRaMod(void);   
    int factory_reset_LoRa(void);
    int getJoinMode();
    int reboot_LoRa(void);
    int SetJoinMode(int);
    int setClass(char *);
    void setDevEui(const char *);
    void setAppEui(const char *);
    void setAppKey(const char *);
    void Lora_Band_Config(void);
    void GetAppKey(void);
    void GetDevEui(void);
    void GetAppEui(void);

    // Private Variables
    int cont_fail=0;

};

extern LoRa Lo;

#endif

