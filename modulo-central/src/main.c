#include <cmsis_os.h>
#include <math.h>
#include <string.h>
#include "ConversorxInts.h"
#include "ICTimer.h"
#include "CommonIO.h"
#include "Entradas.h"
#include "Salidas.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#include "uart.h"
#include "communication.h"
#include "deviceInfo.h"

typedef struct settings {
	uint32_t periodTemp;
	uint32_t periodLgt;

} settings;

void askDataTask(void const * argument);
void managePacket(UART_HandleTypeDef* huart, packet newRxPacket);
void manageUserMessage(uint8_t length, char userMessage[20]);

void Error_Handler(void);

deviceInfo device;
settings periodSettings;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

uint8_t byteRxUart1 = 0;
uint8_t byteRxUart3 = 0;

int main(void)
{
	// Inicializacion de puertos
	HAL_Init();
	USART1_UART_Init();
	USART3_UART_Init();

	// Se crea la tarea periodica que solicitara las magnitudes
	osThreadDef(askDataTask, askDataTask, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);
	osThreadCreate (osThread(askDataTask), NULL); //Crea hilo

	// Configuraciones del equipo
	char latitudeTemp[10] = "-39.011792";
	char longitudeTemp[10] = "-58.580895";
	device.id = 0;
	strcpy(device.latitude, latitudeTemp);
	strcpy(device.longitude, longitudeTemp);

	// Periodicidades por defecto
	periodSettings.periodTemp = 60;
	periodSettings.periodLgt = 60;

	// Se comienza a escuchar por ambas UART
	HAL_UART_Receive_IT(&huart1, &byteRxUart1, 1);
	HAL_UART_Receive_IT(&huart3, &byteRxUart3, 1);

	osKernelStart();

	while (1) {}
}


void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);
}

void USART3_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart3);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static packet newRxPacketUart1 = {0};
	static uint8_t dataCounterUart1 = 0;
	static uint8_t dataInitCounterUart1 = 0;
	static uint8_t payloadIndexUart1 = 0;

	static char userMessage[20] = {0};
	static uint8_t dataCounterUart3 = 0;
	static uint8_t dataInitCounterUart3 = 0;

	if (huart->Instance == USART3) {
	// Se busca secuencia de "incio de mensaje"
	if(dataInitCounterUart3 < 2 && byteRxUart3 == 0xAA) {
		dataInitCounterUart3++;
		dataCounterUart3 = 0;
	} else if(dataInitCounterUart3 == 2 && (char)byteRxUart3 != ';') {
		userMessage[dataCounterUart3] = (char)byteRxUart3;
		dataCounterUart3++;
	} else if(dataInitCounterUart3 == 2 && (char)byteRxUart3 == ';') {
		// Se actua ante un mensaje de usuario
		userMessage[dataCounterUart3]  = (char)byteRxUart3;
		manageUserMessage(dataCounterUart3, userMessage);
		dataCounterUart3 = 0;
		dataInitCounterUart3 = 0;
	}
    /* Receive one byte in interrupt mode */
    HAL_UART_Receive_IT(&huart3, &byteRxUart3, 1);

  } else if (huart->Instance == USART1) {
		// Se busca secuencia de "incio de mensaje"
		if(dataInitCounterUart1 < 2 && byteRxUart1 == 0xAA) {
			dataInitCounterUart1++;
		} else if(dataInitCounterUart1 == 2) {
			// Se reciben los primeros 5 bytes del paquete
			switch(dataCounterUart1) {
				case 0:
					newRxPacketUart1.id = byteRxUart1;
					dataCounterUart1++;
					break;
				case 1:
					newRxPacketUart1.destination = byteRxUart1;
					dataCounterUart1++;
					break;
				case 2:
					newRxPacketUart1.source = byteRxUart1;
					dataCounterUart1++;
					break;
				case 3:
					newRxPacketUart1.transmitter = byteRxUart1;
					dataCounterUart1++;
					break;
				case 4:
					newRxPacketUart1.length = byteRxUart1;
					dataCounterUart1++;
					break;
				default:
					// A partir del sexto byte es payload, y el ultimo byte es checksum
					if(dataCounterUart1 > 4 && payloadIndexUart1 < newRxPacketUart1.length) {
						newRxPacketUart1.payload[payloadIndexUart1] = byteRxUart1;
						payloadIndexUart1++;
					} else if (dataCounterUart1 > 4 && payloadIndexUart1 == newRxPacketUart1.length) {
						newRxPacketUart1.checksum = byteRxUart1;
						// Se verifica si el checksum es correcto
						if(1) {
							// Se analiza si el paquete es para el dispositivo o no
							if(newRxPacketUart1.destination == 0xFF || newRxPacketUart1.destination == device.id) {
								// Se actua dependiendo el mensaje
								managePacket(&huart1, newRxPacketUart1);
							}
						}
						// Se reinician todas las variables auxiliares a la recepcion del mensaje
						dataCounterUart1 = 0;
						dataInitCounterUart1 = 0;
						payloadIndexUart1 = 0;
					}
			}
		}

	    /* Receive one byte in interrupt mode */
	    HAL_UART_Receive_IT(&huart1, &byteRxUart1, 1);
	  }
}

