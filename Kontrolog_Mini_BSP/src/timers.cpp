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

#include "timers.h"

hw_timer_t *Timer0_Cfg = NULL;                           // Create a new timer 0,  ESP32
 
// ----------------------------------------------------------------------------------------------------
void IRAM_ATTR Timer0_ISR(void)                          // Timer 0 Interrup, every 10 mS
// ----------------------------------------------------------------------------------------------------
{
    if(Ko.TIMERS.delay>0)                                // Auxiliar for generate Timer Delays in 10mS Intervals
        Ko.TIMERS.delay--;

// --------------------------------------                
// Timers every 100m Seconds
// ------------ -------------------------- 
        if(++Ko.TIMERS.timer_100ms==10)
            {
            Ko.TIMERS.timer_100ms=0;
            Outputs.PID_ACTIVATION_TIMER();             // Calls funtion to activate Outpus In PID Control Mode Every 100mS
            }
// --------------------------------------                
// Timers of 1 Second
// -------------------------------------- 

            if (++Ko.TIMERS.timer_s==100)                 //  Counter in Seconds
                {
                Ko.TIMERS.timer_s=0;


// Timers for Alarm delays
                
                if (Ko.TIMERS.timer_Sil_IN1>0)                // Timer in seconds for alarm delay in sensor 1
                    Ko.TIMERS.timer_Sil_IN1--;
                    
                if (Ko.TIMERS.timer_Sil_IN2>0)                //  Timer in seconds for alarm delay in sensor 
                    Ko.TIMERS.timer_Sil_IN2--;
                
                if (Ko.TIMERS.timer_Sil_VDC>0)               //  Timer in seconds for alarm delay in DC Voltage sensor
                    Ko.TIMERS.timer_Sil_VDC--;    

//  Timer Delay for activation of the Outpus 1 and Output 2
                if (Ko.OUT1. start_delay>0)
                    Ko.OUT1. start_delay--;
                if (Ko.OUT2. start_delay>0)
                    Ko.OUT2. start_delay--;

//  Auxiliar Timers for Outputs
                if(Ko.OUT1.timer>0)
                    Ko.OUT1.timer--;
                if(Ko.OUT2.timer>0)
                    Ko.OUT2.timer--;                
                  

                
// -------------------------------------- 
// Timers of Minuts
// --------------------------------------            
               if (++Ko.TIMERS.seconds==60)     
                    {
                    Ko.TIMERS.seconds=0;
                    

// -------------------------------------- 
// Timersof hours
// -------------------------------------- 
                    
                    if (++Ko.TIMERS.minutes==60)                   
                        {
                        Ko.TIMERS.timer_reset_com_module++;         // This timer allows the KONTROLOG to restart if there are communication failures.
                        Ko.TIMERS.minutes=0;
                        }
                    }

                }
          
}
// ----------------------------------------------------------------------------------------------------

void Timers::INIT_TIMER(void)                           // Activate ESP32 Timer 0 Ever 10mS
{
    Timer0_Cfg = timerBegin(0, 80, true);               
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, 10000, true);
    timerAlarmEnable(Timer0_Cfg);  
}

// Create a Delay timer in 10mS
void Timers::DELAY_TMR (int delay_ms) {
    Ko.TIMERS.delay=delay_ms/10;
    while(Ko.TIMERS.delay>0)            // Wait for the end of the time
        {
        delay(1);    
        }

}

// User can use the method to put ESP32 in Sleep Mode
void Timers::SLEEP(int Sleep_Time_ms){
    esp_sleep_enable_timer_wakeup(Sleep_Time_ms*1000);
}

