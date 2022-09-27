

#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"

#include "ASCLIN_UART.h"
#include "LED.h"


#include "Bsp.h"

#include "TryCore.h"
#include "BasicCounter.h"
#include "TriCoreCounter.h"
#include "AurixID.h"
#include "MathTest.h"
#include "FunctionTest.h"

#include "Ifx_LutLSincosF32.h"

// source waveform (must be 256bit aligned!)
//__attribute__ ((__align(256))) sint32 fftIn[FFT_SIZE] = SINE_WAVE_N512_FS200_F40_60;

// FFT result (must be 256bit aligned!)
//__attribute__ ((__align(256))) sint32 fftOut[FFT_SIZE*2];

IfxCpu_syncEvent e01 = 0, e02 = 0;







int core0_main(void)
{
    IfxCpu_enableInterrupts();
    

    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    //init_ASCLIN_UART();
    init_LEDs();
    turnOnLED(LED1);
    init_ASCLIN_UART();
    my_printf("Hello World\n\r");
    printMUSpecsH(my_printf);
    printCoreSpecsHeader(my_printf);
    printCoreSpecsH(my_printf);

    TryCoreParameters params;

    while(1);
    //system setting for dcache is enabled or disabled
    for (volatile uint8 dcache_enable = 0; dcache_enable <2; dcache_enable++)
   {
        //traverse through all available memory locations (memory is initialised in TryCoreMemory.* and pointer to memory loc is passed to operations)
        for (volatile uint8 mem=0; mem<TryC_Memory_None; mem++)
        {
            //traverse through all defined math operations (test cases)
            volatile VecMathOperation op;
            for (op=0; op<VecMathOperation_None; op++)
            {
               //traverse through counter mode (pipeline, pcache, dcache, memstall)
                //in each mode, the counter records exec time (in ticks) and number of instructions executed
                //the perf. counter has 4 modes, in which it records 3 more events - see BasicCouter.h for more information
                volatile uint8 c_mode;
                for (c_mode = 0; c_mode<TryC_PC_Mode_None; c_mode++)
                {
                    //traverse through different word count(execution count of internal for loops of each )
                    volatile uint16 words;
                    for (words= 1<<8; words <= TryC_WORDS; words<<=1)
                    {
                        initTryCore(&params,(boolean) dcache_enable, 0,mem, words, c_mode);
                        RESET_WORDS(params.data, words);
                        TryC_runVecMathX(30, (VecMathOperation) op, &params, my_printf);
                        //pass a second so ASCLIN gets some time to print (in case of enabled cache problems can occur while printing)
                        for (volatile int i=0; i<2000000; i++) {}
                    }
                }
            }
        }
    }

    turnOnLED(LED2);

    while(1);

    return 0;
}
