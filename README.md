![Logo-Omicron-Vector](https://github.com/Omicron-IoT-Solutions/Kontrolog/assets/141452095/1d867a2d-2f0b-40eb-bbb9-566f306320ba)
# Kontrolog Mini Board Support Package
## General
This branch contains the documentation and the basic header and implementation files that compose the Board Support Package for the Kontrolog Mini, as well as examples and guided tutorials for you to learn how to program in Full-Code mode your Kontrolog Mini.

## Description
![bitmap2](https://github.com/user-attachments/assets/ce062b7b-13b5-4596-9dc3-d58d14262f44)
The Kontrolog Mini Board Support Package contains the basic header and implementation files, as well as documents related to datasheets, manuals and schematics of the device and the modules that compose it.

For proper operation, in the project to which you want to add the Kontrolog Mini Board Support Package, you must set the board “Espressif ESP32 Dev Module”, or equivalent, and Arduino as Framework.

Just link this library for the Kontrolog Mini in your project libraries from the PlatformIO library registry, which you can find as “Kontrolog Mini” (keywords: “kontrolog”, “mini”, “iot”, “esp32”, “plc”).

## Structure
- Kontrolog Mini
  - .pio
  - .vscode
  - docs
    - (datasheets, manuals and schematics)
  - examples
    - ReadNTC10k.cpp
  - include
  - lib
  - scr
    - (Files .cpp y .h)
  - test
  - .gitignore
  - library.json
  - README.md
  - platformio.ini
 
The important thing about this tool is that you can capture all your ideas and everything you want to run with your Kontrolog Mini. From your main header file, you can call the available inputs and outputs, indicate the functions you want them to perform such as sensor readings, activation of relay outputs, define the types of communication you want to link and assign the functions for the LED and the buzzer. You only need to browse through each of the header files and source codes to know how to call the respective functions and define the desired states.
