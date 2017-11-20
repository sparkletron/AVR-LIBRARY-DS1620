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
#include <stdlib.h>
#include "ds1620.h"

//helper functions
void writeData(struct s_ds1620 *p_ds1620, uint8_t command, int16_t data, uint8_t amount);
int16_t readData(struct s_ds1620 *p_ds1620, uint8_t command, uint8_t amount);
int16_t convertTemp(int16_t temp);

struct s_ds1620 *initDS1620(volatile uint8_t *port, uint8_t const dataPin, uint8_t const clockPin, uint8_t const resetPin)
{
  uint8_t tmpSREG = 0;
	struct s_ds1620 *p_temp = NULL;
	
  tmpSREG = SREG;
  cli();

	p_temp = malloc(sizeof(struct s_ds1620));
	
	if(p_temp == NULL) return NULL;
	
  p_temp->PORT = port;
  p_temp->DDR = port - 1;
  p_temp->PIN = port - 2;

  p_temp->dataPin = dataPin;
  p_temp->clockPin = clockPin;
  p_temp->resetPin = resetPin;

  *p_temp->DDR |= (1 << p_temp->clockPin) | (1 << p_temp->dataPin) | (1 << p_temp->resetPin);

  *p_temp->PORT |= (1 << p_temp->clockPin) | (1 << p_temp->dataPin);

  *p_temp->PORT &= ~(1 << p_temp->resetPin);

  SREG = tmpSREG;
	
	sei();
	
	return p_temp;
}

void startDS1620Conv(struct s_ds1620 *p_ds1620)
{
  uint8_t tmpSREG = 0;
	
	if(p_ds1620 == NULL) return;

  tmpSREG = SREG;
  cli();

  readData(p_ds1620, CMD_START_CONV, 0);

  if(readDS1620Config(p_ds1620) & (1 << DS_CFG_BIT_1SHOT))
  {
    while(!(readDS1620Config(p_ds1620) & (1 << DS_CFG_BIT_DONE)));
  }

  SREG = tmpSREG;
}

int16_t readDS1620TempC(struct s_ds1620 *p_ds1620)
{
	if(p_ds1620 == NULL) return 0;
	
  return convertTemp(readData(p_ds1620, CMD_READ_TEMP, 9));
}

int16_t readDS1620TempF(struct s_ds1620 *p_ds1620)
{
  uint8_t tmpSREG = 0;

	if(p_ds1620 == NULL) return 0;
	
  tmpSREG = SREG;
  cli();

  int16_t temp = readDS1620TempC(p_ds1620);

  temp = (9 * temp + 160) / 5;

  SREG = tmpSREG;

  return temp;
}

int16_t readDS1620TempRAW(struct s_ds1620 *p_ds1620)
{
	if(p_ds1620 == NULL) return 0;
	
  return readData(p_ds1620, CMD_READ_TEMP, 9);
}

int16_t readDS1620TempHighC(struct s_ds1620 *p_ds1620)
{
	if(p_ds1620 == NULL) return 0;
	
  return convertTemp(readData(p_ds1620, CMD_READ_H_TEMP, 9));
}

int16_t readDS1620TempLowC(struct s_ds1620 *p_ds1620)
{
	if(p_ds1620 == NULL) return 0;
	
  return convertTemp(readData(p_ds1620, CMD_READ_L_TEMP, 9));
}

int16_t readDS1620Counter(struct s_ds1620 *p_ds1620)
{
	if(p_ds1620 == NULL) return 0;
	
  return readData(p_ds1620, CMD_READ_COUNTER, 8);
}

int16_t readDS1620Slope(struct s_ds1620 *p_ds1620)
{
	if(p_ds1620 == NULL) return 0;
	
  return readData(p_ds1620, CMD_READ_SLOPE, 8);
}

uint8_t readDS1620Config(struct s_ds1620 *p_ds1620)
{
	if(p_ds1620 == NULL) return 0;
	
  return readData(p_ds1620, CMD_READ_CONFIG, 8);
}

void writeDS1620Config(struct s_ds1620 *p_ds1620, uint8_t const config)
{
  uint8_t tmpSREG = 0;

	if(p_ds1620 == NULL) return;
	
  tmpSREG = SREG;
  cli();

  writeData(p_ds1620, CMD_WRITE_CONFIG, config, 8);
  _delay_ms(WRITE_DELAY);

  SREG = tmpSREG;
}

