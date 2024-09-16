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

#include "communications.h"


void comms::Det_Com_Module(void)
    {

    if (Ko.CONFIG.DT.com_module_prog == mod_lora) 
        {
         
        if (Lo.Active_Modulo_LoRa()) {
            Ko.CONFIG.DT.com_module_det = mod_lora;
            Lo.initLoRa_OTAA(0);
            Lo.GetLoRaDeviceID();
        } else {
            Ko.CONFIG.DT.com_module_det = mod_none;
        }
        Lo.Deep_Sleep_Lora();
        } 
    else if (Ko.CONFIG.DT.com_module_prog == mod_sigfox) 
        {
        if (Si.Active_Modulo_Sigfox()) {
            Ko.CONFIG.DT.com_module_det = mod_sigfox;
            Si.initSigfox();
            Si.GetDeviceID();
        } else {
            Ko.CONFIG.DT.com_module_det = mod_none;
        }
        Si.Deep_Sleep_Sigfox();
        } 
    else if (Ko.CONFIG.DT.com_module_prog == mod_wifi) 
        {
            Ko.CONFIG.DT.com_module_det = mod_wifi;

         } 
    else 
        {
        Ko.CONFIG.DT.com_module_det = mod_none;
        }
}

void comms::Apagar_Com_Module(void){
    if(Ko.CONFIG.DT.com_module_det==mod_sigfox)
        Si.Deep_Sleep_Sigfox(); 
    else if(Ko.CONFIG.DT.com_module_det==mod_wifi)
        Wi.Deep_Sleep_Wifi();
    else if(Ko.CONFIG.DT.com_module_det==mod_lora)
        Lo.Deep_Sleep_Lora();
    else
        {
        Si.Deep_Sleep_Sigfox(); 
        Lo.Deep_Sleep_Lora();
        Wi.Deep_Sleep_Wifi();
        }
}

char comms::Activar_Com_Modulo() {
    // Check if no module is programmed
    if (Ko.CONFIG.DT.com_module_prog == mod_none) {
        Det_Com_Module(); // Try to detect the current communication module
    }

    // Check which module is selected and activate it
    if (Ko.CONFIG.DT.com_module_det == mod_wifi) {
        return Wi.Init_WiFi_Connection();
    } else if (Ko.CONFIG.DT.com_module_det == mod_sigfox) {
        return Si.Active_Modulo_Sigfox();
    } else if (Ko.CONFIG.DT.com_module_det == mod_lora) {
        return Lo.Active_Modulo_LoRa();
    } else {
        return 0;
    }
}

void comms::Transmisitir_Datos(unsigned char *buffer_datos, char ack) {
    
    if (Ko.CONFIG.DT.com_module_det == mod_wifi) { // WiFi Module
        Wi.SendPayloadWiFi(buffer_datos, 16, ack);  // Keep consistent with LoRa
    } 
    else if (Ko.CONFIG.DT.com_module_det == mod_sigfox) { // Sigfox Module
        if (ack) {
            RTC.READ_TIME();
            Ko.RTC.previous_time_stamp = Ko.RTC.time_stamp + 5; // Save the current Time_Stamp to get the correct time on reception
        }
        Si.sendPayload(buffer_datos, 12, ack);
    } 
    else if (Ko.CONFIG.DT.com_module_det == mod_lora) { // LoRa Module
        Lo.sendCMSGHEX(buffer_datos, 18, ack);
    }
}

