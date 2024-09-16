/*
KONTROLOG-MINI LIBRARIES FOR ARDUINO
By:  Manuel Sandoval, July 2024,  Omicron IoT Solutions
	

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

#ifndef RTC_clock_h
#define	RTC_clock_h

#include <Arduino.h>
#include <stdlib.h>
#include "config.h"
#include <ESP32Time.h>
#include "wifi_esp.h"


class RTC_clock_class {

public:

    void SET_TIME (int sec, int min, int hours, int day ,int month ,int year);    // Set the RTC  with date
    void SET_TIME (uint32_t TimeStamp);                                           // Adjusts the RTC with the current timestamp (local time)
    void READ_TIME (void);                                                        // Read the RTC time and timestamp
    tm GET_NEW_TIME_STRUCT(uint32_t NewTimeStamp);                                    // Get a new time struct
    void GET_TIME_FROM_SERVER(void);
private:

};

extern RTC_clock_class RTC;

#endif






