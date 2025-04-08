#include <WiFi.h>
#include <esp_now.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include "FunctionsEspNow.h" //Archivo de cabecera que contiene la funciones personalizadas que se usaran con ESPNOW
#include "Structs.h"
#include "printFunctions.h"

espNowData receivedData;

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


// Credenciales de la Red
const char *ssid = "Xiaomi_F85B";
const char *password = "88889999";

AsyncWebServer Server(80); // Numero de Puerto a Usar
String header;             // Variable para Almacenar el HTTP Request

void setup()
{
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA); // Modo Access Point y Cliente WiFi
  // Conexion a la red de Internet
  Serial.printf("\n\nConectando a la RED: %s\n", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
  }

  // Imprime la Direccion IP del Server para acceder a el
  Serial.println("\n\nWiFi Conectado Corecctamente!\nDireccion IP: ");
  Serial.print(WiFi.localIP());
  Serial.printf("\nCanal Wifi: %d\n\n", WiFi.channel());

  InitEspNow();
  initializeData();

  // Montar el sistema de archivos
  if (!SPIFFS.begin(true))
  {
    Serial.println("Error montando SPIFFS");
    return;
  }

  // Servir el archivo index.html
  Server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  // Archivos estáticos
  Server.serveStatic("/style.css", SPIFFS, "/style.css");
  Server.serveStatic("/script.js", SPIFFS, "/script.js");

  // Ruta para obtener la informacion del modulo de Luz
  Server.on("/getLights", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              SendMessage(2, receivedData);
            });

  // Ruta para obtener la informacion del modulo de Luz
  Server.on("/getTemp", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              SendMessage(1, receivedData);
            });

  // Ruta para obtener la informacion del modulo de Luz
  Server.on("/getAccess", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              SendMessage(3, receivedData);
            });
  Server.begin();
}


void loop()
{
  printAccsDevice(receivedData.accessModule);
  delay(1000);
}

