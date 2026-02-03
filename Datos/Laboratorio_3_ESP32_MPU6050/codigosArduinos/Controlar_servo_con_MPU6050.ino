#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP32Servo.h>

Adafruit_MPU6050 mpu;

// Variables de tiempo
unsigned long TiempoAnterior = 0;
constinit long intervalo = 20;  // 20 ms

// Servo
Servo myservo;
int servoPin = 18;
int angulo;

// Variables del MPU
int16_t ax, ay, az;
int16_t gx, gy, gz;

// Convertir aceleración a ángulo (balanceo)
float getAngle(int16_t ax, int16_t ay, int16_t az) {
  float angle = atan2(ay, az) * 180.0 / PI;
  return angle;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  while (!Serial) {
    delay(10);
  }

  Serial.println("Adafruit MPU6050 test!");

  // Iniciar MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }

  Serial.println("MPU6050 Found!");

  // Opcional: ajustar rangos
  
  delay(100);
  myservo.setPeriodHertz(50);  // 50 Hz para servo
  myservo.attach(servoPin, 500, 2400); // min y max 
  Serial.println("Todo listo.");
}


void loop() {
  sensors_event_t a, g, temp;
  // Leer aceleración

  mpu.getEvent(&a, &g, &temp);

  // Obtener ángulo
  float anguloMPU = getAngle(a.acceleration.x, a.acceleration.y, a.acceleration.z);
  // Calcular ángulo

  // Limitar ángulo a 0–180
  angulo = map((int)anguloMPU, -90, 90, 0, 180);
  angulo = constrain(angulo, 0, 180);

  // Mover servo
  myservo.write(angulo);

  // Mostrar datos
  Serial.print("Ángulo: ");
  Serial.println(angulo);

  delay(20);

}
