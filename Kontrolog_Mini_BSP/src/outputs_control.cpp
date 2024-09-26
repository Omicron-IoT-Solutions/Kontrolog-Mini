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

#include "outputs_control.h"

extern TLA2528Class TLA2528;

/* Output Control Functions types control_functions:
    0: outoff
    1: remote_commands
    2: input_follower
    3: on_off
    4: timer
    5: remote_alarm
    6: event_counter
    7: PID
*/


// -------------------------------
// Auxiliars
void Outputs_class::Set_Ouputs(void)
{
    TLA2528.digitalWriteIO(REL1,Ko.OUT1.value);
    TLA2528.digitalWriteIO(REL2,Ko.OUT2.value);
}

uint8_t Outputs_class::INPUT_FOLLOWER(uint8_t control_input)
{
    uint8_t SAL=0;

        if ((control_input==1)&&(Ko.IN1.type==digital))
            SAL=Ko.IN1.value;
        else if ((control_input==2)&&(Ko.IN2.type==digital))
            SAL=Ko.IN2.value;
        else
           SAL=0;
        
    return SAL;
}

int Outputs_class::Get_Procces_Value(uint8_t variable)
{
    int Value=0,POW=1;
    if(variable==1) {
        Value=Ko.IN1.value;
        POW=Ko.IN1.resolution;
        }
    else if(variable==2) {
        Value=Ko.IN2.value;
        POW=Ko.IN2.resolution;
        }
    else
        Value=0;
    
    if(POW==0)
        return (Value*=10);
    else if(POW==2)
        {
        if(Value%10>5)
            Value=Value/10+1;
        else
            Value=Value/10;
        return Value;
        }
    else
        return Value;
}

uint8_t Outputs_class::CONTROL_ON_OFF(OUT_t OUT_ON_OFF)
{
/* Used registers for ON_OFF Control  
    1: value;
    2: control_funtion;
    3: control_signal;
    4: set_point;
    5: hysteresis;
    6: control_type;
    15: start_delay;
    */

   int CURRENT_VALUE=Get_Procces_Value(OUT_ON_OFF.control_signal);
   bool OUT=0;

    if(OUT_ON_OFF.start_delay==0)
        {   
        if(OUT_ON_OFF.control_signal!=0)                 // There is a Control signal Assigned?
            {
            if (OUT_ON_OFF.control_type==0)              // Cooling
                {
                if (CURRENT_VALUE<=(OUT_ON_OFF.set_point-OUT_ON_OFF.hysteresis))
                    OUT=0;
                else if (CURRENT_VALUE>=(OUT_ON_OFF.set_point+OUT_ON_OFF.hysteresis))
                    OUT=1;
                else
                    OUT=OUT_ON_OFF.value;
                }

            else                                        // Heating
                {
                if (CURRENT_VALUE>=(OUT_ON_OFF.set_point+OUT_ON_OFF.hysteresis))
                    OUT=0;
                else if (CURRENT_VALUE<=(OUT_ON_OFF.set_point-OUT_ON_OFF.hysteresis))
                    OUT=1;
                else
                    OUT=OUT_ON_OFF.value;
                }
            }
        }
    else
        OUT=0;
    
    return OUT;     
}

uint8_t Outputs_class::COUNTER_CONTROL(OUT_t OUTPUT1)
{
/* Used register for Counter Output Control
    1: value;
    2: control_funtion;
    3: control_signal;
    4: set_poin;
    5: hysteresis;
    6: control_type;
    */
    
    int CURRENT_VALUE=Get_Procces_Value(OUTPUT1.control_signal);
    uint8_t OUT=0;
    
    if(OUTPUT1.control_signal!=0)                // if control signal assigned?
        {
        if (OUTPUT1.control_type==0)             // Type 0:  if Pulses < Set-Point ==> Output = OFF
            {
            if (CURRENT_VALUE<OUTPUT1.set_point)
                OUT=0;
            else
                OUT=1;
            }

        else                                      // Type 1:  if Pulses > Set-Point ==> Output = ON
            { 
            if (CURRENT_VALUE>=OUTPUT1.set_point)
                OUT=0;
            else
                OUT=1;
            }
        }
    return OUT;     
}

