#ifndef __KEY_H
#define __KEY_H

#include "sys.h"
#include "delay.h"

#define	KEY_ON	 (uint8_t)1		//按键按下
#define	KEY_OFF	 (uint8_t)0		//按键放开

/*------------------------------------------ IO口配置宏 ----------------------------------*/


#define 	S1           					GPIO_PIN_15        				 // KEY 引脚
#define 	S2           					GPIO_PIN_14        				 // KEY 引脚
#define 	S3           					GPIO_PIN_13        			 	 // KEY 引脚
#define 	S4           					GPIO_PIN_12        				 // KEY 引脚
#define	    KEY_PORT          					GPIOB                 			 // KEY GPIO端口     
#define 	__HAL_RCC_KEY_CLK_ENABLE()         __HAL_RCC_GPIOB_CLK_ENABLE()	 // KEY GPIO端口时钟


/*------------------------------------------ 函数声明 ------------------------------------*/

void KEY_Init(void);	//按键IO口初始化函数
uint8_t KEY_Scan(uint16_t S);   //按键扫描



#endif //__KEY_H











