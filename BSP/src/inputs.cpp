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

version 1.0 June 2024
*/

#include "config.h"
#include "inputs.h"
#include "TLA2528_C.h"


TLA2528Class TLA2528;

int Inputs::PREV_SENS[5] = {0, 0, 0, 0, 0};


int Inputs::DIGITAL_INPUT_R (unsigned char CHN) {        // Digital input: Dry contact.          
// In this type of input, a contactor is placed at the input, then, if the contactor is: opened=0 , closed=1.

    if (READ_A.TEN_READINGS_SUM[CHN]>20480)  
        return 0;                           // If the voltage is greater than the half of the scale (1.65V), it indicates that the contactor is open.
    else
        return 1;                           // In the contrary case, the contactor is closed.
}

short Inputs::i4to20mA_INPUT_R (unsigned char CHN) {     // Analog input: 4-20 mA.
/* For calculating the current:
    1) The voltage read at the port is calculated: for this there are 10 samples of the channel * 3.3V / (4096 x 10)
    2) From this reading 0.0488V is subtracted, which is caused by the current flowing from VCC through the 10K 
       Pull-UP resistor and the 150 Ohms resistor.
    3) The value is divided by the 4-20mA conversion resistor (150 Ohms or divide 0.150 for mA).  
       (as the result is scaled by 100)
*/
    float I_CAL;
    
    I_CAL= (float) READ_A.TEN_READINGS_SUM[CHN];
    I_CAL= (I_CAL*3.3)/40950.0;                        
    I_CAL= 100.0*(I_CAL - 0.0488)/0.150;                
    if(I_CAL>2047)
        I_CAL=2047;
    else if(I_CAL<0)
        I_CAL=0;
    
    return  (int) (I_CAL + 0.5);  // 0.5 is added for rounding purposes
}

short Inputs::V0to10V_INPUT_R (unsigned char CHN) {      // Analog input: 0-10 V.
/*
 * Obtener voltaje DC  0 a 10 VDC
 * Basado en un Divisor de tensi?n externo de 2.2K  de pin a Tierra y una resistencia en Serie con el voltaje de 10K
 * Debido a que el sensor esta prepolarizado (Internamente) con una resistencia de 10K,  y al tener una resistencia de base de 2.2K
 * La f?rmula de conversi?n = VB= 6.545 VN - 3.3
  */
    float V_CAL;
    
    V_CAL= (float) READ_A.TEN_READINGS_SUM[CHN];
    V_CAL= (V_CAL*3.3)/40950.0;    // Se calcula el voltaje Leido en el pin
    V_CAL= 6.545*V_CAL-3.3;
    return (int) (100.0*V_CAL + 0.5);    // Se suman 0.5 para hacer redondeo
}


short Inputs::NTC_TEMP_INPUT_R (unsigned char CHN)
{
// Obtener temperatura para NTC  10K,   polarizada por resistencia de 10K
unsigned short DELTAi;
long int T_CAL;
unsigned char Vi=0;
char s;

	if (READ_A.TEN_READINGS_SUM[CHN] >TEMP_TABLE[0])
		{T_CAL=-501;}
	else if (READ_A.TEN_READINGS_SUM[CHN] <= TEMP_TABLE[34])
		{T_CAL=1201;}
	else
		{
            if  (READ_A.TEN_READINGS_SUM[CHN]>=TEMP_TABLE[10])
                if  (READ_A.TEN_READINGS_SUM[CHN]>=TEMP_TABLE[5])
                    Vi=0;
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
                

		while (READ_A.TEN_READINGS_SUM[CHN]<=TEMP_TABLE[Vi+1])
			Vi++;

		DELTAi = TEMP_TABLE[Vi]-TEMP_TABLE[Vi+1];
		T_CAL=  (TEMP_TABLE[Vi]-READ_A.TEN_READINGS_SUM[CHN]);
        T_CAL*=50;
        
		if ((T_CAL%DELTAi)>(DELTAi/2))
			s=1;
		else
			s=0;
		T_CAL=  T_CAL/(DELTAi)+s;

        T_CAL = T_CAL+50*(Vi-10);
        }
return T_CAL;
}


void Inputs::SUPPLY_VDC_READER(char ini) 
// Las resistencias utilizadas en el divisor de voltaje para leer el Voltaje de Alimentación de 12V son 10K y 1.5K
// El voltaje leido  se calcula en milivoltios como sigue:
// CAL_V= SUMA10LECTURAS/10 * (3.3/4096) *  11.5  /(1.5) +1.4
// SUMA10LECTURAS ya esta escalado por 10,  por eso se divide por 10
// 3.3/4096:  Factor de conversion del ADC a voltaje real ya que se usa voltaje de referencia externo 3.3/4096 
// 11.5 Y 1.5 : Valores dados por las resistencias divisoras de tension.
// Sumar 1.4 Voltios de caida de tensión en el puete de diodos si la alimentación es una fuente externa
//--------------------------
{
float V_CAL;

if(Ko.VDC.enable==1)
    {
    V_CAL = (READ_A.TEN_READINGS_SUM[2])/10;                            // Convierte el valor le?do en mV * 10  ya que 
    V_CAL=(V_CAL*11.5*3.3)/(1.5*4096);                                  // Factor del divisor de resistencias DE 10K Y 1.5K = 11.5 /1.5
    V_CAL=V_CAL*10;                                                     // Se escala por 10
    Ko.VDC.value = (int) (V_CAL+0.5);                                   // Se suman 0.5 para hacer redondeo
    }
else
    Ko.VDC.value=0;
    
    Ko.VDC.resolution=1;

    if(Ko.VDC.value>=52)                                                 // Voltage supply from External Power
        {
        Ko.VDC.value+=14;                                                // Add 1.4 V to compensate the voltage drop across the diode bridge 
        if(Ko.VDC.value<0)
            Ko.VDC.value=0;
        }
     else if (Ko.VDC.value<0)                                            // Is the voltage negative
        Ko.VDC.value=0;
     
     else;                                                               // Supply from USB  5.0V,  USB

}

//------------------------------------------------------------------------------
//  Inicio de la pila para c?lculo de Temperatura
//------------------------------------------------------------------------------

void Inputs::INPUTS_INI(void)
{
    TLA2528.begin();
    uint8_t i;

        Jx1=0;                                   // Se inician los apuntadores de pila
        for (i = 0; i<10;i++)
            {
             READ_ANALOG_AND_STORING ();                     // Se enciende conversor ADC y se lee 10 veces cada canal
            }
}

void Inputs::READ_ANALOG_AND_STORING(void)
{
    uint8_t j,k;
    uint16_t SUMMATORY;
    READ_A.CHN_DATA[0][Jx1]=TLA2528.analogReadIO(AN1);
    READ_A.CHN_DATA[1][Jx1]=TLA2528.analogReadIO(AN2);
    READ_A.CHN_DATA[2][Jx1]=TLA2528.analogReadIO(ANVDC);
       
    if (++Jx1==10)                                          // Se incrementa apuntador general de pila
        Jx1=0;

    for (k=0;k<3;k++)
        {
        SUMMATORY=0;
        for (j=0;j<10;j++)
            SUMMATORY= SUMMATORY+READ_A.CHN_DATA[k][j];

        READ_A.TEN_READINGS_SUM[k]=SUMMATORY;                    // Se saca promedio y almacena
        }

}