//---------------------------------------------------------------------------------------------------------------------
//  ---- INICIA CONEXIÓN WiFi
// Función: 
// Se lanza la página web de configuración
// Retorna  res:
// 0:  Red Configurada, pero no se pudo establecer conexión
// 1:  Conectado a red WiFi preconfigurada
// 2:  Red no configurada,  no se establece conexión
//---------------------------------------------------------------------------------------------------------------------
#include "wifi_esp.h"

ubidots_class Ubi;

WebServer server_esp(80);       // Invoque Services for Autoconnect portal


uint8_t wifi_esp_class::Init_WiFi_Connection(void)
{ 
int cuenta;
    
    if (Ko.WIFI_MOD.on_off==1)
      {
      delay(1);
      return 1;
      }

    if (get_WiFi_configuration())
        {
        WiFi.softAPdisconnect(true);                // Deconecta modo SoftAP
        delay(100); 
        WiFi.disconnect();                           // Se desconecta preliminarmente
         
        WiFi.mode(WIFI_OFF);                         // Apaga toda conexión preliminar
        delay (100);                                // si tenemos una conexión previa establecida


        cuenta=0;
        while ((wifiMulti1.run() != WL_CONNECTED)&&(cuenta <5))         // Espera por conexión correcta
                {
                delay (490);
                digitalWrite(LED_GREEN,HIGH);         
                delay(10);
                digitalWrite(LED_GREEN,LOW);
                cuenta++; 
                }
                
        if (wifiMulti1.run()== WL_CONNECTED)
              {    
              Last_IP= WiFi.localIP().toString();
              Ko.WIFI_MOD.on_off=1;                     // WiFi is connected to the network
              if (debug_activo)
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
        get_stored_network (dir_config1,dir_ssid1,dir_pass1);
        get_stored_network (dir_config2,dir_ssid2,dir_pass2);
        get_stored_network (dir_config3,dir_ssid3,dir_pass3);
        wifiMulti1.addAP(ssid_default,pass_default);                  // Adiciona red por defecto
        return 1;
}

uint8_t wifi_esp_class::get_stored_network (int dir_conf,  int dir_red, int dir_password)
{
String ssid_leida;
String pass_leido;
int    ssid_tamano;
int    pass_tamano;
int    red=0;
  
        int config1= EEPROM_MEM.EEPROM_READ_1BYTE (dir_conf);
        if (config1==1)
           {
              ssid_leida=EEPROM_MEM.lee_string_eeprom(dir_red);                       //leemos ssid y clave
              if (ssid_leida.equals(""))
                  EEPROM_MEM.EEPROM_WRITE_1BYTE(dir_conf,0);
              else
                {
                pass_leido=EEPROM_MEM.lee_string_eeprom(dir_password);                       //Se lee el password
                ssid_tamano=ssid_leida.length()+1;
                ssid_leida.toCharArray(ssid,ssid_tamano);        
                pass_tamano=pass_leido.length()+1;
                pass_leido.toCharArray(pass,pass_tamano);
                wifiMulti1.addAP(ssid,pass);
                red++;
                }
             }
      return red;
}



//---------------------------------------------------------------------------------------------------------------------
//  ----CONFIGURACIÓN DE RED DEL DISPOSITIVO ----

// Función: 
// Crea un AP y servicio para conectarse al ESP_8266 Para poder configurar en el dispositivo:
// SSID, CLAVE, Serividor y Puerto de Transmisión, Rata de Baudios y Si el dispositivo puede dormirse o no.
// A esta función entra si:
// 1)  El dispositivo al que esta conectado por puerto serial esta configurado con ID00 (ID reservada para Configuración, que se pone de forma manual en el dispositivo).

//  Resultado: res;   
//  1:  Correcto : Parámetros Establecidos y conexión exitosa a la red
//  0:  No correcto:   No es posible conectarse a la red establecida por Defecto

//---------------------------------------------------------------------------------------------------------------------
void wifi_esp_class::Launch_Config_Portal(void)
{
char b[30];
String Red = "IoTDevice_";
char  Contad_espera;
char  res;
int cuenta;
byte mac[6];
WiFi.macAddress(mac);

   
     /*cuenta=0;
     while (cuenta <5)         // Espera por conexión correcta
         {
          delay (500);
          Serial.print(".");
          cuenta++; 
          }*/

    Red = Red + String(mac[0], HEX)+ String(mac[1], HEX)+ String(mac[2], HEX)+ String(mac[3], HEX)+ String(mac[4], HEX)+ String(mac[5], HEX);
    Red.toCharArray(b,30); 
    
    WiFi.mode (WIFI_OFF);               // Se apago toda conexión WiFi Preliminar
    delay (500);                        // Correción 23/03/16
    
    WiFi.mode (WIFI_AP_STA);            // Se habilita WiFi AP y STA simultáneamente     23/05/16
    delay (500); 
      
    WiFi.softAP(b);            
    delay (1000);


    server_esp.on("/", []() {server_esp.send(400, "text/html",HTTP_HEAD1+ppal+HTTP_END);}); 

    server_esp.on("/scan", std::bind(&wifi_esp_class::handle_wifi_config_scan,this));         // LLama a la función wifi_conf si se presiona el boton Conectar o Scan
    
    server_esp.on("/wifisave", std::bind(&wifi_esp_class::wifi_conf1,this));                 // Salva parámetros de configuración de red WiFi

    server_esp.on("/info",std::bind(&wifi_esp_class::send_info,this));                      //  More info page

    server_esp.on("/test",std::bind(&wifi_esp_class:: Config_Response,this));                // Respuesta al test de configuración*/

    server_esp.begin();                                     // Inicia el servidor


}
// Web Portal root Manager
void wifi_esp_class:: Portal_Manager(void)
{
  server_esp.handleClient();
}

// Determina la posición adecuada de configuración.
uint8_t  wifi_esp_class::get_stored_position(void)
{
char res;

        int cont_config=EEPROM_MEM.EEPROM_READ_1BYTE (dir_cont_config);         // Se obtiene el contador de configuración
        if ((cont_config<1)|(cont_config>3))                      // Si este no esta en un rango válido,   se le asigna posición 1.
          {cont_config=1; EEPROM_MEM.EEPROM_WRITE_1BYTE(dir_cont_config,2);}
        
 // Primero verifica que la red no exista
           if (EEPROM_MEM.lee_string_eeprom(dir_ssid1).equals(ssid))
              {res = 1;}
           else if (EEPROM_MEM.lee_string_eeprom(dir_ssid2).equals(ssid))
              {res = 2;}
           else if  (EEPROM_MEM.lee_string_eeprom(dir_ssid3).equals(ssid))
              {res = 3;}
           else {
// En caso contrario se va grabando en orden las nuevas redes configuradas
           int cont2=cont_config+1;
           if (cont2>3)
               cont2=1;
  
            EEPROM_MEM.EEPROM_WRITE_1BYTE(dir_cont_config,cont2);
            res = cont_config;
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
// Ejecuta escaneo de redes,  y las lista en la página WEB para que el usuario pueda seleccionar una y registar la clave de red

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
        int indices[n];
        for (i = 0; i < n; i++) {
          indices[i] = i;
            }
        // Organiza de acuerdo a la potencia
        for (i = 0; i < n; i++) 
          {
          for (int j = i + 1; j < n; j++) 
              {
              if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) 
                {
                std::swap(indices[i], indices[j]);
                }
              }
          }

  // remove duplicates ( must be RSSI sorted )
        String cssid;
        for (int i = 0; i < n; i++) 
          {
          if (indices[i] == -1) continue;
            cssid = WiFi.SSID(indices[i]);
          for (int j = i + 1; j < n; j++) 
            {
            if (cssid == WiFi.SSID(indices[j])) 
              {
              indices[j] = -1; // set dup aps to index -1
              }
            }
          }
 
 //display networks in page    
        for (int i=0; i<n ; i++)
          {
          if (indices[i] == -1) continue; // skip dups
          int quality =getRSSIasQuality(WiFi.RSSI(indices[i]));
          if (quality>=30) 
              {
              String item = HTTP_ITEM;
              String rssiQ;
              rssiQ += quality;
              item.replace("{v}", WiFi.SSID(indices[i]));
              item.replace("{r}", rssiQ);
              if (WiFi.encryptionType(indices[i]) != 0) {
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
//  ---  Servicio lanzado cuando se Configura el Dispositivo --- 
// 
// Función:
// se dispara al presionar el botón "ACEPTAR" de la página de configuración
// Entrega resultado Variable Mensaje:
// Sin configurar =0;
// Configuracion_Correcta = 1;
// Configuración incorrecta de Pasword o SSID = 2;
// Confoguración incorrecta de servidor o puerto =3
//---------------------------------------------------------------------------------------------------------------------


void wifi_esp_class::wifi_conf1()                                     
{    
int ssid_tamano=0;
int pass_tamano=0;

int  cuenta;
WiFiMulti wifiMulti_test;

        String getssid = server_esp.arg("ssid");                  //recibo los valores que envía por get el formulario web
        String getpass = server_esp.arg("pass");
        
        getssid=arregla_simbolos(getssid);                   // reemplazo los símbolos que aparecen con UTF8 con el símbolo correcto
        getpass=arregla_simbolos(getpass);
      
        ssid_tamano=getssid.length()+1;                     //calculamos la cantidad de caracteres que tiene el ssid y la clave
        pass_tamano=getpass.length()+1;
        
        getssid.toCharArray(ssid,ssid_tamano); // tansformamos el string en un char array ya que es lo que nos pide WiFI.begin();
        getpass.toCharArray(pass,pass_tamano);


        if (ssid_tamano>1)
          {
          server_esp.send(400, "text/html",HTTP_HEAD1+wifi_save+Test_Config+HTTP_END);        
          
          WiFi.disconnect();
          delay(500);

          wifiMulti_test.addAP(ssid,pass);
          delay(10);                // Pausa antes de intentar conectarse
              
          while ((wifiMulti_test.run() != WL_CONNECTED)&&(cuenta <5))         // Espera por conexión correcta
                {
                delay (500);
                Serial.print(".");
                cuenta++; 
                }
        
        if ((wifiMulti_test.run()== WL_CONNECTED))
            {
            WiFi_Config_Response=1;
            int posicion_grabar = get_stored_position();
            if (debug_activo)
                {
                Serial.println("");
                Serial.printf("SSID %s saved in position: %d\r\n",getssid,posicion_grabar);
                Serial.println();
                }
           if (posicion_grabar==1)
                {
                EEPROM_MEM.EEPROM_WRITE_1BYTE(dir_config1,1);                       // grabamos "configurado" para que cuando la placa arranque, sepa que tiene datos correctos, entonces proceda a intentar una conexión.
                EEPROM_MEM.graba_string_eeprom(dir_ssid1,getssid);                // en la dirección 1 grabamos el ssid.
                EEPROM_MEM.graba_string_eeprom(dir_pass1,getpass);                //en la dirección 30 grabamos el pass. (si sospechamos de que usaran ssid o pass muy largos ojo con los solapamientos, preferiblemente correr el pass de la 30 a la 60 por ejemplo, y el "configurado" pasarlo a la 120 por ejemplo.
                }
           else if (posicion_grabar==2)
                { 
                EEPROM_MEM.EEPROM_WRITE_1BYTE(dir_config2,1);                       // grabamos "configurado" para que cuando la placa arranque, sepa que tiene datos correctos, entonces proceda a intentar una conexión.
                EEPROM_MEM.graba_string_eeprom(dir_ssid2,getssid);                // en la dirección 1 grabamos el ssid.
                EEPROM_MEM.graba_string_eeprom(dir_pass2,getpass);                //en la dirección 30 grabamos el pass. (si sospechamos de que usaran ssid o pass muy largos ojo con los solapamientos, preferiblemente correr el pass de la 30 a la 60 por ejemplo, y el "configurado" pasarlo a la 120 por ejemplo.
                }
              
           else
                {
                EEPROM_MEM.EEPROM_WRITE_1BYTE(dir_config3,1);                       // grabamos "configurado" para que cuando la placa arranque, sepa que tiene datos correctos, entonces proceda a intentar una conexión.
                EEPROM_MEM.graba_string_eeprom(dir_ssid3,getssid);                // en la dirección 1 grabamos el ssid.
                EEPROM_MEM.graba_string_eeprom(dir_pass3,getpass);                //en la dirección 30 grabamos el pass. (si sospechamos de que usaran ssid o pass muy largos ojo con los solapamientos, preferiblemente correr el pass de la 30 a la 60 por ejemplo, y el "configurado" pasarlo a la 120 por ejemplo.
                }               

            }
            else
               WiFi_Config_Response=3;
          }
        else
            server_esp.send(400, "text/html",HTTP_HEAD1+no_wifi_save+config_back+HTTP_END);

}
//------------------
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
//Cuando se Pasan los datos del formulario mediante modo GET los caracteres especiales son reemplazados por codigo UTF-8, esta función reemplaza todos los codigos utf de caracteres especiales 
//por el caracter especial correspondiente, o sea si encuentra un %23 en el string lo reemplazará por un #. 
String wifi_esp_class::arregla_simbolos(String a){ 
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




void wifi_esp_class::Deep_Sleep_Wifi(void)
{
      /*
      WiFi.softAPdisconnect(true);                // Deconecta modo SoftAP
      delay(100); 
      WiFi.disconnect();                           // Se desconecta preliminarmente
         
      WiFi.mode(WIFI_OFF);                         // Apaga toda conexión preliminar
      delay (100);                                // si tenemos una conexión previa establecida
      Ko.WIFI_MOD.on_off=0;*/
}

uint8_t wifi_esp_class::SendPayloadWiFi(unsigned char *outData, const unsigned char len, unsigned char downlink)
{
    uint8_t result=0;
	  char hex_str[40];
    char trama_send[40]={0,0};
    if(Init_WiFi_Connection())
      {
	    
      Comms.ASCII2Hex(outData, len, hex_str);
      strcat(trama_send,hex_str);         // Add Payload
      Ubi.send_data_to_server(trama_send);

      if(downlink){
        RTC.GET_TIME_FROM_SERVER();
        }
      result=1;
      }
  return result;
}

/*
 * Ingresar WiFi a modo AP
 - Return 1 if testing succesfully
 - Return 0 otherwise
*/
uint8_t wifi_esp_class::enter_to_AP_Mode(void)
{
  Launch_Config_Portal();
  return .1;
}


/*
 * Salir WiFi de modo AP
 - Send "AT$AP=1" and wait for "OK" back
 - Return 1 if testing succesfully
 - Return 0 otherwise
*/
int wifi_esp_class::end_AP_Mode(void)
{
  Deep_Sleep_Wifi(); 
  return 1;
}

uint8_t wifi_esp_class::wait_rx_wifi(void)
{
//String Rx =Ubi.downlink_data();
return 1;
}