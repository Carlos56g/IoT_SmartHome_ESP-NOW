#ifndef STRUCTS_H
#define STRUCTS_H
#define MAX_KEYS_NUM 14

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


//Struct del modulo de Temperatura
struct tempDevice
{
  short desiredTemperature;   //Temperatura Deseada
  unsigned short temperatureInterval;  //Intervalo de Temperatura
  short actualTemperature;    //Temperatura Actual
  short actualHumidity;       //Humedad Actual
  short actualPressure;       //Presion Actual
  char Mode;                  //A=Auto, N=Apagado, C=Frio, H=Caliente
};


//Struct del modulo  de Acceso
struct accsDevice
{
  bool createKey;           // True=Pone al modulo en modo de creacion de llave de acceso
  char Key[16];             // Almacena la llave de acceso
  char Mode;                // O=Abrir Puerta,  C=Cerrar Puerta, N=Deshabilitado, K=Activar con Llave de acceso
  char Keys[MAX_KEYS_NUM][16];         //Contiene todas las llaves de Acceso Almacenadas, 100 Keys de 16 Caracteres
  char Status;              //Contiene el Status A=Aceptado, D=Denegado, R=Usuario Registrado
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

#endif