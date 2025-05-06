#ifndef STRUCTS_H
#define STRUCTS_H
#define MAX_KEYS_NUM 10
#define numLightDevices 4

#include <list>

enum ESPNowActions
{
  deleteData = 'A',
  requestData = 'B',
  requestTime = 'C',
  sendActualData = 'M',
  restart = 'Z',
  rUHere = 'a'
};

enum ModulesID
{
  tempModule = 1,
  lightModule = 2,
  accsModule = 3
};

enum accsModes
{
  AccsNFC = 'D',
  accsOpen = 'E',
  accsClose = 'F',
  off = 'G',
  on = 'H',
  createKey = 'I'
};

enum accsStatus
{
  accept = 'J',
  deny = 'K',
  waitingNewKey = 'L',
  userRegistered = 'N'
  // Existen status iguales a los modos (on,off,accsOpen,accsClose y el DeleteKeys)
};

enum tempModes
{
  autoMode = 'O',
  hot = 'P',
  cold = 'Q',
  nothing = 'S',
  air = 'R',
};

enum lightModes
{
  presence = 'T',
  presenceAndAuto = 'V',
};

enum ledStatus
{
  CLEAR = '1',
  ERROR = '2',
  WAITING = '3',
  SENDLIGHT = '4',
  SENDACCS = '5',
  SENDTEMP = '6',
  RECEIVEDLIGHT = '7',
  RECEIVEDACCS = '8',
  RECEIVEDTEMP = '9',
};

// Struct de los dispositivos de LUZ
struct lightDevice
{
  unsigned short pin;         // Pin del led
  char mode = autoMode;       // A=Auto, P=Presence, O=On, L=Off, B=Auto&Presence, T=Time
  unsigned short presencePin; // Pin del sensor de presencia
  unsigned long int timeOn;   // lleva el tiempo que se encendio el dispositivo
  bool state;                 // True=Encendido, False=Apagado
  char onDate[20];            // Fecha programada de encendido
  char offDate[20];           // Fecha programada de apagado
  char pMode = autoMode;
  short desiredBrightness = 110;
  short defaultTimeOn = 5000;
};

struct lightDevices
{
  lightDevice lightDev[numLightDevices];
  bool on=NULL;
};

struct tempDeviceProgram
{
  char mode = autoMode;          // En que modo se activará?
  char onDate[20];               // Fecha para Encendido Automatico
  char offDate[20];              // Fecha para Apagado Automatico
  float desiredTemperature = 25; // A que temperatura?
};

// Struct del modulo de Temperatura
struct tempDevice
{
  float desiredTemperature = 33; // Temperatura Deseada
  float actualTemperature = 0;   // Temperatura Actual
  float actualHumidity = 0;      // Humedad Actual
  char mode=NULL;          // Modo de Operacion Auto, caliente, frio, solo aire
  char status=NULL;              // Encendido, Apagado, Esperando
  tempDeviceProgram tempDataProg;
};

// Struct del modulo  de Acceso
struct accsDevice
{
  char key[16];                // Almacena la llave de acceso
  char mode=NULL;         // Contiene el Modo
  char keys[MAX_KEYS_NUM][16]; // Contiene todas las llaves de Acceso Almacenadas, 100 Keys de 16 Caracteres
  char status=NULL;            // Contiene el Status
  char date[20];              // Almacena la fecha de Acceso
};

// Struct para un Acceso
struct accsEvent
{
  char key[16];  // Almacena la llave de acceso
  char status;   // Contiene el Status
  char date[20]; // Almacena la fecha de Acceso
};

struct espNowData
{
  lightDevices lightModule;     // Datos de los dispositivos de LUZ
  tempDevice temperatureModule; // Datos del modulo de temperatura
  accsDevice accessModule;      // Datos del modulo de acceso
};

struct statusLED
{
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
extern espNowData receivedData;
extern std::list<accsEvent> accsHistory; // Lista global dinamica

extern bool resultAccs;
extern  bool resultLight;
extern  bool resultTemp;
extern std::list<accsEvent> accsHistory; // Lista global dinamica

extern bool newAccsAction = true;
#endif