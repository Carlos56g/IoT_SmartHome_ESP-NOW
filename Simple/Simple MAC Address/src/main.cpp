//Codigo para conocer la direccion MAC del ESP32
#include "Wifi.h"
void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA); //Station Mode, Modo de Cliente
  Serial.println(WiFi.macAddress());
}

void loop() {
  }
