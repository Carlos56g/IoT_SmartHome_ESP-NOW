#ifndef FUNCTIONSESPNOW_H
#define FUNCTIONSESPNOW_H

#include <esp_now.h>
#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include "MAC_addresses.h" //Archivo de cabecera  con las direcciones MAC de los ESP32
#include "Structs.h"
#include "UtilitiesFunctions.h"

// Se ejecutará cada que se ENVIEN datos con ESP-NOW
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
	Serial.print("\r\nLast Packet Send Status:\t");
	Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

// Envia un Mensaje por medio de ESP-NOW al Host con la accion Especificada
void sendData(char action, tempDevice data)
{
	switch (action)
	{
	case sendActualData: // Data
		esp_now_send(MACS[0], (uint8_t *)&data, sizeof(tempDevice));
		break;
	case requestTime: // Peticion para obtner la fecha del Host
		esp_now_send(MACS[0], (uint8_t *)&action, sizeof(char));
		break;
	default:
		Serial.println("Accion Invalida!");
		break;
	}
}

// Devuelve true si la direccion MAC coincide, usado en SearchSender
bool areMacEquals(const uint8_t *src, const uint8_t *dst)
{
	for (auto i = 0; i < 6; i++)
	{
		if (src[i] != dst[i])
			return false;
	}
	return true;
}

// Devuelve el ID del emisor dada una direccion MAC
int searchSender(const uint8_t *mac)
{
	for (auto senderID = 0; senderID < MACS_COUNT; senderID++)
	{
		if (areMacEquals(mac, MACS[senderID]))
			return senderID;
	}
	return -1;
}

void onDataReceived(const uint8_t *mac, const uint8_t *data, int len)
{
	Serial.printf("Packet received from: %02X:%02X:%02X:%02X:%02X:%02X\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	Serial.printf("Bytes received: %d\n", len);

	int senderID = searchSender(mac); // Busca el ID del emisor
	// Crear un objeto de espNowData para almacenar los datos recibidos

	if (senderID == 0)
	{
		if (len == sizeof(char)) //Simpre que sea de tipo accion, despues de modificar los datos, se le mandara al Host de Vuelta
		{
			char action = data[0];
			switch (action)
			{
			case deleteData: // Eliminar toda la informacion del Struct y los archivos internos
				memset(&tempData, 0, sizeof(tempDevice)); //Borra todo el struct
				tempData.mode=deleteData;
				break;

			case requestData: // Request Data
				sendData(sendActualData, tempData); //Mandamos los Datos de Vuelta
				break;

			default:
				return; //Accion Invalida para el Modulo de Acceso!
			break;
			}
		}
		// SOLO PUEDE RECIBIR DEL HOST
		else if (len == sizeof(tempDevice))
		{
			memcpy(&tempData, data, sizeof(tempDevice));
		}
		else if (len == sizeof(char[20])) // Si tiene
		{
			char timeString[20];
			memcpy(timeString, data, len);
			setDateString(timeString);
		}
	}
	else
	{
		Serial.println("ID INVALIDO");
	}
}

// Registra un dispositivo a la red de ESP-NOW con el ID especificado
void registerPeek(uint8_t id, wifi_interface_t iface)
{
	esp_now_peer_info_t peerInfo = {};
	memcpy(peerInfo.peer_addr, MACS[id], 6);
	peerInfo.channel = 0;
	peerInfo.encrypt = false;
	peerInfo.ifidx = iface;

	if (esp_now_add_peer(&peerInfo) != ESP_OK)
	{
		Serial.println("Error registrando peer");
	}
	else
	{
		Serial.print("Peer registrado: ");
		Serial.println(id);
	}
}

// Registra todos los dispositivos
void static registerPeeks()
{
	for (int peekID = 0; peekID < MACS_COUNT; peekID++)
	{
		// RegisterPeek(peek_id, WIFI_IF_STA); //HOST
		registerPeek(peekID, WIFI_IF_AP); // MODULO
	}
}

void getActualDate(char *date, size_t size)
{
	struct tm timeInfo;
	short tries;
	while (!getLocalTime(&timeInfo))
	{
		Serial.println("Error al obtener la hora");
		sendData(requestTime, tempData); //Mandamos una peticion al Host para que nos de la fecha Actual
		delay(1000);
		if (tries > 10)
		{
			Serial.println("No fue posible obtener la fecha por parte del host");
			return;
		}
		tries++;
	}
	strftime(date, size, "%Y-%m-%dT%H:%M:%S", &timeInfo); // Lo castea a tipo ISO
}

// Inicializa ESP-NOW
void static InitEspNow()
{
	// WiFi.mode(WIFI_AP_STA); //Modo AP y Station HOST
	WiFi.mode(WIFI_AP); // Modo AP MODULO
	esp_wifi_set_channel(10, WIFI_SECOND_CHAN_NONE);
	if (esp_now_init() != ESP_OK)
	{
		Serial.println("Error initializing ESP-NOW");
	}
	else
	{
		esp_now_register_send_cb(onDataSent);
		esp_now_register_recv_cb(onDataReceived);
		registerPeeks();
	}
	char date[20];
	getActualDate(date, sizeof(date));
}

#endif
