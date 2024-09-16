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

#ifndef comms_h
#define	comms_h

#include <cstdint>
#include <Arduino.h>

#include "config.h"
#include "eeprom_esp.h"
#include "RTC_clock.h"
#include "sigfox.h"
#include "LoRa.h"
#include "WiFi.h"
#include "timers.h"


class comms {

public:
    // Functions
    void Det_Com_Module(void);
    int Recibir_Datos(char *);
    uint8_t Procesar_Variables(void);
    void Communication_Task(void);
    void Enviar_Comando(char *);
    void BufferRx_Init(void);
    void ENV_TX (char *buffer);
    void RC_INT(void);
    int sendMessage(char *);
    int getRecvMsg(void);
    void ASCII2Hex(unsigned char* input, int length, char* buf_str);
    void SET_LED_RGB(char color);
    void LED_SEND_MSG(void);
    void LED_ERR_MSG(void);

    // External variables
    bool Dato_RxFlag;
    bool FIN_TxFlag;         // Indica final de transmisión                              
    uint16_t Cont_Rx;           // Contador de Bytes recibidos                              
    uint16_t Cont_Tx;           // Contador de Bytes Transmitidos                             
    char BufferRx[200];         // Buffer de datos Recibidos
    
private:
    // Functions
    void Apagar_Com_Module(void);
    void Preparar_Envio(void);
    void Almacenar_Datos_Env(unsigned char *);
    char Activar_Com_Modulo (void);
    void Transmisitir_Datos(unsigned char *, char);

    // Internal variables
    char DATORx;                                             // ?ltimo Dato recibido
    //unsigned char Cont_Rx;                                            // Contador de Bytes recibidos
    //unsigned char Cont_Tx;                                            // Contador de Bytes Transmitidos
    unsigned char buf_str[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    // Parameters limits
    int LI_SENS[14]={1,-400,1,0,0,-250,-400,-400,1,0,1,0,0};
    int LS_SENS[14]={247,500,11,0,1,250,800,2047,7200,0x800,10,4,32};
    const int LI_VB[14]={0,0,0,0,0,-250,0,0,5,0,0,0,0};
    const int LS_VB[14]={0,0,0,0,1,250,150,150,7200,0,0,0,32};
    const int LI_OUT[14]={0,0,0,0,-40,0,0,1,0,1,0,0,1};
    const int LS_OUT[14]={0,1,7,5,2047,20,1,32767,32767,50,50,50,100};
    const int LI_OUT_PID[14]={0,0,0,0,-40,0,0,1,1,1,0,0,1};
    const int LS_OUT_PID[14]={0,1,7,5,80,20,1,60,10,50,50,50,100};
}; 

extern comms Comms;
#endif

