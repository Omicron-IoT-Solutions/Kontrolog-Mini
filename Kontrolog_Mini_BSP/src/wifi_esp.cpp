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

//---------------------------------------------------------------------------------------------------------------------
// ---- START WiFi CONNECTION
// Function: 
// The configuration web page is launched
// Return res:
// 0: Network configured, but connection could not be established
// 1: Connected to preconfigured WiFi network
// 2: Network not configured, no connection established
//---------------------------------------------------------------------------------------------------------------------
#include "wifi_esp.h"

ubidots_class Ubi;

WebServer server_esp(80);       // Invoque Services for Autoconnect portal

uint8_t wifi_esp_class::Init_WiFi_Connection(void)
{ 
int counter;
    
    if (Ko.WIFI_MOD.on_off==1)
      {
      delay(1);
      return 1;
      }

    if (get_WiFi_configuration())                    // Get stored SSID and Password In EEPROM
        {
        WiFi.softAPdisconnect(true);                 // Close mode SoftAP
        delay(100); 
        WiFi.disconnect();                           // Disconnect from previos network
         
        WiFi.mode(WIFI_OFF);                         // Turn off the module
        delay (100);                                


        counter=0;
        while ((wifiMulti1.run() != WL_CONNECTED)&&(counter <5))         // Wait for correct connection
                {
                delay (490);
                digitalWrite(LED_GREEN,HIGH);         
                delay(10);
                digitalWrite(LED_GREEN,LOW);
                counter++; 
                }
                
        if (wifiMulti1.run()== WL_CONNECTED)                    
              {    
              Last_IP= WiFi.localIP().toString();       
              Ko.WIFI_MOD.on_off=1;                                     // This flag indicates WiFi is connected to the network
              if(activate_debug)
                Serial.printf(" connected to %s\r\n", WiFi.SSID().c_str());

              // Get MAC in HEX format
              byte mac[6];
              WiFi.macAddress(mac);
              Ko.WIFI_MOD.MAC = String(mac[0], HEX)+ String(mac[1], HEX)+ String(mac[2], HEX)+ String(mac[3], HEX)+ String(mac[4], HEX)+ String(mac[5], HEX);
              return 1;
              }
        else
            return 0;
        }

    else
         return -1;
}

int wifi_esp_class::get_WiFi_configuration(void)
{ 
        get_stored_network (dir_config1,add_ssid1,dir_pass1);
        get_stored_network (dir_config2,add_ssid2,dir_pass2);
        get_stored_network (dir_config3,add_ssid3,dir_pass3);
        wifiMulti1.addAP(ssid_default,pass_default);                  // Add the default Wifi Network
        return 1;
}

// get stored network data into EEPROM
uint8_t wifi_esp_class::get_stored_network (int conf_Address,  int net_address, int pas_address)
{
String ssid_read;
String pass_read;
int    ssid_size;
int    pass_size;
int    net=0;
  
        int config1= EEPROM_MEM.Read_1Byte_EEPROM (conf_Address);
        if (config1==1)
           {
              ssid_read=EEPROM_MEM.Read_String_EEPROM(net_address);                             //Read ssid and password
              if (ssid_read.equals(""))
                  EEPROM_MEM.Write_1Byte_EEPROM(conf_Address,0);
              else
                {
                pass_read=EEPROM_MEM.Read_String_EEPROM(pas_address);                         //read the password
                ssid_size=ssid_read.length()+1;
                ssid_read.toCharArray(ssid,ssid_size);        
                pass_size=pass_read.length()+1;
                pass_read.toCharArray(pass,pass_size);
                wifiMulti1.addAP(ssid,pass);
                net++;
                }
             }
      return net;
}



//---------------------------------------------------------------------------------------------------------------------
// ----DEVICE NETWORK CONFIGURATION ----

// Function: 
// Create an WiFi AP and Launch a Captive Portal to connect to ESP32 To be able to configure on the device: SSID, PASSWORD
// 

// Result: res;   
// 1: Correct: Parameters set and successful connection to the network
// 0: Not correct: It is not possible to connect to the  network

