/*
    Copyright (C) 2014 John Convertino

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
*/

#define CMD_READ_TEMP 		0xAA
#define CMD_WRITE_H_TEMP	0x01
#define CMD_WRITE_L_TEMP	0x02
#define CMD_READ_H_TEMP		0xA1
#define CMD_READ_L_TEMP		0xA2
#define CMD_READ_COUNTER	0xA0
#define CMD_READ_SLOPE		0xA9
#define CMD_START_CONV		0xEE
#define CMD_STOP_CONV		0x22
#define CMD_WRITE_CONFIG	0x0C
#define CMD_READ_CONFIG		0xAC

#define DUTY_PERIOD		1
#define WRITE_DELAY		20
#define COMMAND_LENGTH		8
#define NEGATIVE_MASK		0xFF00

#include <util/delay.h>
#include <avr/interrupt.h>
#include "ds1620.h"

struct
{
  volatile uint8_t *PORT;
  volatile uint8_t *DDR;
  volatile uint8_t *PIN;
  uint8_t dataPin;
  uint8_t clockPin;
  uint8_t resetPin;
} g_ds1620;

//helper functions
void writeData(uint8_t command, int16_t data, uint8_t amount);
int16_t readData(uint8_t command, uint8_t amount);
int16_t convertTemp(int16_t temp);

void initDS1620(volatile uint8_t *port, uint8_t const dataPin, uint8_t const clockPin, uint8_t const resetPin)
{
  uint8_t tmpSREG = 0;

  tmpSREG = SREG;
  cli();

  g_ds1620.PORT = port;
  g_ds1620.DDR = port - 1;
  g_ds1620.PIN = port - 2;

  g_ds1620.dataPin = dataPin;
  g_ds1620.clockPin = clockPin;
  g_ds1620.resetPin = resetPin;

  *g_ds1620.DDR |= (1 << g_ds1620.clockPin) | (1 << g_ds1620.dataPin) | (1 << g_ds1620.resetPin);

  *g_ds1620.PORT |= (1 << g_ds1620.clockPin) | (1 << g_ds1620.dataPin);

  *g_ds1620.PORT &= ~(1 << g_ds1620.resetPin);

  SREG = tmpSREG;
}

void startConversion()
{
  uint8_t tmpSREG = 0;

  tmpSREG = SREG;
  cli();

  readData(CMD_START_CONV, 0);

  if(readConfig() & (1 << DS_CFG_BIT_1SHOT))
  {
    while(!(readConfig() & (1 << DS_CFG_BIT_DONE)));
  }

  SREG = tmpSREG;
}

int16_t readTempC()
{
  return convertTemp(readData(CMD_READ_TEMP, 9));
}

int16_t readTempF()
{
  uint8_t tmpSREG = 0;

  tmpSREG = SREG;
  cli();

  int16_t temp = readTempC();

  temp = (9 * temp + 160) / 5;

  SREG = tmpSREG;

  return temp;
}

int16_t readTempRAW()
{
  return readData(CMD_READ_TEMP, 9);
}

int16_t readTempHighC()
{
  return convertTemp(readData(CMD_READ_H_TEMP, 9));
}

int16_t readTempLowC()
{
  return convertTemp(readData(CMD_READ_L_TEMP, 9));
}

int16_t readCounter()
{
  return readData(CMD_READ_COUNTER, 8);
}

int16_t readSlope()
{
  return readData(CMD_READ_SLOPE, 8);
}

uint8_t readConfig()
{
  return readData(CMD_READ_CONFIG, 8);
}

void writeConfig(uint8_t const config)
{
  uint8_t tmpSREG = 0;

  tmpSREG = SREG;
  cli();

  writeData(CMD_WRITE_CONFIG, config, 8);
  _delay_ms(WRITE_DELAY);

  SREG = tmpSREG;
}

void writeTempHighC(int16_t temp)
{
  uint8_t tmpSREG = 0;

  tmpSREG = SREG;
  cli();

  temp <<= 1;
  writeData(CMD_WRITE_H_TEMP, temp, 9);

  SREG = tmpSREG;
}

