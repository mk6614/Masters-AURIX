/*
 * NonOSTaskCounter.c
 *
 *  Created on: 22 March 2021
 *      Author: Mojca Kolšek
 */


#ifndef TASK_COUNTER_C_
#define TASK_COUNTER_C_

#include "BasicCounter.h"
#include "Ifx_Types.h"
#include "IfxScuCcu.h"
#include "IfxCpu.h"
#include "AurixID.h"

#include "math.h"


void initTaskCounter(task_counter* counter)
{
	counter->exec_counter = 0;

	counter->clk_mean = 0;
	counter->clk_M2 = 0;
    //counter->clk_max = 0;

    counter->ist_mean = 0;
	counter->ist_M2 = 0;
    //counter->ist_max = 0;

#ifdef C123 //configurable counters
	counter->c1_mean = 0;
	counter->c1_M2 = 0;

    counter->c2_mean = 0;
	counter->c2_M2 = 0;

    counter->c3_mean = 0;
	counter->c3_M2 = 0;
#endif
}

void initTaskCounters(uint8 N, task_counter counters[N])
{
    for(uint8 i=0; i<N; i++) initTaskCounter(&counters[i]);
}


void Welfords(float32* mean, float32* M, float32 sample, uint16 iteration)
{
    float32 d1 = sample - (*mean);
    (*mean) += d1 / (float32)iteration;
    float32 d2 = sample - (*mean);
    (*M) += d1*d2;
}


#define SET_MAX(max, tmp)   if (tmp > max) max = tmp

void finishTask(task_counter* counter)
{
    __stopPerfCounter();
    perf_counter tmp_counter;
    __readPerfCounter(&tmp_counter);
	counter->exec_counter++;
    Welfords(&counter->clk_mean, &counter->clk_M2, (float32)tmp_counter.clk, counter->exec_counter);
    //SET_MAX(counter->clk_max, tmp_counter.clk);
    Welfords(&counter->ist_mean, &counter->ist_M2, (float32)tmp_counter.ist, counter->exec_counter);
    //SET_MAX(counter->ist_max, tmp_counter.ist);

    #ifdef C123 //configurable counter
    Welfords(&counter->c1_mean, &counter->c1_M2, (float32)tmp_counter.m1, counter->exec_counter);
    Welfords(&counter->c2_mean, &counter->c2_M2, (float32)tmp_counter.m2, counter->exec_counter);
    Welfords(&counter->c3_mean, &counter->c3_M2, (float32)tmp_counter.m3, counter->exec_counter);
#endif
}

void printCounterHeader(void (*printf)(const char *fmt, ...))
{

#if defined(PRINT_M2) & !( defined(PRINT_S) | defined(PRINT_VAR) )
    char* var = "M2";

#elif defined(PRINT_S)  & !( defined(PRINT_M2) | defined(PRINT_VAR) )
    char* var = "S";

#elif defined(PRINT_VAR)  & !( defined(PRINT_S) | defined(PRINT_M2) )
    char* var = "VAR";
#else
#error "One of PRINT_M2 PRINTF_S PRINT_VAR has to be defined"
#endif

#ifdef PRINT_MICROSECONDS
    char* time_units = "[mu_s]";
#else
    char* time_units = "[ticks]";
#endif
    printf("EXEC%s", SEPARATOR);

    printf("MEAN_CLK%s%s", time_units, SEPARATOR);
    printf("%s_CLK%s%s", var, time_units, SEPARATOR);

    printf("MEAN_IST%s", SEPARATOR);
    printf("%s_IST%s", var, SEPARATOR);

#ifdef C123 //configurable counters
    printf("MCNT1%s%s_MCNT1%s", SEPARATOR, var, SEPARATOR);
    printf("MCNT2%s%s_MCNT2%s", SEPARATOR, var, SEPARATOR);
    printf("MCNT3%s%s_MCNT3",  SEPARATOR, var);
/*
    Ifx_CPU_CPU_ID reg_impl;
    reg_impl.U = __mfcr(CPU_CPU_ID); //relative address (32bits)

    //two cores, 4 modes, 3 counters
    char* str[4][3] = TC16P_MCOUNTERS;
    char** tmp = str[mode];
    Ifx_CPU_CCTRL cctrl;
    cctrl.U    = __mfcr(CPU_CCTRL);

    printf("%s%s", tmp[0], SEPARATOR);
    printf("%s_%s%s", tmp[0], var, SEPARATOR);

    printf("%s%s", tmp[1], SEPARATOR);
    printf("%s_%s%s", tmp[1], var, SEPARATOR);

    printf("%s%s", tmp[2], SEPARATOR);
    printf("%s_%s", tmp[2], var, SEPARATOR);
    */
#endif
    printf("%s", NEW_LINE);
}


