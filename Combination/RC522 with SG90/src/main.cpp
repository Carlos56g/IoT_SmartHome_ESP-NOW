
#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>

#define RST_PIN 33
#define SS_PIN 5
#define accesKey "CarlosGarcia1234" //16 bytes maximo
#define statusLed 13

Servo servo0;                              // Objeto tipo Servo
int servo0Proporties[3] = {32, 500, 2500}; // PIN, Min, Max

MFRC522::StatusCode status; // variable to get card status

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.
byte buffer[18];                  // data transfer buffer (16+2 bytes data+CRC)
byte size = sizeof(buffer);
uint8_t pageAddr = 0x06; // In this example we will write/read 16 bytes (page 6,7,8 and 9).
// Ultraligth mem = 16 pages. 4 bytes per page.
// Pages 0 to 4 are for special functions.

bool writeNFCData(byte *data);
byte *ReadNFCData();
void servoMove(float time, float degrees, int initialPosition, Servo &servo, int servoProperties[3]);
void unlockAccess(Servo &servo, int servoProperties[3]);
bool validateAccess(byte *data);
bool accessWithNFC();
void writeKey(char *key);

void setup()
{
  Serial.begin(115200); // Initialize serial communications with the PC
  while (!Serial)
    ;                 // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
  SPI.begin();        // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card
  pinMode(statusLed,OUTPUT);
}

void loop()
{
  //writeKey(accesKey);
  accessWithNFC();
}

bool writeNFCData(byte *data)
{
  for (int i = 0; i < 4; i++)
  {
    // data is writen in blocks of 4 bytes (4 bytes per page)
    status = (MFRC522::StatusCode)mfrc522.MIFARE_Ultralight_Write(pageAddr + i, &data[i * 4], 4);
    if (status != MFRC522::STATUS_OK)
    {
      Serial.print(F("MIFARE_Read() failed: "));
      Serial.println(mfrc522.GetStatusCodeName(status));
      return false;
    }
  }
  Serial.println(F("MIFARE_Ultralight_Write() OK "));
  Serial.println();
  return true;
}

byte *ReadNFCData()
{
  Serial.println(F("Reading data ... "));
  // data in 4 block is readed at once.
  status = (MFRC522::StatusCode)mfrc522.MIFARE_Read(pageAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK)
  {
    Serial.print(F("MIFARE_Read() failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return 0;
  }

  Serial.print(F("Readed data: "));
  // Dump a byte array to Serial
  byte *data = new byte[size];

  for (byte i = 0; i < 16; i++)
  {
    data[i] = buffer[i];
    Serial.write(buffer[i]);
  }
  Serial.println();
  mfrc522.PICC_HaltA();
  return data;
}

bool validateAccess(byte *data)
{
  const char *accessName = accesKey; // Definimos la cadena que queremos comparar
  // Usamos strncmp para comparar los primeros 16 caracteres de data con "Carlos"
  if (strncmp((const char *)data, accessName, 16) == 0)
  {
    Serial.print("Acepted");
    return true;
  }
  Serial.print("Denied");
  digitalWrite(statusLed,HIGH);
  delay(300);
  digitalWrite(statusLed,LOW);
  delay(300);
  digitalWrite(statusLed,HIGH);
  delay(300);
  digitalWrite(statusLed,LOW);
  return false;
}

void unlockAccess(Servo &servo, int servoProperties[3])
{
  digitalWrite(statusLed,HIGH);
  servoMove(2000, 180, 0, servo, servoProperties);
  delay(2000);
  servoMove(2000, -180, 180, servo, servoProperties);
  digitalWrite(statusLed,LOW);
}

bool accessWithNFC()
{
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent())
    return false;
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
    return false;

  bool access = false;
  byte *data = ReadNFCData();

  if (data != nullptr)
  {
    Serial.print("Validating Data: ");
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
  servo.attach(servoProperties[0], servoProperties[1], servoProperties[2]); // Inicializa un servo con los parametos de ancho de pulso adecuados y pines adecuados
  float stepSize;
  if (time != 0)               // Si el tiempo es diferente de 0, calculamos el tamaño de cada paso
    stepSize = degrees / time; // calculo del paso
  else
  {
    servo.write(initialPosition + degrees); // Si el tiempo es 0, escribimos directamente la posicion final en el servo
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
  servo.detach(); // Desconecta el servo del pin analogico, para que no haya movimientos erraticos por interferencias o ruido
}

void writeKey(char *key)
{
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if (!mfrc522.PICC_IsNewCardPresent())
    return;
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial())
    return;
  Serial.println("The data on the device will be written");
  size_t longitud = strlen(key);
  byte *data = new byte[longitud];
  for (size_t i = 0; i < longitud; ++i)
  {
    data[i] = static_cast<byte>(key[i]);
  }
  writeNFCData(data);
  delete[] data;
}