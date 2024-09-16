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

#ifndef config_h
#define config_h

#include <Arduino.h>


//-----------------------------------------------------------------------------------------------
// FUNCTIONS
//-----------------------------------------------------------------------------------------------

//int main(int argc, char** argv);

//-----------------------------------------------------------------------------------------------
// PIN DEFINITION
//-----------------------------------------------------------------------------------------------

#define LED_RED    32           // LED RGB RED 
#define LED_GREEN  15           // LED RGB GREEN
#define LED_BLUE   13           // LED RGB BLUE
#define LED2  2                 // LED 2    Relay 2 Status
#define Buzzer 17		        // Alarm Buzzer GPIO17
#define EN_COM_MOD	26          // Enable Communication Module  GPIO26
#define RX_COM 35               // Rx pin for Com Module
#define TX_COM 27               // Tx pin for Com Module

#define EN_RS485 18             // Read/Write RS-485 GPIO18
#define TX_RS485 21             // Tx pin for RS485
#define RX_RS485 19             // Rx pin for RS485

#define SDA1_PIN  23            // SDA Pin for ADC TLA2528
#define SCL1_PIN  22            // SCL Pin for ADC TLA2528


//-----------------------------------------------------------------------------------------------
// ENUMS
//-----------------------------------------------------------------------------------------------

enum com_module_type {            // Supported communication modules.
    mod_none = 0,
    mod_sigfox,
    mod_wifi,
    mod_lora,
    mod_ble
};

enum dev_operation_modes {         // Operational modes of the device.
    status_off,
    status_on,
};

enum com_module_status {         // Communication module status.	
    standby,
    storing,
    joining,
    waiting_to_join,
    sending,
    receiving,
    waiting_for_next_sending,
    rx_lora,
    module_init,
    wifi_AP_mode,
};

enum Lora_zones {            // LoRa band zones.
    AS923,
    AU915,
    CN470,
    CN779,
    EU433,
    EU868,
    KR920,
    IN865,
    US915,
    RU864,
    /*  <band>: [0: AS923, 1: AU915, 2: CN470, 3: CN779,
    4: EU433, 5: EU868, 6: KR920, 7: IN865, 8: US915, 9:
    RU864]  */
};

enum Lora_Modes {            // LoRa modes.
    OTAA,
    ABP,
    none,
};

enum led_color {                // RGB LED colors
    led_off,
    led_white,
    led_red,
    led_blue, 
    led_green,
    led_violet,
    led_orange,
    led_navy_blue
};

//-----------------------------------------------------------------------------------------------
// STRUCTS
//-----------------------------------------------------------------------------------------------

typedef union {                         	    // Inputs descriptor : each data is saved as an integer of 16 bits.
    int16_t Array [16];
    struct {
        int16_t address;						// Addres of the input (1,2), or modbus slave address(1..247).
        int16_t value;							// Current reading value
        int16_t type;							// Type of the sensor, temperature,  humidity,  diferent from sensor
        int16_t resolution;	            		// Resolution, 0, 1, 2 or 3 digits					
        int16_t enable;							// Enable/Disable the sensor
        int16_t offset;							// Offset to correct the reading value
        int16_t L_limit;				    	// Lower alarm limit
        int16_t H_limit;						// Higher alarm limit
        int16_t alarm_delay;					// delay for triggering the alarm [in seconds]
        int16_t slope;                          // Slope to scale the reading value	
        int16_t modbus_address_register;        // Start Reading Register
        int16_t modbus_coils;                   // Number of registers for reading
        int16_t modbus_type;                    // Modbus data format. 0: Integer / 1: Integer multiplied by 10 / 2: Integer multiplied by 100 / 3: Float IEEE 754-2008 / 4: Inverse Float  				    			
        };
}IN_t;

typedef union {								    // Outputs descriptor : each data is saved as an integer of 35 bits.
    int16_t Array [35];
    struct {
        int16_t  address;						// Addres of the output (1,2)
        int16_t  value;							// Current value of the output
        int16_t  control_funtion;			    // Designated control function 
        int16_t  control_signal;				// Control singal (input 1, 2 ; or 0: none).
        int16_t  set_point;						// Set Point
        int16_t  hysteresis;					// Hysteresis in On/Off control mode				
        int16_t  control_type;				    // Control Type (normal or inverted output)
        uint16_t set_time_ON;                   // Cycle time ON for timer / Cycle Time for PID
        uint16_t set_time_OFF;                  // Cycle time OFF for timer / Time to make Control Calculations for PID
        int16_t  Kp;                     		// Proportional Action for PID
        int16_t  Int;                    		// Integral Action for PID
        int16_t  Der;                    		// Derivative Action for PID
        int16_t  Pmax;                   		// Maximum Power to apply at the Output for PID

        // Working Variables
        uint16_t timer;         		        // Timer Counter
        int16_t phase;							// Phase in PID control
        int16_t start_delay;					// Delay to start the control at the output
        int16_t prev_value;                   	// Previus value of the control signal
        int16_t error;                  		// Error =  SetPoint - CurrentValue
        int16_t PB;                     		// Calculated Propotional Band PID
        int16_t TD[4];                  		// Delta of temperatures over time PID
        int16_t power;               			// Calculated Power PID
        float error_sum, max_int;    		    // Sum of integral error PID
        int16_t timer_PID;              		// Cycle timer PID
        int16_t reserved;              			// Reserved
        int16_t power_calc;              		// For calculations of the power delivered
        int16_t aux_timer_calc;         		// Auxiliary timer
        };
}OUT_t;

