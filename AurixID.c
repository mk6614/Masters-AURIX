/*
 * AurixID.c
 *
 *  Created on: 24 Jun 2021
 *      Author: Mojca Kolšek
 */

//Bnehcmark includes
#include "AurixID.h"

//iLLDs' includes
#include "IfxScu_bf.h"
#include "IfxScu_reg.h"
#include "IfxScu_regdef.h"
#include "IfxCpu.h"
#ifdef TryC_1GEN
    #include "IfxLmu_reg.h"
#endif
#include "TryCore_cfg.h"

void getAURIX_MUSpecs(AURIX_MuSpecs* specs)
{
    Ifx_SCU* module = &MODULE_SCU;
    Ifx_SCU_CHIPID_Bits id = module->CHIPID.B;

    //Ifx_LMU* lmu = &MODULE_LMU;
    //
    //flash size data from CHIPID bit-field is not human readable
    //flash size is mapped to a a human readable format when printed out
    specs->falsh_size = (AURIX_FlashSize) id.FSIZE;

    //revision A(0),B(1),C(2) or D(3)
    //revision = CHREV >> 4
    specs->revision = (AURIX_Revision) id.CHREV>>4;

    //G1 : CHTEC = 1,
    //G2 = CHTEC = 2
    specs->generation = (AURIX_Generation) id.CHTEC;

    //also equals to value of IfxCpu_ResourceCpu_none
    specs->core_num = IFXCPU_NUM_MODULES;

    //number of bits for this filed has been changed;
    //8 bits in 1st,
    //4 bits in 2nd generation
    specs->series = (AURIX_Series) (specs->generation == G1) ? id.CHID>>4 : id.CHID;

    //if Mu has emulation or ADAS extension, EEA bit is set
    specs->EMEM = id.EEA;

    //if Mu has HSM security package, SEC bit is set
    specs->HSM = id.SEC;

    //max frequency depends on the MU Series
    specs->max_freq = (float32) IFX_CFG_SCU_PLL_FREQUENCY;
}

void printMUSpecs(AURIX_MuSpecs mu_specs, void (*printf)(const char *fmt, ...))
{


    //generate flash size map
    char* flash_size_string[16] = FLASH_SIZE;
    char* flashSize;
    flashSize = flash_size_string[mu_specs.falsh_size];

    //revision 00 equals to A
    //is printed out using hexadecimal format
    uint8 revision = 0xA + mu_specs.revision;

    //printed out as yes or no
    char* EMEM = (mu_specs.EMEM) ? "yes" : "no";
    char* HSM = (mu_specs.HSM) ? "yes" : "no";

    //prints out the name of the MU
    printf("AURIX TC%d%Xx ", mu_specs.generation+1, mu_specs.series);
    printf("%X-Step", revision); //revisions go from A to C
    printf(", %s FLASH", flashSize); //mapped flash size
    printf(", %d cores", mu_specs.core_num); //this is a constant, available through iLLD definitions
    printf(", HSM: %s", HSM);
    printf(", EMEM: %s", EMEM);

    //printed out in MHz
    printf(", CPU speed: %3.0fMHz", mu_specs.max_freq/1000000.0);
    printf("%s", NEW_LINE);
}


void printMUSpecsH(void (*printf)(const char *fmt, ...))
{

    AURIX_MuSpecs mu_specs;
    getAURIX_MUSpecs(&mu_specs);
    //generate string descriptions where needed
    char* flash_size_string[16] = FLASH_SIZE;
    char* flashSize;
    flashSize = flash_size_string[mu_specs.falsh_size];
    uint8 revision = 0xA + mu_specs.revision; //revision 00 equals to A
    char* EMEM = (mu_specs.EMEM) ? "yes" : "no";
    char* HSM = (mu_specs.HSM) ? "yes" : "no";


    printf("AURIX TC%d%Xx ", mu_specs.generation+1, mu_specs.series);
    printf("%X-Step", revision); //revisions go from A to C
    printf(", %s FLASH", flashSize); //flash size has to be mapped
    printf(", %d cores", mu_specs.core_num); //this is a constant
    printf(", HSM: %s", HSM);
    printf(", EMEM: %s", EMEM);
    printf(", CPU speed: %3fMHz", mu_specs.max_freq/1000000.0);
    printf("%s", NEW_LINE);

}

