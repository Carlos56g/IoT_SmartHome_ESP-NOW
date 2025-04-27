#ifndef STRUCTS_H
#define STRUCTS_H

#define numLightDevices 4
#define SDA_PIN 21
#define SCL_PIN 22
#define LED1_PIN 32
#define LED2_PIN 33
#define LED3_PIN 25
#define LED4_PIN 26
#define S1_PIN 16
#define S2_PIN 17

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

enum lightModes{
  presence = 'T',
  presenceAndAuto = 'V',
};

enum ledStatus {
  CLEAR = '1',
  ERROR = '2',
  WAITING = '3',
  DENIED = '4',
  ACEPTED = '5',
  DATARECEIVED = '6',
  DATASENDED = '7',
  OFF = '8'
}; 


//Struct de los dispositivos de LUZ
struct lightDevice
{
  unsigned short pin;             //Pin del led
  char mode = autoMode;           // A=Auto, P=Presence, O=On, L=Off, B=Auto&Presence, T=Time
  short presencePin;     //Pin del sensor de presencia
  unsigned long int timeOn;          //lleva el tiempo que se encendio el dispositivo
  bool state;                     //True=Encendido, False=Apagado
  char onDate[20];              //Fecha programada de encendido
  char offDate[20];             //Fecha programada de apagado
  char pMode=autoMode;
  short desiredBrightness=110;
  short defaultTimeOn=5000;
};

struct lightDevices{
  lightDevice lightDev[numLightDevices];
  bool on = true;
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

extern lightDevices lightData;

#endif