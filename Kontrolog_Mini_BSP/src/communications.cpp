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

#include "communications.h"


void comms::Found_Com_Module(void)                      // This funtion Detects if the comm. module programated is present or not
    {

    if (Ko.CONFIG.DT.com_module_prog == mod_lora)       // If LoRa Module is Programmed
        {
         
        if (Lo.Active_LoRaMod()) {                      // Found Module LoRa
            Ko.CONFIG.DT.com_module_det = mod_lora;
            Lo.Set_LoRaMod_OTAA();
            Lo.GetLoRaDeviceID();
        } else {
            Ko.CONFIG.DT.com_module_det = mod_none;
        }
        Lo.Set_Sleep_LoRaMod();
        } 
    else if (Ko.CONFIG.DT.com_module_prog == mod_sigfox)  // If Sigfox Module is Programmed
        {
        if (Si.Active_Modulo_Sigfox()) {
            Ko.CONFIG.DT.com_module_det = mod_sigfox;
            Si.initSigfox();
            Si.GetDeviceID();
            Si.GetDevicePAC();
        } else {
            Ko.CONFIG.DT.com_module_det = mod_none;
        }
        Si.Deep_Sleep_Sigfox();
        } 
    else if (Ko.CONFIG.DT.com_module_prog == mod_wifi)    // If WiFi Module is programmed
        {
            Ko.CONFIG.DT.com_module_det = mod_wifi;

         } 
    else                 
        {
        Ko.CONFIG.DT.com_module_det = mod_none;
        }
}

// Turn off the comunication Module
void comms::Turn_Off_Com_Module(void)
{
    if(Ko.CONFIG.DT.com_module_det==mod_lora)
        Lo.Set_Sleep_LoRaMod();
    else if(Ko.CONFIG.DT.com_module_det==mod_sigfox)
        Si.Deep_Sleep_Sigfox(); 
    else if(Ko.CONFIG.DT.com_module_det==mod_wifi)
        {
        delay(1);
        //Wi.Deep_Sleep_Wifi();           // Uncomment if you want to close WiFi connection
        }

    else
        {
        Si.Deep_Sleep_Sigfox(); 
        Lo.Set_Sleep_LoRaMod();
        Wi.Deep_Sleep_Wifi();
        }
}

// Activation of communication module
char comms::Activate_Com_Module() {
    // Check if no module is programmed
    if (Ko.CONFIG.DT.com_module_prog == mod_none) {
        Found_Com_Module(); // Try to detect the current communication module
    }

    // Check which module is selected and activate it
    if (Ko.CONFIG.DT.com_module_det == mod_lora) {
        return Lo.Active_LoRaMod();
    }
    else if (Ko.CONFIG.DT.com_module_det == mod_sigfox) {
        return Si.Active_Modulo_Sigfox();
    } 
    else if (Ko.CONFIG.DT.com_module_det == mod_wifi) {
        return Wi.Init_WiFi_Connection();
    }
    else {
        return 0;
    }
}

// Trasmit data according to selected comm. module
void comms::Transmit_Data(unsigned char *buffer_datos, char ack) {
    
    if (Ko.CONFIG.DT.com_module_det == mod_lora) {          // LoRa Module?
        Lo.sendCMSGHEX(buffer_datos, 18, ack);
    }
    else if (Ko.CONFIG.DT.com_module_det == mod_sigfox) {   // Sigfox Module?
        Si.sendPayload(buffer_datos, 12, ack);
    } 
    else if (Ko.CONFIG.DT.com_module_det == mod_wifi) {      // WiFi Module?
        Wi.SendPayloadWiFi(buffer_datos, 18, ack);           // Keep consistent with LoRa
    } 

}

