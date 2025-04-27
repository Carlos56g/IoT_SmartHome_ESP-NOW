#include <Arduino.h>
#include <Wire.h>            //Libreria para I2C
#include <AHT20.h>           //Libreria para sensor Temperatura
#include <Adafruit_BMP280.h> //Libreria para sensor Humedad
#include "FunctionsEspNow.h"
#include "UtilitiesFunctions.h"

#define SDA_PIN 21
#define SCL_PIN 22
#define fanC 17
#define fanH 16
#define peltier 32
float temperatureMargin = 3; // Intervalo de Temperatura, no modificable por el usuario
char prevStatus;

// Objetos de tipo sensor
Adafruit_BMP280 myBMP; // Presion Atm
AHT20 myAHT20;         // Temperatura y Humedad
tempDevice tempData;
statusLED led;

// Funciones
char validateDesiredTemperature();
void controlTemperatureModule();
void printTempSensorInfo(Adafruit_BMP280 &BMP, AHT20 &AHT20);
void controlTempDevice(char action);
void initPins();
void initAHT20();
void controlTempProgram();

void setup()
{
  initializeLED();
  controlStatusLED(WAITING);
  Serial.begin(115200);
  initAHT20();
  InitEspNow();
  initPins();
  controlStatusLED(CLEAR);
}

void loop()
{
  controlTempProgram();
}

char validateDesiredTemperature()
{
  // Funcion que valida la temperatura
  if (tempData.actualTemperature >= (tempData.desiredTemperature - (temperatureMargin / 2)) && tempData.actualTemperature <= (tempData.desiredTemperature + (temperatureMargin / 2)))
    return nothing; // No hacer nada
  if (tempData.actualTemperature > tempData.desiredTemperature + (temperatureMargin / 2))
    return cold; // Encender para frio
  if (tempData.actualTemperature < tempData.desiredTemperature - (temperatureMargin / 2))
    return hot; // Encender para calor
}

void controlTemperatureModule()
{
  if (tempData.status != off)
  {
    controlStatusLED(CLEAR);
    switch (tempData.mode)
    {
    case off:
      controlTempDevice(off);
      break;

    case air:
      controlTempDevice(air);
      break;

    case hot:
      controlTempDevice(hot);
      break;

    case cold:
      controlTempDevice(cold);
      break;

    case autoMode:
      tempData.actualTemperature = myAHT20.getTemperature();
      tempData.actualHumidity = myAHT20.getHumidity();
      controlTempDevice(validateDesiredTemperature());
      break;
    }
  }
  else{
    controlStatusLED(OFF);
    controlTempDevice(off);
  }
}

void initAHT20()
{
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
    while (1)
      ;
  }

  /* Configuración default según el datasheet. */
  myBMP.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Modo de Operación. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtrado. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Tiempo de Standby. */
}

void initPins()
{
  pinMode(fanC, OUTPUT);    // Pin del motor 1 (Frio)
  pinMode(fanH, OUTPUT);    // Pin del motor 2 (Caliente)
  pinMode(peltier, OUTPUT); // Pin del relevador de la Peltier
  digitalWrite(fanC, LOW);
  digitalWrite(fanH, LOW);
  digitalWrite(peltier, HIGH);
}

void controlTempDevice(char mode)
{
  switch (mode)
  {
  case nothing:
    digitalWrite(fanC, LOW);
    digitalWrite(fanH, LOW);
    digitalWrite(peltier, HIGH);
    controlStatusLED(PELTIEROFF);
    break;

  case cold:
    digitalWrite(fanC, HIGH);
    digitalWrite(fanH, LOW);
    digitalWrite(peltier, LOW);
    controlStatusLED(PELTIERON);
    break;

  case hot:
    digitalWrite(fanC, LOW);
    digitalWrite(fanH, HIGH);
    digitalWrite(peltier, LOW);
    controlStatusLED(PELTIERON);
    break;

  case air:
    digitalWrite(fanH, HIGH);
    digitalWrite(fanC, HIGH);
    digitalWrite(peltier, HIGH);
    controlStatusLED(PELTIEROFF);
    break;

  case off:
    digitalWrite(fanC, LOW);
    digitalWrite(fanH, LOW);
    digitalWrite(peltier, HIGH);
    tempData.status=off;
    break;
  }
}

void controlTempProgram(){
  struct tm actualTime; 

  if(strlen(tempData.tempDataProg.onDate) > 0)//Encendido Automatico
  {
    while (!getLocalTime(&actualTime)) { //Obtenemos la fecha actual
      sendData(requestTime,tempData);
    };

    tm tmOnDate = convertStringToTm(tempData.tempDataProg.onDate);
    //Si el tiempo de encendido es menor al tiempo actual, significa que debe de encenderse
    if(mktime(&tmOnDate)<mktime(&actualTime)){
      tempData.mode=tempData.tempDataProg.mode;
      tempData.desiredTemperature=tempData.tempDataProg.desiredTemperature;
      tempData.status=on;
    }
  }

  if(strlen(tempData.tempDataProg.offDate) > 0){
    while (!getLocalTime(&actualTime)) { //Obtenemos la fecha actual
      sendData(requestTime,tempData);
      };

      tm tmOffDate = convertStringToTm(tempData.tempDataProg.offDate);
      //Si el tiempo de apagado es menor al tiempo actual, significa que debe de apagarse
      if(mktime(&tmOffDate)<mktime(&actualTime)){
        tempData.status=off;
        strcpy(tempData.tempDataProg.offDate, ""); // Ensure offDate is a modifiable character array
        strcpy(tempData.tempDataProg.onDate, "");
      }
    }

  char buffer[25];
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S", &actualTime);
  controlTemperatureModule();
}