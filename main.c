//*****************************************************************************
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************

//*****************************************************************************
//
// Application Name     - Project 6 Demonstration project
// Application Overview - This project is a modification of several CC3200 SDK
//                        example projects to fit our 6th EEC 172 project.
//
//*****************************************************************************

// Standard include
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Simplelink includes
#include "simplelink.h"

// Driverlib includes
#include "hw_types.h"
#include "interrupt.h"
#include "hw_ints.h"
#include "hw_types.h"
#include "hw_apps_rcm.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "hw_uart.h"
#include "hw_i2c.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "timer.h"
#include "utils.h"
#include "uart.h"
#include "spi.h"
#include "i2c.h"

// Common interface includes
#include "timer_if.h"
#include "gpio_if.h"
#include "common.h"
#include "uart_if.h"
#include "i2c_if.h"

#include "pin_mux_config.h"

// Additional Adafruit libraries
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"

// custom function files
#include "networking.h"
#include "timers_and_interrupts.h"
#include "lights_sensors.h"


// external variables
extern bool spaces_status_changed;
extern bool update_garage_commands;
extern long lNetworkingRetVal;
extern volatile bool check_parking_spaces;
extern volatile bool check_commands;
extern volatile bool is_stuck;

//*****************************************************************************
//                      MACRO DEFINITIONS
//*****************************************************************************
#define APPLICATION_VERSION  "1.0.0"
#define APP_NAME             "Smart Parking Garage"
#define FOREVER              1
#define CONSOLE              UARTA0_BASE
#define UartGetChar()        MAP_UARTCharGet(CONSOLE)
#define UartPutChar(c)       MAP_UARTCharPut(CONSOLE,c)
#define MAX_STRING_LENGTH    80

//*****************************************************************************
//
// Application Master/Slave mode selector macro (for OLED)
//
// MASTER_MODE = 1 : Application in master mode
// MASTER_MODE = 0 : Application in slave mode
//
//*****************************************************************************
#define MASTER_MODE      1

#define SPI_IF_BIT_RATE  2000000
#define TR_BUFF_SIZE     100

//*****************************************************************************
//                      Global Variables for Vector Table
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif



//*****************************************************************************
//                      LOCAL DEFINITION
//*****************************************************************************

//*****************************************************************************
//
//! Application startup display on UART
//!
//! \param  none
//!
//! \return none
//!
//*****************************************************************************
static void
DisplayBanner(char * AppName)
{

    Report("\n\n\n\r");
    Report("\t\t *************************************************\n\r");
    Report("\t\t        CC3200 %s Application       \n\r", AppName);
    Report("\t\t *************************************************\n\r");
    Report("\n\n\n\r");
}

// OLED initialization function
//*****************************************************************************
void OLED_Init()
{


    //
    // Reset the peripheral
    //
    MAP_PRCMPeripheralReset(PRCM_GSPI);

    //
    // Reset SPI
    //
    MAP_SPIReset(GSPI_BASE);

    //
    // Configure SPI interface
    //
    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                     SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                     (SPI_SW_CTRL_CS |
                     SPI_4PIN_MODE |
                     SPI_TURBO_OFF |
                     SPI_CS_ACTIVEHIGH |
                     SPI_WL_8));

    MAP_SPIEnable(GSPI_BASE);

    Adafruit_Init();

}

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//*****************************************************************************
//
//!    main function demonstrates the use of the timers to generate
//! periodic interrupts.
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
int
main(void)
 {

    // Initialize board configurations
    BoardInit();


    // Pinmuxing for GPIO, SPI, etc
    //
    PinMuxConfig();

    //
    // Enable the SPI module clock
    //
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);

    //
    // Initializing the Terminal.
    //
    InitTerm();
    //
    // Clearing the Terminal.
    //
    ClearTerm();
    DisplayBanner(APP_NAME);

    MAP_UtilsDelay(80000);

    //
    // I2C Init
    //
    I2C_IF_Open(I2C_MASTER_MODE_FST);

    MAP_UtilsDelay(10000);

    // Test all of the I2C devices
    resetI2CDevices();
    testStatusLEDs();
    Report("\n\rinitProximitySensors\n\r");
    initProximitySensors();
    Report("\n\rend initProximitySensors\n\r");

    OLED_Init();
    // clear screen
    fillScreen(BLACK);

    // Draw the initial info on the display
    drawTitle();
    drawInfo();

    // CC3200 SimpleLink WIFI initialization
    // Connect the CC3200 to the local access point
    lNetworkingRetVal = connectToAccessPoint();
    // Set time so that encryption can be used
    lNetworkingRetVal = set_time();
    if(lNetworkingRetVal < 0) {
        UART_PRINT("Unable to set time in the device");
        LOOP_FOREVER();
    }
    // Connect to the website with TLS encryption
    lNetworkingRetVal = tls_connect();
    if(lNetworkingRetVal < 0) {
        ERR_PRINT(lNetworkingRetVal);
    }

    // timer/interrupt initializations
    initTimers();

    initGPIOInterrupt();

    // Loop forever while the timers run.
    //
    while(FOREVER)
    {
        // every loop processes go here - none yet

        // only flag-triggered processes go here
        if (check_parking_spaces)
        {
            // run a quick sensor poll
            Report("\n\rcheckSensorStatuses\n\r");
            checkSensorStatuses();
            Report("\n\rend checkSensorStatuses\n\r");
        }
        if (spaces_status_changed)
        {
            // process the updated parking space status
            processSensorUpdates();
            drawInfo();
        }
        if (check_commands)
        {
            // retrieve the commands from the device shadow
            getShadowCommand();
        }
        if (update_garage_commands)
        {
            // parse and run the updated commands
            executeCommands();
        }

        if (is_stuck)
        {
            // FULL I2C DEVICE RESET
            MAP_I2CMasterIntClear(I2CA0_BASE);

            MAP_I2CMasterDisable(I2CA0_BASE);


            MAP_PRCMPeripheralClkDisable(PRCM_I2CA0, PRCM_RUN_MODE_CLK);

            MAP_UtilsDelay(100);

            MAP_PRCMPeripheralClkEnable(PRCM_I2CA0, PRCM_RUN_MODE_CLK);
            MAP_PRCMPeripheralReset(PRCM_I2CA0);

            MAP_I2CMasterEnable(I2CA0_BASE);

            // reset I2C devices
            resetI2CDevices();

            // Clear all interrupts.
            MAP_I2CMasterIntClear(I2CA0_BASE);

            // Enable interrupts.
            MAP_I2CMasterIntEnableEx(I2CA0_BASE,
                                     I2C_MASTER_INT_TIMEOUT |        // timeout
                                     I2C_MASTER_INT_DATA            // data transaction complete
                                    );

            MAP_I2CMasterInitExpClk(I2CA0_BASE,SYS_CLK,true);
        }
    }
    // close TLS and return - commented out so that the compiler
    // doesn't give a "statement unreachable" warning
    // sl_Stop(SL_STOP_TIMEOUT);
    // return 0;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