//-------------------------------------------------------------------------------------------------------
// Handle all communication tasks
//-------------------------------------------------------------------------------------------------------
void comms::Communication_Task(void)
{
    if (Ko.TIMERS.timer_publication > 0)
        Ko.TIMERS.timer_publication--;

    if (Ko.TIMERS.timer_reset_com_module >= 720) { // Device hasn't received any Downlink in more than 12 hours?
        esp_restart();                             // Reset device
    }

    if (Ko.PUB.com_task == standby) {
        digitalWrite(LED_GREEN, HIGH);
    } else {
            digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
        }

    switch (Ko.PUB.com_task) {
            
            case module_init:                                        // Init Comm Module Task
                {
                debug_msg("Init Comm. Module");
                Found_Com_Module();                                  // Detect the Communication Module Programmed
                Ko.PUB.counter_for_downlink=Ko.CONFIG.DT.downlink_msg-1;
               
                if (Ko.CONFIG.DT.com_module_det==mod_wifi)            // If the programme module is WiFi
                    {
                    debug_msg("Open WiFi Captive Portal");
                    Wi.enter_to_AP_Mode();                            // Enter into AP Mode 
                    Ko.TIMERS.timer_wifi_AP=180;                      // Init Timer en AP Mode in 180 Seconds : User can modify this time
                    Ko.PUB.com_task=wifi_AP_mode;                     // Go to WiFi AP mode Task
                    }
                else
                    {
                    Turn_Off_Com_Module();
                    Ko.PUB.com_task=standby;
                    Ko.TIMERS.timer_publication=60;
                    }
                }
            break;
            
            case standby: {                                     // Module waiting until TPU is fulfilled
                // Time control to report parameters
                if ((Ko.TIMERS.timer_publication == 0) || (Ko.PUB.send_data == 1)) {
                    Ko.PUB.send_data = 0;
                    Ko.PUB.com_task = storing;
                    Ko.TIMERS.timer_publication = Ko.CONFIG.DT.TPU;
                    Ko.TIMERS.timer_publication *= 60;
                    int time_rnd = rand() % 30 - 15;
                    Ko.TIMERS.timer_publication += time_rnd; // Add a random value between -15 and 15 seconds to desynchronize multiple devices on a same network
                } else {
                    if (Ko.PUB.data_ACK_Rx) {               // Received some data out of time, this is asynchronous
                        if (Ko.CONFIG.DT.com_module_det == mod_lora)
                            Lo.Read_Rx_data_LoRa();
                        else if (Ko.CONFIG.DT.com_module_det == mod_sigfox)
                            Process_Rx_Data(BufferRx);
                        else if (Ko.CONFIG.DT.com_module_det == mod_wifi)
                            Process_Rx_Data(BufferRx);
                        Ko.PUB.data_ACK_Rx = 0;
                    }
                }
            }
            break;
            
            case storing:                             // Prepare data to send
                {
                Prepare_Data_To_Send();               // Prepare Uplink Message
                if(Ko.RTC.RTC_is_synchronized)        // If the RTC is synchronized?
                    { 
                buf_str[12] = (Ko.RTC.time_stamp >> 24) & 0xFF;     // Save the time Stamp
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
                
               
                Ko.PUB.com_task=joining;
                }
            break;
            
            case joining:                                               // Joining to the network task
                { 
                Ko.PUB.counter_of_attempts_to_send=0;
                if (Ko.CONFIG.DT.com_module_det==mod_none)              // If the module in not found
                    {
                    Ko.PUB.com_task=module_init;                        // Go to init task
                    }
                
                else if (Ko.CONFIG.DT.com_module_det==mod_wifi)         // Wifi Module
                    {
                    Ko.PUB.com_task=sending;
                    }
                
                else if (Ko.CONFIG.DT.com_module_det==mod_lora)         // Lora Module
                    {
                    if(Lo.Active_LoRaMod()==1)
                        {
                        if(Ko.LORA_MOD.join_status==1)
                            Ko.PUB.com_task=sending;
                        else
                            {
                            debug_msg("Joining to LoRaWAN Network");
                            Lo.joinNetwork(1);                          // Send the first join command
                            Ko.LORA_MOD.tries2join=0;                   // Init the Join counters
                            Ko.PUB.join_max_timer=0;                        // Init max time to join          
                            Ko.PUB.com_task=waiting_to_join;            // Go to waiting for join task
                            }
                        }    
                    else
                        {
                        Lo.Set_Sleep_LoRaMod();
                        Ko.PUB.com_task=standby;
                        }
                    }

                else if (Ko.CONFIG.DT.com_module_det==mod_sigfox)       // Sigfox Module
                    {
                    if(Si.Active_Modulo_Sigfox()==1)             
                        Ko.PUB.com_task=sending;
                    else
                        {
                        Si.Deep_Sleep_Sigfox();
                        Ko.PUB.com_task=standby;
                        }
                    }
                }
            break;
            
            case waiting_to_join:                           // Wainting for Join if LoRaWAN module is selected
                {
                
                int res=Lo.wait_for_join();                 // Check if Joined
                
                if (res==1)                                 //Join  correct?
                    {
                    debug_msg("Joined");
                    Ko.LORA_MOD.join_status=1; 
                    Ko.PUB.com_task=sending;               // Go to Sending
                    }
                else if (res==-1)                           // Join Fail?
                    {
                    debug_msg("Join Fail.. Trying again");
                    if(++Ko.LORA_MOD.tries2join<=10)      // tries 10 times to join the network
                        {
                        Lo.joinNetwork(1);
                        Ko.PUB.join_max_timer=0;
                        }
                    else
                        {
                        debug_msg("Join Fail.. Reseting LoRa Module");
                        Lo.Set_LoRaMod_OTAA();             // if fails reset LoRa Module
                        Ko.PUB.com_task=standby;
                        }
                    }
                
                else                                     // Wait for answer
                    {
                    if(++Ko.PUB.join_max_timer==30)          // if not recieve answer of the join result in 30 seconds
                        {
                        Lo.Set_Sleep_LoRaMod();          // Abort to send the message
                        Ko.PUB.com_task=standby;
                        }
                    }
                }
            break;
                        
            case sending:                                 // Sending data task
                {
                debug_msg("Sending Data");
                Transmit_Data(buf_str,Ko.PUB.wait_for_downlink);
                if(Ko.CONFIG.DT.com_module_det==mod_lora)
                    Ko.PUB.Rx_max_timer=30;
                else
                    Ko.PUB.Rx_max_timer=60;
                
                Ko.PUB.data_ACK_Rx=0;                   // Clear ACK Rx Flag
                Ko.PUB.counter_of_attempts_to_send=0;   // Init max attempts to send counter
                Ko.PUB.com_task=receiving;              // Go to Recieving task
                }
            break;
                
            case receiving:                             // Receiving task 
                {
                if (Ko.CONFIG.DT.com_module_det==mod_lora)
                    {
                    int res=Lo.wait_Rx_LoRa();
                    
                    if (res==1)                                                    // Correct Send
                        {
                        if(Ko.PUB.wait_for_downlink)
                            {
                            Ko.PUB.Rx_max_timer=0;                          
                            RTC.READ_TIME();                                        // Save the current Time_Stamp to get the correct time on next reception
                            Ko.RTC.previous_time_stamp=Ko.RTC.time_stamp;       
                            }
                        else
                            Ko.PUB.Rx_max_timer=0;   
                        }
                    else if(res==2)                                                 // Correct send and received downlink message.
                        {
                        Ko.PUB.Rx_max_timer=0;
                        Lo.Read_Rx_data_LoRa();                                     // Read the downlik data
                        if(Ko.PUB.wait_for_downlink)
                            {
                            RTC.READ_TIME();                                        // Save the current Time_Stamp to get the correct time on next reception
                            Ko.RTC.previous_time_stamp=Ko.RTC.time_stamp;        
                            }
                        }
                    else if(res==-1)                                                // if the module answer: Not Joined 
                        {
                        Ko.LORA_MOD.join_status=0;                                  // Abort transmission
                        Ko.PUB.com_task=standby;
                        }
                    else if(res==-2)                                                // AT_ERROR,  or other ==> Repit command
                        {
                        Ko.PUB.Rx_max_timer=0; 
                        }

                    }
                
                else if (Ko.CONFIG.DT.com_module_det==mod_wifi)
                    {
                    if(Ko.PUB.wait_for_downlink)                   // Waiting for Downlink?
                        {
                        int res=Wi.wait_rx_wifi();                  

                        if(res==1)
                            {
                            Ko.PUB.Rx_max_timer=0;
                            Process_Rx_Data(BufferRx);            // User can Procces Rx Data 
                            }
                        else if (res==-1)                         // Communication Error
                            { 
                            Ko.PUB.Rx_max_timer=0; 
                            }
                        }
                    else{
                        Ko.PUB.Rx_max_timer=0;                                // End of Rx
                        }
                    }                
                else if(Ko.CONFIG.DT.com_module_det==mod_sigfox)        
                    {
                    if(Ko.PUB.wait_for_downlink)                 // Wait for Downlink
                        {
                        if (Ko.PUB.data_ACK_Rx)
                            {
                            Si.rx_data_Sigfox();                // rx downlink data
                            Ko.PUB.Rx_max_timer=0;               
                            }
                        }
                    else{
                        Ko.PUB.Rx_max_timer=0;                  // End of Rx
                        }
                    }
                
                
                if(--Ko.PUB.Rx_max_timer<=0)                    // Wait until end of the rx max timer
                    {
                    Turn_Off_Com_Module();
                    Ko.PUB.com_task=standby;
                    }

                }           
            break;
            
           
            case wifi_AP_mode:                                          // Wifi in AP mode
                {    
                Wi.Portal_Manager();
                if(--Ko.TIMERS.timer_wifi_AP==0)
                    {
                    debug_msg("Close the Wifi Captive Portal");
                    Wi.end_AP_Mode();
                    Ko.PUB.com_task=standby;
                    Ko.TIMERS.timer_publication=10;
                    }
                }
            break;    
            

            
            }
}
//-------------------------------------------------------------------------------------------------------
void comms::Prepare_Data_To_Send(void)
/*
 Struct of Uplink Message for KONTROLOG-MINI 1.0 
 // Bytes in 00,  are reserve for KONTROLOG 10.0 (This device has moore inputs and outputs)
   
       byte 0  |  byte 1 | byte 2  |  byte 3 | byte 4  | byte 5 | byte 6 | byte 7 | byte 8 | byte 9 | byte 10 | byte 11
        type   | IN1 MSB | IN1 LSB | IN2 MSB | IN2 LSB |  00    |  00    |  00    |   VDC  |  00    |  00     | OUT2|OUT1
*/
{
    unsigned int data;
   
        buf_str[0] = Ko.PUB.alarm_type;                          // Type of Alarm data (For Example Alarm in Input 1)
        Ko.PUB.alarm_type=0;

        // Sensor 1
        data= Ko.IN1.value;
        buf_str[1] =(data>>8)&0x00FF;
        buf_str[2] =(data&0x00FF);

        // Sensor 2
        data = Ko.IN2.value;
        buf_str[3] = (data>>8)&0x00FF;
        buf_str[4] = (data&0x00FF);

        // DC Voltage 
        buf_str[8] = (unsigned char)  Ko.VDC.value;

        // Outpus status
        unsigned char BITS=0;                                     
        BITS|=(Ko.OUT2.value<<2);       // Bit 2  OUT2
        BITS|=(Ko.OUT1.value<<1);       // Bit 1  OUT1
        buf_str[11]|=BITS;
}

//-------------------------------------------------------------------------------------------------------
int comms::Process_Rx_Data(char *Buffer_Datos_Rx)
/* Struct of Downlink Data for KONTROLOG 10.0 and KONTROLOG-MINI 1.0
 // Bytes in 00,  are reserve for KONTROLOG 10.0 (This device has moore inputs and outputs)
   
    byte 0  |  byte 1 | byte 2   |  byte 3 | byte 4 | byte 5 | byte 6 | byte 7 
     Group  | Subgrup | Parameter|  Data 3 | Data 2 | Data 1 | Data 0 | New_Data

where:
    Byte 0: Group of Parameters,  example  Group 1 : Parameters for sensor 1
    Byte 1: SubGroup of Parameters,  User can use for different task
    Byte 2: Parameter,  example  Group = 1 (Sensor 1),  Parameter = 5 (offset) ==> Message for modify the Offset of the sensor 1
    Bytes 3-6: New Data or Value for the parameter to modify (User can use Float or Integer data)
    Byte 7: New Data:  The Platform indicates to KONTROLOG that has other parameter for downlink,  so the device has to prepare the next reception

Example of DownliNk Messages
    RX=01 00 05 0000000A 00     ==>  Command for modify the offset value of sensor 1, en 10
*/

{
int i=0,j=0;
char Aux[20]={0};
char Aux2[10]={0};
unsigned char new_ack=0, mod=0;
unsigned char Buff[12] = {0};
char *pos_rx;
                
    Ko.PUB.data_ACK_Rx=0;                       // Clear ACK Rx Flag bit
    
    pos_rx=strstr(Buffer_Datos_Rx, "RX=");     // The command has to start by RX= command

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
        Ko.GET_DATA.group=strtol(Aux2,0,16);            // Group 
        
        Aux2[0]=Aux[2];
        Aux2[1]=Aux[3];
        Aux2[2]=0;
        Ko.GET_DATA.subgroup=strtol(Aux2,0,16);         // Subgroup
        
        Aux2[0]=Aux[4];
        Aux2[1]=Aux[5];
        Aux2[2]=0;
        Ko.GET_DATA.parameter=strtol(Aux2,0,16);       // Parameter
        
        Aux2[0]=Aux[6];
        Aux2[1]=Aux[7];
        Aux2[2]=Aux[8];
        Aux2[3]=Aux[9];
        Aux2[4]=Aux[10];
        Aux2[5]=Aux[11];
        Aux2[6]=Aux[12];
        Aux2[7]=Aux[13];
        Aux2[8]=0;
        Ko.GET_DATA.value._long= strtoul(Aux2,0,16);      // Get the value (length or in float)   
          
        Aux2[0]=Aux[14];                    // New Downlink Data in the next command?
        Aux2[1]=Aux[15];
        Aux2[2]=0;
        new_ack=strtol(Aux2,0,16);
        if (new_ack>1)
            new_ack=0;
        
        
        mod=User_downlink_data();
        
        if (mod>0)
            {
            if(mod==1)
                {
                // Prepare the answer command
                Buff[0]= 0xF0;
                Buff[1]= Ko.GET_DATA.group;
                Buff[2]= Ko.GET_DATA.subgroup;
                Buff[3]= Ko.GET_DATA.parameter;
                Buff[4]= (Ko.GET_DATA.value._long>>24);
                Buff[5]= (Ko.GET_DATA.value._long>>16);
                Buff[6]= (Ko.GET_DATA.value._long>>8);
                Buff[7]= (Ko.GET_DATA.value._long);    
                }
            else
                {
                Buff[0]= 0xFE;              // Error in data received
                }
            
            if (new_ack)
                {
                Ko.PUB.Rx_max_timer=0;
                Ko.TIMERS.wait_for_sigfox_downlink=60;          // Wait for a new downlink
                }
            
            if (Ko.CONFIG.DT.com_module_prog == mod_wifi)          // Send the confirmation of received data
                Wi.SendPayloadWiFi(Buff,8,new_ack);
            else if(Ko.CONFIG.DT.com_module_prog == mod_sigfox)
                Si.sendPayload(Buff,8,new_ack);
            else if(Ko.CONFIG.DT.com_module_prog == mod_lora)
                Lo.sendCMSGHEX(Buff,8,new_ack);

            }
        }
        
    if (new_ack==1)
        return 1;
    else
        return 0;
                    
}