//---------------------------------------------------------------------------------------------------------------------
void wifi_esp_class::Launch_Config_Portal(void)
{
char ssid_portal_a[30];
String ssid_portal = "Kontrolog_";
char  Wait_Counter;
char  Result;
int Aux_Counter;
byte mac[6];
WiFi.macAddress(mac);

    ssid_portal = ssid_portal + String(mac[0], HEX)+ String(mac[1], HEX)+ String(mac[2], HEX)+ String(mac[3], HEX)+ String(mac[4], HEX)+ String(mac[5], HEX);
    ssid_portal.toCharArray(ssid_portal_a,30); 
    
    WiFi.mode (WIFI_OFF);               // Turn off preliminar connections
    delay (500);                        
    
    WiFi.mode (WIFI_AP_STA);            // Enable Wifi AP and Soft Mode
    delay (500); 
      
    WiFi.softAP(ssid_portal_a);         // Launch the AP           
    delay (1000);


    server_esp.on("/", []() {server_esp.send(400, "text/html",HTTP_HEAD1+ppal+HTTP_END);}); 

    server_esp.on("/scan", std::bind(&wifi_esp_class::handle_wifi_config_scan,this));         // Calls the wifi_conf function if the Connect or Scan button is pressed
    server_esp.on("/wifisave", std::bind(&wifi_esp_class::wifi_conf1,this));                  // Save WiFi network configuration parameters

    server_esp.on("/info",std::bind(&wifi_esp_class::send_info,this));                        //  More info page

    server_esp.on("/test",std::bind(&wifi_esp_class:: Config_Response,this));                 // Response to the configuration test

    server_esp.begin();                                                                       // Start the server


}
// Web Portal root Manager
void wifi_esp_class:: Portal_Manager(void)
{
  server_esp.handleClient();
}

// Get the best stored position for Network into EEPROM.
uint8_t  wifi_esp_class::get_stored_position(void)
{
char res;

        int counter_config=EEPROM_MEM.Read_1Byte_EEPROM (add_count_config);               // Get the configuration counter
        if ((counter_config<1)|(counter_config>3))                                        // If it is not in a valid range, position 1 is assigned.                       
                  {counter_config=1; EEPROM_MEM.Write_1Byte_EEPROM(add_count_config,2);}
        
// First check that the network does not exist
           if (EEPROM_MEM.Read_String_EEPROM(add_ssid1).equals(ssid))
              {res = 1;}
           else if (EEPROM_MEM.Read_String_EEPROM(add_ssid2).equals(ssid))
              {res = 2;}
           else if  (EEPROM_MEM.Read_String_EEPROM(add_ssid3).equals(ssid))
              {res = 3;}
           else {
// Otherwise, the new configured networks are recorded in order
           int count2=counter_config+1;
           if (count2>3)
               count2=1;
  
            EEPROM_MEM.Write_1Byte_EEPROM(add_count_config,count2);
            res = counter_config;
            }           

 return res;
}


void wifi_esp_class:: config_portal_end(void)
{
  server_esp.close();
  WiFi.softAPdisconnect(true);             // End App SoftAP
  delay(100); 
  WiFi.mode(WIFI_STA);                     // Start WiFi in Station Mode
  delay(100);
}

// ----------------------------------------------------------------------------------------------------------------------------
// Runs network scan, and lists them on the WEB page so that the user can select one and register the network Password
void wifi_esp_class::handle_wifi_config_scan (void)
{
    int i;
    String web_page= HTTP_HEAD1;
    
    int n = WiFi.scanNetworks();
    
    if (n == 0) 
      {
      web_page += F("No networks found. Refresh to scan again.");
      } 
    else 
        {   
        int index[n];
        for (i = 0; i < n; i++) {
          index[i] = i;
            }
        // Organize according to power
        for (i = 0; i < n; i++) 
          {
          for (int j = i + 1; j < n; j++) 
              {
              if (WiFi.RSSI(index[j]) > WiFi.RSSI(index[i])) 
                {
                std::swap(index[i], index[j]);
                }
              }
          }

  // remove duplicates ( must be RSSI sorted )
        String cssid;
        for (int i = 0; i < n; i++) 
          {
          if (index[i] == -1) continue;
            cssid = WiFi.SSID(index[i]);
          for (int j = i + 1; j < n; j++) 
            {
            if (cssid == WiFi.SSID(index[j])) 
              {
              index[j] = -1; // set dup aps to index -1
              }
            }
          }
 
 //display networks in page    
        for (int i=0; i<n ; i++)
          {
          if (index[i] == -1) continue; // skip dups
          int quality =getRSSIasQuality(WiFi.RSSI(index[i]));
          if (quality>=30) 
              {
              String item = HTTP_ITEM;
              String rssiQ;
              rssiQ += quality;
              item.replace("{v}", WiFi.SSID(index[i]));
              item.replace("{r}", rssiQ);
              if (WiFi.encryptionType(index[i]) != 0) {
                item.replace("{i}", "l");
              } 
              else 
              item.replace("{i}", "");
              
              web_page += item;
              delay(0);
              }
          else;
          }
        }
  web_page+=config_wifi_scan;
  web_page+=HTTP_END;
  

server_esp.send(400, "text/html", web_page); 
}

