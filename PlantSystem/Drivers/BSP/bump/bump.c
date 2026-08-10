#include "bump.h"  

void BUMP_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_BUMP_CLK_ENABLE;		// 初始化BUMP GPIO时钟	


	BUMP_OFF;		// 关水泵

	GPIO_InitStruct.Pin 	= BUMP_PIN;					// BUMP引脚
	GPIO_InitStruct.Mode 	= GPIO_MODE_OUTPUT_PP;	// 推挽输出模式
	GPIO_InitStruct.Pull 	= GPIO_NOPULL;				// 不上下拉
	GPIO_InitStruct.Speed 	= GPIO_SPEED_FREQ_LOW;	// 低速模式
	HAL_GPIO_Init(BUMP_PORT, &GPIO_InitStruct);
 
}