void printCoreSpecsHeader(void (*printf)(const char *fmt, ...))
{
    printf("CoreId%s", SEPARATOR);
    printf("CoreArch%s", SEPARATOR);
    printf("PCache[KB]%s", SEPARATOR);
    printf("DCache[KB]%s", SEPARATOR);
    printf("PSPR[KB]%s", SEPARATOR);
    printf("DSPR[KB]%s", SEPARATOR);
    printf("LADDR");
    printf("%s", NEW_LINE);
}


void getAURIX_CoreSpecs(AURIX_CoreSpecs* specs)
{
    Ifx_CPU_CORE_ID id_reg;
    id_reg.U = __mfcr(CPU_CORE_ID); //relative address (32bits)

    //P&DCON registers control P&DSPR and P&DCache and contain sizes of implemented memory on the TriCore cores directly in KBytes
    Ifx_CPU_DCON2 DCON_reg;
    DCON_reg.U = __mfcr(CPU_DCON2);
    Ifx_CPU_PCON2 PCON_reg;
    PCON_reg.U = __mfcr(CPU_PCON2);

    //CPU_CPU_ID
    Ifx_CPU_CPU_ID reg_impl;
    reg_impl.U = __mfcr(CPU_CPU_ID); //relative address (32bits)
    //numbered core ID
    specs->core_id = id_reg.B.CORE_ID;

    //core implementation
    specs->core = (AURIX_Core) reg_impl.B.MOD;

    specs->pcache_size = (uint16)PCON_reg.B.PCACHE_SZE;
    specs->dcache_size = (uint16)DCON_reg.B.DCACHE_SZE;

    specs->pspr_size = (uint16)PCON_reg.B.PSCRATCH_SZE;
    specs->dspr_size = (uint16)DCON_reg.B.DSCRATCH_SZE;

    specs->local_memory = ((uint64) IFXCPU_GLB_ADDR_DSPR(specs->core_id, 0xD0000000))>>28;
}

void printCoreSpecs(AURIX_CoreSpecs core_specs, void (*printf)(const char *fmt, ...))
{
    //each implementation has a defined value (documented in users' manuals) in CPU_ID bit-field
    char* implementation;
    if (core_specs.core == C16E) implementation = "1.6E";
    if (core_specs.core == C16P) implementation = "1.6P";
    if (core_specs.core == C162P) implementation = "1.6.2P";

    printf("CPU%u", core_specs.core_id);
    printf("\t%s", implementation);
    printf("\t%u", core_specs.pcache_size);
    printf("\t%u", core_specs.dcache_size);
    printf("\t%u", core_specs.pspr_size);
    printf("\t%u", core_specs.dspr_size);
    printf("\t%u", core_specs.local_memory);
    printf("\n\r");

}


void printCoreSpecsH(void (*printf)(const char *fmt, ...))
{
    AURIX_CoreSpecs core_specs;
    getAURIX_CoreSpecs(&core_specs);
    //each implementation has a defined value (documented in users' manuals) in CPU_ID bit-field
    char* implementation;
    if (core_specs.core == C16E) implementation = "1.6E";
    if (core_specs.core == C16P) implementation = "1.6P";
    if (core_specs.core == C162P) implementation = "1.6.2P";

    printf("CPU%u", core_specs.core_id);
    printf("\t%s", implementation);
    printf("\t%u", core_specs.pcache_size);
    printf("\t%u", core_specs.dcache_size);
    printf("\t%u", core_specs.pspr_size);
    printf("\t%u", core_specs.dspr_size);
    printf("\t%u", core_specs.local_memory);
    printf("%s", NEW_LINE);

}





