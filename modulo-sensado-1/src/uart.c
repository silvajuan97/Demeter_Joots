#include "uart.h"
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;

void USART1_UART_Init(void) {
	// USART1 utiliza los pines: PB6 como TX y PB7 como RX

	// Se habilitan los clocks para USART1 y el GPIOB
	__USART1_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

	// Se inicializa el pin 6 del gpio B
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_6;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART1;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	// Se inicializa el pin 7 del gpio B
	GPIO_InitStructure.Pin = GPIO_PIN_7;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Se inicia la UART
	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl= UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart1);

	/* Peripheral interrupt init*/
	  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

void USART3_UART_Init(void) {
	// USART3 utiliza los pines: PB10 como TX y PB11 como RX

	// Se habilitan los clocks para USART3 y el GPIOB
	__USART3_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

	// Se inicializa el pin 10 del gpio B
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_10;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStructure.Alternate = GPIO_AF7_USART3;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
	// Se inicializa el pin 11 del gpio B
	GPIO_InitStructure.Pin = GPIO_PIN_11;
	GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	// Se inicia la UART
	huart3.Instance = USART3;
	huart3.Init.BaudRate = 115200;
	huart3.Init.WordLength = UART_WORDLENGTH_8B;
	huart3.Init.StopBits = UART_STOPBITS_1;
	huart3.Init.Parity = UART_PARITY_NONE;
	huart3.Init.Mode = UART_MODE_TX_RX;
	huart3.Init.HwFlowCtl= UART_HWCONTROL_NONE;
	huart3.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&huart3);

	/* Peripheral interrupt init*/
	  HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
	  HAL_NVIC_EnableIRQ(USART3_IRQn);
}
