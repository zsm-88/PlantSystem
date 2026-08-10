#include "adc.h"

ADC_HandleTypeDef hadc;

void ADCx_Init(void)
{
  __HAL_RCC_ADC1_CLK_ENABLE();

  hadc.Instance = ADC1;//ADC1通道
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;//数据右对齐
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;//非扫描模式
  hadc.Init.ContinuousConvMode = DISABLE;//单次转换
  hadc.Init.NbrOfConversion = 1;//总通道数
  hadc.Init.DiscontinuousConvMode = DISABLE;//禁用间断转换模式
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;//软件触发

  HAL_ADC_Init(&hadc);
    
  //进行ADC校准
  HAL_ADCEx_Calibration_Start(&hadc); // HAL库自动完成校准过程
}

uint16_t ADC_GetValue(uint32_t Channel)
{
  ADC_ChannelConfTypeDef sConfig = {0};
  sConfig.Channel = Channel;
  sConfig.Rank = ADC_REGULAR_RANK_1;//规则组的第1个转换通道
  sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;

  HAL_ADC_ConfigChannel(&hadc, &sConfig);
  HAL_ADC_Start(&hadc);
  HAL_ADC_PollForConversion(&hadc, HAL_MAX_DELAY);
  return HAL_ADC_GetValue(&hadc);
}

