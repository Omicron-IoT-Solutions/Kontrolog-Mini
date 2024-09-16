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

#include "LoRa.h"


extern Sigfox_class Si;
extern Timers TIM;

int LoRa::Active_Modulo_LoRa(void)
{
    if (Si.testComms_Module()==1)
        return 1;   
    else
        {
        digitalWrite(EN_COM_MOD,LOW);            // Pulso bajo de 10ms para reset al módulo   
        delay(10);
        digitalWrite(EN_COM_MOD,HIGH);
        TIM.DELAY_TMR(1000);                     // Espera para activación del módulo
        Ko.PUB.join_status = 0;                 // Si ya se tenía Join, debe volverse al inicio   
        if (Si.testComms_Module()==1)
           {
           return 1;
           }
        else
            {
            return 0;
            }

        }
}

int LoRa::Deep_Sleep_Lora(void)           // LSM100A &LSM110A,  entran a Sleep de forma Automática.
{ 
   return 1;
}

int  LoRa::Detect_if_Module_LoRa(void)
{
    char msg[]="AT+MODE=?";
    Comms.sendMessage(msg);
    if((strstr(Comms.BufferRx,"1"))!=0)
        return 1;
    else
        return 0;
}

int LoRa::initLoRa_OTAA(int ini)
{
    char msg[40];
    if(!Detect_if_Module_LoRa())
        {
        sprintf(msg,"AT+MODE=1");
        Comms.sendMessage(msg);
        }   
    sprintf(msg,"ATZ");
    Comms.sendMessage(msg);                              // Reinicia el módulo de comunicación
    TIM.DELAY_TMR(1000);
    sprintf(msg,"AT+VL=0");
    Comms.sendMessage(msg);                          // Verbose Mode = 0;
    Lora_Band_Config();                                 // Determina la banda de Lora Configurada
    sprintf(msg,"AT+BAND=%d",Ko.LORA_MOD.band);         // Set LoRa Band;          
    Comms.sendMessage(msg);
    sprintf(msg,"AT+ADR=1");
    Comms.sendMessage(msg);                        // Set Automatic Data Rate
    
    if((Ko.LORA_MOD.band==AU915)|(Ko.LORA_MOD.band==US915))            // Set Maximun Power
        {
        sprintf(msg,"AT+TXP=14");
        Comms.sendMessage(msg);
        }
    else if(Ko.LORA_MOD.band==EU868)   
        {
        sprintf(msg,"AT+TXP=7");
        Comms.sendMessage(msg);
        }
    else;
    
    sprintf(msg,"AT+CLASS=C");
    Comms.sendMessage(msg);                       // Set Device class C
    
    if (ini==1)                                      // Set 
        {    
        setDevEui(LORA_DevEui);
        noop;
        setAppEui(LORA_AppEui);
        noop;
        setAppKey(LORA_AppKey);//NSKEY
        }
    
    Ko.PUB.join_status=0;
    return 1;
}

int LoRa::reboot_lora(void)
{
    char msg[]="ATZ";
    int receivedResult=Comms.sendMessage(msg);
    return receivedResult; 
}

int LoRa::factory_reset_lora(void)
{
    char msg[]="AT+RFS";
    int receivedResult=Comms.sendMessage(msg);
    return receivedResult; 
}

int LoRa::GetLoRaDeviceID(void)
{
    char msg[]="AT+DEUI=?";
    int i=0, j=0;
    int receivedResult=0;
    if (Active_Modulo_LoRa())
        {
        receivedResult = Comms.sendMessage(msg);            //DEV EUI
        i=0; j=0;
        while(Comms.BufferRx[i]!='\r')
            {
            if(Comms.BufferRx[i]!=':')
                Ko.LORA_MOD.OTAA_DEV_EUI[j++]=Comms.BufferRx[i];
            i++;
            }
        Ko.LORA_MOD.OTAA_DEV_EUI[j]=0;
        Serial.printf("EUI: %s\r\n", String(Ko.LORA_MOD.OTAA_DEV_EUI));
        }
    return receivedResult;
}

int LoRa::getJoinMode()
{
   char msg[]="AT+JOIN=?";
   Comms.sendMessage(msg);            
   if (strstr(Comms.BufferRx, "1")!=0)            
       return OTAA;
   else if(strstr(Comms.BufferRx, "0")!=0)
       return ABP;
   else
       return none;
}

// Set mode OTAA, ABP, TEST
int LoRa::SetJoinMode(int Mode)
{
    char SMODE[20] = "AT+JOIN=";
    char Res;
    if (getJoinMode() == Mode)
        Res=1;    
    else
        {
        if(Mode==OTAA)
          strcat(SMODE, "1"); 
        else if (Mode==ABP)
          strcat(SMODE, "0");   
        else 
            {
            Res=0;
            return Res;
            }
        noop;
        Comms.sendMessage(SMODE);
        if (getJoinMode() == Mode)
            Res=1;
        else 
          Res=-1;
        }
        return Res;
}

