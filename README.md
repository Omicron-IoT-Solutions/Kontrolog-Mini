![Logo-Omicron-Vector](https://github.com/Omicron-IoT-Solutions/Kontrolog/assets/141452095/1d867a2d-2f0b-40eb-bbb9-566f306320ba)
# Kontrolog Mini
## General
This branch contains the Firmaware code (default programming of the device) for the Kontrolog Mini to be programmed locally through the graphical HMI interface provided or remotely through the IoT Web platform, as well as examples and guided tutorials for you to learn how to program in No-Code mode your Kontrolog Mini.

## Description
![bitmap3](https://github.com/user-attachments/assets/53b01053-544c-4baa-89e1-4f7cb251cfb4)
The Kontrolog Mini firmware contains the factory default code, which is programmed into the Kontrolog Mini from the moment you receive it for your purchase, where you only need to link it to the graphical HMI or IoT Web platform and you can start configuring alarms, inputs and outputs, and monitor the status of them.

To install the Firmware we provide you, you must install the “FLASH DOWNLOAD TOOL” program, which you can find at the link https://www.espressif.com/en/support/download/other-tools

Once installed, download the Firmware binary file and connect your Kontrolog Mini to your computer using the USB-C cable. Then, run the downloaded application and set, in the download tool mode, ESP32 as the chip type and the working mode as development. 

![Captura de pantalla 2024-09-26 093040](https://github.com/user-attachments/assets/5428a370-f87d-42e4-8ab8-179a462d1211)

Now you must assign to the software the location where the downloaded firmware file is located and set the memory address to 0x10000. Make sure that the following parameters are assigned:
- SPI Speed in 40MHz.
- SPI mode in DIO.
- The “DoNotChgBin” option must be enabled.
  
Select the communication port your Kontrolog Mini is connected to on your computer and set the baud rate to 460800 Baud. With this you can start downloading the Firmaware to your Kontrolog Mini and it is ready to be configured. 

![Captura de pantalla 2024-09-26 093125](https://github.com/user-attachments/assets/965401de-3623-4b86-984d-8fd21320c8ce)

The important thing about this tool is that your Kontrolog Mini is already enabled so you can configure it through the HMI interface or through the IoT web platform. From the tool where you are programming the Kontrolog Mini, you can configure the inputs and outputs, indicate the functions you want them to perform such as sensor readings, activation of relay outputs, define the types of communication you want to use and assign the functions for the LED and the buzzer. Just manipulate the control tools to know how to set the functions you want your Kontrolog Mini to perform.