void manageUserMessage(uint8_t length, char userMessage[20]) {
	uint8_t keyLength = 0;
	uint8_t valueLength = 0;
	char key[20];
	char value[20];

	char askall[6] = "ASKALL";
	char ask[3] = "ASK";
	char config[3] = "CON";
	char lgt [3] = "LGT";
	char tmp [3] = "TMP";
	char pos [3] = "POS";
	char all [3] = "ALL";
	char askper[6] = "ASKPER";

	// Se separa la clave del valor
	uint8_t i = 0;
	// Se separa clave
	while(userMessage[i] != ':') {
		key[keyLength] = userMessage[i];
		keyLength++;
		i++;
	}
	// Se separa valor
	i++;
	while(userMessage[i] != ';') {
		value[valueLength] = userMessage[i];
		valueLength++;
		i++;
	}

	if(memcmp(key, askall, sizeof(askall)) == 0) {
		// Se pide la magnitud por broadcast
		if(memcmp(value, lgt, sizeof(lgt)) == 0) {
				uint8_t payload[8] = "giv:lig;"; // payload: giv:tmp;
				createAndSendPacket(&huart1, device.id, 0xFF, 8, payload);

		} else if(memcmp(value, tmp, sizeof(tmp)) == 0) {
			uint8_t payload[8] = "giv:tmp;"; // payload: giv:lig;
			createAndSendPacket(&huart1, device.id, 0xFF, 8, payload);
		} else if(memcmp(value, pos, sizeof(pos)) == 0) {
			uint8_t payload[8] = "giv:pos;"; // payload: giv:pos;
			createAndSendPacket(&huart1, device.id, 0xFF, 8, payload);
		}

	} else if(memcmp(key, askper, sizeof(askper)) == 0) {
		if(memcmp(value, lgt, sizeof(lgt)) == 0) {
			uint32_t minutesLight = periodSettings.periodLgt / 60;
			char payload[9] = "LGT:";
			payload[4] = '0' + ((uint8_t)(minutesLight / 1000) % 10);
			payload[5] = '0' + ((uint8_t)(minutesLight / 100) % 10);
			payload[6] = '0' + ((uint8_t)(minutesLight / 10) % 10);
			payload[7] = '0' + (uint8_t)(minutesLight % 10);
			payload[8] = ';';

			sendToUser(&huart3, 9, payload);
		} else if(memcmp(value, tmp, sizeof(tmp)) == 0) {
			uint32_t minutesTemp = periodSettings.periodTemp / 60;
			char payload[9] = "TMP:";
			payload[4] = '0' + ((uint8_t)(minutesTemp / 1000) % 10);
			payload[5] = '0' + ((uint8_t)(minutesTemp / 100) % 10);
			payload[6] = '0' + ((uint8_t)(minutesTemp / 10) % 10);
			payload[7] = '0' + (uint8_t)(minutesTemp % 10);
			payload[8] = ';';
			sendToUser(&huart3, 9, payload);
		} else if(memcmp(value, all, sizeof(all)) == 0) {
			uint32_t minutesLight = periodSettings.periodLgt / 60;
			uint32_t minutesTemp = periodSettings.periodTemp / 60;
			char payload[18] = "TMP:";
			payload[4] = '0' + ((uint8_t)(minutesTemp / 1000) % 10);
			payload[5] = '0' + ((uint8_t)(minutesTemp / 100) % 10);
			payload[6] = '0' + ((uint8_t)(minutesTemp / 10) % 10);
			payload[7] = '0' + (uint8_t)(minutesTemp % 10);
			payload[8] = ',';
			payload[9] = 'L';
			payload[10] = 'G';
			payload[11] = 'T';
			payload[12] = ':';
			payload[13] = '0' + ((uint8_t)(minutesLight / 1000) % 10);
			payload[14] = '0' + ((uint8_t)(minutesLight / 100) % 10);
			payload[15] = '0' + ((uint8_t)(minutesLight / 10) % 10);
			payload[16] = '0' + (uint8_t)(minutesLight % 10);
			payload[17] = ';';

			sendToUser(&huart3, 18, payload);
		}
	} else if(memcmp(key, ask, sizeof(ask)) == 0) {
		// Se pide la magnitud a un nodo especifico
		char dstId = key[3];
		uint8_t dst = dstId - '0';
		if(memcmp(value, lgt, sizeof(lgt)) == 0) {
			uint8_t payload[8] = "giv:lig;"; // payload: giv:lig;
			createAndSendPacket(&huart1, device.id, dst, 8, payload);

		} else if(memcmp(value, tmp, sizeof(tmp)) == 0) {
			uint8_t payload[8] = "giv:tmp;"; // payload: giv:tmp;
			createAndSendPacket(&huart1, device.id, dst, 8, payload);

		} else if(memcmp(value, pos, sizeof(pos)) == 0) {
			uint8_t payload[8] = "giv:pos;"; // payload: giv:pos;
			createAndSendPacket(&huart1, device.id, dst, 8, payload);
		}

	} else if(memcmp(key, config, sizeof(config)) == 0) {
		// Se setea la configuracion
		// Se obtiene el valor en segundos del periodo a setear
		uint32_t time = 0;
		uint8_t i = 0;
		// por cada caracter del numero, se obtiene y se multiplica por su posicion decimal (decena, centena, etc.)
		for(i = 0; i < valueLength; i++) {
			time += (value[i] - '0') * pow(10, i);
		}
		// Se obtiene de que magnitud hay que setear el periodo
		char mag[3];
		for(i = 0; i < 3; i++) {
			mag[i] = key[3 + i];
		}
		if(memcmp(mag, tmp, sizeof(tmp)) == 0) {
			periodSettings.periodTemp = time * 60;
		} else if(memcmp(mag, lgt, sizeof(lgt)) == 0) {
			periodSettings.periodLgt = time * 60;
		}
	} else {
		// Se le contesta al usuario que el mensaje no fue reconocido
		char payload[19] = "Comando incorrecto;";
		sendToUser(&huart3, 19, payload);
	}
}

