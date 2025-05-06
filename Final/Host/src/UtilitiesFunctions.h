#ifndef UTILITIESFUNCTIONS_H
#define UTILITIESFUNCTIONS_H

#include "Structs.h"
#include "FunctionsEspNow.h"
#include <Arduino.h>

void controlStatusLED(char state);

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
  case deleteData:
    Serial.println("deleteData");
    break;
  // Puedes agregar más si usas otros en status
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

void printAccsHistory(std::list<accsEvent> accsHistory)
{
  accsEvent accsEvent;
  Serial.println(accsHistory.size());
  Serial.println("Historial De Acceso: ");
  while (!accsHistory.empty())
  {
    accsEvent = accsHistory.back();
    accsHistory.pop_back();
    Serial.print("Key: ");
    Serial.print(accsEvent.key);
    Serial.print("\tFecha: ");
    Serial.print(accsEvent.date);
    Serial.print("\tStatus: ");
    Serial.print(accsEvent.status);
    Serial.println();
  }
}

void getDateServer(char *date, size_t size)
{
  const char *ntpServer = "pool.ntp.org";
  const long gmtOffset_sec = -7 * 3600; // Offset para UTC-6
  const int daylightOffset_sec = 3600;  // Ajuste de horario de verano (si aplica)
  struct tm timeInfo;

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  if (!getLocalTime(&timeInfo))
  {
    Serial.println("Error al obtener la hora");
    controlStatusLED(ERROR);
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
      controlStatusLED(ERROR);
      return;
    }
    tries++;
  }
  strftime(date, size, "%Y-%m-%dT%H:%M:%S", &timeInfo); // Lo castea a tipo ISO
}

void updateAccssData(JsonDocument doc)
{

  jsonCharParse(receivedData.accessModule.key, doc, "key");

  receivedData.accessModule.mode = ((const char *)doc["mode"])[0];

  getActualDate(receivedData.accessModule.date, sizeof(receivedData.accessModule.date));
}

void updateTempData(JsonDocument doc)
{
  receivedData.temperatureModule.status = ((const char *)doc["status"])[0]; // Siempre cambiamos el Status
  if (doc["mode"] != "")
  { // Si tiene Modo, es que estamos en Details de Temperatura
    receivedData.temperatureModule.mode = ((const char *)doc["mode"])[0];
    receivedData.temperatureModule.desiredTemperature = doc["desiredTemperature"];
  }
}

void updateTempProgData(JsonDocument doc)
{
  strncpy(receivedData.temperatureModule.tempDataProg.onDate, doc["tempDataProg"]["onDate"], 20);

  strncpy(receivedData.temperatureModule.tempDataProg.offDate, doc["tempDataProg"]["offDate"], 20);

  receivedData.temperatureModule.tempDataProg.desiredTemperature = doc["tempDataProg"]["desiredTemperature"];

  receivedData.temperatureModule.tempDataProg.mode = ((const char *)doc["tempDataProg"]["mode"])[0];
}

JsonDocument updateDoc(char module)
{
  JsonDocument doc;
  switch (module)
  {
  case accsModule:
    doc["key"] = receivedData.accessModule.key;
    doc["status"] = (signed char)receivedData.accessModule.status;
    doc["date"] = receivedData.accessModule.date;
    doc["mode"] = (signed char)receivedData.accessModule.mode;
    break;

  case tempModule:
    doc["actualTemperature"] = receivedData.temperatureModule.actualTemperature;
    doc["actualHumidity"] = receivedData.temperatureModule.actualHumidity;
    doc["desiredTemperature"] = receivedData.temperatureModule.desiredTemperature;
    doc["mode"] = (signed char)receivedData.temperatureModule.mode;
    doc["status"] = (signed char)receivedData.temperatureModule.status;
    doc["tempDataProg"]["mode"] = (signed char)receivedData.temperatureModule.tempDataProg.mode;
    doc["tempDataProg"]["onDate"] = receivedData.temperatureModule.tempDataProg.onDate;
    doc["tempDataProg"]["offDate"] = receivedData.temperatureModule.tempDataProg.offDate;
    doc["tempDataProg"]["desiredTemperature"] = receivedData.temperatureModule.tempDataProg.desiredTemperature;
    break;

  case lightModule:
    doc["on"] = receivedData.lightModule.on;
    JsonArray arr = doc.createNestedArray("lightDevices");
    for (int i = 0; i < numLightDevices; i++)
    {
      JsonObject individualLight = arr.createNestedObject();
      individualLight["mode"] = (signed char)receivedData.lightModule.lightDev[i].mode;
      individualLight["pMode"] = (signed char)receivedData.lightModule.lightDev[i].pMode;
      individualLight["onDate"] = receivedData.lightModule.lightDev[i].onDate;
      individualLight["offDate"] = receivedData.lightModule.lightDev[i].offDate;
      individualLight["state"] = (signed char)receivedData.lightModule.lightDev[i].state;
      individualLight["desiredBrightness"] = receivedData.lightModule.lightDev[i].desiredBrightness;
      individualLight["defaultTimeOn"] = receivedData.lightModule.lightDev[i].defaultTimeOn;
    }
    break;
  }
  return doc;
}

