#include <Arduino.h>
#include <Wire.h> //Libreria para I2C
#include <AHT20.h> //Libreria para sensor Temperatura
#include <Adafruit_BMP280.h> //Libreria para sensor Humedad

#define SDA_PIN 21
#define SCL_PIN 22


// put function declarations here:
int myFunction(int, int);
Adafruit_BMP280 bmp;
AHT20 myAHT20;

void setup() {
  Serial.begin(115200);
  Serial.println(F("AHT20+BMP280 test"));

  Wire.begin(SDA_PIN, SCL_PIN);

  while (myAHT20.begin() != true) {
    Serial.println(F("No se ha conectado AHT20 o falló al cargar el coeficiente de calibración")); //(F()) guarda el string en la Flash para mantener la memoria dinámica libre
    delay(5000);
  }
  Serial.println(F("AHT20 OK"));
  
  if (!bmp.begin()) {
    Serial.println(F("No se encuentra un sensor BMP280 compatible, revisa la conexión"));
    while (1);
  }

  /* Configuración default según el datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Modo de Operación. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtrado. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Tiempo de Standby. */
}

void loop() {
  float temperature = myAHT20.getTemperature();
  float humidity = myAHT20.getHumidity();
  float pressure = bmp.readPressure();
  //Impresion de Resultados
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
  delay(2000);
  }