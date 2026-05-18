#ifndef __ADC_H
#define __ADC_H

#include "sys.h"
#include <stdio.h>

extern ADC_HandleTypeDef hadc;

void ADCx_Init(void);
uint16_t ADC_GetValue(uint32_t Channel);

#endif

