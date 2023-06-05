/* timers_and_interrupts.c
 * Notes: This file pulls heavily from the timer
 * and OLED examples
 * Author: Ben
 */


// Standard includes
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "hw_ints.h"
#include "hw_i2c.h"
#include "i2c.h"
#include "pin.h"
#include "rom.h"
#include "rom_map.h"
#include "timer.h"
#include "prcm.h"
#include "utils.h"
#include "uart.h"
#include "gpio.h"

// Common interface include
#include "uart_if.h"
#include "i2c_if.h"
#include "gpio_if.h"
#include "timer_if.h"

// function definition includes
#include "timers_and_interrupts.h"


// local variables
static bool reading_data = false, data_read_finished = false;
static unsigned long current_count = 0;

// accessible variables
volatile unsigned long g_ulTimerInts = 0;
volatile bool check_messages = false;


//*****************************************************************************
//
// Variables used by the timer interrupt handlers.
//
//*****************************************************************************
static volatile unsigned long g_ulSysTickValue;
static volatile unsigned long ulFastBase = TIMERA0_BASE;
static volatile unsigned long ulSlowBase = TIMERA1_BASE;
static volatile unsigned long g_ulIntClearVector;


//*****************************************************************************
void
fastTimerIntHandler(void)
{
    // Clear the timer interrupt.
    Timer_IF_InterruptClear(ulFastBase);

    g_ulTimerInts++;
}


//*****************************************************************************
void
slowTimerIntHandler(void)
{
    // Clear the timer interrupt
    Timer_IF_InterruptClear(ulSlowBase);

    // Set a flag to check incoming messages
    check_messages = true;
}


//*****************************************************************************
void initTimers()
{
    // Setup timers

    // Configuring the timers
    //
    Timer_IF_Init(PRCM_TIMERA0, ulFastBase, TIMER_CFG_PERIODIC, TIMER_A, 0);
    Timer_IF_Init(PRCM_TIMERA1, ulSlowBase, TIMER_CFG_PERIODIC, TIMER_A, 0);

    // Setup the interrupts for the timer timeouts.
    //
    Timer_IF_IntSetup(ulFastBase, TIMER_A, fastTimerIntHandler);
    Timer_IF_IntSetup(ulSlowBase, TIMER_A, slowTimerIntHandler);

    // Turn on the timers feeding values in microseconds
    //
    // timer for faster things like checking parking space statuses
    Timer_IF_Start(ulFastBase, TIMER_A, 1000);
    // timer for slower things like polling AWS messages
    Timer_IF_Start(ulSlowBase, TIMER_A, 2000000);

}

//*****************************************************************************
void
GPIOBaseIntHandler(void)
{
    // local variables
    current_count = g_ulTimerInts;

    // clear interrupt
    // interrupt status
    tBoolean status;
    status = MAP_GPIOIntStatus(IR_GPIO_BASE, true);
    MAP_GPIOIntClear(IR_GPIO_BASE, status);

    // Do custom work here

}

//*****************************************************************************
void initGPIOInterrupt()
{
    // GPIO Interrupt Initialize
    MAP_GPIOIntRegister(IR_GPIO_BASE, GPIOBaseIntHandler);

    // configure particular edge
    MAP_GPIOIntTypeSet(IR_GPIO_BASE, IR_PIN_OFFSET, GPIO_RISING_EDGE);

    // interrupt status
    tBoolean status;
    status = MAP_GPIOIntStatus(IR_GPIO_BASE, true);

    // clear interrupt
    MAP_GPIOIntClear(IR_GPIO_BASE, status);

    // enable interrupt
    MAP_GPIOIntEnable(IR_GPIO_BASE, IR_PIN_OFFSET);

}

/**************************************************************************/

