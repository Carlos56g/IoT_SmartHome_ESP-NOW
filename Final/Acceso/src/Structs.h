#ifndef STRUCTS_H
#define STRUCTS_H
#define MAX_KEYS_NUM 10

enum ESPNowActions {
  deleteData = 'A',
  requestData = 'B',
  requestTime = 'C',
  sendActualData = 'M'
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

//Struct del modulo  de Acceso
struct accsDevice
{
  char key[16];                        //Almacena la llave de acceso
  char mode=AccsNFC;                   //Contiene el Modo
  char keys[MAX_KEYS_NUM][16];         //Contiene todas las llaves de Acceso Almacenadas, 100 Keys de 16 Caracteres
  char status=on;                      //Contiene el Status 
  char date[20];                       //Almacena la fecha de Acceso
};


extern accsDevice accsData;
extern char prevStatus;
#endif