OUT_t Outputs_class::TIMER_CONTROL(OUT_t OUT_TIMER) {
/* The timer can operate in three ways
1) With a Control Signal Associated with a single trip, in this case only Set_Time_ON>0 is programmed, and Set_Time_OFF=0
2) With a Control Signal Associated cyclically, in this case both Set_Time_ON>0 and Set_Time_OFF > 0 
3) Without an Associated Control Signal: In this case it only works cyclically and both Set_Time_ON>0 and Set_Time_OFF > 0 

Used registers for Timer Output  
    1: value;
    2: control_funtion;
    3: control_signal;
    4: set_point;
    5: hysteresis;
    6: control_type;
    7: Set_Time_ON;
    8: Set_Time_OFF;
    13 Start Delay
    14: Auxilar Timer
    15: Phase
 */

    int i;
    
    int VALOR_ACTUAL=Get_Procces_Value(OUT_TIMER.control_signal);
     
    if(OUT_TIMER.start_delay==0) {   
        if(OUT_TIMER.control_signal!=0) {                // if control signal  != 0
            if (((OUT_TIMER.control_type==0)&&(VALOR_ACTUAL>= OUT_TIMER.set_point))|((OUT_TIMER.control_type==1)&&(VALOR_ACTUAL<= OUT_TIMER.set_point))) {
                if (OUT_TIMER.phase==0) {
                    OUT_TIMER.timer=OUT_TIMER.set_time_ON;
                    OUT_TIMER.phase=1;
                    OUT_TIMER.value=1;
                } else if (OUT_TIMER.phase==1) {
                    if(OUT_TIMER.timer==0) {                // End of Timer?
                        if(OUT_TIMER.set_time_OFF>0) {
                            OUT_TIMER.timer=OUT_TIMER.set_time_OFF;
                            OUT_TIMER.phase=2;
                        }
                        OUT_TIMER.value=0;
                    }
                } else {                                    // Phase 2
                    if(OUT_TIMER.timer==0) {                // End of timer?
                        OUT_TIMER.timer=OUT_TIMER.set_time_ON;
                        OUT_TIMER.phase=1;
                        OUT_TIMER.value=1;
                    }                 
                }
            } else {
                OUT_TIMER.timer=0;
                OUT_TIMER.phase=0;
                OUT_TIMER.value=0;
            }
        } else {                                            // if control signal  = 0;  free timer
            if((OUT_TIMER.set_time_OFF>0)&&(OUT_TIMER.set_time_ON>0)) {
                if (OUT_TIMER.phase==0) {
                    OUT_TIMER.timer=OUT_TIMER.set_time_ON;
                    OUT_TIMER.phase=1;
                    OUT_TIMER.value=1;
                } else if (OUT_TIMER.phase==1) {
                    if(OUT_TIMER.timer==0) {                // End of the Timer?
                        OUT_TIMER.timer=OUT_TIMER.set_time_OFF;
                        OUT_TIMER.phase=2;
                        OUT_TIMER.value=0;
                    }
                } else {                                    // Phase 2
                    if(OUT_TIMER.timer==0) {                // End of Timer?
                        OUT_TIMER.timer=OUT_TIMER.set_time_ON;
                        OUT_TIMER.phase=1;
                        OUT_TIMER.value=1;
                    }                       
                }
            } else {
                OUT_TIMER.timer=0;
                OUT_TIMER.phase=0;
                OUT_TIMER.value=0;
            }
        }
    } else {
        OUT_TIMER.value=0;
        OUT_TIMER.phase=0;
        OUT_TIMER.timer=0;
    }

    return OUT_TIMER;
 
}

