#include <Arduino.h>
#include <Wire.h>            //Libreria para I2C
#include <AHT20.h>           //Libreria para sensor Temperatura
#include <Adafruit_BMP280.h> //Libreria para sensor Humedad

#define SDA_PIN 21
#define SCL_PIN 22
#define L293D_PIN1 16
#define L293D_PIN2 17
#define PLT_PIN 32

// Variables
float desiredTemperature = 29;
float temperatureInterval = 5;

// Objetos de tipo sensor
Adafruit_BMP280 myBMP; // Presion Atm
AHT20 myAHT20;         // Temperatura y Humedad

// Funciones
void printTempSensorInfo(Adafruit_BMP280& BMP, AHT20& AHT20);
char validateDesiredTemperature(float actualTemperature, float desiredTemperature);
void controlTemperatureModule(float actualTemperature, char action);

void setup()
{
  Serial.begin(115200);
  Serial.println(F("AHT20+BMP280 test"));

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
    while (1);
  }

  /* Configuración default según el datasheet. */
  myBMP.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Modo de Operación. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtrado. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Tiempo de Standby. */

  pinMode(L293D_PIN1, OUTPUT); // Pin del motor 1 (Frio)
  pinMode(L293D_PIN2, OUTPUT); // Pin del motor 2 (Caliente)
  pinMode(PLT_PIN, OUTPUT);    // Pin del relevador de la Peltier
  digitalWrite(L293D_PIN1, LOW);
  digitalWrite(L293D_PIN2, LOW);
  digitalWrite(PLT_PIN, HIGH);
}

void loop()
{
  printTempSensorInfo(myBMP, myAHT20);
  float actualTemperature = myAHT20.getTemperature();
  controlTemperatureModule(actualTemperature,'A');
  delay(750);

}

void printTempSensorInfo(Adafruit_BMP280& BMP, AHT20& AHT20)
{
  // Funcion que imprime la informacion del sensor
  float temperature = AHT20.getTemperature();
  float humidity = AHT20.getHumidity();
  float pressure = BMP.readPressure();
  Serial.print("Temperatura: ");
  Serial.print(temperature, 2);
  Serial.print("°C\t");
  Serial.print("Humedad: ");
  Serial.print(humidity, 2);
  Serial.print("% RH\t");
  Serial.print("Presion: ");
  Serial.print(pressure, 2);
  Serial.println("Pa");
  Serial.println();
}

char validateDesiredTemperature(float actualTemperature, float desiredTemperature, float interval)
{
  // Funcion que valida la temperatura
  if (actualTemperature >= (desiredTemperature - (interval / 2)) && actualTemperature <= (desiredTemperature + (interval / 2)))
    return 'N'; // No hacer nada (Nothing/None)
  if (actualTemperature > desiredTemperature + (interval / 2))
    return 'C'; // Encender para frio (Cold)
  if (actualTemperature < desiredTemperature - (interval / 2))
    return 'H'; // Encender para calor (Hot)
  return 'I';
}

void controlTemperatureModule(float actualTemperature, char action)
{
  if (action == 'A') // Por defecto entra en modo automatico
    action = validateDesiredTemperature(actualTemperature, desiredTemperature, temperatureInterval);
  Serial.print(action);
  switch (action)
  {
  case 'N':
    digitalWrite(L293D_PIN1, LOW);
    digitalWrite(L293D_PIN2, LOW);
    digitalWrite(PLT_PIN, HIGH);
    break;

  case 'C':
    digitalWrite(L293D_PIN1, HIGH);
    digitalWrite(L293D_PIN2, LOW);
    digitalWrite(PLT_PIN, LOW);
    break;

  case 'H':
    digitalWrite(L293D_PIN1, LOW);
    digitalWrite(L293D_PIN2, HIGH);
    digitalWrite(PLT_PIN, LOW);
    break;

  default:
      Serial.print("Not valid Action");
    break;
  }
}