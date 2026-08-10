#include "ts.h"

void TS_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = TS_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(TS_GPIO_PORT, &GPIO_InitStruct);

  ADCx_Init();
}

uint16_t TS_ADC_Read(void)
{
  return ADC_GetValue(ADC_CHANNEL);
}

uint16_t TS_GetData(void)
{
  uint32_t tempData = 0;
  for (uint8_t i = 0; i < TS_READ_TIMES; i++)
  {
    tempData += TS_ADC_Read();
    delay_ms(5);
  }
  tempData /= TS_READ_TIMES;
  return 100 - (float)tempData / 40.96f;
}