// Set Lora Device Class A or C
int LoRa::setClass(char *Class){
  char msg[] = "AT+CLASS=";
  strcat(msg,Class);
  Comms.sendMessage(msg);
  if(strstr(Comms.BufferRx,"CLASS")!=0)               
    return 1;
  else
    return -1;
}

void LoRa::setAppKey(const char *AppKey){               // SET Aplication Key OTAA
   char msg[100];
   sprintf(msg, "AT+APPKEY=%s",AppKey);
   Comms.sendMessage(msg);
}

void LoRa::setDevEui(const char *DevEui){               // Set Dev EUI  OTAA  ???  NO se si se puede
   char msg[100];
   sprintf(msg, "AT+DEUI=%s",DevEui);
   Comms.sendMessage(msg);
}

void LoRa::setAppEui(const char *AppEui){               //Set Aplication EUI OTAA
   char msg[100];
   sprintf(msg, "AT+APPEUI=%s",AppEui);
   Comms.sendMessage(msg);
}

void LoRa::GetAppKey(void){               // Get plication Key OTAA
  char msg[100];
  sprintf(msg, "AT+APPKEY=?");
  Comms.sendMessage(msg);         // Capturar y Guardar
}

void LoRa::GetDevEui(void){               // Get Dev EUI  OTAA  ???  NO se si se puede
   char msg[100];
   sprintf(msg,"AT+DEUI=?"); 
   Comms.sendMessage(msg);           // Capturar y Guardar
}

void LoRa::GetAppEui(void){               //Get Aplication EUI OTAA
   char msg[100];
   sprintf(msg,"AT+APPEUI=?"); 
   Comms.sendMessage(msg);         // Capturar y Guardar
}

 int LoRa::joinNetwork(int Method){
  char msg[40];
  sprintf(msg,"AT+JOIN=%d",Method);
  Comms.sendMessage(msg);  
  return 1;
}
 
 int LoRa::wait_for_join(void)
 {
 int res=0;
    if (strstr(Comms.BufferRx, "JOIN FAILED")!=0) 
      {
      res=-1;
      }
    else if (strstr(Comms.BufferRx, "JOINED")!=0)
      {
      res=1;
      }
    else if(strstr(Comms.BufferRx, "IRQ_RX_TX_TIMEOUT")!=0)
       res=-1;
    else;
  return res;
 }

int LoRa::sendCMSGHEX(unsigned char *payload, int size, int ack){
   char hex_str[50];
   char msg[80];
   Comms.ASCII2Hex(payload, size, hex_str);
   int i=0;
   for(i=0;i<sizeof(msg);i++)
       msg[i]=0;
   int chn_rnd = (rand()%198)+1;
   sprintf(msg,"AT+SEND=%d:1:%s",chn_rnd,hex_str);         // El mensaje anterior se envió ACK,  requiere verificar si se recibe respuesta
   Comms.sendMessage(msg);
   return 1;
}

int LoRa::wait_rx_lora(void)
{
    int res=0;
    if(strstr(Comms.BufferRx, "EVT:SEND_CONFIRMED")!=0)           //Modificado temporalmente,  Octubre 19 mientras solucionamos tema de comandos AT,  EVT:SEND_CONFIRMED
        {
        res=1;    
        }
    else if(strstr(Comms.BufferRx, "EVT:RECV_UNCONFIRMED")!=0)
        {
        res=2;
        }
    else if (strstr(Comms.BufferRx, "EVT:SEND_FAILED")!=0)
        {
        if(++cont_fail==10)
            {
            cont_fail=0;
            factory_reset_lora();
            }
        res=-2;
        }
    else if ((strstr(Comms.BufferRx, "NO_NETWORK_JOINED")!=0))
        {
        res=-1;
        }
    else if((strstr(Comms.BufferRx, "AT_ERROR")!=0))
        {
        res=-2;
        }
    return res;
}

void LoRa::Rx_data_lora(void)
{
    char  Buffer_Test [32]={'R','X','=',};
    char *pos1;
    int i=0,j=0;
    Ko.PUB.Lora_Rx_flag=0;
    pos1= strstr(Comms.BufferRx, "EVT:RECV_UNCONFIRMED");
    if(pos1!=0)
        {
        for(i=3;i<32;i++)
        Buffer_Test[i]=0;
        i=26; j=3;
        while (*(pos1+i)!='\n')
            {
            Buffer_Test[j]=*(pos1+i);
            i++;j++;
            }
        Comms.Recibir_Datos(Buffer_Test);
        }
}

void LoRa::Lora_Band_Config(void)
{
    if(Ko.CONFIG.DT.RF_zone==2)
        Ko.LORA_MOD.band=US915;
    else if (Ko.CONFIG.DT.RF_zone==4)
        Ko.LORA_MOD.band=AU915;
    else if (Ko.CONFIG.DT.RF_zone==1)
        Ko.LORA_MOD.band=EU868;
    else
        Ko.LORA_MOD.band=US915;        //Por defecto
}