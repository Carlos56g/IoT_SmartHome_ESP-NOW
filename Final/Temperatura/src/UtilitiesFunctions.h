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

void printTempDevice(const tempDevice &device) {
  Serial.println("------------ Device Info ------------");
  Serial.print("Desired Temperature: ");
  Serial.println(device.desiredTemperature);
  
  Serial.print("Temperature Margin: ");
  Serial.println(device.temperatureMargin);
  
  Serial.print("Actual Temperature: ");
  Serial.println(device.actualTemperature);
  
  Serial.print("Actual Humidity: ");
  Serial.println(device.actualHumidity);
  
  Serial.print("Actual Pressure: ");
  Serial.println(device.actualPressure);
  
  Serial.print("Mode: ");
  Serial.println(device.mode);
  
  Serial.print("On Date: ");
  Serial.println(device.onDate);
  
  Serial.print("Off Date: ");
  Serial.println(device.offDate);
  Serial.println("-----------------------------------");
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

void initAHT20(){
  // Objetos de tipo sensor
  Adafruit_BMP280 myBMP; // Presion Atm
  AHT20 myAHT20;         // Temperatura y Humedad
  Wire.begin(SDA_PIN, SCL_PIN);
  while (myAHT20.begin() != true)
  {
    Serial.println(F("No se ha conectado AHT20 o falló al cargar el coeficiente de calibración")); //(F()) guarda el string en la Flash para mantener la memoria dinámica libre
    delay(5000);
  }
  Serial.println(F("AHT20 OK"));

  if (!myBMP.begin())
  {
    Serial.println(F("No se encuentra un sensor BMP280 compatible, revisa la conexión"));
    while (1);
  }

  /* Configuración default según el datasheet. */
  myBMP.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Modo de Operación. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtrado. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Tiempo de Standby. */
}


#endif