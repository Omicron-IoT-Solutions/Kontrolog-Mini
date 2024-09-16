#ifndef eeprom_esp_h
#define	eeprom_esp_h

#include <Arduino.h>
#include <EEPROM.h>
#include "Config.h"
#include "RTC_CLOCK.h"

// Data Version in EEPROM
const int VERSION_EEPROM = 0x0013;


const unsigned char
// Entradas
IN1_EEPROM = 0,
IN2_EEPROM = 0x10,
BAT_EEPROM = 0x50,
// Salidas de Control        
OUT1_DIR = 0x80,
OUT2_DIR = 0x90,
        
// Configuraciones Generales        
RTC_DIR  = 0xE0,
EEPROM_CONFIG_INI=0xF0,     // Inicio de parámetros de Configuración
EEPROM_ID      = 0xF0,
DIR_TPU        = 0xF2,      // TPU
DIR_BOOT       = 0xF3,      // Boot Loader
EEPROM_EEINI   = 0xF5,      // Programación inicial de parámetros de fábrica.
DIR_COM_MODULE = 0xF6,      // Dirección módulo de comunicación
DIR_DOWNLINK   = 0xF8,      // Dirección de Downlink
DIR_RF_ZONE    = 0xF9,      // Dirección de zona RF para Sigfox y LoRa
DIR_STATUS     = 0XFA,      // Operational Status Address  0:off, 1: On,  2: rf test mode
DIR_EXP_MOD1   = 0xFE;

// Inicialización de Sensores
 
/* Estructura de Datos
        0:  address;						// UID of the sensor
        1:  value;							// Current reading value
        2:  type;							// Type of the sensor, temperature,  humidity,  diferent from sensor
        3:  resolution;	            		// Resolution, 0, 1, 2 or 3 digits					
        4:  enable;							// Enable/Disable the sensor
        5:  offset;							// Offset to correct the reading value
        6:  L_limit;				    	// Lower alarm limit
        7:  H_limit;						// Higher alarm limit
        8:  alarm_delay;					// delay for triggering the alarm [in seconds]
        9:  slope;                          // Slope Scaled for 1000
        10: modbus_address_register;        // Modbus Start Adrress Register
        11: modbus_coils;                   // Modbus number of reading registers
        12: modbus_type;                    // Modbus type of data

 */

/* Tipos de Sensores
    0: off,
    1: digital,
    2: i4to20mA,
    3: voltDC,
    4: temp,
    5: temp_hume,
    9: Modbus*/

const int Sens_Ini0[16]={1,0,4,1,1,0,0,200,300,1000,1,1,0,0};          // Sensor S1 4..20mA
const int Sens_Ini1[16]={2,0,4,1,1,0,150,300,300,1000,1,1,1,0};        // Sensor S2 Temperatura
const int Sens_Ini5[16]={6,0,3,1,1,0,0,1000,600,1000,1,1,1,0};         // Voltaje DC externo

/*Inicialización de salidas
    0: valor;
    1: Funcion_Control;
    2: Senal_Control;
    3: Set_point;
    4: Delta de Control
    5: Tipo_Control;
    6: Set_Time_ON;
    7: Set_Time_OFF;
    */
/* Tipos de Salidas
    0: Salida Apagada,
    1: Comando remota,
    2: Seguidor_Entrada Digital,
    3: Controlador de Calefacción o Refrigreación,
    4: Temporizador,
    5: Alarma Externa,
 */
const int Sal_Ini1[16]={1,0,0,1,40,20,0,60,60,10,5,3,12,2};          // Salida 1  de Control On/OFF refrigeración Sensor Temperatura 1
const int Sal_Ini2[16]={2,0,0,2,40,20,0,60,60,10,5,3,12,2};          // Salida 2  Comandos Remota

const int Configuracion_Ini [16] = {                                 // Configuraciones Generales
0,221,                 // initial ID
5,                     // TPU: Publication Time
1,                     // Boot Mode
0,                     // Reserve
1,                     // EEPROM Verion
2,                     // Tipo de Módulo de comunicación  Programado 0:  No tiene, 1: Sigfox, 2: Wifi, 3  Lora OTAA
0,                     // Tipo de Grados
3,                     // Mensajes para Downlink
2,                     // Zona RF
1,                     // Status
0,                     // Clave
0,                     // Tarea
0,                     // Auxiliar para grabar Tipo de módulo Seleccionado
0,                     // Mod Expansión 1
0,                     // Mod Expansión 2
};                     



class ee_prom_class {

public:
    void SETUP();
    void EXTRAER_DATOS_EEPROM(void);
    int16_t EEPROM_READ_INT(unsigned int);
    void EEPROM_WRITE_INT(unsigned int, int16_t);
    void EEPROM_WRITE_1BYTE(int, unsigned char);
    uint8_t EEPROM_READ_1BYTE(unsigned int);
    void graba_string_eeprom (unsigned int, String);
    String lee_string_eeprom (unsigned int);


private:
    void INI_SENSOR (int,const int *, int16_t *,char);
    void LEER_SENSOR_INI_EEPROM (int, int16_t *, char);

};

extern ee_prom_class EEPROM_MEM;
#endif
