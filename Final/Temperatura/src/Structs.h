#ifndef STRUCTS_H
#define STRUCTS_H

enum ESPNowActions {
  deleteData = 'A',
  requestData = 'B',
  requestTime = 'C',
  sendActualData = 'M',
  restart = 'Z'
}; 

enum ModulesID {
  tempModule = 1,
  lightModule = 2,
  accsModule = 3
}; 

enum accsModes {
  accsNFC = 'D',
  accsOpen = 'E',
  accsClose = 'F',
  off = 'G',
  on = 'H',
  createKey = 'I'
}; 

enum accsStatus {
  accept = 'J',
  deny = 'K',
  waitingNewKey = 'L',
  userRegistered = 'N'
  //Existen status iguales a los modos (on,off,accsOpen,accsClose y el DeleteKeys)
}; 

enum tempModes{
  autoMode = 'O',
  hot = 'P',
  cold = 'Q',
  nothing = 'S',
  air = 'R',
};

enum ledStatus {
  CLEAR = '1',
  ERROR = '2',
  WAITING = '3',
  PELTIERON = '4',
  PELTIEROFF = '5',
  DATARECEIVED = '6',
  DATASENDED = '7',
  OFF = '8' 
}; 

struct tempDeviceProgram{
  char mode = autoMode;                     //En que modo se activará?
  char onDate[20];                          //Fecha para Encendido Automatico
  char offDate[20];                         //Fecha para Apagado Automatico
  float desiredTemperature = 25;            //A que temperatura?
};

//Struct del modulo de Temperatura
struct tempDevice
{
  float desiredTemperature = 33;            //Temperatura Deseada
  float actualTemperature = 0;              //Temperatura Actual
  float actualHumidity = 0;                 //Humedad Actual
  char mode = autoMode;                     //Modo de Operacion Auto, caliente, frio, solo aire
  char status = on;                        //Encendido, Apagado, Esperando
  tempDeviceProgram tempDataProg;
};

struct statusLED{
  short redPin = 13;
  short bluePin = 14;
  short greenPin = 12;
  short redPwm = 1;
  short bluePwm = 2;
  short greenPwm = 3;
  short red = 0;
  short blue = 0;
  short green = 0;
};

extern statusLED led;

extern tempDevice tempData;                 //Variable Global para acceder en cualquier archivo/funcion del proyecto

#endif