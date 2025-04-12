#ifndef STRUCTS_H
#define STRUCTS_H

//Struct del modulo de Temperatura
struct tempDevice
{
  short desiredTemperature = 25;            //Temperatura Deseada
  unsigned short temperatureMargin = 5;     //Intervalo de Temperatura
  short actualTemperature = 0;              //Temperatura Actual
  short actualHumidity = 0;                 //Humedad Actual
  short actualPressure = 0;                 //Presion Actual
  char mode = 'A';                          //A=Auto, N=Apagado, C=Frio, H=Caliente
  char onDate[20];                          //Fecha para Encendido Automatico
  char offDate[20];                         //Fecha para Apagado Automatico
};

extern tempDevice tempData; //Variable Global para acceder en cualquier archivo/funcion del proyecto

#endif