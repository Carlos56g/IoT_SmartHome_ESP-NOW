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
  sendActualData = 'M'
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

extern lightDevices lightData;

#endif