OUT_t Outputs_class::PID_CONTROL(OUT_t OUT_PID) {
    /* PID control 
     * Performs PID control algorithm on the output given a current value of variable, Set-point, and others PID control parameters

    Use Registers fo PID Controll
        1: value;
        2: control_funtion;
        3: control_signal;
        4: set_point;
        15: PID Control Phase
        16: Proportional Action
        17: Integral Action
        18: Derivative Action
        19: PID Timer
     */

    int i;
    int P_CAL = 0;
   
    if (OUT_PID.control_signal != 0) {
        int CURRENT_VALUE = Get_Procces_Value(OUT_PID.control_signal);

        if (++OUT_PID.timer_PID >= OUT_PID.set_time_OFF) {  // Calculate every PID Calculated time
            OUT_PID.timer_PID = 0;

            // Error= Setpoint - Current Value
            OUT_PID.error = OUT_PID.set_point - CURRENT_VALUE;

            switch (OUT_PID.phase) {
                
                case start: 
                    OUT_PID.PB = OUT_PID.Kp * OUT_PID.set_point / 100; // Calculate the Proportional Band 
                    OUT_PID.max_int = (long)OUT_PID.PB * (long)OUT_PID.Pmax;
                    OUT_PID.max_int = 10 * OUT_PID.max_int / (long)OUT_PID.Int;  // Calculate the maximal Integral Action
                    OUT_PID.error_sum = 0;
                    OUT_PID.prev_value = CURRENT_VALUE;
                    for (unsigned char i = 0; i < 4; i++) // Init the derivate Action
                        OUT_PID.TD[i] = 0;

                    OUT_PID.power = SET_POWER(0,OUT_PID);   // Set power 0%
                   
                   if (OUT_PID.set_time_ON > 60)            // if Cycling Time > 60 Seconds
                        OUT_PID.set_time_ON = 10;           // Set cycling time = 10 Seconds
                    if (OUT_PID.set_time_OFF > 10)          // If Calculate time >10 Seconds
                        OUT_PID.set_time_OFF = 2;           // calculate time =2 Seconds
                    if (OUT_PID.start_delay == 0)
                        OUT_PID.phase = heating;
                    break;

                case heating: //Start and stabilization until 50% of PB
                    if (OUT_PID.error <= -10) {
                        OUT_PID.power = SET_POWER(0,OUT_PID);
                        OUT_PID.phase = cooling;
                    } else if (OUT_PID.error <= (OUT_PID.PB / 2)) {
                        OUT_PID.phase = pid_phase; // End of Heating, go to Control PID Phase
                    } else {
                        OUT_PID.power = SET_POWER(100,OUT_PID);
                    }
                    break;

                case pid_phase: // Control PID Phase
                    if (OUT_PID.error <= -10) {
                        OUT_PID.power = SET_POWER(0,OUT_PID);
                        OUT_PID.phase = cooling;
                    } else {
                        if (OUT_PID.Kp > 0) {
                            OUT_PID.error_sum += OUT_PID.error;
                            if (OUT_PID.error_sum > 0) { // the sum of the error should not be greater than the maximum value admit by the integral action
                                if (OUT_PID.error_sum > OUT_PID.max_int)
                                    OUT_PID.error_sum = OUT_PID.max_int;
                            } else {
                                if (OUT_PID.error_sum < -OUT_PID.max_int)
                                    OUT_PID.error_sum = -OUT_PID.max_int;
                            }

                            P_CAL = OUT_PID.error; // add the Proportional Action
                            P_CAL *= 100;

                            P_CAL = P_CAL + (OUT_PID.error_sum * (float)OUT_PID.Int / 10.0); // Add the Integral Action (Action Integral Is Scaled by 10)

                            P_CAL = P_CAL - DERIVATE_ACTION(CURRENT_VALUE,&OUT_PID.prev_value,OUT_PID.Der, OUT_PID.TD); // ACCION_DER() gets the weight of the Derivate action
                            P_CAL /= OUT_PID.PB; //  All is divide by the proportional Band
                            } 
                        else {                  // If Kp = 0 ==> CONTROL ON_OFF
                            if (OUT_PID.error >= 1)
                                P_CAL = 100;
                            else if (OUT_PID.error <= -1)
                                P_CAL = 0;
                        }

                        if (P_CAL > 100) // Power is normalized [0..100]%
                            P_CAL = 100;
                        if (P_CAL < 0)
                            P_CAL = 0;

                        P_CAL = P_CAL * OUT_PID.Pmax / 100; // Power is normalized from [0..Pmax]

                        OUT_PID.power = SET_POWER(P_CAL,OUT_PID); // Set the Calculate activation Power
                    }
                    break;

                case cooling:
                    if (OUT_PID.error >= 0)
                        OUT_PID.phase = pid_phase;

                    OUT_PID.power = SET_POWER(0,OUT_PID);
                    break;

                default:
                    break;
            }
        }
    } else {
        OUT_PID.power = SET_POWER(0,OUT_PID);
    }
    return OUT_PID;
}

