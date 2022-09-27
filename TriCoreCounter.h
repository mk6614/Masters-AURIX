/*
 * PerformanceCounter.h
 *
 *  Created on: 2 Dec 2020
 *      Author: Mojca Kolšek
 */


#ifndef PERF_COUNTER_H_
#define PERF_COUNTER_H_

#include "Ifx_Types.h"

typedef struct {
	uint32 clk; //number of ticks
	uint32 ist; //number of instructions
	uint32 m1; //configurable
	uint32 m2; //configurable
	uint32 m3; //configurable
} perf_counter;

/**
 * \brief initialises the free-running performance counter on the board
 */
void __initPerfCounter(uint8 M1_cfg, uint8 M2_cfg, uint8 M3_cfg);

/**
 * \brief start the performance counter
 */
void __startPerfCounter(void);

/**
 * \brief stop the performance counter
 */
void __stopPerfCounter(void);

/**
 * \brief read the performance counter
 */
void __readPerfCounter(perf_counter* c);

/**
 * \brief resets the performance counter values
 */
void __resetPerfCounter(void);

/**
 * \brief sets the performance counter values to the desired values
 */

void __writePerfCounter(perf_counter counter);




#endif /* PERF_COUNTER_H_ */
