#ifndef UTILITIESFUNCTIONS_H
#define UTILITIESFUNCTIONS_H

#include "Structs.h"
#include "FunctionsEspNow.h"
#include <Arduino.h>

void printAccsDevice(const accsDevice &device)
{
  Serial.print("createKey: ");
  Serial.println(device.createKey);

  Serial.print("Key: ");
  for (int i = 0; i < 16; i++)
  {
    Serial.print(device.key[i]);
  }
  Serial.println();

  Serial.print("Mode: ");
  Serial.println(device.mode);

  Serial.println("Keys:");
  for (int i = 0; i < MAX_KEYS_NUM; i++)
  {
    Serial.print("Key ");
    Serial.print(i);
    Serial.print(": ");
    for (int j = 0; j < 16; j++)
    {
      Serial.print(device.keys[i][j]);
    }
    Serial.println();
  }

  Serial.print("Status: ");
  Serial.println(device.status);
}

void initializeData()
{
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
  receivedData.temperatureModule.mode = 'A';

  // Llenar el dispositivo de acceso
  receivedData.accessModule.createKey = true;
  strcpy(receivedData.accessModule.key, "CarlosGarcia001"); // Llave de acceso
  receivedData.accessModule.mode = 'K';
}

void printAccsStatus(const accsDevice &device)
{
  switch (device.status)
  {
  case 'A':
    Serial.println("\nEstado: Aceptado");
    Serial.printf("Key: %s", device.key);
    Serial.printf("\nFecha: %s", device.date);
    break;
  case 'D':
    Serial.println("Estado: Denegado");
    Serial.printf("Key: %s", device.key);
    Serial.printf("\nFecha: %s", device.date);
    break;
  case 'R':
    Serial.println("\nEstado: Usuario Registrado");
    Serial.printf("Key: %s", device.key);
    Serial.printf("\nFecha: %s", device.date);
    break;
  case 'W':
    Serial.println("\nEstado: Esperando Registro");
    Serial.printf("Key: %s", device.key);
    Serial.printf("\nFecha: %s", device.date);
    break;
  case 'O':
    Serial.println("\nEstado: Abierto");
    Serial.printf("Key: %s", device.key);
    Serial.printf("\nFecha: %s", device.date);
    break;
  case 'C':
    Serial.println("\nEstado: Cerrado");
    Serial.printf("Key: %s", device.key);
    Serial.printf("\nFecha: %s", device.date);
    break;
  case 'N':
  Serial.println("\nEstado: Deshabilitado");
  Serial.printf("Key: %s", device.key);
  Serial.printf("\nFecha: %s", device.date);
    break;
  case 'K':
  Serial.println("\nEstado: Habilitado");
  Serial.printf("Key: %s", device.key);
  Serial.printf("\nFecha: %s", device.date);
  break;
  }
  delay(1000);
}

void getDateServer(char *date, size_t size)
{
  const char *ntpServer = "pool.ntp.org";
  const long gmtOffset_sec = 0;        // Offset horario GMT (0 para UTC)
  const int daylightOffset_sec = 3600; // Ajuste de horario de verano (si aplica)
  struct tm timeInfo;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  if (!getLocalTime(&timeInfo))
  {
    Serial.println("Error al obtener la hora");
    snprintf(date, size, "Error");
    return;
  }

  strftime(date, size, "%Y-%m-%dT%H:%M:%S", &timeInfo); // Lo castea a tipo ISO
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

void jsonCharParse(char *objective, JsonDocument doc, const char *index)
{
  String objectiveTemp = doc[index];
  for (int i = 0; i < objectiveTemp.length(); i++)
  {
    objective[i] = static_cast<char>(objectiveTemp[i]);
  }
  objective[objectiveTemp.length()] = '\0';
}

void getActualDate(char *date, size_t size)
{
  struct tm timeInfo;
  short tries;
  while (!getLocalTime(&timeInfo))
  {
    Serial.println("Error al obtener la hora");
    getDateServer(date, size);
    delay(1000);
    if (tries > 10)
    {
      Serial.println("No fue posible obtener la fecha por parte de internet");
      return;
    }
    tries++;
  }
  strftime(date, size, "%Y-%m-%dT%H:%M:%S", &timeInfo); // Lo castea a tipo ISO
}

void updateAccssData(JsonDocument doc){
  receivedData.accessModule.createKey = doc["createKey"];
            
  jsonCharParse(receivedData.accessModule.key,doc,"key");

  receivedData.accessModule.mode = ((const char*)doc["mode"])[0];

  receivedData.accessModule.status = ((const char*)doc["status"])[0];

  getActualDate(receivedData.accessModule.date,sizeof(receivedData.accessModule.date));

}

#endif