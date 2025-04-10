#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>
#include <SPIFFS.h>
#include "FunctionsEspNow.h"
#include "Structs.h"

#define RST_PIN 33
#define SS_PIN 5
#define statusLed 13

// Struct del modulo  de Acceso
accsDevice accsData;
Servo servo0;                              // Objeto tipo Servo
int servo0Proporties[3] = {32, 500, 2400}; // PIN, Min, Max
char prevStatus;

MFRC522::StatusCode status; // variable to get card status

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
byte buffer[18];                  // data transfer buffer (16+2 bytes data+CRC)
byte size = sizeof(buffer);
uint8_t pageAddr = 0x06; // In this example we will write/read 16 bytes (page 6,7,8 and 9).
// Ultraligth mem = 16 pages. 4 bytes per page.
// Pages 0 to 4 are for special functions.

bool writeNFCData(unsigned char *data);
char *ReadNFCData();
void servoMove(float time, float degrees, int initialPosition, Servo &servo, int servoProperties[3]);
void unlockAccess(Servo &servo, int servoProperties[3]);
bool validateAccess(char *key);
bool accessWithNFC();
bool writeKey(char *key);
void saveKey(char *key);
void readKeys();
void controlAccess(accsDevice UserData);

void setup()
{
  Serial.begin(115200); // Initialize serial communications with the PC
  while (!Serial)
    ;                 // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  pinMode(statusLed, OUTPUT);

  // Inicializar SPIFFS
  if (!SPIFFS.begin(true))
  {
    Serial.println("Error al montar SPIFFS");
    return;
  }
  servo0.setPeriodHertz(50); // Frecuencia estándar de servo
  servo0.attach(servo0Proporties[0], servo0Proporties[1], servo0Proporties[2]);
  InitEspNow();
  readKeys(); // Leera todas las llaves de acceso
}

void loop()
{
  controlAccess(accsData);
}

void controlAccess(accsDevice UserData)
{
  bool newAction = false;
  if (UserData.createKey)
  {
    if (!writeKey(UserData.key))
    {
      Serial.printf("\nEsperando terjeta NFC para Grabar la Llave: %s", UserData.key);
      digitalWrite(statusLed, HIGH);
      delay(500);
      digitalWrite(statusLed, LOW);
      return;
    };
    saveKey(UserData.key);
    newAction = true;
  }
  else
  {
    if(accsData.status!='Y')
      accsData.status = prevStatus;
    switch (UserData.mode)
    {
    case 'K':
      accessWithNFC();
      if (accsData.status != 'D' && accsData.status != 'A') // AccessWithNFC changes the status
      {
        return;
      }
      newAction = true;
      break;
    case 'O':
      if (accsData.status != 'O')
      {
        servoMove(2000, 180, 0, servo0, servo0Proporties);
        strncpy(accsData.key, "Accion del Host", sizeof(accsData.key));
        accsData.status = 'O';
        newAction = true;
      }
      break;
    case 'C':
      if (accsData.status != 'C')
      {
        servoMove(2000, -180, 180, servo0, servo0Proporties);
        strncpy(accsData.key, "Accion del Host", sizeof(accsData.key));
        accsData.status = 'C';
        accsData.mode = 'K';
        newAction = true;
      }
      break;
    case 'N': // Apagado
      if (accsData.status != 'N')
      {
        accsData.status = 'N';
        strncpy(accsData.key, "Accion del Host", sizeof(accsData.key));
        newAction = true;
      }
      break;
    case 'E':
      accsData.status = 'K';
      strncpy(accsData.key, "Accion del Host", sizeof(accsData.key));
      accsData.mode = 'K';
      newAction = true;
      break;
    }
  }
  if (newAction)
  {
    printAccsDevice(UserData);
    sendData('A', accsData);
    prevStatus = accsData.status;
  }
}

bool writeNFCData(unsigned char *data)
{
  for (int i = 0; i < 4; i++)
  {
    // data is writen in blocks of 4 bytes (4 bytes per page)
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Ultralight_Write(pageAddr + i, &data[i * 4], 4);
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print("MIFARE_Read() failed: ");
      Serial.println(mfrc522.GetStatusCodeName(status));
      return false;
    }
  }
  Serial.println("\nMIFARE_Ultralight_Write() OK ");
  Serial.println();
  return true;
}

