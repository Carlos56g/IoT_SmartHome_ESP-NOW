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

void setup()
{
  Serial.begin(115200);
  initTCS();
  initLightDevices();
  InitEspNow();
}

void loop()
{
  actualBrightness = getLuminosity(myTCS);
  printLightDevices(lightData);
  controlLightsDevices();
  delay(1000);
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
    digitalWrite(lightDevice.pin, HIGH);
    lightDevice.state = true;
    break;
  case off:
    digitalWrite(lightDevice.pin, LOW);
    lightDevice.state = false;
    break;
  case presence:
    if (lightDevice.presencePin!=-1)
    { // Primero debe de estar configurado como modo de presencia
      if (digitalRead(lightDevice.presencePin) == LOW)
      { // Aseguramos que el pin de presencia haya detectado algo
        if (!lightDevice.state)
        { // Si no esta encendido
          digitalWrite(lightDevice.pin, HIGH);
          lightDevice.timeOn = millis(); // Guardamos el momento en que se encendió
          lightDevice.state = true;
        }
      }
      if (lightDevice.state && millis() - lightDevice.timeOn >= lightDevice.defaultTimeOn)
      {
        digitalWrite(lightDevice.pin, LOW);
        lightDevice.state = false;
        lightDevice.timeOn = 0;
      }
    }
    break;
  case autoMode:
    if (lux < lightDevice.desiredBrightness)
    {
      digitalWrite(lightDevice.pin, HIGH);
      lightDevice.state = true;
    }
    else
    {
      digitalWrite(lightDevice.pin, LOW);
      lightDevice.state = false;
    }
    break;
  case presenceAndAuto:
    if (lightDevice.presencePin!=-1)
    { // Primero debe de estar configurado como modo de presencia
      if (digitalRead(lightDevice.presencePin) == LOW)
      { // Aseguramos que el pin de presencia haya detectado algo
        if (!lightDevice.state)
        { // Si no esta encendido
          digitalWrite(lightDevice.pin, HIGH);
          lightDevice.timeOn = millis(); // Guardamos el momento en que se encendió
          lightDevice.state = true;
        }
      }
      if (lightDevice.state && millis() - lightDevice.timeOn >= lightDevice.defaultTimeOn)
      {
        digitalWrite(lightDevice.pin, LOW);
        lightDevice.state = false;
        lightDevice.timeOn = 0;
      }
      if (lightDevice.timeOn != 0)
        return;
    }
    if (lux < lightDevice.desiredBrightness)
    {
      digitalWrite(lightDevice.pin, HIGH);
      lightDevice.state = true;
    }
    else
    {
      digitalWrite(lightDevice.pin, LOW);
      lightDevice.state = false;
    }
    break;
  }

  validateTime(lightDevice);
}

void validateTime(lightDevice &lightDevice){
  struct tm actualTime;
  if(strlen(lightDevice.onDate) > 0)//Encendido Automatico
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

  if(strlen(lightDevice.offDate) > 0){
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
  Wire.begin(SDA_PIN, SCL_PIN);

  if (myTCS.begin())
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
