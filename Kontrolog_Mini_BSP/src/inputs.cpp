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

#include "inputs.h"
#include "TLA2528_C.h"

TLA2528Class TLA2528;                                   // Create Class for A/D Converter 

int Inputs_Class::DIGITAL_INPUT_R (unsigned char CHN) {        // Digital input: Dry contact.          
// In this type of input, a contactor is placed at the input, then, if the contactor is: opened=0 , closed=1.

    if (READ_A.TEN_READINGS_SUM[CHN]>20480)  
        return 0;                           // If the voltage is greater than the half of the scale (1.65V), it indicates that the contactor is open.
    else
        return 1;                           // In the contrary case, the contactor is closed.
}

short Inputs_Class::i4to20mA_INPUT_R (unsigned char CHN) {     // Analog input read: 4-20 mA.
/* For calculating the 4..20mA current:
    1) The voltage read at the Input is calculated with the next formula:  Readinf of 10 samples of the A/D channel/10  * 3.3V /4096
    2) Because the sensor is pre-polarized (internally) with a 10K resistor, From this reading 0.0488V is subtracted, which is caused by the current flowing from VCC through the 10K 
       Pull-UP resistor and the 150 Ohms resistor.
    3) The value is divided by the 4-20mA conversion resistor (150 Ohms or divide 0.150 for mA).  
    4) The result is Multiplied by 100.0 (The result is scaled by 100)
*/
    float I_CAL;
    
    I_CAL= (float) READ_A.TEN_READINGS_SUM[CHN]/10.0;
    I_CAL= (I_CAL*3.3)/4096.0;                        
    I_CAL= 100.0*(I_CAL - 0.0488)/0.150;                
    
    if(I_CAL>2047)              // Limit to 20.47mA
        I_CAL=2047;
    else if(I_CAL<0)
        I_CAL=0;
    
    return  (int) (I_CAL + 0.5);  // 0.5 is added for rounding purposes
}

short Inputs_Class::V0to10V_INPUT_R (unsigned char CHN) {      // Analog input read: 0-10 V.
/*
* Get DC voltage 0 to 10 VDC
 * Based on an external 2.2K pin-to-Ground voltage divider and a series resistor with the voltage of 10K
 * Because the sensor is pre-polarized (internally) with a 10K resistor, and having a 2.2K base resistance
 * The conversion formula = VB= 6.545 VN - 3.3
 * The result is Multiplied by 100.0 (the result is scaled by 100)
  */
    float V_CAL;
    
    V_CAL= (float) READ_A.TEN_READINGS_SUM[CHN]/10.0;
    V_CAL= (V_CAL*3.3)/4096.0;            // Calculate the reading voltage
    V_CAL= 6.545*V_CAL-3.3;
    return (int) (100.0*V_CAL + 0.5);     // 0.5 is added for rounding purposes
}


short Inputs_Class::NTC_TEMP_INPUT_R (unsigned char CHN)
{
// Get the temperature from  thermistor NTC  10K,   The NTC is polarized by Internal 10K resistor
// 1)  Find the interval between which the sum of the last 10 readings of the A/D converter is in the Thermistor 10K (NTC)  data table
// 2)  Each Interval in NTC table is of 5 °C
// 3)  The result is the linear regression of the data in this interval and It is Scaled by 10

unsigned short DELTAi;
long int T_CAL;
unsigned char Vi=0;
char s;

	if (READ_A.TEN_READINGS_SUM[CHN] >TEMP_TABLE[0])    // If the value is bigger than Table[0] (begin of table), Temperarature < 50°C
		{T_CAL=-501;}                                   // Return -50.1°C
	else if (READ_A.TEN_READINGS_SUM[CHN] <= TEMP_TABLE[34])   // If the value is Lower  than Table[34] (end of table), Temperarature > 120°C
		{T_CAL=1201;}                                   // Return 120.1°C
	
    else                                                // Now Find the interval of 10 radings of A/D Data converted in the table   
		{     
            if  (READ_A.TEN_READINGS_SUM[CHN]>=TEMP_TABLE[10])      
                if  (READ_A.TEN_READINGS_SUM[CHN]>=TEMP_TABLE[5])
                    Vi=0;                               // Pre-Interval 0
                else
                    Vi=5;                               
            else if  (READ_A.TEN_READINGS_SUM[CHN]>=TEMP_TABLE[20])
                if  (READ_A.TEN_READINGS_SUM[CHN]>=TEMP_TABLE[15])    
                    Vi=10;                             
                else
                    Vi=15;                             
            else if  (READ_A.TEN_READINGS_SUM[CHN]>=TEMP_TABLE[30])
                if  (READ_A.TEN_READINGS_SUM[CHN]>=TEMP_TABLE[25])    
                    Vi=20;  
                else
                    Vi=25;
            else
                Vi=30;
                

		while (READ_A.TEN_READINGS_SUM[CHN]<=TEMP_TABLE[Vi+1])          // Find the exactly interval
			Vi++;                                                  

		// Calculate the temperature by Linear Regression
        DELTAi = TEMP_TABLE[Vi]-TEMP_TABLE[Vi+1];                       // Deta of readings
		T_CAL=  (TEMP_TABLE[Vi]-READ_A.TEN_READINGS_SUM[CHN]);
        T_CAL*=50;                                                      // Escaled by 5 (interval of table)  and by 10 for 0.1°C of Resolution
        
		if ((T_CAL%DELTAi)>(DELTAi/2))                                  // Calculate if the reading Division Module is Bigger thant the half of Inteval for Better Aproximation
			s=1;                                                        // If yes sum 1
		else
			s=0;
		T_CAL=  T_CAL/(DELTAi)+s;                                       // Linear calculation into the interval

       
        T_CAL = T_CAL+50*(Vi-10);                                       // Sums the temperature base of the interval  (Multiplied by 50 = 5°C scaled by 10,  and Vi-10 is because the table Stars in -50°C)
        }
return T_CAL;
}


