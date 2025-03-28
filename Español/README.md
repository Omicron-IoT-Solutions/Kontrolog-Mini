![Logo-Omicron-Vector](https://github.com/Omicron-IoT-Solutions/Kontrolog/assets/141452095/1d867a2d-2f0b-40eb-bbb9-566f306320ba)
# Kontrolog Mini
## General
Este repositorio contiene instrucciones, manuales, esquemas, paquetes y ejemplos para programar tu dispositivo Kontrolog Mini con las diferentes funciones que mejor se adapten a tu aplicación.

## Description
![bitmap](https://github.com/user-attachments/assets/d31e644a-32ff-4a47-b626-77845fbbe767)
El dispositivo Kontrolog Mini es un controlador programable para IoT (Internet de las Cosas) que incorpora la potente tecnología de procesamiento ESP32, diseñado para el control y monitoreo remoto y en tiempo real de diversos procesos con altos requisitos de seguridad y precisión. Adaptado a tu aplicación, el dispositivo puede utilizarse fácilmente después de la instalación. Aloja componentes electrónicos como un microcontrolador (CPU), memoria y otros elementos de hardware, tales como fuentes de voltaje, salidas de potencia y los puertos necesarios para conectar los sensores y el módulo de comunicación IoT. El microcontrolador contiene el software embebido, encargado de realizar tareas como: leer los sensores conectados a sus puertos, obtener su información, crear los paquetes o tramas de monitoreo, configurar y activar el módulo de comunicación, y enviar los datos en intervalos variables según los tiempos de publicación programados, además de generar alertas y ejecutar tareas de control. Además, el software está disponible en lenguaje C++ de código abierto, lo que permite contar con dos modos de programación, los cuales presentamos en dos ramas: 

- [No-Code](https://github.com/Omicron-IoT-Solutions/Kontrolog-Mini/tree/No-Code)

- [Full-Code](https://github.com/Omicron-IoT-Solutions/Kontrolog-Mini/tree/Full-Code)
  
## Características

Las principales características técnicas de este modelo del dispositivo Kontrolog Mini se detallan a continuación:  

- Dos (2) entradas para sensores, que pueden configurarse como:  
  - Entrada analógica 4-20mA.  
  - Entrada analógica 0-10V.  
  - Entrada analógica de temperatura para el termistor NTC 10K – 3950.  
  - Entrada digital 1/0.  
  - Entrada para sensores analógicos de temperatura y humedad ambiental.  
  - Contador de pulsos digitales.  

- Cuatro (4) puertos GPIO, que el usuario puede programar libremente como entradas o salidas, o utilizarlos para leer otros sensores o con protocolos de comunicación como I2C, SPI, UART, entre otros.  

- Una (1) entrada de voltaje de alimentación de 12-28 VDC, con medición de voltaje de alimentación en corriente continua.  

- Un (1) puerto de entrada Modbus RTU (RS-485) operando como maestro en la red.  

- Alarmas:  
  - Alarmas visuales y audibles incorporadas, además de alarmas visibles de forma remota en la aplicación móvil o web.  
  - Integración con la plataforma para alarmas externas por SMS, mensaje de voz, correo electrónico y servicio de mensajería Telegram, entre otros.  

- Dos (2) salidas tipo relé normalmente abiertas, con rangos máximos de conmutación de 12A y 240VAC a 50/60 Hz.  

- Un (1) puerto USB tipo C, que permite la comunicación con el entorno de programación y simulación, además de servir como fuente de alimentación de 5V para el Kontrolog Mini.  

- El dispositivo permite realizar diversas tareas de control, tales como:  
  - Control por comandos remotos enviados desde la plataforma.  
  - Control ON/OFF con histéresis para enfriamiento o calefacción.  
  - Temporizador.  
  - Activación de salida según el sensor de entrada asociado.  
  - Alarma remota.  
  - Contador de eventos.  
  - Control PID.  

- Pantalla HMI (Interfaz Hombre-Máquina): Pantalla táctil LCD a color de 4.3” para programación y visualización de datos.  

- Puerto de comunicación, que permite conectar un módulo de comunicación dual con protocolos LoRaWAN™/Sigfox para las regiones de EE.UU., Europa, Latinoamérica y Australia, o utilizar la conectividad WiFi integrada del ESP32.  

El sistema se entrega con los módulos de comunicación inalámbrica solicitados. Siguiendo la filosofía del producto, todos los complementos mencionados pueden configurarse según las necesidades del usuario, permitiendo la transmisión remota de datos mediante diversas tecnologías inalámbricas populares. Además, los clientes tienen la opción de adquirir membresías para el uso de una plataforma de monitoreo y configuración remota, respaldada por el fabricante.