#ifndef __BUMP_H
#define __BUMP_H

#include "sys.h"

//PA3
#define BUMP_PIN            			 GPIO_PIN_15        				 	// BUMP 引脚      
#define BUMP_PORT           			 GPIOC                 			 	// BUMP GPIO端口     
#define __HAL_RCC_BUMP_CLK_ENABLE        __HAL_RCC_GPIOC_CLK_ENABLE() 	// RELAY GPIO端口时钟
  
						
#define BUMP_ON 		HAL_GPIO_WritePin(BUMP_PORT,BUMP_PIN,GPIO_PIN_SET)  	// 此时IO口是高电平，开
#define BUMP_OFF		HAL_GPIO_WritePin(BUMP_PORT,BUMP_PIN,GPIO_PIN_RESET)	// 此时IO口是低电平，关			
		
/*---------------------------------------- 函数声明 ------------------------------------*/

void BUMP_Init(void);

#endif //__BUMP_H


