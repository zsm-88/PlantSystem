#ifndef __TS_H
#define __TS_H

#include "sys.h"
#include <stdio.h>
#include "adc.h"
#include "delay.h"

#define TS_READ_TIMES       10
#define TS_GPIO_PORT        GPIOA
#define TS_GPIO_PIN         GPIO_PIN_6
#define ADC_CHANNEL         ADC_CHANNEL_6

void TS_Init(void);
uint16_t TS_GetData(void);

#endif