void writeDS1620TempHighC(struct s_ds1620 *p_ds1620, int16_t temp)
{
  uint8_t tmpSREG = 0;

	if(p_ds1620 == NULL) return;
	
  tmpSREG = SREG;
  cli();

  temp <<= 1;
  writeData(p_ds1620, CMD_WRITE_H_TEMP, temp, 9);

  SREG = tmpSREG;
}

void writeDS1620TempLowC(struct s_ds1620 *p_ds1620, int16_t temp)
{
  uint8_t tmpSREG = 0;

	if(p_ds1620 == NULL) return;
	
  tmpSREG = SREG;
  cli();

  temp <<= 1;
  writeData(p_ds1620, CMD_WRITE_L_TEMP, temp, 9);

  SREG = tmpSREG;
}

void freeDS1620(struct s_ds1620 *p_ds1620)
{
	if(p_ds1620 == NULL) return;
	
	free(p_ds1620);
}

//helper functions
/**
 * Write data to a register of the chip
 *
 * \param command Register address to write to
 * \param data Data to write to the register (command data).
 * \param amount Number of bits the data contains (8 or 9).
 */
void writeData(struct s_ds1620 *p_ds1620, uint8_t command, int16_t data, uint8_t amount)
{
  int index;
  uint8_t tmpSREG = 0;
	
	if(p_ds1620 == NULL) return;

  tmpSREG = SREG;
  cli();

  *p_ds1620->PORT |= (1 << p_ds1620->resetPin);

  _delay_us(DUTY_PERIOD);

  *p_ds1620->PORT |= (1 << p_ds1620->clockPin);

  *p_ds1620->DDR |= (1 << p_ds1620->dataPin);

  for(index = 0; index < COMMAND_LENGTH; index++)
  {
    *p_ds1620->PORT |= ((command & (1 << index)) >> index) << p_ds1620->dataPin;
    *p_ds1620->PORT &= (((command & (1 << index)) >> index) << p_ds1620->dataPin) | ~(1 << p_ds1620->dataPin);

    *p_ds1620->PORT &= ~(1 << p_ds1620->clockPin);
    _delay_us(DUTY_PERIOD);
    *p_ds1620->PORT |= 1 << p_ds1620->clockPin;
    _delay_us(DUTY_PERIOD);
  }

  for(index = 0; index < amount; index++)
  {
    *p_ds1620->PORT |= ((data & (1 << index)) >> index) << p_ds1620->dataPin;
    *p_ds1620->PORT &= (((data & (1 << index)) >> index) << p_ds1620->dataPin) | ~(1 << p_ds1620->dataPin);

    *p_ds1620->PORT &= ~(1 << p_ds1620->clockPin);
    _delay_us(DUTY_PERIOD);
    *p_ds1620->PORT |= 1 << p_ds1620->clockPin;
    _delay_us(DUTY_PERIOD);
  }

  if(amount != 0)
  {
    *p_ds1620->PORT &= ~(1 << p_ds1620->resetPin);
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
int16_t readData(struct s_ds1620 *p_ds1620, uint8_t command, uint8_t amount)
{
  int index;
  int16_t data = 0;
  uint8_t tmpSREG = 0;
	
	if(p_ds1620 == NULL) return 0;

  tmpSREG = SREG;
  cli();

  *p_ds1620->PORT |= (1 << p_ds1620->clockPin);

  writeData(p_ds1620, command, 0, 0);

  *p_ds1620->DDR &= ~(1 << p_ds1620->dataPin);
  *p_ds1620->PORT &= ~(1 << p_ds1620->dataPin);

  for(index = 0; index < amount; index++)
  {
    *p_ds1620->PORT &= ~(1 << p_ds1620->clockPin);

    _delay_us(DUTY_PERIOD);

    *p_ds1620->PORT |= 1 << p_ds1620->clockPin;

    data |= (((*p_ds1620->PIN >> p_ds1620->dataPin) & 1) << index);

    _delay_us(DUTY_PERIOD);
  }

  *p_ds1620->PORT &= ~(1 << p_ds1620->resetPin);

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