char *ReadNFCData()
{
  Serial.println("\nLeyendo Datos de la Tarjeta NFC... ");
  // data in 4 block is readed at once.
  status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(pageAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.println("\nMIFARE_Read() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 0;
  }

  Serial.print("Datos Leidos: ");

  // Dump a byte array to Serial
  char *data = new char[size];

  for (int i = 0; i < 15; i++)
  {
    data[i] = buffer[i];
    Serial.write(buffer[i]);
  }
  Serial.println();
  mfrc522.PICC_HaltA();
  return data;
}

bool validateAccess(char *proposedKey)
{
  bool access = false;
  char filteredProposedKey[16];
  memcpy(filteredProposedKey, proposedKey, 15);
  filteredProposedKey[15] = '\0';

  Serial.printf("\nIntento de Acceso con: %s", filteredProposedKey);

  for (int i = 0; i < MAX_KEYS_NUM; i++)
  {
    if (strcmp(filteredProposedKey, accsData.keys[i]) == 0)
    {
      access = true;
      break;
    }
  }

  memcpy(accsData.key, filteredProposedKey, 15); // Almacenamos el intento de Acceso en el struct

  if (access)
  {
    Serial.println("\nACEPTADO");
    accsData.status = 'A';
    getActualDate(accsData.date, sizeof(accsData.date));
    return true;
  }

  Serial.println("\nDENEGADO");
  accsData.status = 'D';
  getActualDate(accsData.date, sizeof(accsData.date));

  digitalWrite(statusLed, HIGH);
  delay(300);
  digitalWrite(statusLed, LOW);
  delay(300);
  digitalWrite(statusLed, HIGH);
  delay(300);
  digitalWrite(statusLed, LOW);
  return false;
}

void unlockAccess(Servo &servo, int servoProperties[3])
{
  digitalWrite(statusLed, HIGH);
  servoMove(2000, 180, 0, servo, servoProperties);
  delay(2000);
  servoMove(2000, -180, 180, servo, servoProperties);
  digitalWrite(statusLed, LOW);
}

bool accessWithNFC()
{
  accsData.status = 'J'; // Status Random que indica que se esta esperando acceso
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent())
    return false;
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
    return false;

  bool access = false;
  char *data = ReadNFCData();

  if (data != nullptr)
  {
    if (validateAccess(data))
    {
      unlockAccess(servo0, servo0Proporties);
      access = true;
    }
    delete[] data;
  }
  return access;
}

void servoMove(float time, float degrees, int initialPosition, Servo &servo, int servoProperties[3])
{
  // servo.attach(servoProperties[0], servoProperties[1], servoProperties[2]); // Inicializa un servo con los parametos de ancho de pulso adecuados y pines adecuados
  float stepSize;
  if (time != 0)               // Si el tiempo es diferente de 0, calculamos el tamaño de cada paso
    stepSize = degrees / time; // calculo del paso
  else
  {
    servo.write(initialPosition + degrees); // Si el tiempo es 0, escribimos directamente la posicion final en el servo
    // servo.detach();
    return;
  }
  float currentPosition = initialPosition;
  for (int i = 0; i < time; i++)
  {                                               // Ciclo que cambiara la posicion del servo de forma gradual
    currentPosition = currentPosition + stepSize; // El step Size sera + o -
    servo.write(int(currentPosition + 0.5));      // redondeamos la posicion obtenida, ya que servo.write espera valores enteros
    delay(1);                                     // delay de 1ms por cada paso
  }
  delay(500);
  // servo.detach(); // Desconecta el servo del pin analogico, para que no haya movimientos erraticos por interferencias o ruido
}

bool writeKey(char *key)
{
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    return false;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
  {
    return false;
  }

  unsigned char *uKey = (unsigned char *)(key);
  bool result = writeNFCData(uKey);

  if (result)
  {
    accsData.createKey = false;
    accsData.status = 'R';
  }

  return result;
}