int wifi_esp_class::getRSSIasQuality(int RSSI) { 
  int quality = 0;

  if (RSSI <= -100) {
    quality = 0;
  } else if (RSSI >= -50) {
    quality = 100;
  } else {
    quality = 2 * (RSSI + 100);
  }
  return quality;
}

//---------------------------------------------------------------------------------------------------------------------
// --- Service launched when Device is Configured --- 
// 
// Function:
// Launched when pressing the "OK" button on the configuration page
// Result Variable Message:
// Not configured =0;
// Correct_Configuration = 1;
// Incorrect Password or SSID configuration = 2;
// Incorrect server or port configuration =3
//---------------------------------------------------------------------------------------------------------------------


void wifi_esp_class::wifi_conf1()                                     
{    
int ssid_size=0;
int pass_size=0;

int  counter;
WiFiMulti wifiMulti_test;

        String getssid = server_esp.arg("ssid");                  // Receive the values ​​sent by the portal
        String getpass = server_esp.arg("pass");
        
        getssid=Correct_Symbols(getssid);                         // replace the UTF8 symbols with the right symbol
        getpass=Correct_Symbols(getpass);
      
        ssid_size=getssid.length()+1;                             // size of ssid and password
        pass_size=getpass.length()+1;
        
        getssid.toCharArray(ssid,ssid_size);                      // String to char array
        getpass.toCharArray(pass,pass_size);


        if (ssid_size>1)
          {
          server_esp.send(400, "text/html",HTTP_HEAD1+wifi_save+Test_Config+HTTP_END);        
          
          WiFi.disconnect();
          delay(500);

          wifiMulti_test.addAP(ssid,pass);
          delay(10);        
              
          while ((wifiMulti_test.run() != WL_CONNECTED)&&(counter <5))         // Wait for connection
                {
                delay (500);
                Serial.print(".");
                counter++; 
                }
        
        if ((wifiMulti_test.run()== WL_CONNECTED))                             // Correct?
            {
            WiFi_Config_Response=1;
            int sotored_position = get_stored_position();
            if (activate_debug)
                {
                Serial.println("");
                Serial.printf("SSID %s saved in position: %d\r\n",getssid,sotored_position);
                Serial.println();
                }
           if (sotored_position==1)
                {
                EEPROM_MEM.Write_1Byte_EEPROM(dir_config1,1);                       // Record "configured" so that when the board boots, it knows that it has correct data, so it proceeds to try a connection.
                EEPROM_MEM.Write_String_EEPROM(add_ssid1,getssid);                  // Stored ssid 1
                EEPROM_MEM.Write_String_EEPROM(dir_pass1,getpass);                  // Stored Password 1
                }
           else if (sotored_position==2)
                { 
                EEPROM_MEM.Write_1Byte_EEPROM(dir_config2,1);                       
                EEPROM_MEM.Write_String_EEPROM(add_ssid2,getssid);               
                EEPROM_MEM.Write_String_EEPROM(dir_pass2,getpass);                
                }
              
           else
                {
                EEPROM_MEM.Write_1Byte_EEPROM(dir_config3,1);                      
                EEPROM_MEM.Write_String_EEPROM(add_ssid3,getssid);                
                EEPROM_MEM.Write_String_EEPROM(dir_pass3,getpass);                
                }               

            }
            else
               WiFi_Config_Response=3;
          }
        else
            server_esp.send(400, "text/html",HTTP_HEAD1+no_wifi_save+config_back+HTTP_END);

}


