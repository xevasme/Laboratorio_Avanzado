# Sistema de automatización y monitoreo remoto de variables físicas mediante ESP32 y servicios en la nube

## Descripción general

Este repositorio contiene los archivos correspondientes a un sistema de automatización experimental orientado a la adquisición, almacenamiento y visualización remota de variables físicas en tiempo real. El sistema integra un microcontrolador ESP32 con una plataforma de almacenamiento en la nube y una interfaz web, permitiendo el monitoreo continuo de datos experimentales sin intervención manual durante la toma de mediciones.

El desarrollo de este proyecto se enmarca dentro del curso **Laboratorio Avanzado I**, cuyo objetivo principal es el uso de herramientas computacionales modernas para la automatización de experimentos físicos, la gestión de datos y el análisis remoto de fenómenos físicos.

---

## Arquitectura del sistema

El sistema implementado se basa en una arquitectura IoT distribuida compuesta por tres bloques principales:

1. **Sistema embebido (ESP32)**  
   Encargado de la adquisición de la variable física mediante una entrada analógica y de la transmisión automática de los datos a la nube a través de una conexión WiFi.

2. **Plataforma en la nube (Firebase)**  
   Utilizada para el almacenamiento de los datos experimentales en una base de datos en tiempo real y para el alojamiento de la interfaz web de visualización.

3. **Interfaz web de monitoreo**  
   Permite la visualización remota y en tiempo real de los datos adquiridos por el sistema embebido desde cualquier dispositivo con acceso a internet.

Esta arquitectura permite automatizar el ciclo completo del experimento: medición, registro, almacenamiento y visualización de la variable física.

---

## Descripción del sistema embebido

El microcontrolador ESP32 realiza la lectura de una señal analógica a través del pin GPIO34. En el desarrollo experimental, esta señal es generada mediante un potenciómetro, el cual simula el comportamiento de un sensor de humedad u otra variable física analógica.

El firmware implementado permite:
- Conectarse a una red WiFi.
- Autenticarse en la plataforma en la nube.
- Leer periódicamente la señal analógica.
- Enviar automáticamente los datos a una base de datos en tiempo real.

Este proceso elimina la necesidad de intervención manual durante la adquisición de datos y permite el registro continuo de la variable física.

---

## Interfaz web y visualización de datos

La visualización de los datos se realiza mediante un sitio web alojado en la nube. La interfaz fue desarrollada a partir de una plantilla web editable, la cual fue adaptada para conectarse directamente con la base de datos en tiempo real.

El uso de una base de datos en tiempo real permite que los valores mostrados en la interfaz se actualicen de forma casi instantánea cuando se produce un cambio en la variable física medida, facilitando el monitoreo remoto del experimento.



