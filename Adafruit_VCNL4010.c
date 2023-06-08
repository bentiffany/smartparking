/**************************************************************************/
/*!
 * NOTES: This file is largely based on the Adafruit Arduino library
 * for the VCNL4010.
 *
  @file     Adafruit_VCNL4010.cpp

  @mainpage Adafruit VCNL4010 Ambient Light/Proximity Sensor

  @section intro Introduction

  This is a library for the Aadafruit VCNL4010 proximity sensor breakout board
  ----> http://www.adafruit.com/products/466

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  @section author Author

  K. Townsend (Adafruit Industries)

  @section license License

  BSD (see license.txt)
*/
/**************************************************************************/

// stdlib includes
#include "stdlib.h"
#include "stdbool.h"
#include "stdint.h"

// library includes
#include "rom.h"
#include "rom_map.h"
#include "hw_i2c.h"
#include "i2c.h"
#include "utils.h"

// interface includes
#include "i2c_if.h"

#include "Adafruit_VCNL4010.h"

// local variables
static Adafruit_I2CDevice *i2c_dev;
static unsigned char vcnl_addr = VCNL4010_I2CADDR_DEFAULT; // I2C default address


/**************************************************************************/
/*!
    @brief  Setups the I2C connection and tests that the sensor was found. If
    so, configures for 200mA IR current and 390.625 KHz.
    @param addr Optional I2C address (however, all chips have the same address!)
    @param theWire Optional Wire object if your board has more than one I2C
    interface
    @return true if sensor was found, false if not
*/
/**************************************************************************/
bool Adafruit_VCNL4010_begin() {
    if (i2c_dev)
    {
        free (i2c_dev);
        i2c_dev = NULL;
    }
    i2c_dev = malloc(sizeof(Adafruit_I2CDevice));
    i2c_dev->addr = vcnl_addr;

    uint8_t rev = Adafruit_VCNL4010_read8(VCNL4010_PRODUCTID);
    if ((rev & 0xF0) != 0x20) {
        return false;
    }

    Adafruit_VCNL4010_setLEDcurrent(20);             // 200 mA
    Adafruit_VCNL4010_setFrequency(VCNL4010_16_625); // 16.625 readings/second
    // 16 consecutive measurements past threshold
    Adafruit_VCNL4010_write8(VCNL4010_INTCONTROL, 0x08);
    // additional config for what I want
    uint8_t param = 0;
    param = 6 << 4; // 8 samples / second
    param |= 1UL << 7; // continuous conversion mode (faster)
    param |= 1UL << 3; // automatic offset compensation
    param |= 5; // 2^5 = 32 measurements averaged for result
    Adafruit_VCNL4010_write8(VCNL4010_AMBIENTPARAMETER, param);
    return true;
}

/**************************************************************************/
/*!
    @brief checks the I2C Device's address for an ACK
    @param
    @return true if sensor was found, false if not
*/
/**************************************************************************/
bool Adafruit_VCNL4010_detectAddress(Adafruit_I2CDevice *i2c_device) {
    // maybe later add an actual check here

    //
    // Set I2C codec slave address
    //
//    MAP_I2CMasterSlaveAddrSet(I2C_BASE, i2c_device->addr, true);
    //
    // Clear all interrupts
    //
//    MAP_I2CMasterIntClear(I2C_BASE);

    //
    // Set the time-out. Not to be used with breakpoints.
    //
//    MAP_I2CMasterTimeoutSet(I2C_BASE, I2C_TIMEOUT_VAL);

    //
    // Initiate the transfer.
    //
//    MAP_I2CMasterControl(I2C_BASE, ulCmd);

    //
    // Wait until the current byte has been transferred.
    // Poll on the raw interrupt status.
    //
//    while((MAP_I2CMasterIntStatusEx(I2C_BASE, false)
//                & (I2C_MASTER_INT_STOP | I2C_MASTER_INT_DATA)) == 0)
//    {
//        if ((MAP_I2CMasterIntStatusEx(I2C_BASE, false) & I2C_MASTER_INT_TIMEOUT) > 0)
//        {
//            return FAILURE;
//        }
//    }

    //
    // Check for any errors in transfer
    //
//    if(MAP_I2CMasterErr(I2C_BASE) != I2C_MASTER_ERR_NONE)
//    {
//        switch(ulCmd)
//        {
//        case I2C_MASTER_CMD_BURST_SEND_START:
//        case I2C_MASTER_CMD_BURST_SEND_CONT:
//        case I2C_MASTER_CMD_BURST_SEND_STOP:
//            MAP_I2CMasterControl(I2C_BASE,
//                         I2C_MASTER_CMD_BURST_SEND_ERROR_STOP);
//            break;
//        case I2C_MASTER_CMD_BURST_RECEIVE_START:
//        case I2C_MASTER_CMD_BURST_RECEIVE_CONT:
//        case I2C_MASTER_CMD_BURST_RECEIVE_FINISH:
//            MAP_I2CMasterControl(I2C_BASE,
//                         I2C_MASTER_CMD_BURST_RECEIVE_ERROR_STOP);
//            break;
//        default:
//            break;
//        }
//        return false;
//    }

    i2c_device->detected_address = true;

    return true;
}