int Outputs_class::DERIVATE_ACTION(int current_value, int16_t *prev_value,  int16_t der, int16_t *TD) 
{
// The derivative action generates a weight that depends on the historical variation of the last 4 errors calculated
// The greatest weight of the action is the most recent error calculate.
// Delta-T are stored in vector TD[]

    unsigned char d;
    int POW_DER;

    for (d = 3; d >= 1; d--) { // The vector of calculated error is updated
        TD[d] = TD[d - 1];
    }

    TD[0] = current_value - *prev_value; // The last differential read is saved
    *prev_value = current_value;

    // New Derivative action is calculated with diffent power factors
    POW_DER = (40 * TD[0] + 30 * TD[1] + 20 * TD[2] + 10 * TD[3]) * der / 10;   // Derivate Action is Scaled by 10

    return POW_DER;
}

int Outputs_class::SET_POWER(short POW, OUT_t OUT_PID) {
    if ((POW % 5) >= 3) {   // Power is divided by 5, to normalize 20 power steps so as not to activate the actuator for very short periods
        POW = POW / 5 + 1;  // This quantizes the values ​​to multiples of 5
    } else {
        POW = POW / 5;
    }

    POW *= 5;  // Then it is multiplied again by 5, intermediate values ​​of TICs are discarded

    int POWER = POW * OUT_PID.set_time_ON/10;   // POWER= P_CAL * TC * 10 /100;  Calculate the number of TICs (tenths of a second) that activates the output

    return POWER;
}

void Outputs_class::PID_ACTIVATION_TIMER() {
// This funtion is called by TMR 0 ISR,  every 100mS
    
    // OUTPUT1 PID
    if (Ko.OUT1.control_funtion == PID) {
        if (++Ko.OUT1.aux_timer_calc >= Ko.OUT1.set_time_ON * 10) {
            Ko.OUT1.aux_timer_calc = 0;
            Ko.OUT1.power_calc = Ko.OUT1.power;
        }
        // Control Cycle
        if ((Ko.OUT1.power_calc > 0) && (Ko.OUT1.aux_timer_calc <= Ko.OUT1.power_calc)) {
            Ko.OUT1.value = 1;
            Ko.OUT1.power_calc = Ko.OUT1.power;  // Only in this phase is the power updated
        } else {
            Ko.OUT1.value = 0;
        }
    }

    //OUTPUT 2 PID
    if (Ko.OUT2.control_funtion == PID) {
        if (++Ko.OUT2.aux_timer_calc >= Ko.OUT2.set_time_ON * 10) {
            Ko.OUT2.aux_timer_calc = 0;
            Ko.OUT2.power_calc = Ko.OUT2.power;
        }
        // Control Cycle
        if ((Ko.OUT2.power_calc > 0) && (Ko.OUT2.aux_timer_calc <= Ko.OUT2.power_calc)) {
            Ko.OUT2.value = 1;
            Ko.OUT2.power_calc = Ko.OUT2.power;  // Only in this phase is the power updated
        } else {
            Ko.OUT2.value = 0;
        }
    }
}
