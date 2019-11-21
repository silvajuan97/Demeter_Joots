#include <math.h>

#include <cmsis_os.h>
#include <string.h>
#include "ConversorxInts.h"
#include "ICTimer.h"
#include "CommonIO.h"
#include "Entradas.h"
#include "Salidas.h"
#include "SysTick.h"
#include "queue.h"

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#include "uart.h"
#include "communication.h"
#include "deviceInfo.h"
#include "sensingTasks.h"

void managePacket(UART_HandleTypeDef* huart, packet newRxPacket);
void sendMagnitude(UART_HandleTypeDef* huart, uint8_t destination, char magnitude[3]);

void Error_Handler(void);

// Variables del sensado de temperatura
ADC_HandleTypeDef hADC1;
__IO uint16_t ConvertedValue;
uint8_t indexLight = 0;
//Variables que contienen las mediciones
uint16_t temp = 0;
uint16_t light = 100; // en Hz

deviceInfo device;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;

uint8_t byteRxUart1 = 0;
uint8_t byteRxUart3 = 0;

int main(void)
{
	Init_ICTimer();
	ADC1_NTC_Init(); // Inicializacion Entrada Analogica NTC (ADC1 - Channel 2 - PA2).
	if (HAL_ADC_Start_IT(&hADC1) != HAL_OK)
	{
		/* Start Conversation Error */
		Error_Handler();
	}

	HAL_Init();
	USART1_UART_Init();
	USART3_UART_Init();


	osThreadDef(temperatureSensor, temperatureSensor, osPriorityNormal, 1, configMINIMAL_STACK_SIZE);

	osThreadCreate (osThread(temperatureSensor), NULL); //Crea hilo


	char latitudeTemp[10] = "-18.011792";
	char longitudeTemp[10] = "-17.580895";
	device.id = 1;
	strcpy(device.latitude, latitudeTemp);
	strcpy(device.longitude, longitudeTemp);

	HAL_UART_Receive_IT(&huart1, &byteRxUart1, 1);
	HAL_UART_Receive_IT(&huart3, &byteRxUart3, 1);

	osKernelStart();

	packet pa;
	sendACK(&huart3, pa);

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
	static packet newRxPacketUart3 = {0};
	static uint8_t dataCounterUart3 = 0;
	static uint8_t dataInitCounterUart3 = 0;
	static uint8_t payloadIndexUart3 = 0;

	if (huart->Instance == USART3) {
	// Se busca secuencia de "incio de mensaje"
	if(dataInitCounterUart3 < 2 && byteRxUart3 == 0xAA) {
		dataInitCounterUart3++;
	} else if(dataInitCounterUart3 == 2) {
		// Se reciben los primeros 5 bytes del paquete
		switch(dataCounterUart3) {
			case 0:
				newRxPacketUart3.id = byteRxUart3;
				dataCounterUart3++;
				break;
			case 1:
				newRxPacketUart3.destination = byteRxUart3;
				dataCounterUart3++;
				break;
			case 2:
				newRxPacketUart3.source = byteRxUart3;
				dataCounterUart3++;
				break;
			case 3:
				newRxPacketUart3.transmitter = byteRxUart3;
				dataCounterUart3++;
				break;
			case 4:
				newRxPacketUart3.length = byteRxUart3;
				dataCounterUart3++;
				break;
			default:
				// A partir del sexto byte es payload, y el ultimo byte es checksum
				if(dataCounterUart3 > 4 && payloadIndexUart3 < newRxPacketUart3.length) {
					newRxPacketUart3.payload[payloadIndexUart3] = byteRxUart3;
					payloadIndexUart3++;
				} else if (dataCounterUart3 > 4 && payloadIndexUart3 == newRxPacketUart3.length) {
					newRxPacketUart3.checksum = byteRxUart3;
					// Se verifica si el checksum es correcto
					if(1) {
						// Se reenvia aguas abajo
						sendPacket(&huart1, newRxPacketUart3);
						// Se analiza si el paquete es para el dispositivo o no
						if(newRxPacketUart3.destination == 0xFF || newRxPacketUart3.destination == device.id) {
							// Se actua dependiendo el mensaje
							managePacket(&huart3, newRxPacketUart3);
						}

					}
					// Se reinician todas las variables auxiliares a la recepcion del mensaje
					dataCounterUart3 = 0;
					dataInitCounterUart3 = 0;
					payloadIndexUart3 = 0;
				}
		}
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
							// Se reenvia aguas arribas
							sendPacket(&huart3, newRxPacketUart1);

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

void managePacket(UART_HandleTypeDef* huart, packet packet) {
	char payloadKey[3] = {0};

	uint8_t i = 0;
	for(i = 0; i < 3; i++) {
		payloadKey[i] = (char)packet.payload[i];
	}

	if(strcmp(payloadKey, "giv") >= 1) {
		char magnitude[3] = {0};
		for(i = 0; i < 3; i++) {
			magnitude[i] = (char)packet.payload[i + 4];
		}
		sendMagnitude(huart, packet.source, magnitude);
	}

}

void sendMagnitude(UART_HandleTypeDef* huart, uint8_t destination, char magnitude[3]) {
	// Se crea el payload para contestar la solicitud
	uint8_t payload[26];
	uint8_t length = 0;
	uint8_t i = 0;
	for(i = 0; i < 3; i++) {
		payload[i] = magnitude[i];
	}
	payload[3] = ':';

	char lig [3];
	lig[0] = 'l';
	lig[1] = 'i';
	lig[2] = 'g';
	char tmp [3];
	tmp[0] = 't';
	tmp[1] = 'm';
	tmp[2] = 'p';
	char pos [3];
	pos[0] = 'p';
	pos[1] = 'o';
	pos[2] = 's';

	if (memcmp(magnitude, lig, sizeof(lig)) == 0) {
		// Se envia luz
		payload[4] = '0' + (light / 1000);
		payload[5] = '0' + ((light % 1000) / 100);
		payload[6] = '0' + ((light % 100) / 10); //Un decimal
		payload[7] = '0' + (light % 10); //Un decimal
		payload[8] = ';';
		length = 9;
	} else if(memcmp(magnitude, tmp, sizeof(tmp)) == 0) {
		// Se envia temperatura
		payload[4] = '0' + ((temp % 1000) / 100);
		payload[5] = '0' + ((temp % 100) / 10);
		payload[6] = '.'; //Un decimal
		payload[7] = '0' + (temp % 10); //Un decimal
		payload[8] = ';';
		length = 9;
	} else if (memcmp(magnitude, pos, sizeof(pos)) == 0) {
		uint32_t retardo = 0;
		while (retardo < 100000)
		{
			retardo++;
		}
		retardo = 0;
		// Se envia posicion
		for(i = 0; i < 10; i++) {
			payload[4 + i] = device.latitude[i];
		}
		payload[14] = ',';
		for(i = 0; i < 10; i++) {
			payload[15 + i] = device.longitude[i];
		}
		payload[25] = ';';
		length = 26;
	}
	createAndSendPacket(huart, device.id, destination, length, payload);
};

void ADCIRQHook (uint16_t ConversorAD) {
	ConvertedValue=ConversorAD;
}

void ICTimerHook (uint32_t CapturedValue)
{
	uint32_t numero_ciclos = 0;
	static uint32_t anterior = 0;
	// Utilice la Constante FTIM2 para calcular la frecuencia!!!
	#define TIMERFS (uint32_t) 4294967295 //Full scale timer 2^(32)-1
	//Calculo el numero de ciclos que transcurrieron desde la lectura anterior
	numero_ciclos = CapturedValue - anterior;
	if(numero_ciclos < 0) {
		numero_ciclos += TIMERFS;
	}
	//Coloco la captura actual como anterior para la siguiente vez que se ejecute esta funcion

	anterior = CapturedValue;
	light = FTIM2/ numero_ciclos;

}

void Error_Handler(void) {
	packet pack;
	sendACK(&huart3, pack);
	while(1){}
}
