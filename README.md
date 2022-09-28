# Masters-AURIX
For my master thesis AURIX Developement Studio has been used to set up an enviroment for performance evaluation measurements

To use the project, create a new project in AURIX Developement Studio and select your device. 
Copy the code files from this git into zour project directory. 
You may have to change the tranciever pin `UART_PIN_TX` in `ASCLIN_UART.c` - refer to user manual for your specific device.
If LED functionality is not working, check the pins in `LED.h` - refer to your board; the LEDs are marked with hodule in pin numbers.
Files `AURIXId.*` can be used to print out information about the device.
Files `TryCore.*` can be used to initaialise the running system (enable/disable data/program cache, select memory section, determine number of words processed and performance counter mode slect).
Files `MAthTest.*` hold the test cases enwraped with counters, that are able to execute a function X times and record a mean and std.

The AURXI DS creats a linking file `Lcf_Tasking_Tricore_Tc.lsl`, which is used by `TryCoreMemory.*` to initialise arrays in available memory sections.
The printing functionality prints out memory section as well as adress of the array. The map should look something like the following (the first number has to match):
```
0xaf000000 - DFlash
0x60004000 - lDSPR
0x70004000 - gDSPR
0x90000000 - lDLMU
0x90010000 - gDLMU
```

