#include <Arduino.h>

#define INPUT1_PIN 16
#define INPUT2_PIN 17

void setup() {
  Serial.begin(115200);

  pinMode(INPUT1_PIN, INPUT);
  pinMode(INPUT2_PIN, INPUT);

  Serial.println("Lectura de entradas digitales iniciada...");
}

void loop() {
  int estado1 = digitalRead(INPUT1_PIN);
  int estado2 = digitalRead(INPUT2_PIN);

  Serial.print("Entrada 16: ");
  Serial.print(estado1);
  Serial.print(" | Entrada 17: ");
  Serial.println(estado2);

  delay(1000);  // Espera 1 segundo
}