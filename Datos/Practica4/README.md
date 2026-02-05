# Sistema de Monitoreo IoT en Tiempo Real: ESP32 + Firebase

Este proyecto consiste en un sistema de adquisición de datos climáticos y presión (simulada) utilizando un microcontrolador **ESP32**, el sensor **DHT11** y la plataforma **Google Firebase**. Los datos se transmiten vía Wi-Fi y se visualizan en tiempo real a través de un Dashboard web.

---

## 🚀 Características
- **Monitoreo Multisensorial:** Lectura de temperatura, humedad y presión.
- **Base de Datos en Tiempo Real:** Integración con Firebase Realtime Database.
- **Dashboard Web:** Visualización reactiva mediante Firebase Hosting.
- **Seguridad:** Autenticación de usuario para el envío de datos.

## 🛠️ Hardware Requerido
- **ESP32** (Modelo Doit DevKit V1 o similar).
- **Sensor DHT11** (Temperatura y Humedad).
- **Potenciómetro de 10kΩ** (Simulación de Presión).
- **Protoboard y Jumpers**.

### Diagrama de Conexiones
| Componente     | Pin Componente | Pin ESP32 | Función       |
|----------------|----------------|-----------|---------------|
| Potenciómetro  | Centro (Señal) | GPIO 34   | ADC (Presión) |
| Sensor DHT11   | Data (S)       | GPIO 14   | Digital I/O   |
| Alimentación   | VCC / +        | 3.3V      | Energía       |
| Tierra         | GND / -        | GND       | Tierra        |



---

## 💻 Configuración del Software

### 1. Requisitos Previos
- **Arduino IDE** con el soporte para placas ESP32 instalado.
- **Node.js** instalado (para el despliegue web).
- Cuenta en **Google Firebase**.

### 2. Librerías de Arduino
Es necesario instalar las siguientes librerías desde el Gestor de Bibliotecas:
- `Firebase Arduino Client Library for ESP8266 and ESP32` (por Mobizt).
- `DHT sensor library` (por Adafruit).

### 3. Configuración del Proyecto (Firmware)
En el archivo `firmware/main.ino`, debes reemplazar los siguientes marcadores de posición con tus credenciales:
```cpp
#define WIFI_SSID "TU_WIFI"
#define WIFI_PASSWORD "TU_PASSWORD"
#define API_KEY "TU_FIREBASE_API_KEY"
#define USER_EMAIL "tu@email.com"
#define USER_PASSWORD "tu_password"
#define DATABASE_URL "tu-proyecto.firebaseio.com"



