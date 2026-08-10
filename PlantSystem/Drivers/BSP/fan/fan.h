#ifndef __FAN_H
#define __FAN_H

#include "sys.h"
//PA2
#define FAN_PIN            			 GPIO_PIN_14        				 	// FAN 引脚      
#define FAN_PORT           			 GPIOC                 			 	// FAN GPIO端口     
#define __HAL_RCC_FAN_CLK_ENABLE        __HAL_RCC_GPIOC_CLK_ENABLE() 	// FAN GPIO端口时钟
  
						
#define FAN_ON 		HAL_GPIO_WritePin(FAN_PORT,FAN_PIN,GPIO_PIN_SET)  	// 此时IO口是高电平，风扇开
#define FAN_OFF		HAL_GPIO_WritePin(FAN_PORT,FAN_PIN,GPIO_PIN_RESET)	// 此时IO口是低电平，风扇关			
		
/*---------------------------------------- 函数声明 ------------------------------------*/

void FAN_Init(void);

#endif //__FAN_H


