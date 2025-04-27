#ifndef UTILITIESFUNCTIONS_H
#define UTILITIESFUNCTIONS_H

#include "Structs.h"
#include <Arduino.h>
#include "FunctionsEspNow.h"
#include <Wire.h> //Libreria para I2C

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

tm convertStringToTm(char *date)
{
  tm timeInfo;
  memset(&timeInfo, 0, sizeof(timeInfo));

  sscanf(date, "%d-%d-%dT%d:%d:%d",
         &timeInfo.tm_year, &timeInfo.tm_mon, &timeInfo.tm_mday,
         &timeInfo.tm_hour, &timeInfo.tm_min, &timeInfo.tm_sec);

  timeInfo.tm_year -= 1900; // tm_year = años desde 1900
  timeInfo.tm_mon -= 1;     // tm_mon es 0-11

  return timeInfo;
}

void printColorSensorInfo(Adafruit_TCS34725 &tcs)
{
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);
  Serial.print("Color Temp: ");
  Serial.print(colorTemp, DEC);
  Serial.print(" K - ");
  Serial.print("Lux: ");
  Serial.print(lux, DEC);
  Serial.print(" - ");
  Serial.print("R: ");
  Serial.print(r, DEC);
  Serial.print(" ");
  Serial.print("G: ");
  Serial.print(g, DEC);
  Serial.print(" ");
  Serial.print("B: ");
  Serial.print(b, DEC);
  Serial.print(" ");
  Serial.print("C: ");
  Serial.print(c, DEC);
  Serial.print(" ");
  Serial.println(" ");
}

int getLuminosity(Adafruit_TCS34725 &tcs)
{
  uint16_t r, g, b, c, lux;
  tcs.getRawData(&r, &g, &b, &c);
  lux = tcs.calculateLux(r, g, b);
  return lux;
}

void printLightDevices(const lightDevices &devices)
{
  for (int i = 0; i < numLightDevices; i++)
  {
    Serial.print("Dispositivo ");
    Serial.println(i + 1);

    Serial.print("  Pin LED: ");
    Serial.println(devices.lightDev[i].pin);

    Serial.print("  Modo: ");
    Serial.println(devices.lightDev[i].mode);

    Serial.print("  Pin de presencia: ");
    Serial.println(devices.lightDev[i].presencePin);

    Serial.print("  Tiempo encendido (ms): ");
    Serial.println(devices.lightDev[i].timeOn);

    Serial.print("  Estado (1=Encendido, 0=Apagado): ");
    Serial.println(devices.lightDev[i].state);

    Serial.print("  Fecha ON: ");
    Serial.println(devices.lightDev[i].onDate);

    Serial.print("  Fecha OFF: ");
    Serial.println(devices.lightDev[i].offDate);

    Serial.print("  Modo programado: ");
    Serial.println(devices.lightDev[i].pMode);

    Serial.print("  Brillo deseado: ");
    Serial.println(devices.lightDev[i].desiredBrightness);

    Serial.print("  Tiempo encendido por defecto (ms): ");
    Serial.println(devices.lightDev[i].defaultTimeOn);

    Serial.println("-----------------------------------");
  }
}

void initializeLED()
{
  ledcSetup(led.bluePwm, 5000, 8);
  ledcAttachPin(led.bluePin, led.bluePwm);
  ledcSetup(led.greenPwm, 5000, 8);
  ledcAttachPin(led.greenPin, led.greenPwm);
  ledcSetup(led.red, 5000, 8);
  ledcAttachPin(led.redPin, led.redPwm);

  ledcWrite(led.bluePwm, 255);
  delay(500);
  ledcWrite(led.bluePwm, 0);
  ledcWrite(led.greenPwm, 255);
  delay(500);
  ledcWrite(led.greenPwm, 0);
  ledcWrite(led.redPwm, 255);
  delay(500);
  ledcWrite(led.redPwm, 0);
}

void controlStatusLED(char state)
{
  switch (state)
  {
  case CLEAR:                     // '1'
    ledcWrite(led.greenPwm, 255); // Verde fuerte
    ledcWrite(led.redPwm, 0);
    ledcWrite(led.bluePwm, 0);
    break;

  case ERROR:                   // '2'
    ledcWrite(led.redPwm, 255); // Rojo fuerte
    ledcWrite(led.greenPwm, 0);
    ledcWrite(led.bluePwm, 0);
    break;

  case WAITING:                  // '3'
    ledcWrite(led.bluePwm, 255); // Azul fuerte
    ledcWrite(led.redPwm, 0);
    ledcWrite(led.greenPwm, 0);
    break;

  case DENIED:                  // '4'
    ledcWrite(led.redPwm, 255); // Naranja (Rojo fuerte + Verde medio)
    ledcWrite(led.greenPwm, 128);
    ledcWrite(led.bluePwm, 0);
    break;

  case ACEPTED:                  // '5'
    ledcWrite(led.bluePwm, 255); // Violeta (Azul + Rojo medio)
    ledcWrite(led.redPwm, 128);
    ledcWrite(led.greenPwm, 0);
    break;

  case DATASENDED:                // '6'
    ledcWrite(led.greenPwm, 255); // Amarillo (Rojo medio + Verde fuerte)
    ledcWrite(led.redPwm, 128);
    ledcWrite(led.bluePwm, 0);
    break;

  case DATARECEIVED:              // '7'
    ledcWrite(led.greenPwm, 255); // Cian (Verde + Azul fuerte)
    ledcWrite(led.bluePwm, 255);
    ledcWrite(led.redPwm, 0);
    break;
    
  case OFF:
    // Apagar el LED si el estado no se reconoce
    ledcWrite(led.redPwm, 0);
    ledcWrite(led.greenPwm, 0);
    ledcWrite(led.bluePwm, 0);
    break;
  }
}

#endif