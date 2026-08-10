#ifndef __LDR_H
#define __LDR_H

#include "sys.h"
#include <stdio.h>
#include "adc.h"
#include "math.h"
#include "delay.h"

#define LDR_READ_TIMES       10
#define LDR_GPIO_PORT        GPIOA
#define LDR_GPIO_PIN         GPIO_PIN_1
#define LDR_CHANNEL         ADC_CHANNEL_1

void LDR_Init(void);
uint16_t LDR_GetData(void);
uint16_t LDR_LuxData(void);

#endif //__LDR_H



