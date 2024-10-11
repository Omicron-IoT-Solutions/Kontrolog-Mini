![Logo-Omicron-Vector](https://github.com/Omicron-IoT-Solutions/Kontrolog/assets/141452095/1d867a2d-2f0b-40eb-bbb9-566f306320ba)
# Kontrolog Mini Board Support Package (BSP)
## General
![bitmap2](https://github.com/user-attachments/assets/ce062b7b-13b5-4596-9dc3-d58d14262f44)

This branch contains the documentation and the basic header and implementation files that compose the Board Support Package for the Kontrolog Mini, as well as examples and guided tutorials for you to learn how to program in Full-Code mode your Kontrolog Mini.

## BSP Structure

- [Kontrolog_Mini_BSP](https://github.com/Omicron-IoT-Solutions/Kontrolog-Mini/tree/Full-Code/Kontrolog_Mini_BSP)
  - examples
    - ReadModbus_SendLoRa.cpp
    - ReadNTC10k.cpp
  - include
    - (Files .h)
  - scr
    - (Files .cpp)
  - README.md
  - library.json
  - platformio.ini
  - wokwi.toml

## Guide for use
In order to use the Kontrolog Mini library, which is hosted in the PlatformIO library registry, you must first create a project in PlatformIO using the Arduino Framework (default choice) and assigning the “Espressif ESP32 Dev Module” board.

![Captura de pantalla 2024-10-01 084556](https://github.com/user-attachments/assets/755282c3-2c37-470c-a1ab-5080e34b671c)

Once your project is created, go to the PlatformIO library registry and search for our library such as “Kontrolog Mini” (keywords: “kontrolog”, “mini”, “iot”, “esp32”, “plc”). There you will find basic information about the library presented by the platform itself. Proceed to add this library to your project.

![Captura de pantalla 2024-10-01 090355](https://github.com/user-attachments/assets/f00a9eb2-4b1d-4f98-acf3-7baf086da0d0)

Once you have linked the Kontrolog Mini library to your project, go to the README.md file of our library (“Your project”/.pio/libdepsp32dev/Kontrolog_Mini/README.md) and include the libraries shown there to the PlatformIO initialization file (platformio.ini) of your project, and save the modifications made.

![Captura de pantalla 2024-10-01 091138](https://github.com/user-attachments/assets/954452e5-9bf8-4709-97aa-532de264bfa2)

The important thing about this tool is that you can capture all your ideas and everything you want to run with your Kontrolog Mini. From your main header file, you can call the available inputs and outputs, indicate the functions you want them to perform such as sensor readings, activation of relay outputs, define the types of communication you want to link and assign the functions for the LEDs and the buzzer. You only need to browse through each of the header files and source codes to know how to call the respective functions and define the desired states.
