/*
 * communication.h
 *
 *  Created on: Nov 9, 2019
 *      Author: bozza
 */

#ifndef COMMUNICATION_H_
#define COMMUNICATION_H_

#include <cmsis_os.h>
#include "stm32f4xx.h"

typedef struct packet {
	uint8_t id;
	uint8_t destination;
	uint8_t source;
	uint8_t transmitter;
	uint8_t length;
	char payload[50];
	uint8_t checksum;

} packet;

packet createPacket(uint8_t source,uint8_t  destination, uint8_t length, uint8_t payload[]);
void sendPacket(UART_HandleTypeDef* huart, packet packet);
void createAndSendPacket(UART_HandleTypeDef* huart, uint8_t source, uint8_t  destination, uint8_t length, uint8_t payload[]);
void sendACK(UART_HandleTypeDef* huart, packet p);
uint8_t checksumIsValid(packet packet);
uint8_t calculateChecksum(packet packet);
void sendToUser(UART_HandleTypeDef* huart, uint8_t length, char payload[]);

#endif /* COMMUNICATION_H_ */