typedef union {								// Configuration descriptor : each data is saved as an integer of 16 bits.
    int16_t Array [16];
    struct{
        int32_t ID;                        	    // Device ID
        int16_t TPU;                        	// Publication Time
        int16_t service;                    	// Enter to Service Mode
        int16_t reserve1;           			// Not used,  Reserved
        int16_t eeprom_vers;                	// Version of factory parameters (Reset to factory settings)
        int16_t com_module_prog;            	// Type of communication module programmed (0: none, 1: Sigfox, 2: WiFi, 3: Lora, 4: Cellular)
        int16_t reserve2;                		// Not Used, Reserved
        int16_t downlink_msg;           	    // Number of messages for downlink (per day? / Every how many Uplink messages it queries the p?latform if it has a Downlink or configuration message?)
        int16_t RF_zone;                    	// RF zone  (2: USA-US915, 4: Latam-Australia-AU915 / 1: EU-EU868)
        int16_t status;                     	// Current status of the device (off, on, test mode)
        int16_t password;                   	// Password for configuration
        int16_t task;							// Sub task mode 
        int16_t com_module_det;   				// Real communication module detected (0: none, 1: Sigfox, 2: WiFi, 3: Lora, 4: Cellular)
		
    }DT;
}CONFIG_t;

typedef union {                             // RTC descriptor : each data is saved as an integer of 15 bits..
    int16_t Array[50];
    struct{
        uint32_t time_stamp;           
        int16_t time_zone;
        tm timeinfo;
        uint16_t RTC_is_synchronized;       // Flag indica si la hora ya esta configurada
        uint32_t previous_time_stamp;       //

    };
}RTC_t;

typedef struct {                            // Sigfox communications module descriptor.
    char ID[9];                             // Device unique identifier
    int type;                               // ??
    int zone;								// 1: EU, 2: USA, 4: AU/LATAM
}SIGFOX_MODULE_t;

typedef struct {                            // LoRa communications module descriptor.
    int  band;          					// 1: AU915, 5: EU868, 8: US915 
    int  type;                              // ??
    int  int_join;                          // ??
    char OTAA_DEV_EUI[17];                  // ??
}LORA_MODULE_t;

typedef struct{
    bool on_off;
    String MAC;
    int16_t rssi;
}WIFI_MODULE_t;

typedef struct {							// Downlink Messages descriptor: parameters modification commands.
    int16_t group;                          // Group of parameters that can be configured
    int16_t subgroup;                       // Subgroup of parameters that can be configured
    int16_t parameter;                      // Parameter to be configured 
    float value_float;                      // Value assigned to the parameter in float
    int32_t  value_length;                  // Value assigned to the parameter in long                
}GET_DATA_t; 

typedef struct {                            // Timers descriptor.
    uint16_t timer_100ms, timer_s, seconds, minutes;
    uint16_t T_TASK;                                        // cómo traducir??      
    uint16_t delay;                                         
    uint32_t timer_publication;                             // Publication time in seconds
    uint16_t wait_for_sigfox_downlink;                      // Why??
    uint16_t timer_Sil_IN1, timer_Sil_IN2, timer_Sil_VDC;   // Delay timers for sensor alarm
    uint16_t timer_sensor_reading;		                    // Timer for sensor reading
    uint16_t timer_wifi_AP;                                 // Timer to exit WiFi AP Mode
    uint16_t downlink_fail;                                 // Timer for waiting for downlink messages
    uint16_t timer_password;                                // Timer for holding the password
    uint16_t timer_reset_com_error;                         // Timer to reset the device upon communication error
}TIMERS_t;

typedef struct {                            // Publications descriptor.
    int16_t send_data, data_ACK_Rx;          			// Flags and counters for communication tasks
    int16_t wait_for_downlink;
    int16_t message_type;
    int16_t com_status;
    int16_t join_status;
    int16_t Rx_counter;                       
    int16_t join_counter;					  			//Counter for join in Lora Mode
    int16_t counter_for_delay;	
    int16_t counter_for_downlink;					
    int16_t counter_of_attempts_to_send;
    int16_t ACK_Lora;						 			//ACK Lora Receive
    int16_t Lora_Rx_flag;					 			//Flag for Rx packet receiving in Lora mode
    int16_t flag_reset_com_error;			 			//Flag for error in comunication
}PUB_t;


typedef struct {                       // Modbus descriptor.
    uint8_t  slave_address;            // Modbus address (number between 1 and 247)
    uint8_t  funtion_code;             // Modbus function code
    uint16_t start_address;            // Address of the register (to be read/written)
    uint16_t coils_no;                 // Number of registers (to be read/written)
    uint8_t  registers[40];            // Buffer for Serial Modbus RS-485 data read
}MODBUS_t;

class Config {
  
public:
    IN_t IN1,IN2;
    IN_t VDC;
    OUT_t OUT1,OUT2,OUT_PID,OUT_TIMER;
    TIMERS_t TIMERS;
    CONFIG_t CONFIG;
    GET_DATA_t GET_DATA;
    PUB_t PUB;
    RTC_t RTC;
    LORA_MODULE_t LORA_MOD;
    SIGFOX_MODULE_t SIGFOX_MOD;
    WIFI_MODULE_t WIFI_MOD;
    MODBUS_t MODBUS;
   
};

extern Config Ko;                      // Create all Data Struct  (from config.h)

#endif