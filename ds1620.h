/**
 * 
 * \brief Library for interfacing with the DS1620 temprature chip.
 *
 * \author John Convertino (electrobs@gmail.com)
 * \date 05/02/17
 * 
    Copyright (C) 2017 John Convertino

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * 
 * \version 0.1
 * 
 */

//config/status register bits... this could be backwards
#define DS_CFG_BIT_1SHOT	0
#define DS_CFG_BIT_CPU		1
#define DS_CFG_BIT_NA1		2
#define DS_CFG_BIT_NA2		3
#define DS_CFG_BIT_NVB		4
#define DS_CFG_BIT_TLF		5
#define DS_CFG_BIT_THF		6
#define DS_CFG_BIT_DONE		7

//INCLUDE AVR-GCC HEADERS HERE

#include <inttypes.h>

/**
 * initDS1620 sets up DS1620 for single conversion mode.
 * 
 * \param port The data port to use on the atmega328p for all 3 i/o pins.
 * \param dataPin The bit(i/o pin) number to use for data i/o
 * \param clockPin The bit(i/o pin) number to use for clock generation.
 * \param resetPin The bit(i/o pin) number to use for reset generation.
 */
void initDS1620(volatile uint8_t *port, uint8_t const dataPin, uint8_t const clockPin, uint8_t const resetPin);

/**
 * 
 * Start Temp Conversion
 * 
 */
void startConversion();

/**
 * Read the temp as a conversion in celcius.
 * 
 * \return Temprature, negative or positive.
 */
int16_t readTempC();

/**
 * Read the temp as a conversion in F.
 * 
 * \return Temprature, negative or positive.
 */
int16_t readTempF();

/**
 * Read raw temp data, no manipulation.
 * 
 * \return Temprature, raw data
 */
int16_t readTempRAW();

/**
 * Read the high temp, this is the temp set for the thermostate in C.
 * 
 * \return Temprature, negative or positive.
 */
int16_t readTempHighC();

/**
 * Read the low temp, this is the temp set for the thermostate in C.
 * 
 * \return Temprature, negative or positive.
 */
int16_t readTempLowC();

/**
 * Read counter, can be used for more accurate temps.
 * 
 * \return number of counts (see datasheet).
 */
int16_t readCounter();

/**
 * Read slope, can be used for more accurate temps.
 * 
 * \return slope number.
 */
int16_t readSlope();

/**
 * Read a config from the chip
 * 
 * \returns chip config bits
 */
uint8_t readConfig();

/**
 * Write a config to the chip to change its operation mode.
 * 
 * \param config 
 */
void writeConfig(uint8_t const config);

/**
 * Write the temp to be used for high temp on chip.
 * 
 * \param temp Temprature in celcius.
 */
void writeTempHighC(int16_t temp);

/**
 * Write the temp to be used for low temp on chip.
 * 
 * \param temp Temprature in celcius.
 */
void writeTempLowC(int16_t temp);