uint8_t comms::User_downlink_data(void)        
// Create you own code for proccess the data received in downlink messages
{ 
    uint8_t mod = 0, res = 0;

    switch (Ko.GET_DATA.group)
        {
        case 1: // Sensor 1 Parameters
            {
            }
        break;
        
        case 2: // Sensor 2 parameters
            {
            
            }
        break;
        

        case 6: // DC VOLTAGE
            {

            }
        break;

            
        case 11: // OUTPUT 1
            {

            }
        break;

        case 12: // OUTPUT 2
        {

        }
        break;
        
         

        case 20: //General Settings
        {
            if ((Ko.GET_DATA.parameter == 1) && ((Ko.GET_DATA.value._long >= 1) || (Ko.GET_DATA.value._long <= 1440))) { // Publication Time
                Ko.CONFIG.DT.TPU = Ko.GET_DATA.value._long;
                EEPROM_MEM.Write_Int_EEPROM(DIR_TPU, Ko.GET_DATA.value._long);
                Ko.TIMERS.timer_publication = 60;
                mod = 1;
                }

           else if ((Ko.GET_DATA.parameter == 4) && (Ko.GET_DATA.value._long == 1)) { // Reset Kontrolog Mini
                esp_restart();
                }

            else if ((Ko.GET_DATA.parameter == 5) && (Ko.GET_DATA.value._long == 1)) { // Factory Reset Params
                EEPROM_MEM.Write_Int_EEPROM(EEPROM_EEINI, 0);      // Change the EEPROM version reset the parameters to factory settings
                esp_restart();
                }

            }
        break;
            

        case 22: // RTC
            {
            if (Ko.GET_DATA.parameter == 1) { // Parameter to modify RTC Epoch Time
                if (Ko.CONFIG.DT.com_module_det == mod_lora) { 
                    RTC.READ_TIME(); // Read the actual time stamp
                    unsigned long time_adj = Ko.RTC.time_stamp - Ko.RTC.previous_time_stamp;
                    unsigned int tpu = Ko.CONFIG.DT.TPU * 60;
                    if ((time_adj >= 0) && (time_adj <= tpu + 40))
                        RTC.SET_TIME(Ko.GET_DATA.value._long + time_adj);
                } else if (Ko.CONFIG.DT.com_module_det == mod_sigfox) {
                    RTC.READ_TIME(); // Read the actual time stamp
                    unsigned long time_adj = Ko.RTC.time_stamp - Ko.RTC.previous_time_stamp;
                    RTC.SET_TIME(Ko.GET_DATA.value._long + time_adj);
                } else {
                    RTC.SET_TIME(Ko.GET_DATA.value._long);
                }
                mod = 0;
                RTC.READ_TIME();
            }

            else if (Ko.GET_DATA.parameter == 2) { // Time Zone
                mod = 0;
                if ((Ko.GET_DATA.value._long >= -14) && (Ko.GET_DATA.value._long <= 14)) {
                    EEPROM_MEM.Write_Int_EEPROM(RTC_DIR + 2, Ko.GET_DATA.value._long);
                    RTC.READ_TIME();
                    Ko.RTC.time_zone = Ko.GET_DATA.value._long;
                    mod = 1;
                    RTC.SET_TIME(Ko.RTC.time_stamp);
                    RTC.READ_TIME();
                    }
                }

            }
        break;

    }
        
        return mod;
}