void updateLightsData(JsonDocument doc)
{
  JsonArray arr = doc["lightData"].as<JsonArray>();
  for (int i = 0; i < numLightDevices && i < arr.size(); i++)
  {
    JsonObject obj = arr[i];

    receivedData.lightModule.lightDev[i].mode = ((const char *)obj["mode"])[0];
    receivedData.lightModule.lightDev[i].pMode = ((const char *)obj["pMode"])[0];

    strncpy(receivedData.lightModule.lightDev[i].onDate, obj["onDate"] | "", 20);
    strncpy(receivedData.lightModule.lightDev[i].offDate, obj["offDate"] | "", 20);

    receivedData.lightModule.lightDev[i].desiredBrightness = obj["desiredBrightness"] | 0.0;
    receivedData.lightModule.lightDev[i].defaultTimeOn = obj["defaultTimeOn"] | 0.0;
  }
}

void readAccsHistory()
{
  // Abrir el archivo para leer
  File file = SPIFFS.open("/AccsHistory.txt", "r");
  if (!file)
  {
    return;
  }
  String allAccsStringData = file.readString();
  file.close();
  accsEvent accsEvent;
  String individualAccsStringData;
  int startRow = 0;
  int endRow = 0;
  accsHistory.clear();

  while (true)
  {
    endRow = allAccsStringData.indexOf('\n', startRow);
    if (endRow == -1)
    {
      break;
    }
    individualAccsStringData = allAccsStringData.substring(startRow, endRow);

    int endKey = individualAccsStringData.indexOf(',');
    int endDate = individualAccsStringData.lastIndexOf(','); // Encuentra el final del registro = Status

    String key = individualAccsStringData.substring(0, endKey);                   // Del inicio a el primer caracter nulo = Key
    String date = individualAccsStringData.substring(endKey + 1, endDate);        // Lo restante es la fecha
    String status = individualAccsStringData.substring(endDate + 1, endDate + 2); // Lo ultimo siempre sera el Status

    key.toCharArray(accsEvent.key, sizeof(accsEvent.key));
    accsEvent.status = status[0];
    date.toCharArray(accsEvent.date, sizeof(accsEvent.date));
    accsHistory.push_back(accsEvent);
    startRow = endRow + 1;
  }
}

void saveAccsHistory(accsEvent accsEvent)
{
  // Abrir el archivo para registrar
  File file = SPIFFS.open("/AccsHistory.txt", "a"); // crea un archivo si no esta, y lo abre en modo append
  if (!file)
  {
    return;
  }
  if (accsEvent.status == NULL) // Hay veces que el status se manda Nulo y trueba el archivo de historial
    return;

  file.print(accsEvent.key);
  file.print(',');
  file.print(accsEvent.date);
  file.print(',');
  file.print(accsEvent.status);
  file.print('\n');
  file.close();
}

void deleteAccsHistory()
{
  if (SPIFFS.remove("/AccsHistory.txt"))
  {
    return;
  }
}

void printSPIIFFiles()
{
  Serial.println("Archivos en SPIFFS:");
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while (file)
  {
    Serial.println(file.name());
    file = root.openNextFile();
  }
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

  case SENDLIGHT:               // '4'
    ledcWrite(led.redPwm, 255); // Naranja (Rojo fuerte + Verde medio)
    ledcWrite(led.greenPwm, 128);
    ledcWrite(led.bluePwm, 0);
    break;

  case SENDACCS:                 // '5'
    ledcWrite(led.bluePwm, 255); // Violeta (Azul + Rojo medio)
    ledcWrite(led.redPwm, 128);
    ledcWrite(led.greenPwm, 0);
    break;

  case SENDTEMP:                  // '6'
    ledcWrite(led.greenPwm, 255); // Amarillo (Rojo medio + Verde fuerte)
    ledcWrite(led.redPwm, 128);
    ledcWrite(led.bluePwm, 0);
    break;

  case RECEIVEDLIGHT:             // '7'
    ledcWrite(led.greenPwm, 255); // Cian (Verde + Azul fuerte)
    ledcWrite(led.bluePwm, 255);
    ledcWrite(led.redPwm, 0);
    break;

  case RECEIVEDACCS:              // '8'
    ledcWrite(led.bluePwm, 128);  // Azul bajito
    ledcWrite(led.greenPwm, 255); // Verde fuerte
    ledcWrite(led.redPwm, 0);
    break;

  case RECEIVEDTEMP:            // '9'
    ledcWrite(led.redPwm, 255); // Rosa (Rojo fuerte + Azul medio)
    ledcWrite(led.bluePwm, 128);
    ledcWrite(led.greenPwm, 0);
    break;

  default:
    // Apagar el LED si el estado no se reconoce
    ledcWrite(led.redPwm, 0);
    ledcWrite(led.greenPwm, 0);
    ledcWrite(led.bluePwm, 0);
    break;
  }
}

#endif