unsigned long tiempoAnterior = 0;
const long intervalo = 1000;  // 1 segundo
int led = 2;

void setup() {
  pinMode(led,OUTPUT);

}

void loop() {
  unsigned long tiempoActual = millis();

  if (tiempoActual - tiempoAnterior >= intervalo) {
    tiempoAnterior = tiempoActual;  // Actualiza el tiempo
  
  digitalWrite(led,!digitalRead(led)); // Cambia el estado del Led
    
}
}
