#include "ldr.h"

void LDR_Init(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();// 打开 ADC IO端口时钟
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = LDR_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;// 设置为模拟输入
  HAL_GPIO_Init(LDR_GPIO_PORT, &GPIO_InitStruct);// 初始化 ADC IO

  ADCx_Init();
}

uint16_t LDR_ADC_Read(void)
{
  return ADC_GetValue(LDR_CHANNEL);
}

//光敏电阻数据读取
uint16_t LDR_GetData(void)
{
  uint32_t tempData = 0;
  for (uint8_t i = 0; i < LDR_READ_TIMES; i++)
  {
    tempData += LDR_ADC_Read();
    delay_ms(5);
  }
  tempData /= LDR_READ_TIMES;
  return tempData;
}

//光照强度
uint16_t LDR_LuxData(void)
{
	float voltage = 0;	
	float R = 0;	
	uint16_t Lux = 0;
	voltage = LDR_GetData();
	voltage  = voltage / 4096 * 3.3f;
	
	R = voltage / (3.3f - voltage) * 10000;
		
	Lux = 40000 * pow(R, -0.6021);
	
	if (Lux > 999)
	{
		Lux = 999;
	}
	return Lux;
}

