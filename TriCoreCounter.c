/*
 * TriCoreCounter.c
 *
 *  Created on: 22 Mar 2021
 *      Author: MojcaKolšek
 */

#include "TriCoreCounter.h"
#include "IfxCpu_regdef.h"
#include "IfxCpu_reg.h"


//value of sticky overflow flag
#define OVF(c)	(0x8000 & c)
//returns the value of the counter c without the sticky bit
#define VALUE(c) (0x7FFF & c)

#define READ_COUNTER(c)		__mfcr(c)
#define RESET_COUNTER(c)	__mtcr(c, 0x0000)

static Ifx_CPU_CCTRL cctrl; //holds values for counter configuration

void __stopPerfCounter(void)
{
	/*clear Count Enable bit */
	cctrl.B.CE = 0;
	__mtcr(CPU_CCTRL, cctrl.U);
}

void __startPerfCounter(void)
{
	cctrl.B.CE = 1;
	__mtcr(CPU_CCTRL, cctrl.U);
}



void __readPerfCounter(perf_counter* counter)
{
	counter->clk = READ_COUNTER(CPU_CCNT);
	counter->ist = READ_COUNTER(CPU_ICNT);
	counter->m1 = READ_COUNTER(CPU_M1CNT);
	counter->m2 = READ_COUNTER(CPU_M2CNT);
	counter->m3 = READ_COUNTER(CPU_M3CNT);
}

void __resetPerfCounter(void)
{
	RESET_COUNTER(CPU_CCNT);
	RESET_COUNTER(CPU_ICNT);
	RESET_COUNTER(CPU_M1CNT);
	RESET_COUNTER(CPU_M2CNT);
	RESET_COUNTER(CPU_M3CNT);
}


/**
 * \brief init performance counter with specified modes
 *
 * \param m 	0 (normal) or 1 (tasking)
 * \param m1 	3 bits, implementation specific (TCXXX user manual, for example TC26x_B-step UM)
 * \param m2 	3 bits, implementation specific
 * \param m3 	3 bits, implementation specific
 *
 * The M1,M2,M3 can be configured to measure instruction/data cache misses/hits, dispatch stall, branching...
 * The configuration depends on the CPU architecture (i.e. 1.6E does not have cache)
 */

void __initPerfCounter(uint8 M1_cfg, uint8 M2_cfg, uint8 M3_cfg)
{
	//read the unsigned access
	cctrl.U    = __mfcr(CPU_CCTRL);
	cctrl.B.CE = 0;
	//every M register can be configured to count hardware events such as program/data cache hits/misses
	//the available options are implementation specific (1.6E core has no data cache)
	cctrl.B.M1 = 0x7 & M1_cfg;
	cctrl.B.M2 = 0x7 & M2_cfg;
	cctrl.B.M3 = 0x7 & M3_cfg;
	//initialise the counter

	//reset the counter values
	__mtcr(CPU_CCNT, 0);
	__mtcr(CPU_ICNT, 0);
	__mtcr(CPU_M1CNT, 0);
	__mtcr(CPU_M2CNT, 0);
	__mtcr(CPU_M3CNT, 0);

	//set the counter mode (0 - normal, 1 - tasking (in tasking mode, the counter can be set to trigger with debugger events))
	cctrl.B.CM = 0x1 & 0; //only one bit
	//write params to register
	__mtcr(CPU_CCTRL, cctrl.U);
}

void __writePerfCounter(perf_counter counter)
{
	__stopPerfCounter();
	__mtcr(CPU_CCNT, counter.clk);
	__mtcr(CPU_ICNT, counter.ist);
	__mtcr(CPU_M1CNT, counter.m1);
	__mtcr(CPU_M2CNT, counter.m2);
	__mtcr(CPU_M3CNT, counter.m3);

}



