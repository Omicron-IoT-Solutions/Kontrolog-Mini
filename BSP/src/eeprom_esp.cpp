#include "eeprom_esp.h"


void ee_prom_class::SETUP() {
    // Initialize EEPROM
    EEPROM.begin(0x1200); // Adjust size as needed based on your ESP32's EEPROM size
}

void ee_prom_class::EXTRAER_DATOS_EEPROM(void)
{
unsigned int i,j;
    EEPROM.begin(0x1200); // Adjust size as needed based on your ESP32's EEPROM size
    
    if (EEPROM_READ_INT(EEPROM_EEINI)!=VERSION_EEPROM)      // Pregunta dos veces si los parámetros iniciales ya estan escritos en la eeprom?
        {
        Serial.printf("Factory Settings %d\r\n",VERSION_EEPROM );
        INI_SENSOR (IN1_EEPROM,Sens_Ini0,Ko.IN1.Array,16);
        INI_SENSOR (IN2_EEPROM,Sens_Ini1,Ko.IN2.Array,16);
        INI_SENSOR (BAT_EEPROM,Sens_Ini5,Ko.VDC.Array,16);
        
        INI_SENSOR (OUT1_DIR,Sal_Ini1,Ko.OUT1.Array,16);
        INI_SENSOR (OUT2_DIR,Sal_Ini2,Ko.OUT2.Array,16);

        for (i=0;i<16;i++)               // Configuraciones Generales
            {
            EEPROM_WRITE_INT(EEPROM_CONFIG_INI+i,Configuracion_Ini[i]);            // Configuraciones generales 8..12
            Ko.CONFIG.Array[i]=Configuracion_Ini[i];                              // Indica que la memoria ya fue inicializada.
            }
 
        Ko.SIGFOX_MOD.zone=2;
        Ko.LORA_MOD.band=US915;
        Ko.CONFIG.DT.RF_zone=2;
        EEPROM_WRITE_INT(0xE2,-5);                                                 // Zona horaria por defecto
        //RTC.Time_Zone=-5;  
        //INICIO_AJUSTE_RTC (20,1,1,6,0,0,0,1);                                  // Inicio de RTC
        
        /* Escribir version de configuración de EEPROM */
        EEPROM_WRITE_INT(EEPROM_EEINI, VERSION_EEPROM);
        
        EEPROM_WRITE_INT(0xFF,0);                                                  // Posición de almacenamiento de datos para backup
        //BACK_HIST.Pos_Alma=0;
        }
    
    else                                                                        // Lectura de los parámetros de configuración del dispositivo
        {
        // Inputs
        LEER_SENSOR_INI_EEPROM (IN1_EEPROM,Ko.IN1.Array,16);
        LEER_SENSOR_INI_EEPROM (IN2_EEPROM,Ko.IN2.Array,16);
        LEER_SENSOR_INI_EEPROM (BAT_EEPROM,Ko.VDC.Array,16);
        // Outputs
        LEER_SENSOR_INI_EEPROM (OUT1_DIR,Ko.OUT1.Array,16);
        LEER_SENSOR_INI_EEPROM (OUT2_DIR,Ko.OUT2.Array,16);
        // General Configuration
        for (i=0;i<16;i++)
            {
            Ko.CONFIG.Array[i]=EEPROM_READ_INT(i+EEPROM_CONFIG_INI); 
            }
        
        // Parámetros RTC
        
        for(i=0;i<12;i++)
            Ko.RTC.Array[i]=EEPROM_READ_INT(RTC_DIR+i);  
        
        if(Ko.RTC.RTC_is_synchronized)
            RTC.SET_TIME(Ko.RTC.time_stamp);
        else;
        }
    
    // Parametros almacenamiento de datos
       /*
        BACK_HIST.Pos_Alma=EEPROM_READ(255)+1;    // Para tomar el siguiente dato (Se guardan cada dos).)
        if ((BACK_HIST.Pos_Alma>=N_Datos_Historico))
            {
            BACK_HIST.Pos_Alma=0;
            EEPROM_WRITE(0xFF,0); 
            }
        else
            {
            for(i=0;i<N_Datos_Historico;i++)
                {
                for(j=0;j<8;j++)
                    {   
                    BACK_HIST.valores[i][j]=EEPROM_READ(8*i+j+256);
                    }
                ClrWdt();
                }
            }
        */ 

    // Init Timers
    Ko.TIMERS.timer_Sil_IN1=Ko.IN1.alarm_delay;
    Ko.TIMERS.timer_Sil_IN2=Ko.IN2.alarm_delay;
    Ko.TIMERS.timer_Sil_VDC=Ko.VDC.alarm_delay;
    Ko.OUT1.start_delay=10;
    Ko.OUT2.start_delay=10;
}

// Inicializar sensores EEPROM
void ee_prom_class::INI_SENSOR (int pos,const int *Dat_ini, int16_t *Array, char Size)
{
    unsigned char i;
    for (i=0;i<Size;i++)
        {
        EEPROM_WRITE_INT(pos+i,*(Dat_ini+i));                                      // Inicialización Sensor TCamara
        *(Array+i)=*(Dat_ini+i);
    } 
}

// Inicializar sensores EEPROM
void ee_prom_class::LEER_SENSOR_INI_EEPROM (int pos, int16_t *Array, char Size)
{
    unsigned char i;
    for (i=0;i<Size;i++)
        {
        *(Array+i)=EEPROM_READ_INT(pos+i);                                      // Inicialización Sensor TCamara
        } 
}

// LEE UN ENTERO DE LA EEPROM, ALMACENADO EN 2 BYTES Adyacentes.
int16_t ee_prom_class::EEPROM_READ_INT(unsigned int index) {
    index *= 2; // Se multiplica por 2 ya que la EEPROM está organizada en bytes y los Parámetros son enteros.
    int16_t data_eeprom = EEPROM.readShort(index);
    return data_eeprom;
}

void ee_prom_class::EEPROM_WRITE_INT(unsigned int index, int16_t newData) {
    index *= 2; // Se multiplica por 2 ya que la EEPROM está organizada en bytes y los Parámetros son enteros.
    EEPROM.writeShort(index,newData);
    EEPROM.commit(); // Asegura que los datos se escriben en la memoria flash
}

void ee_prom_class::EEPROM_WRITE_1BYTE(int index, unsigned char newData) {
    EEPROM.write(index, newData); // Write a single byte to EEPROM at 'index'
    EEPROM.commit(); // Ensure data is written to flash (required for ESP32)
}

uint8_t ee_prom_class::EEPROM_READ_1BYTE(unsigned int index)
{
    uint8_t data=EEPROM.read(index);
    return data;
}

//---------------------------------------------------------------------------------------------------------------------
//  ---- GRABAR DATOS A LA EEPROM ----

// Función: 
// Guarda un String "a"  en dirección "addr"
//---------------------------------------------------------------------------------------------------------------------
 
void ee_prom_class::graba_string_eeprom (unsigned int addr, String a)
{ 
    EEPROM.writeString(addr,a);
    EEPROM.commit();  // para que la grabación de datos cobre efecto inmediato
}


//---------------------------------------------------------------------------------------------------------------------
//  ---- LEER STRING GRABADO EN LA EEPROM ----
// Función: 
// Retorna un String "nuevostring"  almacenado en la dirección "addr",  
//---------------------------------------------------------------------------------------------------------------------
String ee_prom_class::lee_string_eeprom (unsigned int addr){ 
    String nuevostring=EEPROM.readString(addr);
return nuevostring; // devuelvo el string.
}

