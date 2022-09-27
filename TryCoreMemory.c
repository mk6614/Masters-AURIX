/*
 * TryCore_Memory.c
 *
 *  Created on: 20 Jul 2021
 *      Author: Mojca Kolšek
 */

#include "TryCoreMemory.h"

#include "IfxCpu.h"
#include "TryCore_cfg.h"

/*
 * data space in specific memory locations:
 * [local,global,bridged]DSPR,
 * LMU,
 * [local,global,bridged] DLMU
 * dFlash
 */

//DSPR SECTION
#pragma section farbss bss_cpu0
uint32 dDSPR0[TryC_WORDS]; //Data Scratch-Pad RAM, local to CPU0

#if IFXCPU_NUM_MODULES > 1
#pragma section farbss bss_cpu1
uint32 dDSPR1[TryC_WORDS];    //Data Scratch-Pad RAM, local to CPU1
#endif

#if IFXCPU_NUM_MODULES > 4
#pragma section farbss bss_cpu4
uint32 dDSPR1[TryC_WORDS];    //Data Scratch-Pad RAM, local to CPU1
#endif

#pragma section farbss restore

//LMU SECTION
#ifdef LMU_MEMORY
#pragma section farbss lmubss
uint32 dLMU[TryC_WORDS]; //Local Memory Unit (not present on all micro-controllers)
#pragma section farbss restore
#endif


//DLMU SECTION
#ifdef DLMU_MEMORY

#pragma section fardata dlmu0
uint32 dDLMU0[TryC_WORDS]; // Distributed Local Memory Unit (not present on any 1G (TC2xx) micro-controllers)

#if IFXCPU_NUM_MODULES > 1
#pragma section fardata dlmu2
uint32 dDLMU1[TryC_WORDS]; // Distributed Local Memory Unit (not present on any 1G (TC2xx) micro-controllers)
#endif

#if IFXCPU_NUM_MODULES > 4
#pragma section fardata dlmu4
uint32 dDLMU4[TryC_WORDS]; // Distributed Local Memory Unit (not present on any 1G (TC2xx) micro-controllers)
#endif

#pragma section fardata restore
#endif


uint8 default_var = 0; //the default address for data is gathered from this variable;

//the memory locations are available throug this pointer


char* TryC_Memory_STRING[TryC_Memory_None] = {
       "DFlash",
       "lDSPR",
    #if IFXCPU_NUM_MODULES > 1
       "gDSPR",
    #endif

    #if IFXCPU_NUM_MODULES > 4
       "bDSPR",
    #endif

    #ifdef LMU_MEMORY
       "LMU",
    #endif

#ifdef DLMU_MEMORY
       "lDLMU",
    #if IFXCPU_NUM_MODULES > 1
       "gDLMU",
    #endif

    #if IFXCPU_NUM_MODULES > 4
       "bDLMU",
    #endif
#endif

};


char* getMemoryLocationString(TryC_Memory location)
{
    return TryC_Memory_STRING[location];
}



void printAvailableMemory(void(*printf)(const char *fmt, ...))
{
    uint8 default_section = (uint8)(((uint64)&default_var)>>28);
    printf("Default memory section: %u\n\r", default_section );
    for (uint16 i=0; i<IFXCPU_NUM_MODULES; i++)
    {
        printf("CPU%d: section %u\n\r", i, ((uint64)IFXCPU_GLB_ADDR_DSPR(i, 0xD0000000))>>28);
    }
#ifdef LMU_MEMORY
    uint8 lmu_section = (uint8)((uint64)dLMU)>>28;
    printf("LMU section: %u", lmu_section );
    if (lmu_section == default_section) printf(" - not present\n\r"); else printf("\n\r");

#endif
#ifdef DLMU_MEMORY
    uint8 dlmu_section = (uint8)(((uint64)dDLMU0)>>28);
    printf("dDLMU section: section %u", dlmu_section);
    if (dlmu_section == default_section) printf(" - not present\n\r"); else printf("\n\r");
#endif
}

uint32 dFlash[TryC_WORDS] __at(FLASH_MEM_ADDRES);


uint32* getMemorySection(TryC_Memory location, uint8 core_id)
{
    switch (location)
    {
        case (TryC_DFlash):
            return (uint32*)FLASH_MEM_ADDRES;
            break;
    #if IFXCPU_NUM_MODULES > 4
        case TryC_gDSPR
            return core_id) ? dDSPR0 : dDSPR1;
            break;
        case TryC_lDSPR:
            if (core_id==0) return dDSPR0;
            else if (core_id==1) return = dDSPR0;
            else if return = dDSPR4;
            break;
        case TryC_bDSPR:
            return (core_id) ? dDSPR0 : dDSPR4;
            break;

#elif IFXCPU_NUM_MODULES > 1
        case TryC_gDSPR:
            return (core_id) ? dDSPR0 : dDSPR1;
            break;
        case TryC_lDSPR:
            return (core_id) ? dDSPR1 : dDSPR0;
#else
        case TryC_lDSPR:
            return dDSPR0;
            break;
#endif


#ifdef LMU_MEMORY
        case LMU:
            return dLMU
#endif

#ifdef DLMU_MEMORY
#if IFXCPU_NUM_MODULES > 4
    case TryC_gDLMU
        return core_id) ? dDLMU0 : dDLMU1;
        break;
    case TryC_lDLMU:
        if (core_id==0) return dDLMU0;
        else if (core_id==1) return = dDLMU0;
        else if return = dDLMU4;
        break;
    case TryC_bDLMU:
        return (core_id) ? dDLMU0 : dDLMU4;
        break;

#elif IFXCPU_NUM_MODULES > 1
    case TryC_gDLMU:
        return (core_id) ? dDLMU0 : dDLMU1;
        break;
    case TryC_lDLMU:
        return (core_id) ? dDLMU1 : dDLMU0;
#else
    case TryC_lDLMU:
        return dDLMU0;
        break;
#endif

#endif


        default:
            return 0;
            break;
    }
    return 0;

}





