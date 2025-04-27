#ifndef UTILITIESFUNCTIONS_H
#define UTILITIESFUNCTIONS_H

#include "Structs.h"
#include <Arduino.h>
#include "FunctionsEspNow.h"

void printAccsDevice(const accsDevice &device)
{
  Serial.println("----- accsDevice Info -----");

  Serial.print("Key: ");
  Serial.println(device.key);

  Serial.print("Mode: ");
  switch (device.mode)
  {
  case AccsNFC:
    Serial.println("AccsNFC");
    break;
  case accsOpen:
    Serial.println("accsOpen");
    break;
  case accsClose:
    Serial.println("accsClose");
    break;
  case off:
    Serial.println("off");
    break;
  case on:
    Serial.println("on");
    break;
  case createKey:
    Serial.println("createKey");
    break;
  default:
    Serial.print("Unknown (");
    Serial.print(device.mode);
    Serial.println(")");
    break;
  }

  Serial.print("Status: ");
  switch (device.status)
  {
  case accept:
    Serial.println("accept");
    break;
  case deny:
    Serial.println("deny");
    break;
  case waitingNewKey:
    Serial.println("waitingNewKey");
    break;
  case userRegistered:
    Serial.println("userRegistered");
    break;
  case accsOpen:
    Serial.println("accsOpen");
    break;
  case accsClose:
    Serial.println("accsClose");
    break;
  case on:
    Serial.println("on");
    break;
  case off:
    Serial.println("off");
    break;

  default:
    Serial.print("Unknown (");
    Serial.print(device.status);
    Serial.println(")");
    break;
  }

  Serial.print("Date: ");
  Serial.println(device.date);

  Serial.println("Stored Keys:");
  for (int i = 0; i < MAX_KEYS_NUM; i++)
  {
    if (device.keys[i][0] != '\0')
    { // Solo imprime si hay algo
      Serial.print("  [");
      Serial.print(i);
      Serial.print("] ");
      Serial.println(device.keys[i]);
    }
  }

  Serial.println("---------------------------");
}

void setDateServer(char *date, size_t size)
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

void readKeys()
{
  // Abrir el archivo para leer
  File file = SPIFFS.open("/Keys.txt", "r");
  if (!file)
  {
    Serial.println("No se encontró el archivo");
    return;
  }

  String allKeys = file.readString();
  file.close();
  int start = 0;
  int end = 0;
  int keyID = 0;
  String individualKey = "";

  // Recorrer el string y separar las claves usando '\0' como delimitador
  while (end < allKeys.length())
  {
    if (allKeys[end] == '\n')
    {
      // Guardar la clave y mover al siguiente índice
      individualKey = allKeys.substring(start, end);
      for (int i = 0; i < 15; i++)
      {
        accsData.keys[keyID][i] = individualKey[i]; // Almacenar la clave en el arreglo `keys`
      }
      keyID++; // Incrementar el índice del arreglo de claves

      start = end + 1; // Actualizar el punto de inicio para la siguiente clave
    }
    end++;
  }

  Serial.println("\n\nTodas las Llaves de Acceso:");
  for (keyID = 0; keyID < MAX_KEYS_NUM; keyID++)
  {
    for (int i = 0; i < 15; i++)
    {
      Serial.print(accsData.keys[keyID][i]);
    }
    Serial.println();
  }
}

bool deleteKeys()
{
  if (SPIFFS.remove("/Keys.txt"))
  {
    Serial.println("Llaves de acceso borradas!");
    return true;
  }
  return false;
}

void saveKey(char *key)
{
  // Abrir el archivo para escribir (o crear si no existe)
  File file = SPIFFS.open("/Keys.txt", "a");
  if (!file)
  {
    Serial.println("Error al abrir el archivo de Keys");
    return;
  }

  // Convertir el arreglo de bytes en un String
  String keyStr = "";
  for (int i = 0; i < 15; i++)
  {
    // Convertir cada byte a un carácter y agregarlo al string
    keyStr += (char)key[i];
  }

  file.println(keyStr);
  file.close();
  Serial.printf("\nKey: %s guardado en SPIFFS", keyStr.c_str());
  readKeys();
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