void managePacket(UART_HandleTypeDef* huart, packet packet) {
	char sourceId = '0' + packet.source;
	char payloadKey[3] = {0};

	uint8_t i = 0;
	for(i = 0; i < 3; i++) {
		payloadKey[i] = (char)packet.payload[i];
	}
	uint8_t tmp[3] = "tmp";
	uint8_t lig[3] = "lig";
	uint8_t pos[3] = "pos";

	if(memcmp(payloadKey, tmp, sizeof(payloadKey)) == 0) {
		// Se el envia al usuario la temperatura que llego
		char payload[11];
		payload[0] = sourceId;
		payload[1] = ':';
		payload[2] = 'T';
		payload[3] = 'M';
		payload[4] = 'P';
		payload[5] = ':';
		uint8_t i = 0;
		for(i = 0; i < 4; i++) {
			payload[6 + i] = packet.payload[4 + i];
		}
		payload[10] = ';';
		sendToUser(&huart3, 11, payload);

	} else if(memcmp(payloadKey, lig, sizeof(payloadKey)) == 0) {
		// Se el envia al usuario la luz que llego
		char payload[11];
		payload[0] = sourceId;
		payload[1] = ':';
		payload[2] = 'L';
		payload[3] = 'G';
		payload[4] = 'T';
		payload[5] = ':';
		uint8_t i = 0;
		for(i = 0; i < 4; i++) {
			payload[6 + i] = packet.payload[4 + i];
		}
		payload[10] = ';';
		sendToUser(&huart3, 11, payload);
	} else if(memcmp(payloadKey, pos, sizeof(payloadKey)) == 0) {
		// Se el envia al usuario la posicion que llego
		char payload[26];
		payload[0] = sourceId;
		payload[1] = ':';
		payload[2] = 'P';
		payload[3] = 'O';
		payload[4] = 'S';
		payload[5] = ':';
		uint8_t i = 0;
		for(i = 0; i < 21; i++) {
			payload[6 + i] = packet.payload[4 + i];
		}
		payload[25] = ';';
		sendToUser(&huart3, 26, payload);
	}

}

void askDataTask(void const * argument) {
	static uint32_t timerTemp = 0;
	static uint32_t timerLight = 0;

	while(1) {
		if(timerTemp >= periodSettings.periodTemp) {
			//Solicito temperatura
			uint8_t payload[8] = "giv:tmp;";
			createAndSendPacket(&huart1, device.id, 0xFF, 8, payload);
			timerTemp = 0;
		}

		if(timerLight >= periodSettings.periodLgt) {
			//Solicito luz
			osDelay(100);
			uint8_t payload[8] = "giv:lig;";
			createAndSendPacket(&huart1, device.id, 0xFF, 8, payload);
			timerLight = 0;
		}

		timerTemp++;
		timerLight++;
		osDelay(1000);
	}
}

void Error_Handler(void) {
	packet pack;
	sendACK(&huart3, pack);
	while(1){}
}
