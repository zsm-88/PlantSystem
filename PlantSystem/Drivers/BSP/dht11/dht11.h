#ifndef __DHT11_H__
#define __DHT11_H__

#include "stdio.h"
#include "stm32f1xx.h"
 
#define DHT11_PORT              GPIOB
#define DHT11_PIN               GPIO_PIN_9
#define DHT11_CLK_ENABLE()      __HAL_RCC_GPIOB_CLK_ENABLE()

#define DHT11_DQ_OUT(x)     do{ x ? \
                                HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_SET) : \
                                HAL_GPIO_WritePin(DHT11_PORT, DHT11_PIN, GPIO_PIN_RESET); \
                            }while(0)                                                /* 数据端口输出 */
#define DHT11_DQ_IN         HAL_GPIO_ReadPin(DHT11_PORT, DHT11_PIN)                 /* 数据端口输入 */

void dht11_read(void);
    
#endif
