#include <WiFi.h>
#include <esp_now.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "FunctionsEspNow.h" //Archivo de cabecera que contiene la funciones personalizadas que se usaran con ESPNOW
#include "Structs.h"
#include "UtilitiesFunctions.h"
espNowData receivedData;

AsyncWebServer Server(80); // Numero de Puerto a Usar

void setup()
{
  Serial.begin(115200);
  initializeData();
  initEspNow();
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
            { sendData(2, receivedData); });

  // Ruta para obtener la informacion del modulo de Luz
  Server.on("/getTemp", HTTP_GET, [](AsyncWebServerRequest *request)
            { sendData(1, receivedData); });

  /////////////////////////////////////// ACCESO
  // Metodo para Crear un Nuevo Registro
  Server.on("/Accs/CreateKey", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
                String body = String((char*)data).substring(0, len);

                Serial.println("📩 Body recibido:");
                Serial.println(body);

                JsonDocument doc; // Usar JsonDocument

                // Deserializar el JSON recibido
                DeserializationError error = deserializeJson(doc, body);
            
                if (error) {
                  Serial.println("Error al deserializar el JSON");
                  request->send(400, "application/json", "{\"error\":\"Bad JSON\"}");
                  return;
                }
            
                // Asignar los valores del JSON al struct, haciendo la conversión de char a unsigned char
                updateAccssData(doc);
                sendData(3, receivedData);

                // Lógica de procesamiento de datos
                  // Función que procesa y envía la data

                request->send(200, "application/json", "{\"status\":\"ok\"}"); });
  Server.on("/Accs/Open", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
                String body = String((char*)data).substring(0, len);

                Serial.println("📩 Body recibido:");
                Serial.println(body);

                JsonDocument doc; // Usar JsonDocument

                // Deserializar el JSON recibido
                DeserializationError error = deserializeJson(doc, body);
            
                if (error) {
                  Serial.println("Error al deserializar el JSON");
                  request->send(400, "application/json", "{\"error\":\"Bad JSON\"}");
                  return;
                }
            
                // Asignar los valores del JSON al struct, haciendo la conversión de char a unsigned char

                updateAccssData(doc);
                sendData(3, receivedData);

                request->send(200, "application/json", "{\"status\":\"ok\"}"); });
  Server.on("/Accs/Close", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
                String body = String((char*)data).substring(0, len);

                Serial.println("📩 Body recibido:");
                Serial.println(body);

                JsonDocument doc; // Usar JsonDocument

                // Deserializar el JSON recibido
                DeserializationError error = deserializeJson(doc, body);
            
                if (error) {
                  Serial.println("Error al deserializar el JSON");
                  request->send(400, "application/json", "{\"error\":\"Bad JSON\"}");
                  return;
                }
            
                updateAccssData(doc);
                sendData(3, receivedData);

                request->send(200, "application/json", "{\"status\":\"ok\"}"); });
  Server.on("/Accs/Off", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
                String body = String((char*)data).substring(0, len);

                Serial.println("📩 Body recibido:");
                Serial.println(body);

                JsonDocument doc; // Usar JsonDocument

                // Deserializar el JSON recibido
                DeserializationError error = deserializeJson(doc, body);
            
                if (error) {
                  Serial.println("Error al deserializar el JSON");
                  request->send(400, "application/json", "{\"error\":\"Bad JSON\"}");
                  return;
                }
            
                updateAccssData(doc);
                sendData(3, receivedData);

                request->send(200, "application/json", "{\"status\":\"ok\"}"); });
  Server.on("/Accs/On", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
                String body = String((char*)data).substring(0, len);

                Serial.println("📩 Body recibido:");
                Serial.println(body);

                JsonDocument doc; // Usar JsonDocument

                // Deserializar el JSON recibido
                DeserializationError error = deserializeJson(doc, body);
            
                if (error) {
                  Serial.println("Error al deserializar el JSON");
                  request->send(400, "application/json", "{\"error\":\"Bad JSON\"}");
                  return;
                }
            
                updateAccssData(doc);
                sendData(3, receivedData);

                request->send(200, "application/json", "{\"status\":\"ok\"}"); });
  Server.begin();
}

void loop()
{
  // ACCESO
  printAccsStatus(receivedData.accessModule);
}
