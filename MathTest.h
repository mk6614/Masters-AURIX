/*
 * MathTest.h
 *
 *  Created on: 12 May 2021
 *      Author: mojca
 */

#ifndef MATHTEST_H_
#define MATHTEST_H_

#include "BasicCounter.h"
#include "TestCases.h"
#include "stdio.h"
#include "TryCore.h"


typedef enum _VecMathOp
{
    VecSum, //sum a floating point vector
    VecMax, //find maximum in a floating point vector
    VecWave, //generate a sinusoidal wave
    VecLUT, //execution of a sin look-up table for every step
    VecGain, //multiplication of a floating point vector by a scalar (pi),
    VecRead, //simple read operation
    VecWrite, //simple write operation (pi)
    VecPower,    //square each component
    VecSqrt,   //square root of each component
    VecDivide,
    VecMathOperation_None
} VecMathOperation;
#define MATH_OPERATION_STRING   \
{             \
    "VecSum", \
    "VecMax", \
    "VecWave",\
    "VecLUT", \
    "VecGain",\
    "VecRead",\
    "VecWrite",\
    "VecPower",\
    "VecSqrt", \
    "VecDivide" \
}


typedef struct _TryCore_Math
{
    void (*operation)(float32 *data, uint32 data_len);
    task_counter counter;
    uint32 words;
    float32* data;
    char str[10];
} TryCoreVecMath;


//runs operation op  X times and prints out task counter
void TryC_runVecMathX(uint8 X, VecMathOperation op, TryCoreParameters* params, void (*printf)(const char *fmt, ...));
void TryC_runVecMathXTmp(uint8 executions, VecMathOperation op, TryCoreParameters* params, void (*printf)(const char *fmt, ...), uint32* data);

//one can also allocate a task counter and run TryCoreVecMath test case any time.
//to ude this, one must first initialise the test Case
void TryCore_initVecMathCase(TryCoreVecMath* testCase, VecMathOperation op, TryCoreParameters* params);
void TryCore_runVecMathCase(TryCoreVecMath* testCase, void (*printf)(const char *fmt, ...));

//when printing out, one must know the TryC Parameters, which is why TryCoreParameters is passed along when printing
void TryCore_printVecMathHeadder(void (*printf)(const char *fmt, ...));
void TryCore_printVecMathCounter(TryCoreParameters* params, TryCoreVecMath* testCase, void (*printf)(const char *fmt, ...));


/*
typedef struct
{
    float32* data; //pointer to crc data
    sint16 word_count; //size of data in words (32-bit = 4Byte units)
    task_counter counter[VecOp_None]; //performance counter
    float32 (*function[VecOp_None])(float32* data, sint16 data_len);
    float32 res;

    uint8 tmp_params[MathParameters_None];
} TestCaseVecMathF32;



typedef struct
{
    cfloat32* data;
    uint32 doubleword_count;
    task_counter counter[CplxVecOp_None];
    float32* (*function[CplxVecOp_None])(cfloat32 *data, sint16 data_len);
    float32* res;

    uint8 tmp_params[MathParameters_None];
} TestCaseCplxVecMathF32;



//init TestCaseVecMathF32 or TestCaseCplxVecMathF32 before use
void initTestCaseVecMathF32(TestCaseVecMathF32* testCase, float32* data, uint32 word_count);
void initTestCaseCplxVecMathF32(TestCaseCplxVecMathF32* testCase, float32* data, uint32 doubleword_count, float32* res);

void runTestCaseVecMathF32(TestCaseVecMathF32* testCase, VecOp operation);
void runTestCaseCplxVecMathF32(TestCaseCplxVecMathF32* testCase, CplxVecOp operation);

void runNextTestCaseVecMathF32(TestCaseVecMathF32* testCase);
void runNextTestCaseCplxVecMathF32(TestCaseCplxVecMathF32* testCase);


void printHeaderMath(void (*printf)(const char *fmt, ...));
void printCountersVecMathF32(TestCaseVecMathF32* testCase, void (*printf)(const char *fmt, ...));
void printCountersCplxVecMathF32(TestCaseCplxVecMathF32* testCase, void (*printf)(const char *fmt, ...));

void runTryCoreVecMath(TryCore_Parameters* params, void (*printf)(const char *fmt, ...));


uint8 collectTestCaseCplxVecMathF32(TryCoreCounter* counter,TestCaseCplxVecMathF32* testCase, CplxVecOp operation);

*/


#endif /* MATHTEST_H_ */
