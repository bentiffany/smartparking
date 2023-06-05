/*
 * timers_and_interrupts.h
 *
 * Author: Ben
 */

#ifndef TIMERS_AND_INTERRUPTS_H_
#define TIMERS_AND_INTERRUPTS_H_

// Standard includes
#include <stdbool.h>

// header includes
#include "pin_mux_config.h"
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_gpio.h"
#include "pin.h"
#include "gpio.h"


// Constant definitions
// TODO: modify GPIO constants for interrupts here
#define IR_GPIO_BASE    GPIOA0_BASE
#define IR_PIN_OFFSET   0x20

//*****************************************************************************
void fastTimerIntHandler();

//*****************************************************************************
void slowTimerIntHandler();

//*****************************************************************************
void initTimers();

//*****************************************************************************
void GPIOBaseIntHandler();

//*****************************************************************************
void initGPIOInterrupt();

/**************************************************************************/


#endif /* TIMERS_AND_INTERRUPTS_H_ */
