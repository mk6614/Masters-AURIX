/*
 * LED.h
 *
 *  Created on: 3 May 2021
 *      Author: Mojca Kolšek
 *
 * This header file is written to be used with TriBoard TC264 B-Step
 */

#ifndef LED_H_
#define LED_H_

#include "IfxCpu.h"


//the LED pins should be visible on the board, change if needed
#define LED1        &MODULE_P33,6
#define LED2        &MODULE_P33,7
#define LED3        &MODULE_P33,8
#define LED4        &MODULE_P33,9
#define LED5        &MODULE_P33,10
#define LED6        &MODULE_P33,11
#define LED7        &MODULE_P33,12
#define LED8        &MODULE_P33,13



void init_LEDs(void);
void toggleLED(Ifx_P *port, uint8 pinIndex); //use with definitions, i.e. toggleLED(LED1)
void turnOnLED(Ifx_P *port, uint8 pinIndex);
void turnOffLED(Ifx_P *port, uint8 pinIndex);



#endif /* LED_H_ */
