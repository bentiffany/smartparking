/*
 * lights_sensors.h
 *
 *  Created on: May 05, 2023
 *      Author: Ben
 *  Notes: OLED-specific code may originate from Adafruit.
 *  Other code is likely custom or based on a
 *  CC3200 SDK example
 */

#ifndef LIGHTS_SENSORS_H_
#define LIGHTS_SENSORS_H_


// Standard includes
#include <stdbool.h>

// Color definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define GREEN           0x07E0
#define CYAN            0x07FF
#define RED             0xF800
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF

#define NUM_COLORS      7

// Setting the size of the buffer used for incoming commands
#define MAX_CHAR_SIZE   255

// Layout definitions for the parking sign
#define TITLE_X         12
#define TITLE_Y         12
#define INFO_X          12
#define INFO_Y          64

// constants for parking layout/status
#define NUM_SPACES      8

// constants for communication
#define I2C_SWITCH_ADDR             0x70
#define BASE_LIGHT_SENSOR_ADDR      0x13
#define BASE_LIGHT_SWITCH_ADDR      0x20

// threshold for proximity
#define PROX_THRESHOLD              6000

/**************************************************************************/
void drawTitle(void);

/**************************************************************************/
void drawInfo(void);

/**************************************************************************/
void clearData(void);

/**************************************************************************/
void resetIncomingCommand(void);

/**************************************************************************/
void initProximitySensors(void);

/**************************************************************************/
void checkSensorStatuses(void);

/**************************************************************************/
void processSensorUpdates(void);

/**************************************************************************/
void updateStatusLEDs(void);

/**************************************************************************/
void testStatusLEDs(void);

/**************************************************************************/
bool prepareOutgoingString();

/**************************************************************************/
void closeOutgoingTransaction();

/**************************************************************************/
void executeCommands();

/**************************************************************************/


#endif /* LIGHTS_SENSORS_H_ */
