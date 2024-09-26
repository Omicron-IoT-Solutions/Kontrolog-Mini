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

// BASED ON MODULE SJI LSM110A
// https://github.com/Support-SJI/LSM110A

#include "sigfox.h"

int Sigfox_class::initSigfox() {
/*
	- Output: return 0 if init succesfully. Return -1 otherwise.
	- Purpose: Init Serial and set UL Frequency to 920.8 MHz
*/
    char zone;
    if (!Detect_if_Module_Sigfox()) {
        Comms.sendMessage((char *)"AT+MODE=0");
        Ti.DELAY_TMR(1000);
    }
    Comms.sendMessage((char *)"ATZ");
    Ti.DELAY_TMR(200);
    if (strstr(Comms.BufferRx, "LSM110") != nullptr) {
        if (Ko.SIGFOX_MOD.zone == RCZ1) {               // If the detected module is LSM110A: USA and AU
            Ko.SIGFOX_MOD.zone = Ko.CONFIG.DT.RF_zone = RCZ2;
            EEPROM_MEM.Write_Int_EEPROM(DIR_RF_ZONE,Ko.SIGFOX_MOD.zone);      // Save default zone
        }
    } else if (strstr(Comms.BufferRx, "LSM100") != nullptr) {
        if (Ko.SIGFOX_MOD.zone != RCZ1) {               // If the detected module is LSM110A: USA and AU
            Ko.SIGFOX_MOD.zone = Ko.CONFIG.DT.RF_zone = RCZ1;
            EEPROM_MEM.Write_Int_EEPROM(DIR_RF_ZONE,Ko.SIGFOX_MOD.zone);      // Save default zone
        }
    }
    Comms.sendMessage((char *)"AT+VL=0");           // Verbose Mode = 0;
   
    zone = getZone();
    Ko.SIGFOX_MOD.zone = Ko.CONFIG.DT.RF_zone;
    if (zone != Ko.SIGFOX_MOD.zone) {
        if ((Ko.SIGFOX_MOD.zone >= 1) && (Ko.SIGFOX_MOD.zone <= 4)) {  
            if (Ko.SIGFOX_MOD.zone == RCZ4) {
                Comms.sendMessage((char *)"AT$RC=4");
                Comms.sendMessage((char *)"ATS302=22");
            } else if (Ko.SIGFOX_MOD.zone==RCZ2) {
                Comms.sendMessage((char *)"AT$RC=2");
                Comms.sendMessage((char *)"ATS302=22");
            } else if (Ko.SIGFOX_MOD.zone==RCZ1) {
                Comms.sendMessage((char *)"AT$RC=1");
                Comms.sendMessage((char *)"ATS302=15");
            }
        }  
        return 0;
    } else {
        return 1;
    }
}


int Sigfox_class::Detect_if_Module_Sigfox () {         //Checks if a Sigfox module is detected by sending an AT command
/*
 * test mode for Sigfox
	- Output: return 0 if no module present
	- Purpose: Start/Stop run emission test for Sigfox Certification
*/
    Comms.sendMessage((char *)"AT+MODE=?");
    if (strstr(Comms.BufferRx, "0") != nullptr) {  
        return 1;
    } else {
        return 0;
    }
}

int Sigfox_class::GetDeviceID(void)
{
    int i;
    int receivedResult=0;
    if (Active_Modulo_Sigfox())
        {
        receivedResult = Comms.sendMessage((char *)"AT$ID");
        for (i=0;i<8;i++)
            Ko.SIGFOX_MOD.ID[i]=Comms.BufferRx[i];
        
        Ko.SIGFOX_MOD.ID[i]=0;
        }
    if(activate_debug)
        Serial.printf("ID: %s\r\n", Ko.SIGFOX_MOD.ID);   
    return receivedResult;
}

int Sigfox_class::GetDevicePAC(void)
{
    char Aux[25];
    int i=0;
    int receivedResult=0;
    if (Active_Modulo_Sigfox())
        {   
        receivedResult = Comms.sendMessage((char *)"AT$PAC");
        while(Comms.BufferRx[i]!=0x0D)
            {
            Aux[i]=Comms.BufferRx[i];
            i++;
            }
        Aux[i]=0;
        }
        if(activate_debug)
            Serial.printf("PAC: %s\r\n", Aux);
    return receivedResult;
}

int Sigfox_class::Active_Modulo_Sigfox() {
    if (testComms_Module() == 1) {
           return 1;   
    } else {
        digitalWrite(EN_COM_MOD, LOW);            // Low pulse of 10 ms 
        delay(10);
        digitalWrite(EN_COM_MOD, HIGH);
        Ti.DELAY_TMR(1000);                      // Wait for the module activation
       if (testComms_Module() == 1) {
            return 1;
        } else {
            return 0;
        }
    }
}

int Sigfox_class::Deep_Sleep_Sigfox(void)         // LSM100 &LSM110A,  Enter in Sleep by itself in sleep mode
{
   return 1;
}

