# DS1620 library

This library allows usage of the DS1620 chip. The DS1620 is a 3 wire serial interface temprature chip and thermostate.

* Example Code:

```c
#include <inttypes.h>
#include <avr/common.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ds1620.h"

void main(void)
{
  initDS1620(&PORTB, PORTB2, PORTB1, PORTB0);

  writeDS1620Config((1 << DS_CFG_BIT_CPU) | (1 << DS_CFG_BIT_1SHOT));
	
  startDS1620Conv();

  for(;;)
  {
    int16_t temp = 0;

    temp = readDS1620TempF();
  }
}
```
