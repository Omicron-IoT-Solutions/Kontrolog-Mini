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

#ifndef sigfox_h
#define sigfox_h

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "config.h"
#include "eeprom_esp.h"
#include "communications.h"
#include "timers.h"

// Constants
#define RCZ1 1
#define RCZ2 2
#define RCZ3 3
#define RCZ4 4

class Sigfox_class{

public:
    // Functions
    int initSigfox();
    int GetDeviceID(void);
    int Deep_Sleep_Sigfox(void);
    int Active_Modulo_Sigfox();
    int testComms_Module();
    int sendPayload(unsigned char *, const unsigned char , const int);
    void rx_data_Sigfox(void);

private:
    // Functions
    int Detect_if_Module_Sigfox(void);
    int getZone(void);
    int SendDataSigfox(unsigned char *, const unsigned char, const int);
    int prepareZone(char);
    int checkChangeZone(char);
    int GetDevicePAC(void);
    void Prepare_Sending();
};
    extern Sigfox_class Si;
#endif
