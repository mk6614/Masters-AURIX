/*
 * ComplexArithmetics.c
 *
 *  Created on: 12 May 2021
 *      Author: Mojca Kolšek
 */

//Benchmark includes
#include "MathTest.h"

#include "BasicCounter.h"
#include "TryCore.h"

//iLLD includes
#include "Ifx_Cf32.h"
#include "Ifx_LutLSincosF32.h"

//std includes
#include "stdio.h"

//////////////////////////////////////////////////////////////////////
// Test Functions for math operations
//////////////////////////////////////////////////////////////////////

void _TryC_wave(float32* data, uint32 data_len)//, uint16 f, uint16 fs) //sinusodial wave generation function
{
    for (uint32 i=0; i<data_len; i++) data[i] = 3.0 * Ifx_LutLSincosF32_sin(IFX_LUT_F32_TO_FXPANGLE(200.0*3.14*(float32)i/1000)); //f=200, fs=1kHz, scaling by 3
}

void _TryC_sum(float32* data, uint32 data_len)//, uint16 f, uint16 fs) //sinusodial wave generation function
{
    volatile float32 sum = 0;
    for (uint32 i=0; i<data_len; i++) sum += data[i];
}

void _TryC_max(float32* data, uint32 data_len)//, uint16 f, uint16 fs) //sinusodial wave generation function
{
    volatile float32 max = 0;
    for (uint32 i=0; i<data_len; i++) if (data[i]>max) max = data[i];
}

void _TryC_LUT(float32* data, uint32 data_len)//, uint16 f, uint16 fs) //sinusodial wave generation function
{
    volatile float32 lut = 0;
    for (uint32 i=0; i<data_len; i++) lut = Ifx_LutLSincosF32_sin((Ifx_Lut_FxpAngle)i);
}

void _TryC_gain(float32* data, uint32 data_len)//, uint16 f, uint16 fs) //sinusodial wave generation function
{
    for (uint32 i=0; i<data_len; i++) data[i] *= 3.14;
}

void _TryC_read(float32* data, uint32 data_len)//, uint16 f, uint16 fs) //sinusodial wave generation function
{
    volatile float32 read = 0;
    for (uint32 i=0; i<data_len; i++) read = data[i];
}

void _TryC_write(float32* data, uint32 data_len)//, uint16 f, uint16 fs) //sinusodial wave generation function
{
    for (uint32 i=0; i<data_len; i++) data[i] = 3.14;
}

void _TryC_power(float32 *data, uint32 data_len)
{
    for (uint32 i=0; i<data_len; i++) data[i] = data[i]*data[i];
}

void _TryC_sqrt(float32 *data, uint32 data_len)
{
    for (uint32 i=0; i<data_len; i+=2) data[i] = sqrt(data[i]);
}

void _TryC_divide(float32 *data, uint32 data_len)
{
    for (uint32 i=0; i<data_len; i+=2) data[i] = 3.14;
}

//an array of functions, included in math tests
void (*_MathFunctions[VecMathOperation_None])(float32 *data, uint32 data_len) = {
    _TryC_sum, //sum a floating point vector
    _TryC_max, //find maximum in a floating point vector
    _TryC_wave, //generate a sinusoidal wave
    _TryC_LUT, //execution of a sin look-up table for every step
    _TryC_gain, //multiplication of a floating point vector by a scalar,
    _TryC_read, //simple read operation
    _TryC_write, //simple write operation
    _TryC_power,    //square each component
    _TryC_sqrt,     //square root of each component
    _TryC_divide //divide each component by pi
};

void TryCore_initVecMathCase(TryCoreVecMath* testCase, VecMathOperation op, TryCoreParameters* params)
{
    testCase->operation = _MathFunctions[op];
    initTaskCounter(&testCase->counter);
    testCase->data = (float32*) params->data;
    testCase->words = params->data_words;
    char* tmp[] = MATH_OPERATION_STRING;
    uint8 i;
    for (i = 0; tmp[op][i] != '\0' && i<9; i++) testCase->str[i] = tmp[op][i];
    while (i++ < 9) testCase->str[i] = '\0';
}

void TryC_runVecMathX(uint8 executions, VecMathOperation op, TryCoreParameters* params, void (*printf)(const char *fmt, ...))
{
    task_counter c;
    initTaskCounter(&c);
    for (uint8 i=0; i<executions; i++)
    {
        uint16 cpuWdtPw = IfxScuWdt_getCpuWatchdogPassword();
        IfxScuWdt_clearCpuEndinit(cpuWdtPw);
        Ifx_SCU *scu = &MODULE_SCU;
        scu->OVCCON.B.CSEL0 = 1;
        scu->OVCCON.B.CSEL1 = 1;
        scu->OVCCON.B.CSEL2 = 1;
        scu->OVCCON.B.DCINVAL = 1;
        IfxScuWdt_setCpuEndinit(cpuWdtPw);


        startTask();
        _MathFunctions[op]((float32*) params->data, params->data_words);
        stopTask();
        finishTask(&c);
    }

    char* str[VecMathOperation_None] = MATH_OPERATION_STRING;
    TryCore_printParameters(params, printf); //PCACHE, DCACHE, DSEC

    printf("%s%s", SEPARATOR, str[op]); //Operation
    printf("%s%u", SEPARATOR, params->data_words); //number of processed words
    printf("%s", SEPARATOR);
    printCounter(&c, printf); //print counter men and dev
    //printf("%s", NEW_LINE);
}


void TryC_runVecMathXTmp(uint8 executions, VecMathOperation op, TryCoreParameters* params, void (*printf)(const char *fmt, ...), uint32* data)
{
    printf("0x%p\n\n", data);
    task_counter c;
    initTaskCounter(&c);
    for (uint8 i=0; i<executions; i++)
    {
        startTask();                                                        \
        _MathFunctions[op]((float32*) data, params->data_words);                                                      \
        finishTask(&c);
    }
    //print

    char* str[VecMathOperation_None] = MATH_OPERATION_STRING;
    params->data = data;
    TryCore_printParameters(params, printf); //PCACHE, DCACHE, DSEC
    printf("%s%s", SEPARATOR, str[op]); //Operation
    printf("%s%u", SEPARATOR, params->data_words); //number of processed words
    printf("%s", SEPARATOR);
    printCounter(&c, printf); //print counter men and dev
    //printf("%s", NEW_LINE);
}

void TryCore_runVecMathCase(TryCoreVecMath* testCase, void (*printf)(const char *fmt, ...))
{
    startTask();
    testCase->operation(testCase->data, testCase->words);
    finishTask(&testCase->counter);
}

void TryCore_printVecMathCounter(TryCoreParameters* params,TryCoreVecMath* testCase, void (*printf)(const char *fmt, ...))
{
    TryCore_printParameters(params, printf); //PCACHE, DCACHE, DSEC
    printf("%s%s", SEPARATOR, testCase->str); //Operation
    printf("%s%u", SEPARATOR, testCase->words); //number of processed words
    printf("%s", SEPARATOR);
    printCounter(&testCase->counter, printf); //print counter men and dev
}


void TryCore_printVecMathHeadder(void (*printf)(const char *fmt, ...))
{
    TryCore_printParametersHeader(printf);
    printf("%sVECMATH", SEPARATOR);
    printf("%sWORDS", SEPARATOR);
    printf("%s", SEPARATOR);
    printCounterHeader(printf);
    //printf("%s", NEW_LINE);
}

