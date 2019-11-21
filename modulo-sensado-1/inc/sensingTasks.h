/*
 * sensingTasks.h
 *
 *  Created on: Nov 9, 2019
 *      Author: bozza
 */

#ifndef SENSINGTASKS_H_
#define SENSINGTASKS_H_

// Definicion de Constantes NTC para la medicion de temperatura
#define B (float) 4050	// [K] Constante ecuacion NTC
#define To (float) 273		// [K] Constante ecuacion NTC
#define Ro (float) 34.71	// [Kohms] Constante ecuacion NTC
#define R (float) 10	// [Kohms] Constante ecuacion R
#define Vcc (float) 5	// [Volts] Constante ecuacion Vcc
#define Vref (float) 3	// [Volts] Constante ecuacion Vref
#define N (float) 12	// [Volts] Constante ecuacion N

void temperatureSensor(void const * argument); // Tarea que sensa temperatura

#endif /* SENSINGTASKS_H_ */
