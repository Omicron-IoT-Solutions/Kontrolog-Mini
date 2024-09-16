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

version 1.0 July 2024
*/

#ifndef lora_h
#define lora_h

#include "sigfox.h"
#include "config.h"
#include "RTC_clock.h"
#include "communications.h"
#include "timers.h"

class LoRa {

public:
    // Functions
    int  Active_Modulo_LoRa(void);
    int  Deep_Sleep_Lora(void);
    int  initLoRa_OTAA(int);
    int  GetLoRaDeviceID(void);
    int  joinNetwork(int);
    int  wait_for_join(void);
    int  sendCMSGHEX(unsigned char *, int, int);
    int  wait_rx_lora(void);
    void Rx_data_lora(void);

private:
    // Functions
    int Detect_if_Module_LoRa(void);   
    int factory_reset_lora(void);
    int getJoinMode();
    int cont_fail=0;
    int reboot_lora(void);
    int SetJoinMode(int);
    int setClass(char *);
    void setDevEui(const char *);
    void setAppEui(const char *);
    void setAppKey(const char *);
    void Lora_Band_Config(void);
    void GetAppKey(void);
    void GetDevEui(void);
    void GetAppEui(void);

    // Constants
    const char* LORA_AppKey  = "B1:6B:6F:08:5C:88:78:67:0C:7B:A4:22:37:D5:9C:AC";    // OTAA
    const char* LORA_NwkSKey = "84:46:98:82:E0:C7:6A:EE:62:FE:24:DA:D9:88:4D:D3";    // ABP
    const char* LORA_AppSKey = "A6:74:3A:AD:5E:82:E7:10:A5:E7:22:59:96:18:82:D9";    // ABP
    const char* LORA_DevEui  = "70:B3:D5:7E:D0:04:5B:4F";                            // OTAA
    const char* LORA_AppEui  = "0A:10:20:30:40:50:60:78";                            // OTAA
    const char* LORA_ID      = "26:0C:40:61";                                        // ABP
};

extern LoRa Lo;

#endif