void Inputs_Class::SUPPLY_VDC_READER(void) 
// The resistors used in the voltage divider to read the 12V-24V Supply Voltage are 10K and 1.5K
// The voltage read is calculated in  volts (Scaled by 10) as follows:
// CAL_V= SUM_10_READINGS /10 * (3.3/4096) * 11.5 /(1.5) + 1.4
// SUM_1_ READINGS is already is divided by 10
// 3.3/4096: ADC conversion factor to real voltage since external reference voltage is used 3.3 and the conversor is 12 bit 2^12=4096
// 11.5 and 1.5: Values ​​given by the voltage divider resistors.
// Add 1.4 Volts is the voltage drop across the diode bridge if the power is from an external 12V or 24V source
// If the Supply is internal (By USB Cable)  don´t rest the 1.4V
//--------------------------
{
float V_CAL;

if(Ko.VDC.enable==1)
    {
    V_CAL = (READ_A.TEN_READINGS_SUM[2])/10;                            // Divided by 10
    V_CAL=(V_CAL*11.5*3.3)/(1.5*4096.0);                                // Resistor divisor are:  10K and 1.5K = 11.5 /1.5
    V_CAL=V_CAL*10;                                                     // Scaled by 10 (0.1V of resolution)
    Ko.VDC.value = (int) (V_CAL+0.5);                                   // 0.5 is added for rounding purposes
    }
else
    Ko.VDC.value=0;
    
    Ko.VDC.resolution=1;

    if(Ko.VDC.value>=60)                                                 // Is the Voltage supply from Internal USB or by External Power ?
        {
        Ko.VDC.value+=14;                                                // if yes Add 1.4 V to compensate the voltage drop across the internal diode bridge 
        if(Ko.VDC.value<0)
            Ko.VDC.value=0;
        }
     else if (Ko.VDC.value<0)                                            // Is the voltage negative?
        Ko.VDC.value=0;
     
     else;                                                               // In this case the Votage is Supply from USB  aprox 5.0V

}

//------------------------------------------------------------------------------
//  Init the A/D channels
//------------------------------------------------------------------------------

void Inputs_Class::INPUTS_INI(void)
{
    TLA2528.begin();
    uint8_t i;

        Jx1=0;                                              // Init stack index
        for (i = 0; i<10;i++)
            {
             READ_ANALOG_AND_STORING ();                    // Store the firt 10 readings of A/D conversor for each channel
            }
}

// Read A/D channels and stored them in the stack of each one
void Inputs_Class::READ_ANALOG_AND_STORING(void)
{
    uint8_t j,k;
    uint16_t SUMMATORY;
    READ_A.CHN_DATA[0][Jx1]=TLA2528.analogReadIO(AN1);
    READ_A.CHN_DATA[1][Jx1]=TLA2528.analogReadIO(AN2);
    READ_A.CHN_DATA[2][Jx1]=TLA2528.analogReadIO(ANVDC);
       
    if (++Jx1==10)                                                  // Increases the stack pointer
        Jx1=0;

    for (k=0;k<3;k++)                                               // Add the value of the last 10 readings for each A/D channel            
        {
        SUMMATORY=0;
        for (j=0;j<10;j++)
            SUMMATORY= SUMMATORY+READ_A.CHN_DATA[k][j];

        READ_A.TEN_READINGS_SUM[k]=SUMMATORY;                       //Store the result of the sum of the 10 A/D readings for each channel
        }

}