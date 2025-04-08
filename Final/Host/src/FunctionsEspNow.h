#include <esp_now.h>
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "MAC_addresses.h" //Archivo de cabecera  con las direcciones MAC de los ESP32
#include "Structs.h"


//Se ejecutará cada que se ENVIEN datos con ESP-NOW
void OnDataSent(const uint8_t* mac_addr, esp_now_send_status_t status){
	Serial.print("\r\nLast Packet Send Status:\t");
	Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

//Envia un Mensaje por medio de ESP-NOW con el ID especificado
void SendMessage(uint8_t peer_id, espNowData data)
{
	switch (peer_id)
	{
	case 1: //Temp
		esp_now_send(MACS[peer_id], (uint8_t*) &data.temperatureModule, sizeof(tempDevice)); 
		break;
	case 2: //Luz
		esp_now_send(MACS[peer_id], (uint8_t*) &data.lightModule, sizeof(lightDevice)*4); 
		break;
	case 3: //Accs
		esp_now_send(MACS[peer_id], (uint8_t*) &data.accessModule, sizeof(accsDevice));
		Serial.println(sizeof(data.accessModule));
		break;
	default:
		Serial.println("ID INVALIDO");
		break;
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

void OnMessageReceived(const uint8_t* mac, const uint8_t* data, int len) {
    Serial.printf("Packet received from: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.printf("Bytes received: %d\n", len);

	int senderID = SearchSender(mac); // Busca el ID del emisor
	// Crear un objeto de espNowData para almacenar los datos recibidos

	switch (senderID)
	{
	case 1: //Temp
		memcpy(&receivedData.temperatureModule, data, sizeof(tempDevice));
		break;
	case 2: //Luz
		memcpy(&receivedData.lightModule, data, sizeof(lightDevice)); 
		break;
	case 3: //Accs
		memcpy(&receivedData.accessModule, data, sizeof(accsDevice)); 
		break;
	default:
		Serial.println("ID INVALIDO");
		break;
	}

}



//Registra un dispositivo a la red de ESP-NOW con el ID especificado
void RegisterPeek(uint8_t id, wifi_interface_t iface)
{
	esp_now_peer_info_t peerInfo = {};
	memcpy(peerInfo.peer_addr, MACS[id], 6);
	peerInfo.channel = 0;
	peerInfo.encrypt = false;
	peerInfo.ifidx = iface;

	if (esp_now_add_peer(&peerInfo) != ESP_OK) {
		Serial.println("Error registrando peer");
	} else {
		Serial.print("Peer registrado: ");
		Serial.println(id);
	}
}

//Registra todos los dispositivos
void static RegisterPeeks()
{
	for(auto peek_id = 0; peek_id < MACS_COUNT; peek_id ++)
	{
		RegisterPeek(peek_id, WIFI_IF_STA); //HOST
		//RegisterPeek(peek_id, WIFI_IF_AP); //MODULO
	}
}

//Inicializa ESP-NOW
void static InitEspNow()
{
	WiFi.mode(WIFI_AP_STA); //Modo AP y Station HOST
	//WiFi.mode(WIFI_AP); //Modo AP MODULO
	//esp_wifi_set_channel(10, WIFI_SECOND_CHAN_NONE);
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
