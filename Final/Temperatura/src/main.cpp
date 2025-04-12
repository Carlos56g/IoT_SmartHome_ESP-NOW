#include <Arduino.h>

#define fanC 17
#define fanH 16
#define peltier 32

// Funciones
void printTempSensorInfo(Adafruit_BMP280& BMP, AHT20& AHT20);
char validateDesiredTemperature(float actualTemperature, float desiredTemperature);
void controlTemperatureModule(float actualTemperature, char action);

void setup()
{
  Serial.begin(115200);

  pinMode(fanC, OUTPUT); // Pin del motor 1 (Frio)
  pinMode(fanH, OUTPUT); // Pin del motor 2 (Caliente)
  pinMode(peltier, OUTPUT);    // Pin del relevador de la Peltier
  digitalWrite(fanC, LOW);
  digitalWrite(fanH, LOW);
  digitalWrite(peltier, HIGH);
}

void loop()
{
  printTempSensorInfo(myBMP, myAHT20);
  float actualTemperature = myAHT20.getTemperature();
  controlTemperatureModule(actualTemperature,'A');
  yield();

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
    digitalWrite(fanC, LOW);
    digitalWrite(fanH, LOW);
    digitalWrite(peltier, HIGH);
    break;

  case 'C':
    digitalWrite(fanC, HIGH);
    digitalWrite(fanH, LOW);
    digitalWrite(peltier, LOW);
    break;

  case 'H':
    digitalWrite(fanC, LOW);
    digitalWrite(fanH, HIGH);
    digitalWrite(peltier, LOW);
    break;

  default:
      Serial.print("Not valid Action");
    break;
  }
}