void comms::Communication_Task(void)
{
    if (Ko.TIMERS.timer_publication > 0)
        Ko.TIMERS.timer_publication--;

    if (Ko.TIMERS.timer_reset_com_error >= 720) { // Device hasn't received any Downlink in more than 12 hours?
        esp_restart();
    }

    if (Ko.PUB.com_status == standby) {
        digitalWrite(LED_GREEN, HIGH);
    } else {
        if (Ko.TIMERS.seconds & 0x01) {
            digitalWrite(LED_GREEN, LOW);
        } else {
            digitalWrite(LED_GREEN, HIGH);
        }
    }

    switch (Ko.PUB.com_status) {
            case module_init:                              // El m?dulo no se ha podido detectar.
                {
                Det_Com_Module();
                Ko.PUB.counter_for_downlink=Ko.CONFIG.DT.downlink_msg-1;
               
                if (Ko.CONFIG.DT.com_module_det==mod_wifi)            // Si el m?dulo es Wifi se ingresa a modo AP
                    {
                    Wi.enter_to_AP_Mode();
                    Ko.TIMERS.timer_wifi_AP=180;                     // Init Timer en AP Mode
                    Ko.PUB.com_status=wifi_AP_mode;
                    }
                else
                    {
                    Apagar_Com_Module();
                    Ko.PUB.com_status=standby;
                    Ko.TIMERS.timer_publication=60;
                    }
                }
            break;
            
            case standby: { // Module waiting until TPU is fulfilled
                // Time control to report parameters
                if ((Ko.TIMERS.timer_publication == 0) || (Ko.PUB.send_data == 1)) {
                    Ko.PUB.send_data = 0;
                    Ko.PUB.com_status = storing;
                    Ko.TIMERS.timer_publication = Ko.CONFIG.DT.TPU;
                    Ko.TIMERS.timer_publication *= 60;
                    int time_rnd = rand() % 30 - 15;
                    Ko.TIMERS.timer_publication += time_rnd; // Add a random value between -15 and 15 seconds to desynchronize multiple devices on a same network
                } else {
                    if (Ko.PUB.data_ACK_Rx) { // Received some data out of time, this is asynchronous
                        if (Ko.CONFIG.DT.com_module_det == mod_lora)
                            Lo.Rx_data_lora();
                        else if (Ko.CONFIG.DT.com_module_det == mod_sigfox)
                            Recibir_Datos(BufferRx);
                        else if (Ko.CONFIG.DT.com_module_det == mod_wifi)
                            Recibir_Datos(BufferRx);
                        Ko.PUB.data_ACK_Rx = 0;
                    }
                }
            }
            break;
            
            case storing:                     // Lee la informaci?n de los sensores y almacena el dato
                {
                Preparar_Envio();               // Recoge y prepara los datos para ser enviados
                if(Ko.RTC.RTC_is_synchronized)        // Tenemos hora correcta?
                    { 
                buf_str[12] = (Ko.RTC.time_stamp >> 24) & 0xFF;
                buf_str[13] = (Ko.RTC.time_stamp >> 16) & 0xFF;
                buf_str[14] = (Ko.RTC.time_stamp >> 8) & 0xFF;
                buf_str[15] = Ko.RTC.time_stamp & 0xFF;
                }   
                else 
                    {
                    buf_str[12]=0;
                    buf_str[13]=0;
                    buf_str[14]=0;
                    buf_str[15]=0;
                    }

                Ko.PUB.wait_for_downlink=0;
                if(++Ko.PUB.counter_for_downlink >=Ko.CONFIG.DT.downlink_msg)
                    {
                    Ko.PUB.counter_for_downlink=0;
                    Ko.PUB.wait_for_downlink=1;
                    }
                
               
                Ko.PUB.com_status=joining;
                }
            break;
            
            case joining:
                { 
                Ko.PUB.counter_of_attempts_to_send=0;
                if (Ko.CONFIG.DT.com_module_det==mod_none)               // No se encontr? m?dulo
                    {
                    Ko.PUB.com_status=module_init;
                    }
                
                else if (Ko.CONFIG.DT.com_module_det==mod_wifi)         // Modulo WiFi?
                    {
                    Ko.PUB.com_status=sending;
                    }
                
                else if (Ko.CONFIG.DT.com_module_det==mod_lora)         // Modulo Sigfox
                    {
                    if(Lo.Active_Modulo_LoRa()==1)
                        {
                        if(Ko.PUB.join_status==1)
                            Ko.PUB.com_status=sending;
                        else
                            {
                            Lo.joinNetwork(1);                          // Se Envia primer comando de Join
                            Ko.LORA_MOD.int_join=0;                     // Se inicializa el contador de intentos de Join
                            Ko.PUB.join_counter=0;                      // Y el temporizador de Join
                            Ko.PUB.com_status=waiting_to_join;    
                            }
                        }    
                    else
                        {
                        Lo.Deep_Sleep_Lora();
                        Ko.PUB.com_status=standby;
                        }
                    }

                else if (Ko.CONFIG.DT.com_module_det==mod_sigfox)
                    {
                    if(Si.Active_Modulo_Sigfox()==1)             // El modulo no debe dormirse,  entonces pregruantar primero por Join????
                        Ko.PUB.com_status=sending;
                    else
                        {
                        Si.Deep_Sleep_Sigfox();
                        Ko.PUB.com_status=standby;
                        }
                    }
                }
            break;
            
            case waiting_to_join:
                {
                
                int res=Lo.wait_for_join();
                
                if (res==1)                         //Join  correcto?
                    {
                    Ko.PUB.join_status=1; 
                    Ko.PUB.com_status=sending;
                    }
                else if (res==-1)                   // Join Fail?
                    {
                    if(++Ko.LORA_MOD.int_join<=10)      // Intenta hasta 10 veces
                        {
                        Lo.joinNetwork(1);
                        Ko.PUB.join_counter=0;
                        }
                    else
                        {
                        Lo.initLoRa_OTAA(0);                   // Se reinicia m?dulo y se reconfigura
                        Ko.PUB.com_status=standby;
                        }
                    }
                else
                    {
                    if(++Ko.PUB.join_counter==30)  // Join no se logra recibir respuesta al join en 30 segundos
                        {
                        Lo.Deep_Sleep_Lora();
                        Ko.PUB.com_status=standby;
                        }
                    }
                }
            break;
                        
            case sending:
                {
                Transmisitir_Datos(buf_str,Ko.PUB.wait_for_downlink);
                if(Ko.CONFIG.DT.com_module_det==mod_lora)
                    Ko.PUB.Rx_counter=30;
                else
                    Ko.PUB.Rx_counter=60;
                
                Ko.PUB.data_ACK_Rx=0;
                Ko.PUB.counter_of_attempts_to_send=0;
                Ko.PUB.com_status=receiving;
                }
            break;
                
            case receiving:
                {
                if (Ko.CONFIG.DT.com_module_det==mod_lora)
                    {
                    int res=Lo.wait_rx_lora();
                    
                    if (res==1)                                                   //Envío correcto
                        {
                        Serial.println("EVT:SEND_CONFIRMED");
                        if(Ko.PUB.wait_for_downlink)
                            {
                            Ko.PUB.Rx_counter=0;                                   
                            RTC.READ_TIME();
                            Ko.RTC.previous_time_stamp=Ko.RTC.time_stamp;        // Guarda el Time_Stamp Actual, para tomar la hora correcta en pr?xima recepci?n
                            }
                        else
                            Ko.PUB.Rx_counter=0;   
                        }
                    else if(res==2)                                                 // Envio Correcto, Se recibio Downlink.
                        {
                        Serial.println("EVT:SEND_UNCONFIRMED");
                        Ko.PUB.Rx_counter=0;
                        Lo.Rx_data_lora();                                             // Recibo comando anterior
                        if(Ko.PUB.wait_for_downlink)
                            {
                            RTC.READ_TIME();
                            Ko.RTC.previous_time_stamp=Ko.RTC.time_stamp;                    // Guarda el Time_Stamp Actual, para tomar la hora correcta en pr?xima recepci?n
                            }
                        }
                    else if(res==-1)                                                // Si no hay Join
                        {
                        Ko.PUB.join_status=0;
                        Ko.PUB.com_status=standby;
                        }
                    else if(res==-2)                                                // Error AT_ERROR,  u otro==> Repetir comando, se est? sincronizando la primera vez
                        {
                        Ko.PUB.Rx_counter=0; 
                        }
                    }
                
                else if (Ko.CONFIG.DT.com_module_det==mod_wifi)
                    {
                    if(Ko.PUB.wait_for_downlink)                // Waiting for Downlink?
                        {
                        int res=Wi.wait_rx_wifi();

                        if(res==1)
                            {
                            Ko.PUB.Rx_counter=0;
                            Recibir_Datos(BufferRx);            // Responde con Nivel de se?al WiFi   
                            }
                        else if(res==2)
                            {
                            if(Ko.PUB.data_ACK_Rx)
                            {
                            Recibir_Datos(BufferRx);
                            Ko.PUB.Rx_counter=0;              // Para finalizar ciclo,  eperando respuesta del servidor
                            }
                            }
                        else if (res==-1)                                   // Error de comunicaci?n WiFi.
                            { 
                            Ko.PUB.Rx_counter=0; 
                            }
                        }
                    else{
                        Ko.PUB.Rx_counter=0;                                // End of Rx
                        }
                    }                
                else if(Ko.CONFIG.DT.com_module_det==mod_sigfox)
                    {
                    if(Ko.PUB.wait_for_downlink)                 // Se espera Downlink?
                        {
                        if (Ko.PUB.data_ACK_Rx)
                            {
                            Si.rx_data_Sigfox();
                            Ko.PUB.Rx_counter=0;               // Para finalizar ciclo
                            }
                        }
                    else{
                        Ko.PUB.Rx_counter=0;                  // End of Rx
                        }
                    }
                
                
                if(--Ko.PUB.Rx_counter<=0)
                    {
                    Apagar_Com_Module();
                    Ko.PUB.com_status=standby;
                    }

                }           
            break;
            
           
            case wifi_AP_mode:                                          // Wifi en Modo AP
                {    
                Wi.Portal_Manager();
                if(--Ko.TIMERS.timer_wifi_AP==0)
                    {
                    Wi.end_AP_Mode();
                    Ko.PUB.com_status=standby;
                    Ko.TIMERS.timer_publication=10;
                    }
                }
            break;    
            

            
            }
}

