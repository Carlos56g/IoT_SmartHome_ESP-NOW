#include <WiFi.h>
#include <esp_now.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include "FunctionsEspNow.h" //Archivo de cabecera que contiene la funciones personalizadas que se usaran con ESPNOW
#include "Structs.h"
#include "UtilitiesFunctions.h"

espNowData receivedData;
std::list<accsEvent> accsHistory;

AsyncWebServer Server(80); // Numero de Puerto a Usar

void setup()
{
  Serial.begin(115200);
  initEspNow();
  // Montar el sistema de archivos
  if (!SPIFFS.begin(true))
  {
    Serial.println("Error montando SPIFFS");
    return;
  }
  //printSPIIFFiles();

  // Servir el archivo index.html
  Server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  // Servir el archivo AccsHistory.html
  Server.on("/Accs/History", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/accsHistory.html", "text/html"); });

  Server.on("/Accs/Keys", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/accsKeys.html", "text/html"); });

  Server.on("/Temp/Prog", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/tempProg.html", "text/html"); });

  Server.on("/api/Accs/Keys/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              requestModule(accsModule, requestData);
              JsonDocument doc;
              JsonArray arr = doc.to<JsonArray>();
              for (int i = 0; i < MAX_KEYS_NUM; i++)
              {
                if (strlen(receivedData.accessModule.keys[i]) > 0)
                { // Si la llave no está vacía
                  JsonObject obj = arr.createNestedObject();
                  obj["key"] = receivedData.accessModule.keys[i]; // Agregar la llave al array
                }
              }

              String output;
              serializeJson(doc, output);                     // Serializa el JSON
              request->send(200, "application/json", output); // Envia la respuesta
            });

  Server.on("/api/Accs/History/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
  readAccsHistory();
  JsonDocument doc;
  JsonArray arr = doc.to<JsonArray>();

  for (const accsEvent& evt : accsHistory) {
    JsonObject obj = arr.createNestedObject();
    obj["key"] = evt.key;
    obj["date"] = evt.date;
    obj["status"] = String(evt.status);
  }

  String output;
  serializeJson(doc, output);
  request->send(200, "application/json", output); });

  Server.on("/api/Accs/History/delete", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    deleteAccsHistory();
    request->send(200, "text/plain", "Historial eliminado!"); });

  Server.on("/api/Accs/Keys/delete", HTTP_GET, [](AsyncWebServerRequest *request)
            {
    requestModule(accsModule,deleteData);
    request->send(200, "text/plain", "Todas las Keys Eliminadas!"); });

  Server.on("/api/Accs/Mode", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
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
    sendData(accsModule, receivedData);
    request->send(200, "application/json", "{\"status\":\"ok\"}"); });

  Server.on("/api/Temp/Mode", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
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
    updateTempData(doc);
    sendData(tempModule, receivedData);
    request->send(200, "application/json", "{\"status\":\"ok\"}"); });

    Server.on("/api/Light/Mode", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
            {
    String body = String((char*)data).substring(0, len);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, body);

    if(doc["on"]){
      Serial.println("Encender Modulo");
      receivedData.lightModule.on=true;
    }
    else{
      Serial.println("Apagar Modulo");
      receivedData.lightModule.on=false;
    }
    
    sendData(lightModule, receivedData);
    request->send(200, "application/json", "{\"status\":\"ok\"}"); });


  Server.on("/api/Lights/Update", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
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
    updateLightsData(doc);
    sendData(lightModule, receivedData);
    request->send(200, "application/json", "{\"status\":\"ok\"}"); });

    Server.on("/api/TempProg/Update", HTTP_POST, [](AsyncWebServerRequest *request) {}, NULL, [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
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
updateTempProgData(doc);
sendData(tempModule, receivedData);
request->send(200, "application/json", "{\"status\":\"ok\"}"); });

  Server.on("/api/Temp/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              requestModule(tempModule, requestData); // Pedimos los Datos
              JsonDocument doc;
              doc = updateDoc(tempModule); // Actualizamos la respuesta con los datos recibidos
              String output;
              serializeJson(doc, output);                     // Serializa el JSON
              request->send(200, "application/json", output); // Envia la respuesta
            });

  Server.on("/api/Lights/get", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              requestModule(lightModule, requestData); // Pedimos los Datos
              JsonDocument doc;
              doc = updateDoc(lightModule);
              String output;
              serializeJson(doc, output);           // Serializa el JSON
              Serial.println(output);                     
              request->send(200, "application/json", output); // Envia la respuesta
            });

  // Archivos estáticos
  Server.serveStatic("/style.css", SPIFFS, "/style.css");
  Server.serveStatic("/script.js", SPIFFS, "/script.js");

  // Ruta para obtener la informacion del modulo de Luz



  Server.begin();
}

void loop()
{
}
