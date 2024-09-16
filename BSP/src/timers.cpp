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

#include "timers.h"

hw_timer_t *Timer0_Cfg = NULL;
 

void IRAM_ATTR Timer0_ISR(void)                          // Interrupt every 10 mS
{
    if (Ko.TIMERS.T_TASK>0)
        Ko.TIMERS.T_TASK--;

    if(Ko.TIMERS.delay>0)                                // Timer delays
        Ko.TIMERS.delay--;
    
            
// --------------------------------------                
// Timers de 1 segundo
// -------------------------------------- 

            if (++Ko.TIMERS.timer_s==100)                 //  Contador a un segundo
                {
                Ko.TIMERS.timer_s=0;

// Temporizadores de Salidas
                if(Ko.OUT1.timer>0)
                    Ko.OUT1.timer--;
                if(Ko.OUT2.timer>0)
                    Ko.OUT2.timer--;

//  Temporizador para eliminar clave de acceso a configuración temporal
                if (Ko.TIMERS.timer_password>0)
                    if(--Ko.TIMERS.timer_password==0)
                        Ko.CONFIG.DT.password=0;

// --- Retardos para alarmas
                
                if (Ko.TIMERS.timer_Sil_IN1>0)                // Tiempo de retardar alarma sensor 1
                    Ko.TIMERS.timer_Sil_IN1--;
                    
                if (Ko.TIMERS.timer_Sil_IN2>0)                // Tiempo de retardar alarma sensor 1
                    Ko.TIMERS.timer_Sil_IN2--;
                
                if (Ko.TIMERS.timer_Sil_VDC>0)               // Tiempo de retardar alarma sensor Batería
                    Ko.TIMERS.timer_Sil_VDC--;    
                
                  
//  Temporizadores de Arranque para control de salidas
                if (Ko.OUT1. start_delay>0)
                    Ko.OUT1. start_delay--;
                if (Ko.OUT2. start_delay>0)
                    Ko.OUT2. start_delay--;
                
// -------------------------------------- 
// Temporizadores de minutos 
// --------------------------------------            
               if (++Ko.TIMERS.seconds==60)                  // Temporizadores de minuto
                    {
                    Ko.TIMERS.seconds=0;
                    

//----------Temporizadores de una hora
                    
                    if (++Ko.TIMERS.minutes==60)                   
                        {
                        Ko.TIMERS.timer_reset_com_error++;          // Este timer permite reiniciar el equipo si hay fallas en la comunicación.
                        Ko.TIMERS.minutes=0;
                        }
                    }

                }
          
}

void Timers::DELAY_TMR (int delay_ms) {
    Ko.TIMERS.delay=delay_ms/10;
    while(Ko.TIMERS.delay>0)            // Wait for the end of the time
        {
        delay(1);    
        }

}

void Timers::SLEEP(int Sleep_Time_ms){
    esp_sleep_enable_timer_wakeup(Sleep_Time_ms*1000);
}
void Timers::INIT_TIMER(void)
{
    Timer0_Cfg = timerBegin(0, 80, true);
    timerAttachInterrupt(Timer0_Cfg, &Timer0_ISR, true);
    timerAlarmWrite(Timer0_Cfg, 10000, true);
    timerAlarmEnable(Timer0_Cfg);  
}