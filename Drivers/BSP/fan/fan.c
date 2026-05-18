#include "fan.h"  

void FAN_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_FAN_CLK_ENABLE;		// 初始化FAN GPIO时钟	


	FAN_OFF;		// 关风扇

	GPIO_InitStruct.Pin 	= FAN_PIN;					// FAN引脚
	GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;	// 推挽输出模式
	GPIO_InitStruct.Pull 	= GPIO_NOPULL;				// 不上下拉
	GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_LOW;	// 低速模式
	HAL_GPIO_Init(FAN_PORT, &GPIO_InitStruct);
 
    
}


