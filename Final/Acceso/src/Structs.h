#ifndef STRUCTS_H
#define STRUCTS_H
#define MAX_KEYS_NUM 10

enum ESPNowActions {
  deleteData = 'A',
  requestData = 'B',
  requestTime = 'C',
  sendActualData = 'M',
  restart = 'Z'
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
  //Existen status iguales a los modos (on,off,accsOpen,accsClose y DeleteKeys)
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

//Struct del modulo  de Acceso
struct accsDevice
{
  char key[16]="EMPTY";                        //Almacena la llave de acceso
  char mode=AccsNFC;                   //Contiene el Modo
  char keys[MAX_KEYS_NUM][16];         //Contiene todas las llaves de Acceso Almacenadas, 100 Keys de 16 Caracteres
  char status=on;                      //Contiene el Status 
  char date[20]="0000-00-00T00:00:00";                       //Almacena la fecha de Acceso
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

extern accsDevice accsData;
extern char prevStatus;
#endif