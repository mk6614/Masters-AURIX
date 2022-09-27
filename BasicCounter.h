/*
 * NonOSTaskCounter.h
 *
 *  Created on: 22 March 2021
 *      Author: Mojca Kolšek
 */

#ifndef TASK_COUNTER_H_
#define TASK_COUNTER_H_

#include "TriCoreCounter.h"
#include "TryCore_cfg.h"

/*
 * define one of this to either print
 * a) M2 of welford's algorithm
 * b) sample variance ( s = sqrt(M2/(s_size-1) )
 * c) variance ( var = sqrt(M2/s_size) )
 */


//counters C1, C2 and C3 can be programmed to one of

/*
 *  MultiCount Configuration (TC1.6P)
    CFG         M1CNT               M2CNT               M3CNT
    000         IP_DISPATCH_STALL   LS_DISPATCH_STALL   LP_DISPATCH_STALL
    001         PCACHE_HIT          PCACHE_MISS         MULTI_ISSUE
    010         DCACHE_HIT          DCACHE_MISS_CLEAN   DCACHE_MISS_DIRTY
    011         TOTAL_BRANCH        PMEM_STALL          DMEM_STALL

    MultiCount Configuration (TC1.6E)
    (has no DCache, has DRB instead (fully associative, 4 cache lines (=4*256bits) )
    CFG         M1CNT               M2CNT               M3CNT
    000         IP_DISPATCH_STALL   LS_DISPATCH_STALL   -
    001         PCACHE_HIT          PCACHE_MISS         -
    010         DRB_HIT             DRB_MISS            -
    011         TOTAL_BRANCH        PMEM_STALL          DMEM_STALL
 */

/*
 * you can define this macro to get
 * the measurements of time in micro seconds ([mu_s])
 * instead in clock cycles
 */

/*
#define TC16E_MCOUNTERS { \
    { "IP_DISPATCH_STALL",   "LS_DISPATCH_STALL",   "none"                  },\
    { "PCACHE_HIT",          "PCACHE_MISS",         "none"                  },\
    { "DRB_HIT",             "DRB_MISS",            "none"                  },\
    { "TOTAL_BRANCH",        "PMEM_STALL",          "DMEM_STALL"            },\
}
*/
#define TC16P_MCOUNTERS { \
    { "IP_DISPATCH_STALL",   "LS_DISPATCH_STALL",   "LP_DISPATCH_STALL**"     },\
    { "PCACHE_HIT",          "PCACHE_MISS",         "MULTI_ISSUE**"           },\
    { "DCACHE_HIT*",          "DCACHE_MISS_CLEAN*",   "DCACHE_MISS_DIRTY**"     },\
    { "TOTAL_BRANCH",        "PMEM_STALL",          "DMEM_STALL"            },\
}


typedef enum _TryC_PC_Mode
{
    pipeline,
    pcache,
    dcache,
    memstall,
    TryC_PC_Mode_None
} TryC_PC_Mode;




typedef struct {
	//the mean and variance are calculated using Welford's online algorithm
    // variance = sqrt(M2 / exec_counter-1),  stdv = sqrt(variance)
	float32 clk_mean;
	float32 clk_M2;
	//uint32 clk_max;

    float32 ist_mean;
    float32 ist_M2;
    //uint32 ist_max;

#ifdef C123 //3 counters are configurable and implementation specific 
    float32 c1_mean;
    float32 c1_M2;
    float32 c2_mean;
    float32 c2_M2;
    float32 c3_mean;
    float32 c3_M2;
#endif

	uint16 exec_counter;

} task_counter;



void initTaskCounter(task_counter* counter);
void initTaskCounters(uint8 N, task_counter counters[N]);

#define startTask()     __stopPerfCounter(); __resetPerfCounter(); __startPerfCounter()
#define pauseTask()     __stopPerfCounter()
#define resumeTask()     __startPerfCounter()
#define stopTask()      __stopPerfCounter()
void finishTask(task_counter* counter);

void printCounterHeader(void (*printf)(const char *fmt, ...));

void printCounter(task_counter* counter, void (*printf)(const char *fmt, ...));




#endif /* TASK_COUNTER_H_ */
