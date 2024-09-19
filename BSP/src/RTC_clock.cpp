/*
KONTROLOG-MINI LIBRARIES FOR ARDUINO
By:  Manuel Sandoval, September 2024,  Omicron IoT Solutions
	

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

#include "RTC_clock.h"


ESP32Time rtc(Ko.RTC.time_zone*1000);       // Create a RTC struct from  <ESP32Time.h>


void RTC_clock_class::READ_TIME (void)      // Read the Current RTC time
{  
    Ko.RTC.timeinfo=rtc.getTimeStruct();
    Ko.RTC.time_stamp=rtc.getEpoch();
};                                                     

void RTC_clock_class::SET_TIME (uint32_t TimeStamp)         // Set the RTC new Epoch
{
    rtc.setTime(TimeStamp+Ko.RTC.time_zone*3600);
    Ko.RTC.timeinfo=rtc.getTimeStruct();
}

void RTC_clock_class::SET_TIME (int sec, int min, int hours, int day ,int month ,int year)          // Set the RTC with date
{
    rtc.setTime(sec,min,hours,day,month,year);
    Ko.RTC.time_stamp=rtc.getEpoch();

}

// Get time from Ubidots Server
// Also the User can read the data time from NPT Server
void RTC_clock_class::GET_TIME_FROM_SERVER(void)
{
    uint32_t newepoch=Ubi.get_utc();
    if(newepoch>0){
        SET_TIME(newepoch);
        READ_TIME();
        }
}