/**************************************************************************/
/*!
    @brief  Set the LED current.
    @param  current_10mA  Can be any value from 0 to 20, each number represents
   10 mA, so if you set it to 5, its 50mA. Minimum is 0 (0 mA, off), max is 20
   (200mA)
*/
/**************************************************************************/

void Adafruit_VCNL4010_setLEDcurrent(uint8_t current_10mA) {
  if (current_10mA > 20)
    current_10mA = 20;
  Adafruit_VCNL4010_write8(VCNL4010_IRLED, current_10mA);
}

/**************************************************************************/
/*!
    @brief  Get the LED current
    @return  The value directly from the register. Each bit represents 10mA so 5
   == 50mA
*/
/**************************************************************************/

uint8_t Adafruit_VCNL4010_getLEDcurrent(void) { return Adafruit_VCNL4010_read8(VCNL4010_IRLED); }

/**************************************************************************/
/*!
    @brief  Set the measurement signal frequency
    @param  freq Sets the measurement rate for proximity. Can be VCNL4010_1_95
   (1.95 measurements/s), VCNL4010_3_90625 (3.9062 meas/s), VCNL4010_7_8125
   (7.8125 meas/s), VCNL4010_16_625 (16.625 meas/s), VCNL4010_31_25 (31.25
   meas/s), VCNL4010_62_5 (62.5 meas/s), VCNL4010_125 (125 meas/s) or
   VCNL4010_250 (250 measurements/s)
*/
/**************************************************************************/

void Adafruit_VCNL4010_setFrequency(vcnl4010_freq freq) {
  Adafruit_VCNL4010_write8(VCNL4010_PROXRATE, freq);
}

/**************************************************************************/
/*!
    @brief  Get proximity measurement
    @return Raw 16-bit reading value, will vary with LED current, unit-less!
*/
/**************************************************************************/

uint16_t Adafruit_VCNL4010_readProximity(void) {
  uint8_t i = Adafruit_VCNL4010_read8(VCNL4010_INTSTAT);
  i &= ~0x80;
  Adafruit_VCNL4010_write8(VCNL4010_INTSTAT, i);

  Adafruit_VCNL4010_write8(VCNL4010_COMMAND, VCNL4010_MEASUREPROXIMITY);
  while (1) {
    // Serial.println(Adafruit_VCNL4010_read8(VCNL4010_INTSTAT), HEX);
    uint8_t result = Adafruit_VCNL4010_read8(VCNL4010_COMMAND);
    // Serial.print("Ready = 0x"); Serial.println(result, HEX);
    if (result & VCNL4010_PROXIMITYREADY) {
      return Adafruit_VCNL4010_read16(VCNL4010_PROXIMITYDATA);
    }
    MAP_UtilsDelay(10000);
  }
}

/**************************************************************************/
/*!
    @brief  Get ambient light measurement
    @return Raw 16-bit reading value, unit-less!
*/
/**************************************************************************/

uint16_t Adafruit_VCNL4010_readAmbient(void) {
  uint8_t i = Adafruit_VCNL4010_read8(VCNL4010_INTSTAT);
  i &= ~0x40;
  Adafruit_VCNL4010_write8(VCNL4010_INTSTAT, i);

  Adafruit_VCNL4010_write8(VCNL4010_COMMAND, VCNL4010_MEASUREAMBIENT);
  while (1) {
    // Serial.println(Adafruit_VCNL4010_read8(VCNL4010_INTSTAT), HEX);
    uint8_t result = Adafruit_VCNL4010_read8(VCNL4010_COMMAND);
    // Serial.print("Ready = 0x"); Serial.println(result, HEX);
    if (result & VCNL4010_AMBIENTREADY) {
      return Adafruit_VCNL4010_read16(VCNL4010_AMBIENTDATA);
    }
    MAP_UtilsDelay(10000);
  }
}

/**************************************************************************/
/*!
    @brief  I2C low level interfacing
*/
/**************************************************************************/

// Read 1 byte from the VCNL4000 at 'address'
uint8_t Adafruit_VCNL4010_read8(uint8_t address) {
  unsigned char buffer = address;
  I2C_IF_ReadFrom(vcnl_addr,
                &buffer,
                (unsigned char) 1,
                &buffer,
                (unsigned char) 1);
  return buffer;
}

// Read 2 byte from the VCNL4010 at 'address'
uint16_t Adafruit_VCNL4010_read16(uint8_t address) {
    unsigned char buffer[2] = {address, 0};
    I2C_IF_ReadFrom(vcnl_addr,
                (unsigned char *)&buffer,
                (unsigned char) 1,
                (unsigned char *)&buffer,
                (unsigned char) 2);
    uint16_t result = ((uint16_t) buffer[0]) << 8;
    result |= (uint16_t) buffer[1];
    return result;
}

// write 1 byte
void Adafruit_VCNL4010_write8(uint8_t address, uint8_t data) {
    uint8_t buffer[2] = {address, data};
    I2C_IF_Write(vcnl_addr,
                (unsigned char *) &buffer,
                (unsigned char) 2,
                true);
}
