#ifndef UTILITIESFUNCTIONS_H
#define UTILITIESFUNCTIONS_H

#include "Structs.h"
#include <Arduino.h>
#include "FunctionsEspNow.h"
#include <Wire.h>            //Libreria para I2C
#include <AHT20.h>           //Libreria para sensor Temperatura
#include <Adafruit_BMP280.h> //Libreria para sensor Humedad

#define SDA_PIN 21
#define SCL_PIN 22

void printTempDevice(const tempDevice& device) {
  Serial.println("===== tempDevice Info =====");

  Serial.print("Modo: ");
  Serial.println(device.mode);

  Serial.print("Estado: ");
  Serial.println(device.status);

  Serial.print("Temperatura deseada: ");
  Serial.println(device.desiredTemperature);

  Serial.print("Temperatura actual: ");
  Serial.println(device.actualTemperature);

  Serial.print("Humedad actual: ");
  Serial.println(device.actualHumidity);

  Serial.println("---- Programación ----");

  Serial.print("Modo programado: ");
  Serial.println(device.tempDataProg.mode);

  Serial.print("Fecha encendido: ");
  Serial.println(device.tempDataProg.onDate);

  Serial.print("Fecha apagado: ");
  Serial.println(device.tempDataProg.offDate);

  Serial.print("Temperatura programada: ");
  Serial.println(device.tempDataProg.desiredTemperature);

  

  Serial.println("=========================");
}


void setDateString(char *date)
{
  struct tm timeinfo;
  memset(&timeinfo, 0, sizeof(timeinfo)); // Limpiar estructura
  Serial.println("String Recibido: ");
  for (int i = 0; i++; i < sizeof(date))
  {
    Serial.print(date[i]);
  }
  // Parsear string ISO: "YYYY-MM-DDTHH:MM:SS"
  if (sscanf(date, "%d-%d-%dT%d:%d:%d",
             &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday,
             &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec) != 6)
  {
    Serial.println("Error al parsear la fecha");
    return;
  }

  timeinfo.tm_year -= 1900; // Año desde 1900
  timeinfo.tm_mon -= 1;     // Mes 0-11

  time_t t = mktime(&timeinfo); // Convertir a timestamp (segundos desde 1970)
  if (t == -1)
  {
    Serial.println("Error al convertir a timestamp");
    return;
  }
  struct timeval now = {.tv_sec = t};
  settimeofday(&now, NULL);
  Serial.print("Hora establecida desde string: ");
  Serial.println(date);
}

tm convertStringToTm(char* date){
  tm timeInfo;
  memset(&timeInfo, 0, sizeof(timeInfo));

  sscanf(date, "%d-%d-%dT%d:%d:%d",
    &timeInfo.tm_year, &timeInfo.tm_mon, &timeInfo.tm_mday,
    &timeInfo.tm_hour, &timeInfo.tm_min, &timeInfo.tm_sec);
    
    timeInfo.tm_year -= 1900; // tm_year = años desde 1900
    timeInfo.tm_mon -= 1;     // tm_mon es 0-11

    return timeInfo;
}


#endif