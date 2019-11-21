#include <cmsis_os.h>
#include <math.h>
#include "sensingTasks.h"

extern uint16_t temp;
extern uint16_t ConvertedValue;

void temperatureSensor(void const * argument) {
	float Vlsb = Vref / (exp2(N) - 1);
	float tempAux = 0; // En esta variable se hara el calculo en kelvins

	while(1) {
		//Calculo la temperatura
		tempAux = 1 / ( (log((ConvertedValue * Vlsb * R) / (Vcc * Ro - ConvertedValue * Vlsb * Ro)) / B) + (1 / To));
		temp = (uint16_t)((tempAux - 273) * 10);
		osDelay(500); //Leo temperatura cada medio segundo (500 ms)
	}
}
