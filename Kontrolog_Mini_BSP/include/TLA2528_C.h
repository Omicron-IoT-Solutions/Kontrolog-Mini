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

#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "TLA2528.h"
#include "config.h"

//  TLA2528 Pin connections for  Kontrolog-Mini 1.0
#define REL1 3				 // Relay 1 Pin Output
#define REL2 2				 // Relay 3 Pin Output		
#define AN1  5			     // Input 1 Channel
#define AN2  4				 // Input 2 Channel
#define ANVDC 0 			 // DC voltage Analog Input
#define tla2528_i2c_address  0x13   // Slave device address of TLA2528 


class TLA2528Class {
public:
	TwoWire MyI2C_1 = TwoWire(0);

	int  begin();
	void end();
	void digitalWriteIO(uint8_t pin, uint8_t val);
	uint8_t digitalReadIO(uint8_t pin);
	int analogReadIO(uint8_t pin);
	void reset();
	int  Read_Register(uint8_t address);
	void Read_TLA2528_All_registers_Status(void);

private:
	void Set_pinModeIO(uint8_t pin, uint8_t mode);
	int I2C_readRegisters(uint8_t address, uint8_t* data, size_t length);
	int I2C_writeRegister(uint8_t address, uint8_t value);
	int I2C_setBit(uint8_t address, uint8_t bit);
	int I2C_resetBit(uint8_t address, uint8_t bit);
	int I2C_readAnalogInput(uint8_t input, uint8_t* data);
};

extern TLA2528Class TLA2528;