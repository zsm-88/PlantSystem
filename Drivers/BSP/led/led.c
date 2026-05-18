#include "led.h"
#include "sys.h"

//初始化GPIO函数
void led_init(void)
{
    GPIO_InitTypeDef gpio_initstruct;
    //打开时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();                           // 使能GPIOA时钟
    
    //调用GPIO初始化函数
    gpio_initstruct.Pin = GPIO_PIN_8;          // LED对应的引脚
    gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP;             // 推挽输出
    gpio_initstruct.Pull = GPIO_NOPULL;                     //不上下拉
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;           // 高速
    HAL_GPIO_Init(GPIOA, &gpio_initstruct);
    //关闭LED
    led1_off();
}

//点亮LED的函数
void led1_on(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);   
}

//熄灭LED的函数
void led1_off(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);     
}

//翻转LED状态的函数
void led1_toggle(void)
{
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
}