//*******************************************************************************************************************************
// Rx and Tx Funtions
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
    Prepare_Command_to_Send(outData);          // Envía los datos
    // read ack or return payload
	receivedResult = get_answer_Msg(); 
	return receivedResult;
}

/*
recvMsg getRecvMsg()
	- Output: return receive message from LSM110A
*/
int comms::get_answer_Msg(void)
{
	int receivedResult;
    int wait_for_rx=20;
    Count_Rx=0;
    while (wait_for_rx-->0)
        {
        delay(10);
        Serial_Rx();
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

void comms::Prepare_Command_to_Send(char *comando) {
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
    Serial_Tx(BufferTx);  // This presumably starts the transmission process
}

void comms::BufferRx_Init(void)
{
    int i;

    // Initialize the receive buffer with zeros
    for (i = 0; i < sizeof(BufferRx); i++) {
        BufferRx[i] = 0;
    }
    
    Count_Rx = 0;    // Reset receive buffer index
    Dato_RxFlag=0;  // Reset Rx Flag
}

// Change .DEC data .HEX to prepare the data to send  by Sigfox or LoRaWAN in HEX 
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


// Transmit TX Buffer

void comms::Serial_Tx (char *buffer)                
{

    Cont_Tx=0;                      // Init the counter
    while (*(buffer+Cont_Tx)!=0) 
        { 
        Serial1.write(*(buffer+Cont_Tx));
        Serial1.flush();
        Cont_Tx++;
        }
    Ti.DELAY_TMR(20);              // Wait 20 ms for wait module confirmation
}

//----------------------------------------------------------------------------
//	SERIAL RX FROM COMM. MODULE
//----------------------------------------------------------------------------

void comms::Serial_Rx() 
{
    while (Serial1.available() > 0) 
        {
        DataRx = Serial1.read();            // Read the incoming byte
        BufferRx[Count_Rx] = DataRx;        // Store in the receive buffer
        Count_Rx++;
        //if(activate_debug)                  // UnComment if you want to see all module Answers
        //    Serial.print(DataRx);           
        if (Count_Rx >= sizeof(BufferRx)){
            Count_Rx = 0;                    // Reset index if buffer overflow
            }
        
        if(Serial1.available()==0)
            {
            Dato_RxFlag=1;
            if(Ko.CONFIG.DT.com_module_det==mod_sigfox)     // Check if there is a Sigfox Rx Data
                {
                if (strstr(BufferRx, "RX=")!=0)             // Answer of LSM110A
                    {
                    Ko.PUB.data_ACK_Rx=1;
                    Ko.TIMERS.timer_reset_com_module=0;
                    }
                else;
                }
            else if(Ko.CONFIG.DT.com_module_det==mod_lora)      // Check if there is a LoRa Rx Data
                {
                if (strstr(BufferRx, "EVT:RECV_UNCONFIRMED")!=0)        // Answer of LM110A            
                    {
                    Ko.PUB.data_ACK_Rx=1;
                    Ko.TIMERS.timer_reset_com_module=0;
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
        case led_off:                           // Off
            digitalWrite(LED_RED,HIGH);
            digitalWrite(LED_GREEN,HIGH);
            digitalWrite(LED_BLUE,HIGH);
        break;

        case led_white:                         // White       
            digitalWrite(LED_RED,LOW);
            digitalWrite(LED_GREEN,LOW);
            digitalWrite(LED_BLUE,LOW);
        break;
        
        case led_red:                          // Red 
            digitalWrite(LED_RED,LOW);
            digitalWrite(LED_GREEN,HIGH);
            digitalWrite(LED_BLUE,HIGH);
        break;
        
        case led_green:                        // Green    
            digitalWrite(LED_RED,HIGH);
            digitalWrite(LED_GREEN,LOW);
            digitalWrite(LED_BLUE,HIGH);
        break;
        
        case led_blue:                          // Blue
            digitalWrite(LED_RED,HIGH);
            digitalWrite(LED_GREEN,HIGH);
            digitalWrite(LED_BLUE,LOW);
        break;
        
        case led_violet:                         // Violet
            digitalWrite(LED_RED,LOW);
            digitalWrite(LED_GREEN,HIGH);
            digitalWrite(LED_BLUE,LOW);
        break;
        
        case led_yellow:                         // Yellow
            digitalWrite(LED_RED,LOW);
            digitalWrite(LED_GREEN,LOW);
            digitalWrite(LED_BLUE,HIGH);
        break;
        
        case led_navy_blue:                     // Navy Blue
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
    Ti.DELAY_TMR(100);
    
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
            SET_LED_RGB(led_yellow);
        else
            SET_LED_RGB(led_green);
        Ti.DELAY_TMR(300);
        SET_LED_RGB(led_off);                       
        Ti.DELAY_TMR(300);
        }
}

void comms::LED_ERR_MSG(void)
{
    uint8_t i;
    Ti.DELAY_TMR(100);
    for (i=0;i<3;i++)
        {
        SET_LED_RGB(led_red);
        Ti.DELAY_TMR(300);
        SET_LED_RGB(led_off);                      
        Ti.DELAY_TMR(300);
        }
}

void comms::debug_msg (String Mesagge2send)
{
    if(activate_debug)
        Serial.println(Mesagge2send);
    
}
