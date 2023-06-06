
/* This file has most of the core OLED and message handling functions
 *
 */


// Graphics includes
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"


// Standard includes
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

// other header includes
#include "lights_sensors.h"
#include "networking.h"

extern int cursor_x;
extern int cursor_y;

float p = 3.1415926;

// oled variables
unsigned int color_options[NUM_COLORS] = {WHITE, BLUE, GREEN, CYAN, RED, MAGENTA, YELLOW};

// sensor variables
unsigned int free_spaces = NUM_SPACES, num_cars = 0;
bool occupied_spaces[NUM_SPACES];

// variables for composing the content string for HTTP POST
unsigned char * outgoing_message_string = NULL;
unsigned int outgoing_message_string_length = 0;
bool sending_update = false;

// accessible variables
bool update_garage_commands = false;
bool spaces_status_changed = false;
volatile bool ready_for_cleanup = false;
char incoming_command[MAX_CHAR_SIZE];

// external variables from timers_and_interrupts
extern volatile unsigned long g_ulTimerInts;

//*****************************************************************************
//  function delays 3*ulCount cycles
void delay(unsigned long ulCount){
	int i;

  do{
    ulCount--;
		for (i=0; i< 65535; i++) ;
	}while(ulCount);
}


/**************************************************************************/
void drawTitle(void)
{
    unsigned char t1[5] = "Open";
    unsigned char t2[7] = "Spaces";
    unsigned int i, x, y, font_size, font_color;

    font_size = 2;
    font_color = CYAN;

    i = 0;
    y = TITLE_Y;
    // print out the title first line
    for (i = 0; i < 5; ++i)
    {
        // x position
        x = TITLE_X + i * 6 * font_size;
        // print character string in current font color
        drawChar(x, y, t1[i], font_color, BLACK, font_size);
    }

    i = 0;
    y = TITLE_Y + 8*font_size + 4;
    // print out the title second line
    for (i = 0; i < 7; ++i)
    {
        // x position
        x = TITLE_X + i * 6 * font_size;
        // print character string in current font color
        drawChar(x, y, t2[i], font_color, BLACK, font_size);
    }

}


/**************************************************************************/
void drawInfo(void)
{

    char s[MAX_CHAR_SIZE];
    unsigned int i, j, x, y, font_size, font_color, str_length;

    // create string
    sprintf(s, "%d", free_spaces);
    i = strlen(s);
    j = 0;
    char s2[] = " free";
    for (j = 0; j < 7; ++j)
    {
        s[i++] = s2[j];
    }
    str_length = strlen(s);

    font_size = 2;
    font_color = WHITE;

    i = 0;
    y = INFO_Y;
    // print out the title first line
    for (i = 0; i < str_length; ++i)
    {
        // x position
        x = INFO_X + i * 6 * font_size;
        // print character string in current font color
        drawChar(x, y, s[i], font_color, BLACK, font_size);
    }

}

/**************************************************************************/
void clearData(void)
{
    int font_size = 2;
    // fill the info row with black
    fillRect(INFO_X, INFO_Y, SSD1351WIDTH - INFO_X, 1*8*font_size, BLACK);
}


/**************************************************************************/
void resetIncomingCommand(void)
{
    // local variables
    int i;

    // clear command char array
    for (i = 0; i < MAX_CHAR_SIZE; ++i)
    {
        incoming_command[i] = '\0';
    }
}


/**************************************************************************/
void resetSpaceStatus(void)
{
    // local variables
    int i;

    // clear space status array
    for (i = 0; i < NUM_SPACES; ++i)
    {
        occupied_spaces[i] = false;
    }
}


//*****************************************************************************
void checkSensorStatuses(void)
{
    int i = NUM_SPACES;
    bool status_has_changed = false;

    // read in the current statuses

    // compare each one, and change the flag if they've changed

    // if the status has changed, update the appropriate flag
    if (status_has_changed)
    {
        spaces_status_changed = true;
    }
}


/**************************************************************************/
void processSensorUpdates(void)
{

    // send the sensor update to the AWS device shadow

    // first check if we are sending a status update
    if (sending_update)
    {
        // currently sending an update,
        // ignore sensor update processing
        return;
    }

    // change the LEDs that are lit
    updateStatusLEDs();

    // process string to send
    if (!prepareOutgoingString())
    {
        // ran into preparation error
        // just skip this processing for now
        return;
    }

    sending_update = true;

    // Now update the shadow's status variables
    updateShadowStatus((char *)outgoing_message_string);

    // now clean it up
    closeOutgoingTransaction();
    ready_for_cleanup = true;


}


/**************************************************************************/
void updateStatusLEDs(void)
{

    // update the status LEDs



}


/**************************************************************************/
bool prepareOutgoingString(void)
{
    // return true if successful, false if need to wait

    // local variables
    unsigned int i = 0, status_int = 0;
    unsigned int str_length, str_index;
    char int_buf[MAX_CHAR_SIZE];
    unsigned int int_buf_length = 0;

    char json_start[] = "{\"state\": {\n\r\"desired\" : {\n\r\"status\" : \"";
    char json_end[] = "\"\n\r}}}\n\r\n\r";

    unsigned int json_start_len = strlen(json_start);
    unsigned int json_end_len = strlen(json_end);

    // check if can modify
    if (sending_update || NULL != outgoing_message_string)
    {
        // sending the string now, don't touch
        return false;
    }

    // bit pack the status update
    for (i = 0; i < NUM_SPACES; ++i)
    {
        status_int |= (((unsigned int) occupied_spaces[i]) << i);
    }
    sprintf(int_buf, "%d", status_int);
    int_buf_length = strlen(int_buf);

    // allocate memory for the string
    // str_length = num_buffer_chars + num_color_changes;
    str_length = int_buf_length + json_start_len + json_end_len + 1;
    str_index = 0;
    outgoing_message_string_length = str_length;

    outgoing_message_string = malloc(str_length*sizeof(unsigned char));

    // write the json start
    memcpy(outgoing_message_string, json_start, json_start_len*sizeof(char));
    str_index = json_start_len;

    // now write the string
    for (i = 0; i < int_buf_length; ++i)
    {
        outgoing_message_string[str_index++] = int_buf[i];
    }

    // now write the json ending
    memcpy(outgoing_message_string + str_index, json_end, json_end_len*sizeof(char));
    // append null character
    outgoing_message_string[str_length-1] = '\0';

    return true;

}

/**************************************************************************/
void closeOutgoingTransaction(void)
{
    // return true if successful, false if need to wait

    // check if in transaction
    if (!sending_update)
    {
        // sending the string now, don't touch
        return;
    }

    // free string memory
    if (NULL == outgoing_message_string)
    {
        // can't free already freed memory
    } else {
        free(outgoing_message_string);
        outgoing_message_string = NULL;
        outgoing_message_string_length = 0;
    }

    sending_update = false;
}


/**************************************************************************/
void executeCommands(void)
{
    // local variables

    // parse the commands

    // run the commands

    // cleanup the buffer
    update_garage_commands = false;
}

/**************************************************************************/

