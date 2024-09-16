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

version 1.0 July 2024
*/


#include "TLA2528.h"
#include "TLA2528_c.h"


int TLA2528Class::begin()
{
  // Start I2C communication
    Serial.println("INIT TLA228");
    MyI2C_1.begin(SDA1_PIN,SCL1_PIN,100000); 
  
  // Set Analog and Digital Output Pins for  tla2528
    
    reset();                    // Reset TLA228
    pinModeIO(AN1,ANALOG);      
    pinModeIO(AN2,ANALOG);
    pinModeIO(ANVDC,ANALOG);
    pinModeIO(REL1,OUTPUT);     
    pinModeIO(REL2,OUTPUT);         
    return 1;
}

void TLA2528Class::end()
{
    MyI2C_1.end();
}

void TLA2528Class::Read_registers_Status(void)
{
    Serial.println("-------- TLA2528 --------");
    Serial.println(readRegister(SYSTEM_STATUS_ADDRESS));
    Serial.println(readRegister(GENERAL_CFG_ADDRESS));
    Serial.println(readRegister(DATA_CFG_ADDRESS));
    Serial.println(readRegister(OSR_CFG_ADDRESS));
    Serial.println(readRegister(OPMODE_CFG_ADDRESS));
    Serial.println(readRegister(PIN_CFG_ADDRESS));
    Serial.println(readRegister(GPIO_CFG_ADDRESS));
    Serial.println(readRegister(GPO_DRIVE_CFG_ADDRESS));
    Serial.println(readRegister(GPO_VALUE_ADDRESS));
    Serial.println(readRegister(GPI_VALUE_ADDRESS));
    Serial.println(readRegister(SEQUENCE_CFG_ADDRESS));
    Serial.println(readRegister(MANUAL_CH_SEL_ADDRESS));
}

int TLA2528Class::readRegister(uint8_t address)
{
    uint8_t value;

    if (readRegisters(address, &value, sizeof(value)) != 1) {
        return -1;
    }

    return value;
}

int TLA2528Class::readRegisters(uint8_t address, uint8_t* data, size_t length)
{
    MyI2C_1.beginTransmission(tla2528_i2c_address);  // Start transmission to device
    MyI2C_1.write(0b00010000);  // Read Register Command
    MyI2C_1.write(address);     // Register Address
    MyI2C_1.endTransmission();  // End transmission
    MyI2C_1.requestFrom(tla2528_i2c_address,length);  // Request data from device
    if (MyI2C_1.available()) {
        *data=MyI2C_1.read();   // Read received data
        return 1;
    }
    
return 0;
}

int TLA2528Class::readAnalogInput(uint8_t input, uint8_t* data)
{
    MyI2C_1.beginTransmission(tla2528_i2c_address);    // Start transmission to device
    MyI2C_1.write(0b00001000);                  // Send Write Command
    MyI2C_1.write(MANUAL_CH_SEL_ADDRESS);       // Dir Sel channel
    MyI2C_1.write(input);                       // Set Channel        
    MyI2C_1.endTransmission();                  // End transmission

    uint8_t bytesReceived = MyI2C_1.requestFrom(tla2528_i2c_address,sizeof(data));  // Request data from device
    if ((bool)bytesReceived)                    //If received more than zero bytes
      {  
      MyI2C_1.readBytes(data, bytesReceived);
      return 1;
      }
return 0;
}

void TLA2528Class::digitalWriteIO(uint8_t pin, uint8_t val)
{
    if (pin >= 0 && pin <= 7)
    {
        if (pin == 0)
        {
            //Pin 0 is used for alarm and needs to be specialtreated. Inverted for correct behaviour
            if (val)
            {
                resetBit(ALERT_PIN_CFG, 1 << pin);
            }
            else
            {
                setBit(ALERT_PIN_CFG, 1 << pin);
            }

        }
        else
        {
            if (val)
            {
                setBit(GPO_VALUE_ADDRESS, 1 << pin);
            }
            else
            {
                resetBit(GPO_VALUE_ADDRESS, 1 << pin);
            }
        }
    }
}

uint8_t TLA2528Class::digitalReadIO(uint8_t pin)
{
    if (pin >= 0 && pin < 8)
    {
        int result = readRegister(GPI_VALUE_ADDRESS);

        return (result >> pin) & 0x1;
    }
    return uint8_t();
}

int TLA2528Class::analogReadIO(uint8_t pin)
{
    uint8_t value[3];
    readAnalogInput(pin, (uint8_t*)value);
    return (value[0] << 4) | (value[1] >> 4);
}

void TLA2528Class::reset()
{
    setBit(GENERAL_CFG_ADDRESS,1 << RST_MASK);
}

void TLA2528Class::pinModeIO(uint8_t pin, uint8_t mode)
{
    if (pin < 8)
    {
        switch (mode)
        {
        case INPUT:
            setBit(PIN_CFG_ADDRESS, 1 << pin);
            resetBit(GPIO_CFG_ADDRESS, 1 << pin);
            break;
        case OUTPUT:
            setBit(PIN_CFG_ADDRESS, 1 << pin);
            setBit(GPIO_CFG_ADDRESS, 1 << pin);
            setBit(GPO_DRIVE_CFG_ADDRESS, 1 << pin);
            break;
        case ANALOG:
            resetBit(PIN_CFG_ADDRESS, 1 << pin);
            resetBit(GPIO_CFG_ADDRESS, 1 << pin);
            break;
        }
    }
}

int TLA2528Class::writeRegister(uint8_t address, uint8_t value)
{
    MyI2C_1.beginTransmission(tla2528_i2c_address);
    MyI2C_1.write(0b00001000);  // Write Command
    MyI2C_1.write(address);     // Register Addres
    MyI2C_1.write(value);       // Set Value
    MyI2C_1.endTransmission();  // End transmission
    return 1;
}

int TLA2528Class::setBit(uint8_t address, uint8_t value)
{
    MyI2C_1.beginTransmission(tla2528_i2c_address);
    MyI2C_1.write(0b00011000);  // Set Bit Command
    MyI2C_1.write(address);     // Register Addres
    MyI2C_1.write(value);       // Set Value
    MyI2C_1.endTransmission();  // End transmission
    return 1;
}

int TLA2528Class::resetBit(uint8_t address, uint8_t value)
{
    MyI2C_1.beginTransmission(tla2528_i2c_address);
    MyI2C_1.write(0b00100000);  // Reset Bit Command
    MyI2C_1.write(address);     // Register Addres
    MyI2C_1.write(value);       // Set Value
    MyI2C_1.endTransmission();  // End transmission
    return 1;
}
