/*
 * AurixID.h
 *
 *  Created on: 23 Jun 2021
 *      Author: Mojca Kolšek
 */

#ifndef AURIXID_H_
#define AURIXID_H_


//iLLD includes
#include "Ifx_Types.h"
#include "IfxCbs_reg.h"
#include "IfxCpu.h"

#define NUMBER_OF_CORES     IFXCPU_NUM_MODULES

typedef enum _AURIX_FlashSize
{
    KB265,
    KB500,
    KB1000,
    KB1500,
    KB2000,
    KB2500,
    KB3000,
    KB4000,
    KB5000,
    KB6000,
    KB7000,
    KB8000,
    KB10000,
    KB12000,
    KB14000,
    KB16000
} AURIX_FlashSize;
//this map is documented in users' manuals
#define FLASH_SIZE  {"256KB", "0.5MB", "1MB", "1.5MB", "2MB", "2.5MB", "3MB", "4MB", "5MB", "6MB", "7MB", "8MB", "10MB", "12MB", "14MB", "16MB" }


typedef enum _AURIX_Generation
{
    G1 = 1,
    G2 = 2
} AURIX_Generation;

typedef enum _AURIX_Series
{
    S0,
    S1,
    S2,
    S3,
    S4,
    S5,
    S6,
    S7,
    S8,
    S9,
    SA,
    SB,
    SC,
    SD,
    SE,
    SF,
} AURIX_Series;

typedef enum _AURIX_Core
{ //0xB7, 0xC0, 0xC0C21
    C16E = 0xB7,
    C16P = 0xC0,
    C162P = 0x00C0C021,
    AURIX_Core_None = 0
} AURIX_Core;

typedef enum _AURIX_CoreID
{
    ID0,
    ID1,
    ID2,
    ID3,
    ID4,
    ID5,
    ID6
} AURIX_CoreID;

typedef enum _AURIX_Revision
{
    A = 0,  //00XXXXb
    B = 1,  //01XXXXb
    C = 2,  //10XXXXb
    D = 3  //11XXXXb
} AURIX_Revision;

typedef struct _AURIX_MuSpecs
{
    AURIX_Generation generation;
    AURIX_Series series;
    AURIX_Revision revision;
    AURIX_FlashSize falsh_size;
    boolean HSM;
    boolean EMEM;
    boolean ADAS;
    float32 max_freq;
    uint8 core_num;
} AURIX_MuSpecs;

typedef struct _AURIX_CoreSpecs
{
    AURIX_CoreID core_id;
    AURIX_Core core;
    uint16 dcache_size;
    uint16 pcache_size;
    uint16 dspr_size;
    uint16 pspr_size;
    uint8 local_memory;
} AURIX_CoreSpecs;

//fills AURIX_MuSpecs with values, read from the chip
void getAURIX_MUSpecs(AURIX_MuSpecs* specs);
//fills AURIX_CoreSpecs with values, read from the executing core
void getAURIX_CoreSpecs(AURIX_CoreSpecs* specs);
//printing functions for MU specifications
void printMUSpecs(AURIX_MuSpecs mu_specs, void (*printf)(const char *fmt, ...));
void printMUSpecsH(void (*printf)(const char *fmt, ...));
//printing functions for core specifications
void printCoreSpecs(AURIX_CoreSpecs core_specs, void (*printf)(const char *fmt, ...));
void printCoreSpecsHeader(void (*printf)(const char *fmt, ...));
void printCoreSpecsH(void (*printf)(const char *fmt, ...));



#endif /* AURIXID_H_ */
