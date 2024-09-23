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

#ifndef inputs_h
#define	inputs_h

#include <Arduino.h>
#include <cmath>
#include "config.h"
#include "timers.h"

class Inputs_Class {

public:
     void INPUTS_INI (void);                  // Function for initializing inputs 
     void SUPPLY_VDC_READER(char ini);
   
    // Functions for input reading according to selected sensor type
     short NTC_TEMP_INPUT_R (unsigned char);
     int   DIGITAL_INPUT_R (unsigned char CHN);
     short i4to20mA_INPUT_R (unsigned char CHN);
     short V0to10V_INPUT_R (unsigned char CHN);
     void  READ_ANALOG_AND_STORING (void);         // Functions for read and storing Analog chanel data

private:



    // Struct for channel readings
    struct {
        unsigned short CHN_DATA[3][10];             // Stores 10 readings per channel IN1, IN2, VDC
        unsigned short TEN_READINGS_SUM[3];         // Sums 10 readings to average channel readings
    } READ_A;


    // NTC 10K temperature calculation table
    const unsigned short TEMP_TABLE[35] =     // NTC data Table of values to calculate the temperature of Thermistor NTC 10K. Step of 5 degree Celsius, range [-50 to 120 degree Celsius].
    {40358,40110,39778,39339,38769,38043,37135,36023,34690,33132,
    31356,29387,27261,25030,22750,20480,18276,16182,14234,12452,
    10847,9426,8166,7064,6109,5284,4571,3959,3435,2985,2598,2266,
    1981,1736,1525};

    // Auxiliary variables
    int Jx1,Kv;
    int Mx1=0;
};
    extern Inputs_Class In;
#endif