void writeTempLowC(int16_t temp)
{
  uint8_t tmpSREG = 0;

  tmpSREG = SREG;
  cli();

  temp <<= 1;
  writeData(CMD_WRITE_L_TEMP, temp, 9);

  SREG = tmpSREG;
}

//helper functions
/**
 * Write data to a register of the chip
 *
 * \param command Register address to write to
 * \param data Data to write to the register (command data).
 * \param amount Number of bits the data contains (8 or 9).
 */
void writeData(uint8_t command, int16_t data, uint8_t amount)
{
  int index;
  uint8_t tmpSREG = 0;

  tmpSREG = SREG;
  cli();

  *g_ds1620.PORT |= (1 << g_ds1620.resetPin);

  _delay_us(DUTY_PERIOD);

  *g_ds1620.PORT |= (1 << g_ds1620.clockPin);

  *g_ds1620.DDR |= (1 << g_ds1620.dataPin);

  for(index = 0; index < COMMAND_LENGTH; index++)
  {
    *g_ds1620.PORT |= ((command & (1 << index)) >> index) << g_ds1620.dataPin;
    *g_ds1620.PORT &= (((command & (1 << index)) >> index) << g_ds1620.dataPin) | ~(1 << g_ds1620.dataPin);

    *g_ds1620.PORT &= ~(1 << g_ds1620.clockPin);
    _delay_us(DUTY_PERIOD);
    *g_ds1620.PORT |= 1 << g_ds1620.clockPin;
    _delay_us(DUTY_PERIOD);
  }

  for(index = 0; index < amount; index++)
  {
    *g_ds1620.PORT |= ((data & (1 << index)) >> index) << g_ds1620.dataPin;
    *g_ds1620.PORT &= (((data & (1 << index)) >> index) << g_ds1620.dataPin) | ~(1 << g_ds1620.dataPin);

    *g_ds1620.PORT &= ~(1 << g_ds1620.clockPin);
    _delay_us(DUTY_PERIOD);
    *g_ds1620.PORT |= 1 << g_ds1620.clockPin;
    _delay_us(DUTY_PERIOD);
  }

  if(amount != 0)
  {
    *g_ds1620.PORT &= ~(1 << g_ds1620.resetPin);
  }

  SREG = tmpSREG;
}

/**
 * Read data from a register of the chip
 *
 * \param command Register address to read from.
 * \param amount Number of bits the data contains (8 or 9).
 * \return 16 bits worth of data.
 */
int16_t readData(uint8_t command, uint8_t amount)
{
  int index;
  int16_t data = 0;
  uint8_t tmpSREG = 0;

  tmpSREG = SREG;
  cli();

  *g_ds1620.PORT |= (1 << g_ds1620.clockPin);

  writeData(command, 0, 0);

  *g_ds1620.DDR &= ~(1 << g_ds1620.dataPin);
  *g_ds1620.PORT &= ~(1 << g_ds1620.dataPin);

  for(index = 0; index < amount; index++)
  {
    *g_ds1620.PORT &= ~(1 << g_ds1620.clockPin);

    _delay_us(DUTY_PERIOD);

    *g_ds1620.PORT |= 1 << g_ds1620.clockPin;

    data |= (((*g_ds1620.PIN >> g_ds1620.dataPin) & 1) << index);

    _delay_us(DUTY_PERIOD);
  }

  *g_ds1620.PORT &= ~(1 << g_ds1620.resetPin);

  SREG = tmpSREG;

  return data;
}

/**
 * Convert temp to whole decimal
 *
 * \param temp DS1620 coded temp
 * \return DS1620 temp converted to decimal
 */
int16_t convertTemp(int16_t temp)
{
  uint8_t tmpSREG = 0;

  tmpSREG = SREG;
  cli();

  if(temp & NEGATIVE_MASK)
  {
    temp |= NEGATIVE_MASK;
  }

  temp >>= 1;

  SREG = tmpSREG;

  return temp;
}
