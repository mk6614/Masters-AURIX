/*
 * TryCore_Memory.h
 *
 *  Created on: 20 Jul 2021
 *      Author: Mojca Kolšek
 */

#ifndef TRYCOREMEMORY_H_
#define TRYCOREMEMORY_H_

#include "IfxCpu_cfg.h"
#include "TryCore_cfg.h"

typedef enum _TryC_Memory
{
    TryC_DFlash,
    TryC_lDSPR,
#if IFXCPU_NUM_MODULES > 1
    TryC_gDSPR,
#endif
#if IFXCPU_NUM_MODULES > 4
    TryC_bDSPR,
#endif

#ifdef LMU_MEMORY
    TryC_LMU,
#endif

#ifdef DLMU_MEMORY
    TryC_lDLMU,
#if IFXCPU_NUM_MODULES > 1
    TryC_gDLMU,
#endif

#if IFXCPU_NUM_MODULES > 4
    TryC_bDLMU,
#endif

#endif
    TryC_Memory_None
} TryC_Memory;




//Initialise memory based on number of cores and definitions of LMU and DLMU
uint32* getMemorySection(TryC_Memory location, uint8 core_id);
char* getMemoryLocationString(TryC_Memory location);
void printAvailableMemory(void(*printf)(const char *fmt, ...));






#endif /* TRYCOREMEMORY_H_ */