void comms::Preparar_Envio(void)
{
    unsigned int dato;
   
        buf_str[0] = (Ko.PUB.message_type<<4);                          // Dato de monitoreo
        Ko.PUB.message_type=0;

        // Sensor 1
        dato= Ko.IN1.value & 0x0FFF;
        buf_str[0] = buf_str[0]|(dato>>8);
        buf_str[1] = (dato&0x00FF);

        // Sensor 2
        dato = Ko.IN2.value & 0x0FFF;
        buf_str[2] = (dato>>4)&0x00FF;
        buf_str[3] = (dato&0x000F)<<4;

        // Voltaje 
        buf_str[8] = (unsigned char)  Ko.VDC.value;

        // Estados de salidas y alarma sonora 
        unsigned char BITS=0;                                     
        BITS|=(Ko.OUT2.value<<2);
        BITS|=(Ko.OUT1.value<<1);
        buf_str[11]|=BITS;
}


int comms::Recibir_Datos(char *Buffer_Datos_Rx)
{
int i=0,j=0;
char Aux[20]={0};
char Aux2[10]={0};
unsigned char ack_nuevo=0, mod=0;
unsigned char Buff[12] = {0};
char *pos_rx;
                
    Ko.PUB.data_ACK_Rx=0;                     // Se confirma que se recibi? comando de respuesta.
    
    pos_rx=strstr(Buffer_Datos_Rx, "RX=");

    if (pos_rx!=0)
        {
        pos_rx+=3;
        while (*(pos_rx + i) != '\r' && j < 16)
            {
            
            if (*(pos_rx+i)!=0x20)
                Aux[j++]=*(pos_rx+i);
            i++;
            }
        Aux[j]=0;
    
        // Extracting and converting received data fields
        Aux2[0]=Aux[0];
        Aux2[1]=Aux[1];
        Aux2[2]=0;
        Ko.GET_DATA.group=strtol(Aux2,0,16);
        
        Aux2[0]=Aux[2];
        Aux2[1]=Aux[3];
        Aux2[2]=0;
        Ko.GET_DATA.subgroup=strtol(Aux2,0,16);
        
        Aux2[0]=Aux[4];
        Aux2[1]=Aux[5];
        Aux2[2]=0;
        Ko.GET_DATA.parameter=strtol(Aux2,0,16);
        
        Aux2[0]=Aux[6];
        Aux2[1]=Aux[7];
        Aux2[2]=Aux[8];
        Aux2[3]=Aux[9];
        Aux2[4]=Aux[10];
        Aux2[5]=Aux[11];
        Aux2[6]=Aux[12];
        Aux2[7]=Aux[13];
        Aux2[8]=0;
        Ko.GET_DATA.value_length= strtoul(Aux2,0,16);
        Ko.GET_DATA.value_float= strtoul(Aux2,0,16);
        
        Aux2[0]=Aux[14];                    // Recibe informaci?n de si hay mas datos de configuraci?n pendientes
        Aux2[1]=Aux[15];
        Aux2[2]=0;
        ack_nuevo=strtol(Aux2,0,16);
        if (ack_nuevo>1)
            ack_nuevo=0;
        
        
        mod=Procesar_Variables();
        
        if (mod>0)
            {
            if(mod==1)
                {
                // Prepara respuesta a comando
                Buff[0]= 0xF0;
                Buff[1]= Ko.GET_DATA.group;
                Buff[2]= Ko.GET_DATA.subgroup;
                Buff[3]= Ko.GET_DATA.parameter;
                Buff[4]= (Ko.GET_DATA.value_length>>24);
                Buff[5]= (Ko.GET_DATA.value_length>>16);
                Buff[6]= (Ko.GET_DATA.value_length>>8);
                Buff[7]= (Ko.GET_DATA.value_length);    
                }
            else
                {
                Buff[0]= 0xFE;              // Error en el dato recibido
                }
            
            if (ack_nuevo)
                {
                Ko.PUB.Rx_counter=0;
                Ko.TIMERS.wait_for_sigfox_downlink=60;          //???? Revisar para Sigfox
                }
            
            if (Ko.CONFIG.DT.com_module_prog == mod_wifi)
                Wi.SendPayloadWiFi(Buff,8,ack_nuevo);
            else if(Ko.CONFIG.DT.com_module_prog == mod_sigfox)
                Si.sendPayload(Buff,8,ack_nuevo);
            else if(Ko.CONFIG.DT.com_module_prog == mod_lora)
                Lo.sendCMSGHEX(Buff,8,ack_nuevo);

            }
        }
        
    if (ack_nuevo==1)
        return 1;
    else
        return 0;
                    
}

