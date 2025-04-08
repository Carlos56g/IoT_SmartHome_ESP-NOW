#include "Structs.h"
#include <Arduino.h>

void printAccsDevice(const accsDevice& device) {
    Serial.print("createKey: ");
    Serial.println(device.createKey);
    
    Serial.print("Key: ");
    for (int i = 0; i < 16; i++) {
      Serial.print(device.Key[i]);
    }
    Serial.println();
  
    Serial.print("Mode: ");
    Serial.println(device.Mode);
  
    Serial.println("Keys:");
    for (int i = 0; i < MAX_KEYS_NUM; i++) {
      Serial.print("Key ");
      Serial.print(i);
      Serial.print(": ");
      for (int j = 0; j < 16; j++) {
        Serial.print(device.Keys[i][j]);
      }
      Serial.println();
    }
  
    Serial.print("Status: ");
    Serial.println(device.Status);
  }

  void initializeData() {
  // Rellenar el struct con los datos que deseas enviar
  receivedData.idR = 1;
  receivedData.idE = 2;
  receivedData.instruction = 10;
  
  // Llenar los dispositivos de luz
  receivedData.lightModule[0].pin = 12;
  receivedData.lightModule[0].mode = 'A';
  receivedData.lightModule[0].presence = true;
  receivedData.lightModule[0].presencePin = 5;
  receivedData.lightModule[0].timeOn = 0;
  receivedData.lightModule[0].state = false;
  receivedData.lightModule[0].defaultTime = 10;

  // Llenar el dispositivo de temperatura
  receivedData.temperatureModule.desiredTemperature = 22.5;
  receivedData.temperatureModule.temperatureInterval = 2.0;
  receivedData.temperatureModule.actualTemperature = 21.0;
  receivedData.temperatureModule.actualHumidity = 50.0;
  receivedData.temperatureModule.actualPressure = 1015;
  receivedData.temperatureModule.Mode = 'A';

  // Llenar el dispositivo de acceso
  receivedData.accessModule.createKey = true;
  strcpy(receivedData.accessModule.Key, "CarlosGarcia001");  // Llave de acceso
  receivedData.accessModule.Mode = 'K';
}