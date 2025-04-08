#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_TCS34725.h>

#define SDA_PIN 21
#define SCL_PIN 22


struct lightDevice
{
  int pin; //Pin del led
  char mode; // A=Auto, P=Presence, O=On, L=Off, B=Auto&Presence, T=Time
  bool presence; //True=Puede Configurarse como Luz con Presencia
  int presencePin; //Pin del sensor de presencia
  int timeOn; //lleva el tiempo que se encendio el dispositivo
  bool state; //True=Encendido, False=Apagado
  int defaultTime; //Tiempo de encedido cuando detecta presencia
  char *timeToOn; //Fecha programada de encendido
  char *timeToOff; //Fecha programada de apagado
};

lightDevice lightDevices[] = {
    {14, 'A', false, 0, 0, false, 0},
    {27, 'A', false, 0, 0, false, 0},
    {26, 'P', true, 35, 0, false, 5000},
    {25, 'B', true, 34, 0, false, 5000}};

int  desiredBrightness = 50;
int numberLightDevices = sizeof(lightDevices) / sizeof(lightDevices[0]);

// Objeto de tipo sensor de Luz
Adafruit_TCS34725 myTCS = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);

int getLuminosity(Adafruit_TCS34725 &tcs);
void printColorSensorInfo(Adafruit_TCS34725 &tcs);
void controlLight(float lux, lightDevice &lightDevice);

void setup(void)
{

  Serial.begin(115200);
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


  for (int i = 0; i < numberLightDevices; i++)
  {
    pinMode(lightDevices[i].pin, OUTPUT);
    digitalWrite(lightDevices[i].pin, HIGH);
    delay(300);
    digitalWrite(lightDevices[i].pin, LOW);
    if (lightDevices[i].presence)
    {
      pinMode(lightDevices[i].presencePin, INPUT);
    }
  }
}

void loop(void)
{
  printColorSensorInfo(myTCS);
  float actualBrightness = getLuminosity(myTCS);
  Serial.println(actualBrightness);
  for (int i = 0; i < numberLightDevices; i++)
  {
    controlLight(actualBrightness, lightDevices[i]);
  }
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

void controlLight(float lux, lightDevice &lightDevice)
{
  switch (lightDevice.mode)
  {
  case 'O':
    digitalWrite(lightDevice.pin, HIGH);
    lightDevice.state = true;
    break;
  case 'L':
    digitalWrite(lightDevice.pin, LOW);
    lightDevice.state = false;
    break;
  case 'P':
    if (lightDevice.presence)
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
      if (lightDevice.state && millis() - lightDevice.timeOn >= lightDevice.defaultTime)
      {
        digitalWrite(lightDevice.pin, LOW);
        lightDevice.state = false;
        lightDevice.timeOn=0;
      }
    }
    break;
  case 'A':
    if (lux < desiredBrightness)
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
  case 'B':
    if (lightDevice.presence)
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
      if (lightDevice.state && millis() - lightDevice.timeOn >= lightDevice.defaultTime)
      {
        digitalWrite(lightDevice.pin, LOW);
        lightDevice.state = false;
        lightDevice.timeOn=0;
      }
      if(lightDevice.timeOn!=0)
        return;
    }
    if (lux < desiredBrightness)
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
  case 'T':
    Serial.print("Pending Implementation");
    // Se valida la fecha actual con la fecha que esta configurado el sensor
    // Si se encuentra entre el intervalo de fecha establecido, se encedera el sensor
    // La fecha la proporcionará el HOST
    break;
  default:
    Serial.print("Not valid Mode");
    break;
  }
}