// Get the Configurated Sigfox Zone
int Sigfox_class::getZone()
{
	int receivedResult;
	int ret = 0;
	receivedResult = Comms.sendMessage((char *)"AT$RC=?");           
	if (strstr(Comms.BufferRx, "2")!=0) {
		ret = RCZ2;
	} else if (strstr(Comms.BufferRx, "1")!=0) {
		ret = RCZ1;
	} else if (strstr(Comms.BufferRx, "3")!=0) {
		ret = RCZ3;
    } else if (strstr(Comms.BufferRx, "4")!=0) {                  //#### Complementar respuesta
		ret = RCZ4;
	} else {
		ret = 0;
	}
	return ret;
}
// Send AT command for test if the module is present
int Sigfox_class::testComms_Module()
{
	char testchar[] = "AT";
	int receivedResult;
	int ret = 0;
	receivedResult = Comms.sendMessage(testchar); // Send message
	if (receivedResult == 1)
		ret = 1;
	else
        ret = 0;
	return ret;
}

// Send payload to sigfox
int Sigfox_class::sendPayload(unsigned char *outData, const unsigned char len, const int downlink)
{
    int receivedResult;
	if ((len > 12) || (len <= 0)){//		println("Payload length must be positive and not be longer than 12 bytes.");
		return -1;
	}

	if (outData == NULL){
//		println("outData is NULL.");
		return -1;
	}

	if (!((downlink == 0) || (downlink == 1))){//		println("downlink must be 0 or 1.");
		return -1;
	}
	receivedResult = SendDataSigfox(outData, len, downlink);
	return receivedResult;
}


int Sigfox_class::SendDataSigfox (unsigned char *outData, const unsigned char len, const int downlink ){
	char header[] = "AT$SF=";
	int actualLen;
	int sendLen;
	char hex_str[25];
    char trama_send[40]={0,0};
	int receivedResult=0;
	actualLen = len;
	Comms.ASCII2Hex(outData, actualLen, hex_str);
	sendLen = 2*actualLen;

// Prepare the date by zone
      receivedResult = prepareZone(downlink);
	
        if (receivedResult == -1){      //		debug_msg("Prepare zone failed");
            Prepare_Sending();
            return -1;
            }
	
	Ti.DELAY_TMR(20);
    
    strcat(trama_send,header);          // Add header
    strcat(trama_send,hex_str);         // Add Payload
	if (downlink == 1)
        { 
		 strcat(trama_send,",1");       //Add downlink
        }
    else
        {
         strcat(trama_send,",0");        //Not downlink
        }
    strcat(trama_send,",1");             //Repeat the messagges 3 times
    
    Comms.Prepare_Command_to_Send(trama_send);      //Send the data by Serial port to the Module
    

	if (downlink==1){
		{
        receivedResult = Comms.get_answer_Msg();
        }
	} else {
		return 0; // No wait for ack
	}
	return receivedResult;
}

int Sigfox_class::prepareZone(char downlink){

	int receivedResult=0;
	switch (Ko.SIGFOX_MOD.zone){
		case RCZ1:
            {
			receivedResult = Comms.sendMessage((char *)"ATS302=15");
            }
        break;
		
        case RCZ2:
			checkChangeZone(downlink);
        break;
        
		case RCZ4:
			checkChangeZone(downlink);
		
        break;

		case RCZ3:
            {
			receivedResult = Comms.sendMessage((char *)"ATS302=15");
            }	
        break;
		
        default:
            {
			receivedResult = -1;
            }
        break;
	}
	return receivedResult;
}


// check the need of reset the macro channels
int Sigfox_class::checkChangeZone(char downlink) {
	char msg[] = "AT$GI?";
	char Reset_Chanel[] = "AT$RC";
	char X, Y;
	int receivedResult=0;
	if (!downlink)
        {
        receivedResult = Comms.sendMessage(msg);
        X = Comms.BufferRx[0];
        Y = Comms.BufferRx[2];
        if ((X=='0') || (Y<'3')) 
            {
            receivedResult = Comms.sendMessage(Reset_Chanel);
            }
        }
    else
        receivedResult = Comms.sendMessage(Reset_Chanel);
	Prepare_Sending();
	return receivedResult;
}


void Sigfox_class::Prepare_Sending(){

	if(Ko.CONFIG.DT.com_module_det==mod_sigfox)
        {
        switch (Ko.SIGFOX_MOD.zone)
            {
            case RCZ1:
                {
                Ti.DELAY_TMR(100);
                break;
                }
            case RCZ2:
                {
                Ti.DELAY_TMR(50);
                break;
                }
            case RCZ4:
                {
                Ti.DELAY_TMR(50);
                break;
                }
            default:
                {

                }
            }
        }
   
}
// RX Data From LSM110A Module
void Sigfox_class::rx_data_Sigfox(void)
{
    unsigned char AUX[10]={0,0,0,0,0,0,0,0,0,0};
    char AUX_HEX[20];
    int i;
    char *pos;
    Comms.debug_msg(String(Comms.BufferRx));
    pos= strstr(Comms.BufferRx, "+RX="); 
    for (i=0;i<8;i++)
        {
        AUX[i]=*(pos+i+4);
        }
    Comms.ASCII2Hex(AUX,8,AUX_HEX);
    char DATOS_RX[20]={'R','X','='};
    strcat(DATOS_RX,AUX_HEX);
    Comms.Process_Rx_Data(DATOS_RX);                 // Rx the data
}