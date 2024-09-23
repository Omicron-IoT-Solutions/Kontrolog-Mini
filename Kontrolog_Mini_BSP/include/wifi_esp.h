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

#ifndef wifi_esp_h
#define	wifi_esp_h

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiMulti.h> 
#include <WebServer.h>
#include <HTTPClient.h>
#include "config.h"
#include "RTC_clock.h"
#include "eeprom_esp.h"
#include "communications.h"

#define activate_debug 1            // Comment this line if you don´t see debug messages

//------------------------------------------------------------------------------------------------------------------------------------------------------
// HTML CODE FOR LAUNCH INTERNAL WEB PORTAL
//------------------------------------------------------------------------------------------------------------------------------------------------------
const String HTTP_HEAD1 =  "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/><title>KONTROLOG WEB PORTAL </title>"
"<style>.c{text-align: center;} div,input{padding:5px;font-size:1em;} h1{margin:0;font-size:1.2rem;text-transform:uppercase;font-weight:700} h2{margin:0;font-size:1.0rem;font-weight:700}"
"h3{margin:0;font-size:0.7 rem;font-weight:400} input{width:95%;} body{text-align: center;font-family:verdana;} button{border:0;border-radius:0.3rem;background-color:#1fa3ec;color:#fff;line-height:2.4rem;font-size:1.2rem;width:100%;}" 
".q{float: right;width: 64px;text-align: right;} </style>"
"<script>function c(l){document.getElementById('s').value=l.innerText||l.textContent;document.getElementById('p').focus();}</script>"
"</head> <body><header><div class='col'><h1>KONTROLOG-MINI 1.0 CONFIG. </h1><br/></div></header>"
"<div style='text-align:left;display:inline-block;min-width:260px;'>";

const String ppal= "<form action='scan' method='get'><button>Configure WiFi</button></form><br/>"
//"<form action='Modules' method='get'><button>Wireless Mod. Setting </button></form><br/>"
//"<form action='Int_Sensors' method='get'><button>Sensors Setting </button></form><br/>"
"<form action='info' method='get'><button>More Info </button></form><br/>";
//"<form action='reset' method='get'><button>Reset Device</button></form><br/>";

const String HTTP_ITEM= "<div><a href='#p' onclick='c(this)'>{v}</a>&nbsp;<span class='q {i}'>{r}%</span></div>";

const String config_wifi_scan = "<form method='get' action='wifisave'><br>"
"<input id='s' name='ssid' length=32 placeholder='SSID'><br>"
"<input id='p' name='pass' length=64 type='text' placeholder='password'><br>"
"<br><button type='submit'>save</button></form>"
"<div class=\"c\"><a href=\'scan'>Scan</a><br>";

const String no_wifi_save = "<div><h3>Credentials NOT Saved <br/> Invalid SSID</h3><br/>";
const String wifi_save = "<div><h3>Trying to connect your Device to network...<br> Wait for 30 seconds and press to view if <br> WiFi connection was correct</h3><br/>";
const String config_back = "<form action='/' method='get'><button> Back to Home </button></form>";    //ppal
const String Test_Config = "<form action='test' method='get'><button> WiFi Results </button></form>";

const String HTTP_END =  "<div style='text-align:center;'><h2>Omicron IoT Solutions</h2><br/></div> </div></body></html>";



//------------------------------------------------------------------------------------------------------------------------------------------------------
// Data Stored for Wifi Configuration in EEPROM

const unsigned int
dir_base= 0x1000,                // Base address for stored wifi configurations into EEPROM
dir_port=dir_base+0,             // Server port 
dir_server=dir_base+2,           // Server

add_count_config=dir_base+38,     // Stored wifi configuration counter
dir_config1=dir_base+40,         // Is ssid 1 configured?
add_ssid1=dir_base+42,           // ssid 1
dir_pass1=dir_base+74,           // pass 1

dir_config2=dir_base+110,        // Is ssid 2 configured?
add_ssid2=dir_base+112,          // ssid 2
dir_pass2=dir_base+144,          // pass 2

dir_config3=dir_base+170,        // Is ssid 2 configured?
add_ssid3=dir_base+172,          // ssid 3
dir_pass3=dir_base+204;          // pass 3


class wifi_esp_class {

public:
    // Network Parameters
    char  ssid [32];                     // Network SSID 
    char  pass[32];                      // your network password (use for WPA, or use as key for WEP)
  
    uint8_t Init_WiFi_Connection(void);
    void Launch_Config_Portal(void);
    void Deep_Sleep_Wifi(void);
    void Portal_Manager(void);
    void config_portal_end(void);
    uint8_t SendPayloadWiFi(unsigned char *outData, const unsigned char len, unsigned char downlink);
    uint8_t enter_to_AP_Mode(void);
    int end_AP_Mode(void);
    uint8_t wait_rx_wifi(void);

private:

    WiFiMulti wifiMulti1;
    WiFiClient WiFi_client;


    char  server_default[80]= "parse.ubidots.com";       // Ubidots server
    int   Port_default = 80;                             // Default port
    char  ssid_default [30] =  "YOUR_SSID";              // Replace with your Default SSID
    char  pass_default [30] =  "YOUR_DEFAULT_PASSWORD";  // Replace with your default Password

    

// Variables
   
    int  WiFi_Config_Response=0;
    String Last_IP = "0.0.0.0";
    String Rx_Data;

    int  get_WiFi_configuration(void);
    uint8_t get_stored_network (int dir_conf,  int dir_red, int dir_password);
    uint8_t get_stored_position(void);
    String Correct_Symbols(String);
    void send_info (void);
    void Config_Response (void);
    void wifi_conf1();
    int getRSSIasQuality(int);
    void handle_wifi_config_scan (void);
};

class ubidots_class {

public:
int8_t send_data_to_server(String);
String get_downlink_data(void);
char Procces_Rx_Data (String Data);
float parsetofloat(String,int);
uint32_t get_utc();


private:
unsigned int hexToDec(String hexString);
uint8_t hexAsciiToDec(char Data);

};

extern wifi_esp_class Wi;
extern ubidots_class Ubi;
#endif