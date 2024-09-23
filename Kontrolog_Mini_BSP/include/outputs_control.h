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

#ifndef outputs_control_h
#define	outputs_control_h

#include <Arduino.h>
#include <math.h>
#include "inputs.h"
#include "config.h"
#include "eeprom_esp.h"
#include "TLA2528_C.h"


class Outputs_class {

public:
    // Functions
    void ACTIVATION_TIMER_PID(void);
    void Set_Ouputs(void);

private:
    // Functions
    int GET_VALOR_ACTUAL(uint8_t);
    uint8_t INPUT_FOLLOWER(uint8_t);
    uint8_t CONTROL_ON_OFF(OUT_t);
    uint8_t COUNTER(OUT_t);
    uint8_t CONTROL_TEMPORIZADOR1(OUT_t);
    void CONTROL_PID(OUT_t);
    int SET_POWER (short POT, OUT_t);
    int DERIVATE_ACTION(int, OUT_t);

    // Internal enum
    enum{
        start,
        heating,
        pid_phase,
        Cooling
    };

};
extern Outputs_class Outputs; 

#endif