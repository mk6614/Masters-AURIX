/*
 * TryCore.c
 *
 *  Created on: 16 Jul 2021
 *      Author: Mojce Kolšek
 */

#include "TryCore.h"
#include "TriCoreCounter.h"
#include "math.h"

#include "IfxScuCcu.h"


void initTryCore(TryCoreParameters* params, boolean dcache, boolean pcache, TryC_Memory memory_loc, uint32 words, TryC_PC_Mode c_mode)
{
    Ifx_CPU_CORE_ID id_reg;
    id_reg.U = __mfcr(CPU_CORE_ID); //relative address (32bits)
    uint8 core_id = id_reg.B.CORE_ID;

    //invalidate and set enable/disable data cache:
    //first, disable cachebility
    __dsync();
    IfxCpu_setDataCache(FALSE);
    uint16 enable = 0;
    //second, enable data reads for foreign DSPRs and PSPRs
    //! own dspr (in section 7-core_id) is restrained to be uncacheable
    for (uint16 i=0; i < IFXCPU_NUM_MODULES; i++) enable |= 1<<(7-i);
    enable |= 1<<0x8;//enable cacheability for data reads from PFlash and BROOM
    enable |= 1<<0xC;//enable cacheability for data reads from own PSPR
    enable |= 1<<0xB; //enable cacheability for data reads from LMU and EMEM for both of available sections
    enable |= 1<<0x9;//enable cacheability for data reads from LMU and EMEM for both of available sections
    //0xA: access of DFlash, PFlash and BROM, restrained to be uncacheable
    //0xD: access of own DSPR and, restrained to be uncacheable for data access (can still be cacheable for instruction reads)
    //0xF: access of SRI, peripherals, SFRs (Special Function Registers) and CSFRs (Core SFRs), restrained to be uncacheable
    //0xE: access to ?, restrained to be uncacheable
    IfxCpu_enableSegmentSpecificDataAccessCacheability(enable, TRUE);
    //set dcache enable/disable
    IfxCpu_setDataCache(dcache);


    uint16 cpuWdtPassword = IfxScuWdt_getCpuWatchdogPassword();
        {
            IfxScuWdt_clearCpuEndinit(cpuWdtPassword);
            Ifx_SCU *scu = &MODULE_SCU;
            scu->OVCCON.B.CSEL0 = 1;
            scu->OVCCON.B.CSEL1 = 1;
            scu->OVCCON.B.CSEL2 = 1;
            scu->OVCCON.B.DCINVAL = 1;
            IfxScuWdt_setCpuEndinit(cpuWdtPassword);
        }

    params->data_cachebility = dcache;

    IfxCpu_setProgramCache(pcache);
    IfxCpu_invalidateProgramCache();
    params->program_cachebility = pcache;


    //CPU_CPU_ID
    Ifx_CPU_CPU_ID reg_impl;
    reg_impl.U = __mfcr(CPU_CPU_ID); //relative address (32bits)
    //params->mcnt_str = (((AURIX_Core) reg_impl.B.MOD) == C16E ) ? 0 : 1;


    params->CPU_freqHz = IfxScuCcu_getCpuFrequency((IfxCpu_ResourceCpu) core_id);
    //set counter mode
    params->counter_mode = c_mode;
    __initPerfCounter(params->counter_mode, params->counter_mode, params->counter_mode);

    params->data = 0;
    params->data = getMemorySection(memory_loc, core_id);
    params->memory_location = memory_loc;
    params->data_words = (params->data) ? words : 0;
}

void TryCore_printParametersHeader(void (*printf)(const char *fmt, ...))
{
    printf("C_MODE");
    printf("%sDCACHE", SEPARATOR);
    printf("%sPCACHE", SEPARATOR);
    printf("%sDSEC", SEPARATOR);
    //printf("%sWORDS", SEPARATOR);
}

void TryCore_printParameters(TryCoreParameters* params, void (*printf)(const char *fmt, ...))
{
    //printCoreSpecs(getAURIX_CoreSpecs(), printf);
    printf("%d", params->counter_mode);
    printf("%s%u", SEPARATOR,(uint8)params->data_cachebility);
    printf("%s%u", SEPARATOR, (uint8)params->program_cachebility);
    printf("%s%p", SEPARATOR, params->data);
    printf("%s%s", SEPARATOR, getMemoryLocationString(params->memory_location));
    //printf("%s%lu", SEPARATOR, params->data_words);
}


//reject yes or no
unsigned int tTest(float mean0, float mean1, float sqrt_var0, float sqrt_var1, int N)
{
    float t = 0;
    if (sqrt_var0 == 0 && sqrt_var1 == 0) t = mean0-mean1;
    else t = (mean0 - mean1) / sqrt((sqrt_var0 + sqrt_var1) / N);
    //critical value for two way, N = 30, 95% confidence interval is 2.0
    return (t>2.0 | t<-2.0) ? 1 : 0;
}

float32 sum_array(float32* array, uint32 N)
{
    float sum = 0;
    for (int i=0; i<N; i++) sum += array[i];
    return sum;
}

uint8 fourWay_tTest(float32 means[4], float32 vars[4], uint32 N)
{
    float32 mean_grand = sum_array(means, 4)/4;
    float32 mean_var = sum_array(vars, 4)/4;
    uint8 result = 0;
    for (uint8 i=0; i<4; i++)
    {
        result |= tTest(mean_grand, means[i], mean_var, vars[i], N);
        result<<=1;
    }
    return result>>1;
}

void TryCore_printTryCoreCounter(TryCoreCounter* counter, void (*printf)(const char *fmt, ...))
{
    printf("%f",counter->clk);
    printf("%s%f", SEPARATOR, counter->ist);
    for (int i=0; i<4; i++)
        for (uint8 j=0; j<3; j++)
            printf("%s%f", SEPARATOR, counter->MCNT[i][j]);
}

/*
#define TC16E_MCOUNTERS { \
    { "IP_DISPATCH_STALL",   "DP_DISPATCH_STALL",   "none"                  },\
    { "PCACHE_HIT",          "PCACHE_MISS",         "none"                  },\
    { "DRB_HIT",             "DRB_MISS",            "none"                  },\
    { "TOTAL_BRANCH",        "PMEM_STALL",          "DMEM_STALL"            },\
}

#define TC16P_MCOUNTERS { \
    { "IP_DISPATCH_STALL",   "DP_DISPATCH_STALL",   "LP_DISPATCH_STALL"     },\
    { "PCACHE_HIT",          "PCACHE_MISS",         "MULTI_ISSUE"           },\
    { "DCACHE_HIT",          "DCACHE_MISS_CLEAN",   "DCACHE_MISS_DIRTY"     },\
    { "TOTAL_BRANCH",        "PMEM_STALL",          "DMEM_STALL"            },\
}
*/
/*
void TryCore_printTryCoreCounterHeader(uint8 mcnt_core, void (*printf)(const char *fmt, ...))
{
    char* mcnt[2][4][3] = { TC16E_MCOUNTERS, TC16P_MCOUNTERS };
    printf("CLOCK");
    printf("%sINSTRUCTIONS", SEPARATOR);
    for (int i=0; i<4; i++)
        for (uint8 j=0; j<3; j++)
            printf("%s%s", SEPARATOR, mcnt[mcnt_core][i][j]);
}*/





