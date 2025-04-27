#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include "Structs.h"
#include "FunctionsEspNow.h"


lightDevices lightData;
float actualBrightness;

// Objeto de tipo sensor de Luz
Adafruit_TCS34725 myTCS = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);

int getLuminosity(Adafruit_TCS34725 &tcs);
void printColorSensorInfo(Adafruit_TCS34725 &tcs);
void controlLight(float lux, lightDevice &lightDevice);
void initTCS();
void initLightDevices();
void controlLightsDevices();
void validateTime(lightDevice &lightDevice);
void turnOffLights(lightDevices &lightDevices);
void turnOffLight(lightDevice &lightDevice);
void turnOnLight(lightDevice &lightDevice);

void setup()
{
  Serial.begin(115200);
  initTCS(); //Inicializar Sensor de Color
  initLightDevices(); //Inicializa los dispositivos de luz con los parametros preEstablecidos
  InitEspNow(); //Inicia la conexion ESPNOW
}

void loop()
{
  if(lightData.on){
    actualBrightness = getLuminosity(myTCS);
    controlLightsDevices();
  }
  else{
    turnOffLights(lightData);
  }
}

void controlLightsDevices()
{
  for (int i = 0; i < numLightDevices; i++)
  {
    controlLight(actualBrightness, lightData.lightDev[i]);
  }
}

void controlLight(float lux, lightDevice &lightDevice)
{
  switch (lightDevice.mode)
  {
  case on:
    turnOnLight(lightDevice);
    break;

  case off:
    turnOffLight(lightDevice);
    break;
    
  case presence:
    if (lightDevice.presencePin!=-1)
    { // Primero debe de estar configurado un sensor de presencia pora poder usarse
      if (digitalRead(lightDevice.presencePin)==LOW)
      { // Aseguramos que el pin de presencia haya detectado algo
        if (!lightDevice.state)
        { // Si no esta encendido
          turnOnLight(lightDevice);
          lightDevice.timeOn = millis(); // Guardamos el momento en que se encendió
        }
      }
      if (lightDevice.state && millis() - lightDevice.timeOn >= lightDevice.defaultTimeOn) //Millis devuelve el tiempo que lleva prendido el ESP32, si está prendido, y el tiempo en que se encendio el LED-  TiempoActual es mayor que el tiempo configurado, apagarlo
      {
        turnOffLight(lightDevice);
        lightDevice.timeOn = 0;
      }
      if(lightDevice.timeOn==0) //Aseguramos que inicie siempre apagado cuando entre a este modo (Caso de cuando esta encendido y lo cambian a modo de presencia)
        turnOffLight(lightDevice);
    }
    break;

  case autoMode:
    if (lux < lightDevice.desiredBrightness) //Si hay menos luz que la deseada, entonces
      turnOnLight(lightDevice);
    else
      turnOffLight(lightDevice);
    break;

  case presenceAndAuto:
    if (lightDevice.presencePin!=-1)
    { // Primero debe de estar configurado como modo de presencia
      if (digitalRead(lightDevice.presencePin) == LOW)
      { // Aseguramos que el pin de presencia haya detectado algo
        if (!lightDevice.state)
        { // Si no esta encendido
          turnOnLight(lightDevice);
          lightDevice.timeOn = millis(); // Guardamos el momento en que se encendió
        }
      }
      if (lightDevice.state && millis() - lightDevice.timeOn >= lightDevice.defaultTimeOn)
      {
        turnOffLight(lightDevice);
        lightDevice.timeOn = 0;
      }
      if (lightDevice.timeOn != 0)
        return;
    }
    if (lux < lightDevice.desiredBrightness)
    {
      turnOnLight(lightDevice);
    }
    else
    {
      turnOffLight(lightDevice);
    }
    break;
  }

  validateTime(lightDevice);
}

void validateTime(lightDevice &lightDevice){
  struct tm actualTime;
  if(strlen(lightDevice.onDate) > 0)//Encendido Automatico, ya que tiene configurada una fecha de encendido
  {
    while (!getLocalTime(&actualTime)) { //Obtenemos la fecha actual
      sendData(requestTime,lightData);
    };

    tm tmOnDate = convertStringToTm(lightDevice.onDate);
    //Si el tiempo de encendido es menor al tiempo actual, significa que debe de encenderse
    if(mktime(&tmOnDate)<mktime(&actualTime)){
      lightDevice.mode=lightDevice.pMode;
    }
  }

  if(strlen(lightDevice.offDate) > 0){ //Apagado Automatico, ya que tiene configurada una fecha de apagado
    while (!getLocalTime(&actualTime)) { //Obtenemos la fecha actual
      sendData(requestTime,lightData);
      };

      tm tmOffDate = convertStringToTm(lightDevice.offDate);
      //Si el tiempo de apagado es menor al tiempo actual, significa que debe de apagarse
      if(mktime(&tmOffDate)<mktime(&actualTime)){
        lightDevice.mode=off;
        strcpy(lightDevice.offDate, "");
        strcpy(lightDevice.onDate, "");
      }
    }
}

void initTCS()
{
  Wire.begin(SDA_PIN, SCL_PIN); //Inicializa el protocolo de comunicacion I2C

  if (myTCS.begin()) //Inicializa el Sensor
  {
    Serial.println("Found sensor");
  }
  else
  {
    Serial.println("No TCS34725 found ... check your connections");
    while (1)
      ;
  }
}

void initLightDevices()
{

  // Pines de los Leds
  lightData.lightDev[0].pin = LED1_PIN;
  lightData.lightDev[1].pin = LED2_PIN;
  lightData.lightDev[2].pin = LED3_PIN;
  lightData.lightDev[3].pin = LED4_PIN;

  // Pines de los sensores de presencia
  lightData.lightDev[0].presencePin = S1_PIN;
  lightData.lightDev[1].presencePin = S2_PIN;
  lightData.lightDev[2].presencePin = -1;
  lightData.lightDev[3].presencePin = -1;

  for (int i = 0; i < numLightDevices; i++)
  {
    pinMode(lightData.lightDev[i].pin, OUTPUT);
    digitalWrite(lightData.lightDev[i].pin, HIGH);
    delay(300);
    digitalWrite(lightData.lightDev[i].pin, LOW);
    if (lightData.lightDev[i].presencePin != -1)
    {
      pinMode(lightData.lightDev[i].presencePin, INPUT);
    }
  }
}



void turnOffLights(lightDevices &lightDevices){
  for(int i=0;i<numLightDevices;i++){
    turnOffLight(lightDevices.lightDev[i]);
  }
}


void turnOnLight(lightDevice &lightDevice){
  if(!lightDevice.state){
    digitalWrite(lightDevice.pin,HIGH);
    lightDevice.state=true;
    sendData(sendActualData,lightData); //Mandamos el nuevo Status SOLAMENTE SI HUBO CAMBIO
  }
}


void turnOffLight(lightDevice &lightDevice){
  if(lightDevice.state){
    digitalWrite(lightDevice.pin,LOW);
    lightDevice.state=false;
    sendData(sendActualData,lightData); //Mandamos el nuevo Status SOLAMENTE SI HUBO CAMBIO
  }
}