uint8_t comms::Procesar_Variables(void)        
{ 
    
       uint8_t result;
       return result;
}


//*******************************************************************************************************************************
// Subrutinas de envi? y recepci?n de datos
//*******************************************************************************************************************************


/*-------------------------------------------------------------------------
	sendMessage
	- Input: outData is a pointer to the sending message.
	         len is the length of the sending message.
	- Output: recvMsg is received message
-------------------------------------------------------------------------*/
int comms::sendMessage(char *outData)
{
    int receivedResult;
	if (outData == NULL){      
		return -1;
	}
    // send message
	//Prepare_Sending();              // Inicia el buffer de rx
    Enviar_Comando(outData);          // Envía los datos
    // read ack or return payload
	receivedResult = getRecvMsg(); 
	return receivedResult;
}

/*
recvMsg getRecvMsg()
	- Output: return receive message from WISOL
*/
int comms::getRecvMsg(void)
{
	int receivedResult;
    int wait_for_rx=20;
    Cont_Rx=0;
    while (wait_for_rx-->0)
        {
        delay(10);
        RC_INT();
        }
    
    if ((strstr(BufferRx, "OK")!=0))
            {
			return 1;
            }		
        else 
            {
			return 0;
            }

}

void comms::Enviar_Comando(char *comando) {
    BufferRx_Init();  // Initialize the receive buffer and flags

    char BufferTx[200];  // Buffer for transmitting command
    unsigned char i = 0;

    // Copy the command string into the transmission buffer
    while (*(comando + i) != 0) {
        BufferTx[i] = *(comando + i);
        i++;
    }

    // Append '\r' and '\n' to the command
    BufferTx[i++] = '\r';
    BufferTx[i++] = '\n';
    BufferTx[i] = 0;  // Null-terminate the string (optional)

    // Transmit the constructed command
    ENV_TX(BufferTx);  // This presumably starts the transmission process
}

