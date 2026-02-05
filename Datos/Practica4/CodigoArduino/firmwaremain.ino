#include <Arduino.h> // Libreria base de Arduino para controlar las funciones principales
#if defined(ESP32)
  #include <WiFi.h> // Libreria para manejar WiFi en ESP32
#elif defined(ESP8266)
  #include <ESP8266WiFi.h> // Libreria para manejar WiFi en ESP8266
#endif
#include <Firebase_ESP_Client.h> // Libreria para conectar y manejar Firebase
#include <Wire.h> // Libreria para comunicación I2C

// ----------- Librerías agregadas para el DHT11 -----------
#include <DHT.h>
#include <DHT_U.h>
// ----------------------------------------------------------

// Proporciona la información del proceso de generación de tokens
#include "addons/TokenHelper.h"
// Proporciona funciones adicionales para la impresión de la carga útil de RTDB y otras funciones auxiliares
#include "addons/RTDBHelper.h"

// Reemplaza con las credenciales de tu red WiFi
#define WIFI_SSID "" // Nombre de la red WiFi
#define WIFI_PASSWORD "" // Contraseña de la red WiFi

// Reemplaza con las credenciales del proyecto Firebase
#define API_KEY "" // Clave API del proyecto Firebase
#define USER_EMAIL "" // Correo electrónico del usuario autorizado
#define USER_PASSWORD "" // Contraseña del usuario autorizado
#define DATABASE_URL "" // URL de la base de datos en tiempo real de Firebase

// Definir objetos de Firebase
FirebaseData fbdo; // Objeto para manejar las operaciones de Firebase
FirebaseAuth auth; // Objeto para la autenticación del usuario
FirebaseConfig config; // Objeto para la configuración de Firebase

// Variable para guardar el UID del usuario
String uid;

// Variables para guardar las rutas de la base de datos
String databasePath; // Ruta principal de la base de datos
String presPath; // Ruta especifica para el valor del potenciómetro
String tempPath; // Ruta especifica para la temperatura
String humPath;  // Ruta específica para la humedad

// Variables de temporizador (enviar nuevas lecturas cada cinco segundos)
unsigned long sendDataPrevMillis = 0; // Última vez que se enviaron datos
unsigned long timerDelay = 3000; // Intervalo de tiempo para enviar datos (5 segundos)

// Pin analógico del ESP32 que queremos leer (puedes usar cualquier otro pin analógico disponible)
const int Pinadc = 34; // En este ejemplo, usamos el pin GPIO 34 del ESP32

// ----------- Configuración del DHT11 -----------
#define DHTPIN 14       // Pin donde conectas la señal del DHT11
#define DHTTYPE DHT11  // Tipo de sensor DHT
DHT dht(DHTPIN, DHTTYPE);
// -------------------------------------------------

// Función para inicializar WiFi con un timeout de 10 segundos
void initWiFi() {
  WiFi.mode(WIFI_STA);    
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Conectar al WiFi usando SSID y contraseña
  Serial.print("Conectando a WiFi ...");
  unsigned long startAttemptTime = millis();

  // Intentar conectar por 10 segundos
  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    Serial.print('.');
    delay(1000);
  }

  // Verificar si la conexión fue exitosa
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("No se pudo conectar al WiFi.");
  } else {
    Serial.println("\nConectado a WiFi. Dirección IP: ");
    Serial.println(WiFi.localIP());  // Imprimir la dirección IP obtenida
  }
}

// Function para escribir valores de tipo float en la base de datos de Firebase
void sendFloat(String path, float value) {
  if (Firebase.RTDB.setFloat(&fbdo, path.c_str(), value)) {
    Serial.print("Escribiendo valor: ");
    Serial.print(value);
    Serial.print(" en la siguiente ruta: ");
    Serial.println(path);
    Serial.println("Aprobado");
  } else {
    Serial.println("Error al escribir en Firebase");
    Serial.println("Razón: " + fbdo.errorReason());
  }
}

void setup() {
  Serial.begin(115200);

  // Inicializar sensores
  pinMode(Pinadc, INPUT);
  dht.begin();  // -------- INICIALIZACIÓN DEL DHT11 --------

  // Inicializar WiFi
  initWiFi();

  // Configurar Firebase
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;

  Firebase.reconnectWiFi(true);

  fbdo.setResponseSize(4096);
  config.token_status_callback = tokenStatusCallback;
  config.max_token_generation_retry = 5;

  Firebase.begin(&config, &auth);

  Serial.println("Obteniendo UID del usuario");
  unsigned long startMillis = millis();
  unsigned long timeout = 10000;

  while ((auth.token.uid) == "") {
    Serial.print('.');
    delay(1000);
    if (millis() - startMillis > timeout) {
      Serial.println("\nError: No se pudo obtener el UID del usuario.");
      break;
    }
  }

  if (auth.token.uid != "") {
    uid = auth.token.uid.c_str();
    Serial.print("UID del usuario: ");
    Serial.println(uid);

    // Actualizar rutas
    databasePath = "/UsersData/" + uid;
    presPath = databasePath + "/pressure";
    tempPath = databasePath + "/temperature";
    humPath = databasePath + "/humidity";
  }
}

void loop() {
  if (Firebase.ready() && (millis() - sendDataPrevMillis > timerDelay || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // ----------- LECTURA DE PRESIÓN (POTENCIÓMETRO) -----------
    int potValue = analogRead(Pinadc);
    float pressure = (potValue / 4096.0) * 3.3;
    sendFloat(presPath, pressure);

    // ----------- LECTURA DEL DHT11 -----------
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Error al leer el DHT11");
    } else {
      sendFloat(tempPath, temperature);
      sendFloat(humPath, humidity);
    }
  }
}
