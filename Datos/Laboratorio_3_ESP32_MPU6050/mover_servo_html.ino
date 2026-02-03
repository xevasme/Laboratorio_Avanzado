#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

// ===========================
// CONFIG WiFi
// ===========================
const char* SSID = "GALLO";
const char* PASSWORD = "tarrojalaputa";

// Servo
Servo myservo;
int servoPin = 18;
int angulo;

WebServer server(80);
WiFiClient clients[10];
int client_count = 0;

Adafruit_MPU6050 mpu;

// Filtro complementario
float roll = 0, pitch = 0;
float alpha = 0.98;
unsigned long lastTime = 0;

// ===========================
// FUNCION PARA ANGULO
// ===========================
float getAngle(float ax, float ay, float az) {
  return atan2(ay, az) * 180.0 / PI;
}

// ===========================
// PÁGINA HTML
// ===========================
const char INDEX_HTML[] PROGMEM =
"<!doctype html>"
"<html><head>"
"<meta charset='utf-8'>"
"<title>ESP32 MPU6050 Live</title>"
"<style>"
"body{font-family:Arial;text-align:center;}"
"canvas{background:#f6f6f6;margin-top:20px;border:1px solid #ccc;}"
".info{margin-top:10px;}"
"</style>"
"</head><body>"
"<h2>MPU6050 Live — ESP32</h2>"
"<div class='info'>Roll: <span id='roll'>0</span>° Pitch: <span id='pitch'>0</span>°</div>"
"<canvas id='c' width='360' height='240'></canvas>"

"<script>"
"const evtSource=new EventSource('/stream');"
"let roll=0,pitch=0;"

"evtSource.onmessage=function(e){"
"  const d=JSON.parse(e.data);"
"  roll=d.roll; pitch=d.pitch;"
"  document.getElementById('roll').innerText=roll.toFixed(1);"
"  document.getElementById('pitch').innerText=pitch.toFixed(1);"
"};"

"const c=document.getElementById('c');"
"const ctx=c.getContext('2d');"

"function draw(){"
"  ctx.clearRect(0,0,c.width,c.height);"
"  ctx.save();"
"  ctx.translate(c.width/2,c.height/2);"
"  ctx.rotate(roll*Math.PI/180);"
"  ctx.fillStyle='#3a86ff';"
"  ctx.fillRect(-100,-40,200,80);"
"  ctx.restore();"
"  requestAnimationFrame(draw);"
"} draw();"
"</script></body></html>";


// ===========================
// SSE HEADERS
// ===========================
void sendSSEHeaders(WiFiClient &client) {
  client.print(
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/event-stream\r\n"
    "Cache-Control: no-cache\r\n"
    "Connection: keep-alive\r\n\r\n"
  );
}


// ===========================
// SERVIDOR HTTP
// ===========================
void handleRoot() {
  server.send(200, "text/html", INDEX_HTML);
}

void handleStream() {
  WiFiClient client = server.client();
  sendSSEHeaders(client);

  if (client_count < 10) {
    clients[client_count++] = client;
  }
}


// ===========================
// LECTURA IMU + SSE
// ===========================
void sendIMUData() {
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x;
  float ay = a.acceleration.y;
  float az = a.acceleration.z;

  float gx = g.gyro.x * PI / 180.0;
  float gy = g.gyro.y * PI / 180.0;

  // Calcular ángulos acelerómetro
  float acc_roll  = atan2(ay, az) * 180 / PI;
  float acc_pitch = atan2(-ax, sqrt(ay*ay + az*az)) * 180 / PI;

  unsigned long now = millis();
  float dt = (now - lastTime) / 1000.0;
  lastTime = now;

  // Filtro complementario
  roll  = alpha * (roll + gx * dt) + (1 - alpha) * acc_roll;
  pitch = alpha * (pitch + gy * dt) + (1 - alpha) * acc_pitch;

  // JSON
  String payload = "{\"roll\":";
  payload += String(roll, 2);
  payload += ",\"pitch\":";
  payload += String(pitch, 2);
  payload += "}";

  // broadcast
  for (int i = 0; i < client_count; i++) {
    if (!clients[i].connected()) continue;
    clients[i].print("data: " + payload + "\n\n");
  }
}


// ===========================
// SETUP
// ===========================
void setup() {
  Serial.begin(115200);
  delay(500);

  // WiFi
  WiFi.begin(SSID, PASSWORD);
  Serial.println("Conectando a WiFi...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }
  Serial.print("\nIP: ");
  Serial.println(WiFi.localIP());

  // MPU
  if (!mpu.begin()) {
    Serial.println("MPU6050 no encontrado");
    while (1);
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);

  server.on("/", handleRoot);
  server.on("/stream", handleStream);
  server.begin();

  lastTime = millis();

  // Servo
  myservo.setPeriodHertz(50);
  myservo.attach(servoPin, 500, 2400);
}


// ===========================
// LOOP
// ===========================
void loop() {
  server.handleClient();
  sendIMUData();

  // ---------------------------
  // CONTROL DEL SERVO
  // ---------------------------
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float anguloMPU = getAngle(a.acceleration.x, a.acceleration.y, a.acceleration.z);

  angulo = map((int)anguloMPU, -90, 90, 0, 180);
  angulo = constrain(angulo, 0, 180);

  myservo.write(angulo);

  delay(50);
}

