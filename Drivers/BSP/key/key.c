#include "key.h"

// 初始化按键引脚（上拉输入，按下为低）
void KEY_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};			//定义结构体

    __HAL_RCC_KEY_CLK_ENABLE();   //  必须加括号

    GPIO_InitStruct.Pin  = S1 | S2 | S3 | S4;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLUP; // 按下接地
    HAL_GPIO_Init(KEY_PORT, &GPIO_InitStruct);
}

/**
  * 非阻塞按键扫描函数（带消抖）
  * 返回：KEY_ON 一次按下事件；KEY_OFF 无事件
  */

//默认返回未按下

uint8_t KEY_Scan(uint16_t S)
{
    static uint8_t key_last = 1;   // 上一次状态
    uint8_t key_now = HAL_GPIO_ReadPin(KEY_PORT, S);

    if (key_last == 1 && key_now == 0)   // 检测到下降沿（按下）
    {
        delay_ms(10); // 简单消抖
        if (HAL_GPIO_ReadPin(KEY_PORT, S) == 0)
        {
            key_last = 0;
            return KEY_ON;
        }
    }
    else if (key_last == 0 && key_now == 1)
    {
        key_last = 1; // 松手
    }

    return KEY_OFF;
}