//---------------------------------------------------------------------------------------------
void wifi_esp_class::Config_Response (void)
{
    if (WiFi_Config_Response==1)
        server_esp.send(400, "text/html",HTTP_HEAD1+"<div style='text-align:center;'> <h3> !! CORRECT  !! <br>  Settings  Saved </h3></div><br/>"+config_back +HTTP_END);
    else if (WiFi_Config_Response==2)
        server_esp.send(400, "text/html",HTTP_HEAD1+"<div style='text-align:center;'> <h3> !! ERROR: Check Your Server Settings !! </h3></div><br/>"+ config_back + HTTP_END);
    else 
        server_esp.send(400, "text/html",HTTP_HEAD1+"<div style='text-align:center;'> <h3> !! ERROR: Check Your SSID And Password !! </h3></div><br/>"+config_back+HTTP_END);
}


//---------------------------------------------------------------------------------------------
void wifi_esp_class::send_info (void)
{
  String page = HTTP_HEAD1;

  page += "<h3>Station MAC: ";
  page += WiFi.macAddress();
  page += "</h3><br>";
  page += "<h3>Software Version 1.0";
  page += "</h3><br>";
  page += config_back;
  page += HTTP_END;
  server_esp.send(200, "text/html", page);
}

//---------------------------------------------------------------------------------------------------------------------
//When the form data is passed through GET mode, the special characters are replaced by UTF-8 code, this function replaces all UTF codes of special characters 
//by the corresponding special character, that is, for explample if it finds a %23 in the string it will replace it with a #.
String wifi_esp_class::Correct_Symbols(String a){ 
     a.replace("%C3%A1","á");
     a.replace("%C3%A9","é");
     a.replace("%C3%A","í");
     a.replace("%C3%B3","ó");
     a.replace("%C3%BA","ú");
     a.replace("%21","!");
     a.replace("%23","#");
     a.replace("%24","$");
     a.replace("%25","%");
     a.replace("%26","&");
     a.replace("%2F","/");
     a.replace("%28","(");
     a.replace("%29",")");
     a.replace("%3D","=");
     a.replace("%3F","?");
     a.replace("%27","'");
     a.replace("%40","@");
     a.replace("%C2%BF","¿");
     a.replace("%C2%A1","¡");
     a.replace("%C3%B1","ñ");
     a.replace("%C3%91","Ñ");
     a.replace("+"," ");
     a.replace("%2B","+");
     a.replace("%22","\"");
     return a;
 }

//---------------------------------------------------------------------------------------------------------------------
// Turn Off Wifi Module
void wifi_esp_class::Deep_Sleep_Wifi(void)
{
      
      WiFi.softAPdisconnect(true);                // Turn off Wifi Soft Mode
      delay(100); 
      WiFi.disconnect();                          // Disconnect from network                 
         
      WiFi.mode(WIFI_OFF);                        // Turn Off Wifi Module in ESP32
      delay (100);                                
      Ko.WIFI_MOD.on_off=0;                       // Flag indicates not Wifi Connection
}

//---------------------------------------------------------------------------------------------------------------------
uint8_t wifi_esp_class::SendPayloadWiFi(unsigned char *outData, const unsigned char len, unsigned char downlink)
{
    uint8_t result=0;
	  char hex_str[40];
    char data2Send[40]={0,0};
    if(Init_WiFi_Connection())
      {
	    
      Comms.ASCII2Hex(outData, len, hex_str);
      strcat(data2Send,hex_str);                  // Add Payload
      Ubi.send_data_to_server(data2Send);

      if(downlink){ 
        RTC.GET_TIME_FROM_SERVER();               // Get Epoch from Ubiserver
        Rx_Data=Ubi.get_downlink_data();          // Get Downlink Data from Ubidots
        }
      result=1;
      }
  return result;
}


uint8_t wifi_esp_class::wait_rx_wifi(void)
{
  Ubi.Procces_Rx_Data(Rx_Data);                 // User can handle the downlink data frorm server

return 1;
}

/*
*Enter WiFi to AP mode

*/
uint8_t wifi_esp_class::enter_to_AP_Mode(void)
{
  Launch_Config_Portal();
  return 1;
}


/*
End to Wifi Ap Mode
*/
int wifi_esp_class::end_AP_Mode(void)
{
  Deep_Sleep_Wifi(); 
  return 1;
}


