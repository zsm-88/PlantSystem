/*
	TFT-LCD-7735S Port Distribute

	SCL		GPIOA_PIN5
	SDA   GPIOA_PIN7
	RST   GPIOB_PIN0
	DC    GPIOB_PIN1
	CS    GPIOA_PIN4
	BLK   GPIOB_PIN10
	
*/


#ifndef __LCD_H
#define __LCD_H
#include "sys.h"
#include <stdio.h>
#include "delay.h"


#define		RED  						0xf800
#define		GREEN						0x07e0
#define		BLUE 						0x001f
#define		WHITE						0xffff
#define		BLACK						0x0000
#define		YELLOW					0xFFE0
#define		GRAY0 					0xEF7D   			//灰色0 3165 00110 001011 00101
#define		GRAY1 					0x8410      	//灰色1      00000 000000 00000
#define		GRAY2 					0x4208      	//灰色2  1111111111011111

#define 	LCD_CTRLA   	 	GPIOA					
#define 	LCD_CTRLB   	 	GPIOB					

#define		LCD_SCL        	GPIO_PIN_5		
#define		LCD_SDA        	GPIO_PIN_7		
#define		LCD_CS        	GPIO_PIN_4		
													
#define		LCD_LED        	GPIO_PIN_10		
#define		LCD_RS         	GPIO_PIN_1		
#define		LCD_RST     		GPIO_PIN_0		

//液晶控制口置1操作语句宏定义
#define LCD_SCL_SET()					HAL_GPIO_WritePin(LCD_CTRLA,LCD_SCL,GPIO_PIN_SET)
#define LCD_SDA_SET()					HAL_GPIO_WritePin(LCD_CTRLA,LCD_SDA,GPIO_PIN_SET)
#define LCD_CS_SET()					HAL_GPIO_WritePin(LCD_CTRLA,LCD_CS,GPIO_PIN_SET)
#define LCD_LED_SET()					HAL_GPIO_WritePin(LCD_CTRLB,LCD_LED,GPIO_PIN_SET)
#define LCD_RS_SET()					HAL_GPIO_WritePin(LCD_CTRLB,LCD_RS,GPIO_PIN_SET)
#define LCD_RST_SET()					HAL_GPIO_WritePin(LCD_CTRLB,LCD_RST,GPIO_PIN_SET)

//液晶控制口置0操作语句宏定义
#define LCD_SCL_RESET()				HAL_GPIO_WritePin(LCD_CTRLA,LCD_SCL,GPIO_PIN_RESET)
#define LCD_SDA_RESET()				HAL_GPIO_WritePin(LCD_CTRLA,LCD_SDA,GPIO_PIN_RESET)
#define LCD_CS_RESET()				HAL_GPIO_WritePin(LCD_CTRLA,LCD_CS,GPIO_PIN_RESET)
#define LCD_LED_RESET()				HAL_GPIO_WritePin(LCD_CTRLB,LCD_LED,GPIO_PIN_RESET)
#define LCD_RS_RESET()				HAL_GPIO_WritePin(LCD_CTRLB,LCD_RS,GPIO_PIN_RESET)
#define LCD_RST_RESET()				HAL_GPIO_WritePin(LCD_CTRLB,LCD_RST,GPIO_PIN_RESET)


#define LCD_DATAOUT(x) LCD_DATA->ODR=x; //数据输出
#define LCD_DATAIN     LCD_DATA->IDR;   //数据输入

#define LCD_WR_DATA(data){\
LCD_RS_SET;\
LCD_CS_CLR;\
LCD_DATAOUT(data);\
LCD_WR_CLR;\
LCD_WR_SET;\
LCD_CS_SET;\
} 

#define X_MAX_PIXEL	        128
#define Y_MAX_PIXEL	        160



void LCD_GPIO_Init(void);
void Lcd_WriteCmd(uint8_t cmd);
void Lcd_WriteData(uint8_t Data);
void Lcd_WriteReg(uint8_t Index,uint8_t Data);

void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(uint16_t Color);
void Lcd_SetXY(uint16_t x,uint16_t y);
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);
uint16_t Lcd_ReadPoint(uint16_t x,uint16_t y);
void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end);
void LCD_WriteData_16Bit(uint16_t Data);
void Lcd_Fill(uint16_t x_star,uint16_t y_star,uint16_t x_end,uint16_t y_end,uint16_t color);
void Lcd_test(void);
void LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint16_t color, uint16_t bg);
void LCD_ShowString(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t bg);

void LCD_Show_Single_Chinese(uint16_t x, uint16_t y, uint8_t Index, uint16_t color, uint16_t bg);
void LCD_Show_Single_Chinese2(uint16_t x, uint16_t y, uint8_t Index, uint16_t color, uint16_t bg);
void LCD_Show_Single_Chinese3(uint16_t x, uint16_t y, uint8_t Index, uint16_t color, uint16_t bg);
void LCD_Show_Chinese_String(uint16_t x, uint16_t y, const uint8_t *index_array, uint8_t count, uint16_t color, uint16_t bg);
void LCD_Show_Chinese_String2(uint16_t x, uint16_t y, const uint8_t *index_array, uint8_t count, uint16_t color, uint16_t bg);
void LCD_Show_Chinese_String3(uint16_t x, uint16_t y, const uint8_t *index_array, uint8_t count, uint16_t color, uint16_t bg);


#endif
