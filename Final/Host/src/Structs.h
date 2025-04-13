#ifndef STRUCTS_H
#define STRUCTS_H
#define MAX_KEYS_NUM 10

#include <list>

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
  AccsNFC = 'D',
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


//Struct de los dispositivos de LUZ
struct lightDevice
{
  unsigned short pin;            //Pin del led
  char mode;          // A=Auto, P=Presence, O=On, L=Off, B=Auto&Presence, T=Time
  bool presence;      //True=Puede Configurarse como Luz con Presencia
  unsigned short presencePin;    //Pin del sensor de presencia
  unsigned short timeOn;         //lleva el tiempo que se encendio el dispositivo
  bool state;         //True=Encendido, False=Apagado
  unsigned short defaultTime;    //Tiempo de encedido cuando detecta presencia
  char timeToOn[20];     //Fecha programada de encendido
  char timeToOff[20];    //Fecha programada de apagado
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

//Struct del modulo  de Acceso
struct accsDevice
{
  char key[16];                        //Almacena la llave de acceso
  char mode=AccsNFC;                   //Contiene el Modo
  char keys[MAX_KEYS_NUM][16];         //Contiene todas las llaves de Acceso Almacenadas, 100 Keys de 16 Caracteres
  char status=on;                      //Contiene el Status 
  char date[20];                       //Almacena la fecha de Acceso
};

//Struct para un Acceso
struct accsEvent
{
  char key[16];             // Almacena la llave de acceso
  char status;              //Contiene el Status
  char date[20];            //Almacena la fecha de Acceso
};

struct espNowData {
  unsigned short idR;                      //ID del dispositivo que recibe (0=Host, 1=Temp, 2=Luz, 3=Accs)
  unsigned short idE;                      //ID del dispositivo que envia (0=Host, 1=Temp, 2=Luz, 3=Accs)
  char instruction;              //Instruccion que se le pedira 
  lightDevice lightModule[4];  //Datos de los dispositivos de LUZ
  tempDevice temperatureModule;        //Datos del modulo de temperatura
  accsDevice accessModule;        //Datos del modulo de acceso
};


extern espNowData receivedData;
extern std::list<accsEvent> accsHistory; //Lista global dinamica

#endif