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

#ifndef ds1620
#define ds1620

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

struct s_ds1620
{
  volatile uint8_t *PORT;
  volatile uint8_t *DDR;
  volatile uint8_t *PIN;
  uint8_t dataPin;
  uint8_t clockPin;
  uint8_t resetPin;
};

/**
 * initDS1620 sets up DS1620 for single conversion mode.
 * 
 * \param port The data port to use on the atmega328p for all 3 i/o pins.
 * \param dataPin The bit(i/o pin) number to use for data i/o
 * \param clockPin The bit(i/o pin) number to use for clock generation.
 * \param resetPin The bit(i/o pin) number to use for reset generation.
 * 
 * \return Struct that contains info about the sensor.
 */
struct s_ds1620 *initDS1620(volatile uint8_t *port, uint8_t const dataPin, uint8_t const clockPin, uint8_t const resetPin);

/**
 * 
 * Start Temp Conversion
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * 
 */
void startDS1620Conv(struct s_ds1620 *p_ds1620);

/**
 * Read the temp as a conversion in celcius.
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * 
 * \return Temprature, negative or positive.
 */
int16_t readDS1620TempC(struct s_ds1620 *p_ds1620);

/**
 * Read the temp as a conversion in F.
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * 
 * \return Temprature, negative or positive.
 */
int16_t readDS1620TempF(struct s_ds1620 *p_ds1620);

/**
 * Read raw temp data, no manipulation.
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * 
 * \return Temprature, raw data
 */
int16_t readDS1620TempRAW(struct s_ds1620 *p_ds1620);

/**
 * Read the high temp, this is the temp set for the thermostate in C.
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * 
 * \return Temprature, negative or positive.
 */
int16_t readDS1620TempHighC(struct s_ds1620 *p_ds1620);

/**
 * Read the low temp, this is the temp set for the thermostate in C.
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * 
 * \return Temprature, negative or positive.
 */
int16_t readDS1620TempLowC(struct s_ds1620 *p_ds1620);

/**
 * Read counter, can be used for more accurate temps.
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * 
 * \return number of counts (see datasheet).
 */
int16_t readDS1620Counter(struct s_ds1620 *p_ds1620);

/**
 * Read slope, can be used for more accurate temps.
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * 
 * \return slope number.
 */
int16_t readDS1620Slope(struct s_ds1620 *p_ds1620);

/**
 * Read a config from the chip
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * 
 * \returns chip config bits
 */
uint8_t readDS1620Config(struct s_ds1620 *p_ds1620);

/**
 * Write a config to the chip to change its operation mode.
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * \param config 
 */
void writeDS1620Config(struct s_ds1620 *p_ds1620, uint8_t const config);

/**
 * Write the temp to be used for high temp on chip.
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * \param temp Temprature in celcius.
 */
void writeDS1620TempHighC(struct s_ds1620 *p_ds1620, int16_t temp);

/**
 * Write the temp to be used for low temp on chip.
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * \param temp Temprature in celcius.
 */
void writeDS1620TempLowC(struct s_ds1620 *p_ds1620, int16_t temp);

/**
 * Free struct that contains DS1620 info
 * 
 * \param p_ds1620 struct that contains the sensor needed.
 * 
 */
void freeDS1620(struct s_ds1620 *p_ds1620);

#endif

