/*
KONTROLOG-MINI LIBRARIES FOR ARDUINO
By Omicron IoT Solutions
	

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


#include "wifi_esp.h"

// Variables 

WiFiClient WiFi_client;         // Constructor for Wifi Client Services
HTTPClient http2;
extern wifi_esp_class Wi;
 

// Ubidots Parser for Uplink and Downlink
String Ubiserver = "http://things.ubidots.com/api/v1.6/devices/kontrolog-mini-";               // Free Ubidots Server
String Ubi_token=  "BBUS-xxxxxxxxxxxxxxxxxxxxxxxxxxx";                                         // Replace with Your Ubitoken


/*---------------------------------------------------------------------------------------------------------------------
//  ---- Send Data to Ubidots Server

// This method makes a HTTP connection to Ubidots server:
// Create and Send .json Object with the data of the INPUTS and OUPUTS from Communications Class (Prepare_Data_To_Send) 

// Inputs  String with  .HEX  Data to send
// Result:

// Result:
  String with rx data

//---------------------------------------------------------------------------------------------------------------------
*/
int8_t ubidots_class::send_data_to_server(String Data_to_Send) 
{
    int8_t result=0;
    JsonDocument  Data_json;

// Get RSSI
    int rssi = WiFi.RSSI();
    Data_json["rssi"]["value"]=rssi;
    Ko.WIFI_MOD.rssi=rssi;


// Get timeStamp from the Data String   
    String Data_time =  Data_to_Send.substring(24,32);
    int64_t timeStamp =hexToDec(Data_time);
    Data_to_Send.remove(24,8);                    // Remove the last 4 Bytes:  Time Stamp

//  Create .json Object
    int alarm_type =hexToDec(Data_to_Send.substring(0,1));
    Data_json["Alam_type"]["value"]=alarm_type;
    Data_json["IN1"]["value"]= parsetofloat(Data_to_Send.substring(1,4),Ko.IN1.resolution);
    Data_json["IN2"]["value"]= parsetofloat(Data_to_Send.substring(4,7),Ko.IN2.resolution);
    Data_json["VDC"]["value"]= parsetofloat(Data_to_Send.substring(16,18),Ko.VDC.resolution);
     
    int io = hexToDec(Data_to_Send.substring(22,24));
    int R1 = (io&0x02)/2; //Output 1
    Data_json["OUT1"]["value"] = R1;
    
    int R2 = (io&0x04)/4; //Output 2
    Data_json["OUT2"]["value"] = R2;

    if (timeStamp>1609462800)     // Is Timestamp Correct?    (Compare with Jan 1, 2021)
      {
      Data_json["Alam_type"]["timestamp"]=timeStamp*1000;
      Data_json["IN1"]["timestamp"]=timeStamp*1000;
      Data_json["IN2"]["timestamp"]=timeStamp*1000;
      Data_json["VDC"]["timestamp"]=timeStamp*1000;
      Data_json["OUT1"]["timestamp"]=timeStamp*1000;
      Data_json["OUT2"]["timestamp"]=timeStamp*1000;
      }

 
// Connect to to Ubidots Server
    String serverPath = Ubiserver + Ko.WIFI_MOD.MAC;
    if(http2.begin(WiFi_client, serverPath))
      {
      http2.addHeader("Content-Type", "application/json");
      http2.addHeader("X-Auth-Token", Ubi_token);

      String Json="";
      serializeJson(Data_json, Json);
      int httpCode=http2.POST(Json);
      if (httpCode == HTTP_CODE_OK)
        {
         String payload = http2.getString();
         result= httpCode;
        }
      else 
        result= httpCode;
      }
    else
      {
      result = -1;
      }
    
    http2.end();
    return result;
}

  /* ------------------------------
//  ---- Receive data from  Ubidots Server 

// This method makes a HTTP connection to Ubidots server:

// Inputs :
   Read the last values of variable "downlink" From Ubidots Server
   Use the Time Epoch from the last send data

// Result:
  String with rx data
  // ------------------------------
  */
String ubidots_class::get_downlink_data(void)
{
    String URL_Downlink;
    String rx_line="";

    URL_Downlink = Ubiserver + Ko.WIFI_MOD.MAC + "/downlink/values/?start="+String(Ko.RTC.time_stamp-Ko.RTC.time_zone*3600-Ko.CONFIG.DT.TPU*60)+"000";
    if(http2.begin(WiFi_client,URL_Downlink))
      {
      http2.addHeader("X-Auth-Token", Ubi_token);
      int httpCode = http2.GET();
      if (httpCode > 0) 
          {
          if (httpCode == HTTP_CODE_OK) 
              {
              rx_line = http2.getString();
              if(activate_debug)
                Serial.println(rx_line);  
              }
          }
      }
    http2.end();
    return rx_line;
  }

//---------------------------------------------------------------------------------------------------------------------
// Function: 
// User manage the Ubidots server response
//---------------------------------------------------------------------------------------------------------------------

char ubidots_class::Procces_Rx_Data(String Data)
{

return 1;
}



// Converting String Hex to Decimal:

// NOTE: This function can handle a positive hex value from 0 - 65,535 (a four digit hex string).
// For larger/longer values, change "unsigned int" to "long" in both places.


unsigned int ubidots_class::hexToDec(String hexString) {
  
  unsigned int decValue = 0;
  int nextInt;

  for (int i = 0; i < hexString.length(); i++) {
    
    nextInt = int(hexString.charAt(i));
    if (nextInt >= 48 && nextInt <= 57) nextInt = map(nextInt, 48, 57, 0, 9);
    if (nextInt >= 65 && nextInt <= 70) nextInt = map(nextInt, 65, 70, 10, 15);
    if (nextInt >= 97 && nextInt <= 102) nextInt = map(nextInt, 97, 102, 10, 15);
    nextInt = constrain(nextInt, 0, 15);
    decValue = (decValue * 16) + nextInt;
  }
  
  return decValue;
}

// Converting  Ascii Hex Char to Decimal (0 to 15):
uint8_t ubidots_class::hexAsciiToDec(char Data) {

uint8_t decValue = 0;
    if (Data >= 48 && Data <= 57) Data = map(Data, 48, 57, 0, 9);
    else if (Data >= 65 && Data <= 70) Data = map(Data, 65, 70, 10, 15);
    else if (Data >= 97 && Data <= 102) Data = map(Data, 97, 102, 10, 15);
    else
    Data = 15;
    
    Data = constrain(Data, 0, 15);
    
    return Data;
    
    }

float ubidots_class::parsetofloat(String hexString, int pow)
{   
     float input = hexToDec(hexString);           // The variables in KONTROLOG are stored in 12 Bits
     if(input>2047)                               // if the value is grater than 2047 indicate that It is negative                    
        input-=4096;                              // if it´s negative,  rest from 4096 = 2^12

     if(pow==1)
        return input/10.0;
     else if(pow==2)
        return input/100.0;
    else
      return input;
}

// GET UTC Epoch from Ubidots Server
uint32_t ubidots_class::get_utc() {

   uint32_t New_epoch=0;
   String  url = "http://industrial.api.ubidots.com/api/v1.6/utils/utcnow/";
   http2.begin(WiFi_client, url);
   int httpCode = http2.GET();
   if (httpCode > 0) 
          {
          if (httpCode == HTTP_CODE_OK) 
              {
              String rx_line = http2.getString();
              char epocharray[12];
              rx_line.toCharArray(epocharray,11);
              New_epoch=atol(epocharray);
              }
          }
    http2.end();
    return New_epoch;
}