void comms::BufferRx_Init(void)
{
    int i;

    // Initialize the receive buffer with zeros
    for (i = 0; i < sizeof(BufferRx); i++) {
        BufferRx[i] = 0;
    }
    
    Cont_Rx = 0;    // Reset receive buffer index
    Dato_RxFlag=0;  // Reset Rx Flag
}


#define TO_HEX(i) (i <= 9 ? '0' + i : 'A' - 10 + i)
void comms::ASCII2Hex(unsigned char* input, int length, char* buf_str)
{
    int i,j=0;

    for (i=0;i<length;i++)
    {
        *(buf_str+j++) = TO_HEX(((*(input+i) & 0xF0) >> 4));
        *(buf_str+j++) = TO_HEX((*(input+i) & 0x0F));  
    }
    *(buf_str+j)=0;
}


// Transmitir datos hasta finalizar el Buffer

void comms::ENV_TX (char *buffer)                 // Se habilita transmisi?n
{

    Cont_Tx=0;                      // Inicia el contador de Tx
    while (*(buffer+Cont_Tx)!=0) 
        { 
        Serial1.write(*(buffer+Cont_Tx));
        Serial1.flush();
        Cont_Tx++;
        }
    TIM.DELAY_TMR(20);
}

//----------------------------------------------------------------------------
//	RECEPCION SERIAL DE DATOS POR INTERRUPCI?N
//----------------------------------------------------------------------------
// Recibe Bytes de consulta por puerto Serial
// Byte 1:  Id del dispositivo
// Byte 2...N: Datos recibidos
// M?ximo 16 caracteres ASCII
// Rata de Baudios: 19200 bps
// Control de duraci?n de recepci?n
// Variable MAXTIME_Rx = 20;   activada por TMR0 el cual tiene peri?dos de 4mS;  Maximo tiempo 20mS
//----------------------------------------------------------------------------

