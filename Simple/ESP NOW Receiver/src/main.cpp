//RECEIVER
#include "Wifi.h"
#include "esp_now.h"
#include "MAC_addresses.h"

//Se ejecutará cada que se ENVIEN datos con ESP-NOW
void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status){
	Serial.print("\r\nLast Packet Send Status:\t");
	Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//Envia un Mensaje por medio de ESP-NOW con el ID especificado
void SendMessage(uint8_t peer_id)
{
	String payload = "MY STRING";

	esp_err_t result = esp_now_send(MACS[peer_id], (uint8_t*)payload.c_str(), payload.length());

	if(result == ESP_OK)
	{
		Serial.println("Sent with success");
	}
	else
	{
		Serial.println("Error sending the data");
	}
}

//Devuelve true si la direccion MAC coincide, usado en SearchSender
bool AreMacEquals(const uint8_t* src, const uint8_t* dst)
{
	for(auto i = 0; i < 6; i++)
	{
		if(src[i] != dst[i]) return false;
	}
	return true;
}

//Devuelve el ID del emisor dada una direccion MAC
int SearchSender(const uint8_t* mac)
{
	for(auto sender_id = 0; sender_id < MACS_COUNT; sender_id++)
	{
		if(AreMacEquals(mac, MACS[sender_id])) return sender_id;
	}
	return -1;
}

//Se ejecutará cada que se RECIBAN datos con ESP-NOW
void OnMessageReceived(const uint8_t* mac, const uint8_t* data, int len)
{
	Serial.printf("Packet received from: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	Serial.printf("Bytes received: %d\n", len);

	String payload;
	payload.reserve(len); //Reserva Memoria del espacio especificado
	for(auto i = 0; i < len; i++)
	{
		payload += (char)data[i]; //Concatena los datos recibidos
	}

	Serial.println(payload);
	auto sender_id = SearchSender(mac);
	Serial.printf("Sender id: %d\n", sender_id);
}

//Registra un dispositivo a la red de ESP-NOW con el ID especificado
void static RegisterPeek(uint8_t id)
{
	esp_now_peer_info_t peerInfo = {}; //Struct que almacena las propiedades de los dispositivos0
	memcpy(peerInfo.peer_addr, MACS[id], 6); //Copia la direccion MAC en el parametro correspondiente
	peerInfo.channel = 0; //Canal 0
	peerInfo.encrypt = false; //Sin encriptacion
	peerInfo.ifidx = WIFI_IF_STA; //Interfaz que usaran para enviar/recibir datos

	if(esp_now_add_peer(&peerInfo) != ESP_OK)
	{
		Serial.println("Failed to add peer");
	}
	else
	{
		Serial.print("Registered peer ");
		Serial.println(id);
	}
}

//Registra todos los dispositivos
void static RegisterPeeks()
{
	for(auto peek_id = 0; peek_id < MACS_COUNT; peek_id ++)
	{
		RegisterPeek(peek_id);
	}
}

//Inicializa ESP-NOW
void static InitEspNow()
{
	WiFi.mode(WIFI_STA); //Modo Station (Cliente)
	if(esp_now_init() != ESP_OK)
	{
		Serial.println("Error initializing ESP-NOW");
	}
	else
	{
		esp_now_register_send_cb(OnDataSent);
		esp_now_register_recv_cb(OnMessageReceived);
		RegisterPeeks();
	}
}




void setup() {
  Serial.begin(115200);
  InitEspNow();
}

void loop()
{
	
}