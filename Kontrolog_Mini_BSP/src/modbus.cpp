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

#include "modbus.h"


//----------------------------------------
// Modbus communication funtions

MODBUS_t ModbusClass::Modbus_Telegram(MODBUS_t MOD)
 {
int i,j; 

enum RS458_Mode{
    Rx=0,
    Tx,
};

char BufferRx[40];


    au8Buffer[0]= MOD.slave_address;
    au8Buffer[1]= MOD.funtion_code;
    au8Buffer[2]= MOD.start_address>>8;
    au8Buffer[3]= MOD.start_address;
    au8Buffer[4]= MOD.coils_no>>8;
    au8Buffer[5]= MOD.coils_no;

    uint16_t u16crc = Modbus_calcCRC(6);
    au8Buffer[6] = u16crc >> 8;
    au8Buffer[7] = u16crc & 0x00ff;
    au8Buffer[8] = 0;
    j=0;

    while(j<1)                          // Put the number of tries to find the right answer
        {
        Tx_Rx_Modbus(Tx);               // Set Tx In RS-485 Chip
        Serial2.write(au8Buffer,8);     // Send Modbus Telegram
        Serial2.flush();                // Wait until end of transmission and Clear Rx Buffer
        delay(1);                       // Delay for change pin to Rx
        Tx_Rx_Modbus(Rx);               // Set Rx In RS-485 Chip
        delay(100);                     // Wait For Answer
        i=0;                            
        while(Serial2.available()>0)    // Read and Stored Modbus Answer
            {
            BufferRx[i]=Serial2.read();
            delay(10);
            i++;
            }
        int k=0;
        while((BufferRx[k]!=au8Buffer[0])&&k<50)
            k++;
        if((BufferRx[k]==au8Buffer[0])&&(BufferRx[k+1]==au8Buffer[1]))      // Find the ID and  funtion code in the answer
            {                                                               // Also user can check the Rx CRC for correct reading
            int qty = MOD.coils_no*2+3;
            for(i=k;i<(qty+k);i++)
                {
                MOD.registers[i-k]=BufferRx[i];
                }
                for (int x = 0; x < 40; x++)
                {
                    Ko.MODBUS.registers[x]=MOD.registers[x];
                }
            return MOD;
            }
        else 
            j++;
        }
    return MOD;
}    


void ModbusClass::Tx_Rx_Modbus (unsigned char status)
// Set Pin for put RS-485 Chip in:
// 0: Rx mode
// 1: Tx Mode
{
    if(status)
        digitalWrite(EN_RS485,HIGH);
    else
        digitalWrite(EN_RS485,LOW);
}

unsigned int ModbusClass::Modbus_calcCRC(unsigned char u8length)
{
    unsigned int temp, temp2, flag;
    temp = 0xFFFF;
    unsigned char i,j;
    for (i = 0; i < u8length; i++)
    {
        temp = temp ^ au8Buffer[i];
        for (j = 1; j <= 8; j++)
        {
            flag = temp & 0x0001;
            temp >>=1;
            if (flag)
                temp ^= 0xA001;
        }
    }
    // Reverse byte order.
    temp2 = temp >> 8;
    temp = (temp << 8) | temp2;
    temp &= 0xFFFF;
    // the returned value is already swapped
    // crcLo byte is first & crcHi byte is last
    return temp;
}