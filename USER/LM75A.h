#ifndef __LM75A_H__
#define __LM75A_H__
#include <includes.h>
#include "lpc17xx_gpio.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
void initLM(void);
int16_t getLMWet(void);

#endif
