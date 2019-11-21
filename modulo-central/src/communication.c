#include "communication.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "deviceInfo.h"

extern deviceInfo device;

void sendACK(UART_HandleTypeDef* huart, packet p) {
	uint8_t payload[6];
	payload[0] = 'a';
	payload[1] = 'c';
	payload[2] = 'k';
	payload[3] = ':';
	payload[4] = p.id;
	payload[5] = ';';
	packet ackPacket = createPacket(device.id, p.transmitter, 6, payload);

	sendPacket(huart, ackPacket);

}

packet createPacket(uint8_t source,uint8_t  destination, uint8_t length, uint8_t payload[]) {
	static uint8_t universalPacketId = 0;
	packet newPacket;

	newPacket.id = universalPacketId;
	newPacket.destination = destination;
	newPacket.source = source;
	newPacket.transmitter = device.id;
	newPacket.length = length; // Todos los ack tendran un payload de 6 bytes

	uint8_t i = 0;
	for(i = 0; i < newPacket.length; i++) {
		newPacket.payload[i] = payload[i];
	}
	newPacket.checksum = calculateChecksum(newPacket);

	universalPacketId++;
	return newPacket;
}

void sendPacket(UART_HandleTypeDef* huart, packet packet) {
	uint8_t txBuffer[58];
	txBuffer[0] = 0xAA;
	txBuffer[1] = 0xAA;
	txBuffer[2] = packet.id;
	txBuffer[3] = packet.destination;
	txBuffer[4] = packet.source;
	txBuffer[5] = device.id;
	txBuffer[6] = packet.length;

	uint8_t i = 0;
	for(i = 0; i <= packet.length; i++) {
		txBuffer[7 + i] = packet.payload[i];
	}

	txBuffer[7 + packet.length] = packet.checksum;

	HAL_UART_Transmit(huart, txBuffer, packet.length + 8, 500);
}

void sendToUser(UART_HandleTypeDef* huart, uint8_t length, char payload[]) {
	HAL_UART_Transmit(huart, payload, length, 500);
}

void createAndSendPacket(UART_HandleTypeDef* huart, uint8_t source, uint8_t  destination, uint8_t length, uint8_t payload[]) {
	packet newPacket = createPacket(source, destination, length, payload);
	sendPacket(huart, newPacket);
}

uint8_t checksumIsValid(packet packet) {
	// Verificación: Sumar todos los bytes (incluido el checksum). Si el resultado es igual a 0xFF el checksum es correcto.

	uint32_t sum = 0;
	sum = sum + packet.id + packet.destination + packet.source + packet.transmitter + packet.length + packet.checksum;
	uint8_t i = 0;
	for(i = 0; i < packet.length; i++){
		sum += (uint8_t)packet.payload[i];
	}

	// Si la suma es 0xFF, entonces el checksum es correcto y se devuelve 1 (true)
	if((uint8_t)sum == 0xFF) {
		return 1;
	}
	return 0; // Si no es valido se devuelve 0 (false)
}

uint8_t calculateChecksum(packet packet) {
	// Calculo: Sumar todos los bytes dejando solo los 8 bits menos significativos del resultado y restarle esa cantidad a 0xFF.

	uint32_t sum = 0;
	sum = sum + packet.id + packet.destination + packet.source + packet.transmitter + packet.length;
	int i = 0;
	for(i = 0; i < packet.length; i++){
		sum += (uint8_t)packet.payload[i];
	}
	return 0xFF - (uint8_t)sum;
}