void printCounter(task_counter* counter, void (*printf)(const char *fmt, ...))
{
#ifdef PRINT_MICROSECONDS
    float32 cpu_f_Mhz = IfxScuCcu_getCpuFrequency(IfxCpu_getCoreIndex())/1000000.0;
    float32 time = counter->clk_mean/cpu_f_Mhz;
    float32 m_time = counter->clk_M2/cpu_f_Mhz;
#else
    float32 time = counter->clk_mean;
    float32 m_time = counter->clk_M2;
#endif

#if defined(PRINT_M2) & !( defined(PRINT_S) | defined(PRINT_VAR) )
    float32 m_clk = m_time;
    float32 m_ist = counter->ist_M2;
#ifdef C123 //configurable counters
    float32 m_c1 = counter->c1_M2;
    float32 m_c2 = counter->c2_M2;
    float32 m_c3 = counter->c3_M2;
#endif
#elif defined(PRINT_S)  & !( defined(PRINT_M2) | defined(PRINT_VAR) )
    float32 m_clk = sqrt(m_time/(float32)(counter->exec_counter-1));
    float32 m_ist = sqrt(counter->ist_M2/(float32)(counter->exec_counter-1));
#ifdef C123 //configurable counters
    float32 m_c1 = sqrt(counter->c1_M2/(float32)(counter->exec_counter-1));
    float32 m_c2 = sqrt(counter->c2_M2/(float32)(counter->exec_counter-1));
    float32 m_c3 = sqrt(counter->c3_M2/(float32)(counter->exec_counter-1));
#endif
#elif defined(PRINT_VAR)  & !( defined(PRINT_S) | defined(PRINT_M2) )
    float32 m_clk = sqrt(m_time/(float32)counter->exec_counter);
    float32 m_ist = sqrt(counter->ist_M2/(float32)counter->exec_counter);
#ifdef C123 //configurable counters
    float32 m_c1 = sqrt(counter->c1_M2/(float32)counter->exec_counter);
    float32 m_c2 = sqrt(counter->c2_M2/(float32)counter->exec_counter);
    float32 m_c3 = sqrt(counter->c3_M2/(float32)counter->exec_counter);
#endif
#else
#error "One of PRINT_M2 PRINTF_S PRINT_VAR has to be defined"
#endif
    printf("%u%s",(unsigned int) counter->exec_counter, SEPARATOR);

    printf("%6.2f%s",time, SEPARATOR);
    printf("%6.2f%s", m_clk, SEPARATOR);
    //printf("%d%s",counter->clk_max, SEPARATOR);

    printf("%6.2f%s", counter->ist_mean, SEPARATOR);
    printf("%6.2f%s", m_ist, SEPARATOR);
    //printf("%d",counter->ist_max, SEPARATOR);
#ifdef C123 //configurable counters
    printf("%6.2f%s", counter->c1_mean, SEPARATOR);
    printf("%6.2f%s", m_c1, SEPARATOR);
    printf("%6.2f%s", counter->c2_mean, SEPARATOR);
    printf("%6.2f%s", m_c2, SEPARATOR);
    printf("%6.2f%s", counter->c3_mean, SEPARATOR);
    printf("%6.2f", m_c3);
#endif
		printf("%s", NEW_LINE);

}


#endif /* TASK_COUNTER_C_ */