void comms::RC_INT() 
{
    while (Serial1.available() > 0) 
        {
        DATORx = Serial1.read();            // Read the incoming byte
        BufferRx[Cont_Rx] = DATORx;         // Store in the receive buffer
        Cont_Rx++;
        //Serial.print(DATORx);               // Debug
        if (Cont_Rx >= sizeof(BufferRx)){
            Cont_Rx = 0;                    // Reset index if buffer overflow
            }
        
        if(Serial1.available()==0)
            {
            Dato_RxFlag=1;
            if(Ko.CONFIG.DT.com_module_det==mod_sigfox)     // Check if there is a Sigfox Rx Data
                {
                if (strstr(BufferRx, "RX=")!=0)   
                    {
                    Ko.PUB.data_ACK_Rx=1;
                    Ko.TIMERS.timer_reset_com_error=0;
                    }
                else;
                }
            else if(Ko.CONFIG.DT.com_module_det==mod_lora)      // Check if there is a LoRa Rx Data
                {
                if (strstr(BufferRx, "EVT:RECV_UNCONFIRMED")!=0)                
                    {
                    Ko.PUB.data_ACK_Rx=1;
                    Ko.TIMERS.timer_reset_com_error=0;
                    }
                else;
                }
            }
        }
}


//-----------------------------------------------------------------------------------
// LED RGB  OPERATION
//-----------------------------------------------------------------------------------


void comms::SET_LED_RGB(char color)
{
    switch (color)
    {
        case led_off:
            digitalWrite(LED_RED,HIGH);
            digitalWrite(LED_GREEN,HIGH);
            digitalWrite(LED_BLUE,HIGH);
        break;

        case led_white:                         
            digitalWrite(LED_RED,LOW);
            digitalWrite(LED_GREEN,LOW);
            digitalWrite(LED_BLUE,LOW);
        break;
        
        case led_red:                         
            digitalWrite(LED_RED,LOW);
            digitalWrite(LED_GREEN,HIGH);
            digitalWrite(LED_BLUE,HIGH);
        break;
        
        case led_green:                        
            digitalWrite(LED_RED,HIGH);
            digitalWrite(LED_GREEN,LOW);
            digitalWrite(LED_BLUE,HIGH);
        break;
        
        case led_blue:                        
            digitalWrite(LED_RED,HIGH);
            digitalWrite(LED_GREEN,HIGH);
            digitalWrite(LED_BLUE,LOW);
        break;
        
        case led_violet:                         // Morado
            digitalWrite(LED_RED,LOW);
            digitalWrite(LED_GREEN,HIGH);
            digitalWrite(LED_BLUE,LOW);
        break;
        
        case led_orange:                         // Naranja
            digitalWrite(LED_RED,LOW);
            digitalWrite(LED_GREEN,LOW);
            digitalWrite(LED_BLUE,HIGH);
        break;
        
        case led_navy_blue:                     // violeta
            digitalWrite(LED_RED,HIGH);
            digitalWrite(LED_GREEN,LOW);
            digitalWrite(LED_BLUE,LOW);
        break;


        default:
        break;
  
    }

}

void comms::LED_SEND_MSG(void)
{
    uint8_t i, rep;
    SET_LED_RGB(led_off);
    TIM.DELAY_TMR(100);
    
    if(Ko.CONFIG.DT.com_module_det==mod_sigfox)
        {
        if(Ko.SIGFOX_MOD.zone==1)
            rep=3;
        else
            rep=Ko.SIGFOX_MOD.zone;
        }
    else if(Ko.CONFIG.DT.com_module_det==mod_lora)
        {
        if(Ko.LORA_MOD.band==AU915)
            rep=4;
        else if(Ko.LORA_MOD.band==US915)
            rep=2;
        else
            rep=3;
        }
    else
        rep=3;        
        
    
    for (i=0;i<rep;i++)
        {
        if(Ko.CONFIG.DT.com_module_det==mod_sigfox)
            SET_LED_RGB(led_orange);
        else
            SET_LED_RGB(led_green);
        TIM.DELAY_TMR(300);
        SET_LED_RGB(led_off);                       
        TIM.DELAY_TMR(300);
        }
}

void comms::LED_ERR_MSG(void)
{
    uint8_t i;
    TIM.DELAY_TMR(100);
    for (i=0;i<3;i++)
        {
        SET_LED_RGB(led_red);
        TIM.DELAY_TMR(300);
        SET_LED_RGB(led_off);                      
        TIM.DELAY_TMR(300);
        }
}