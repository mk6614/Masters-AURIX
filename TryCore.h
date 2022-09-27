/*
 * TryCore.h
 *
 *  Created on: 16 Jul 2021
 *      Author: Mojca Kolšek
 */

#ifndef TRYCORE_H_
#define TRYCORE_H_

#include "IfxCpu_cfg.h"
#include "TryCore_cfg.h"
#include "AurixID.h"
#include "TryCoreMemory.h"
#include "BasicCounter.h"

//pseudo random data
#define RESET_WORDS(data, words) for(uint32 i=0; i<words; i++) data[i] = (i*7) | (i%13)<<30


typedef struct _TryCoreParameters
{
    uint32 CPU_freqHz;
    TryC_Memory memory_location;
    boolean data_cachebility;
    boolean program_cachebility;
    uint32* data;
    uint32 data_words;
    //uint8 mcnt_str; unnecesarry complication
    TryC_PC_Mode counter_mode;
} TryCoreParameters;


typedef struct _TryCoreCounter
{
        float32 clk;
        float32 ist;
        float32 MCNT[4][3]; //three MCNT counters in 4 measuring modes
} TryCoreCounter;


typedef enum
{
    CPU0,
#if IFXCPU_NUM_MODULES > 1
    CPU1,
#endif
#if IFXCPU_NUM_MODULES > 2
    CPU2,
#endif
#if IFXCPU_NUM_MODULES > 3
    CPU3,
#endif
#if IFXCPU_NUM_MODULES > 4
    CPU4,
#endif
#if IFXCPU_NUM_MODULES > 5
    CPU5,
#endif
    TryCore_CpuIndex_None
} TryCore_CpuIndex;


void initTryCore(TryCoreParameters* params, boolean dcache, boolean pcache, TryC_Memory memory_loc, uint32 words, TryC_PC_Mode c_mode);
void TryCore_printParametersHeader(void (*printf)(const char *fmt, ...));
void TryCore_printParameters(TryCoreParameters* params, void (*printf)(const char *fmt, ...));



#endif /* TRYCORE_H_ */
