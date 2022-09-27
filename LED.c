/*
 * LED.c
 *
 *  Created on: 3 May 2021
 *      Author: Mojca Kolšek
 */

#include "LED.h"

void init_LEDs(void)
{
    //Configure LEDs to general output, push/pull mode
    IfxPort_setPinMode(LED1, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(LED2, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(LED3, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(LED4, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(LED5, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(LED6, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(LED7, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinMode(LED8, IfxPort_Mode_outputPushPullGeneral);

    //leds are turned of with PinHigh
    IfxPort_setPinHigh(LED1);
    IfxPort_setPinHigh(LED2);
    IfxPort_setPinHigh(LED3);
    IfxPort_setPinHigh(LED4);
    IfxPort_setPinHigh(LED5);
    IfxPort_setPinHigh(LED6);
    IfxPort_setPinHigh(LED7);

    //To indicate that LEDs are configured correctly
    IfxPort_setPinLow(LED8);

}

void toggleLED(Ifx_P *port, uint8 pinIndex)
{
    IfxPort_togglePin(port, pinIndex);

}
void turnOnLED(Ifx_P *port, uint8 pinIndex)
{
    IfxPort_setPinLow(port, pinIndex);
}

void turnOffLED(Ifx_P *port, uint8 pinIndex)
{
    IfxPort_setPinHigh(port, pinIndex);
}



