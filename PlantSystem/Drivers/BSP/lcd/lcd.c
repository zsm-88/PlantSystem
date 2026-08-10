#include "lcd.h"
#include "font.h"

void LCD_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pin = LCD_SCL | LCD_SDA | LCD_CS ; 
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(LCD_CTRLA,&GPIO_InitStructure);
	
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Pin = LCD_LED | LCD_RS | LCD_RST ; 
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	
	HAL_GPIO_Init(LCD_CTRLB,&GPIO_InitStructure);
}

//向SPI总线传输一个8位数据
void  SPI_WriteData(uint8_t Data)
{
	uint8_t i=0;
	for(i=8;i>0;i--)
	{
			if(Data&0x80)	
			LCD_SDA_SET(); //输出数据
			else LCD_SDA_RESET();
	   
      LCD_SCL_RESET();       
      LCD_SCL_SET();
      Data<<=1; 
	}
}

//向液晶屏写一个8位指令
void Lcd_WriteCmd(uint8_t cmd)
{
	//SPI 写命令时序开始
	LCD_CS_RESET();
  LCD_RS_RESET();
	SPI_WriteData(cmd);
  LCD_CS_SET();
}

//向液晶屏写一个8位数据
void Lcd_WriteData(uint8_t Data)
{
	LCD_CS_RESET();
  LCD_RS_SET();
  SPI_WriteData(Data);
  LCD_CS_SET(); 
}

//向液晶屏写一个16位数据
void LCD_WriteData_16Bit(uint16_t Data)
{
   LCD_CS_RESET();
   LCD_RS_SET();
	 SPI_WriteData(Data>>8); 	//写入高8位数据
	 SPI_WriteData(Data); 			//写入低8位数据
   LCD_CS_SET(); 
}

//向 LCD 显示屏的寄存器写入数据
void Lcd_WriteReg(uint8_t Index,uint8_t Data)
{
	Lcd_WriteCmd(Index);
  Lcd_WriteData(Data);
}


void Lcd_Reset(void)
{
	LCD_RST_RESET();
	delay_ms(100);
	LCD_RST_SET();
	delay_ms(50);
}

void Lcd_Init(void)
{	
	LCD_GPIO_Init();
	Lcd_Reset(); //Reset before LCD Init.

	//LCD Init For 1.44Inch LCD Panel with ST7735R.
	Lcd_WriteCmd(0x11);//Sleep exit 
	delay_ms (120);
		
	//ST7735R Frame Rate
	Lcd_WriteCmd(0xB1); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 

	Lcd_WriteCmd(0xB2); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 

	Lcd_WriteCmd(0xB3); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 
	Lcd_WriteData(0x01); 
	Lcd_WriteData(0x2C); 
	Lcd_WriteData(0x2D); 
	
	Lcd_WriteCmd(0xB4); //Column inversion 
	Lcd_WriteData(0x07); 
	
	//ST7735R Power Sequence
	Lcd_WriteCmd(0xC0); 
	Lcd_WriteData(0xA2); 
	Lcd_WriteData(0x02); 
	Lcd_WriteData(0x84); 
	Lcd_WriteCmd(0xC1); 
	Lcd_WriteData(0xC5); 

	Lcd_WriteCmd(0xC2); 
	Lcd_WriteData(0x0A); 
	Lcd_WriteData(0x00); 

	Lcd_WriteCmd(0xC3); 
	Lcd_WriteData(0x8A); 
	Lcd_WriteData(0x2A); 
	Lcd_WriteCmd(0xC4); 
	Lcd_WriteData(0x8A); 
	Lcd_WriteData(0xEE); 
	
	Lcd_WriteCmd(0xC5); //VCOM 
	Lcd_WriteData(0x0E); 
	
	Lcd_WriteCmd(0x36); //MX, MY, RGB mode 
	Lcd_WriteData(0xC0); 
	
	//ST7735R Gamma Sequence
	Lcd_WriteCmd(0xe0); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x1a); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x18); 
	Lcd_WriteData(0x2f); 
	Lcd_WriteData(0x28); 
	Lcd_WriteData(0x20); 
	Lcd_WriteData(0x22); 
	Lcd_WriteData(0x1f); 
	Lcd_WriteData(0x1b); 
	Lcd_WriteData(0x23); 
	Lcd_WriteData(0x37); 
	Lcd_WriteData(0x00); 	
	Lcd_WriteData(0x07); 
	Lcd_WriteData(0x02); 
	Lcd_WriteData(0x10); 

	Lcd_WriteCmd(0xe1); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x1b); 
	Lcd_WriteData(0x0f); 
	Lcd_WriteData(0x17); 
	Lcd_WriteData(0x33); 
	Lcd_WriteData(0x2c); 
	Lcd_WriteData(0x29); 
	Lcd_WriteData(0x2e); 
	Lcd_WriteData(0x30); 
	Lcd_WriteData(0x30); 
	Lcd_WriteData(0x39); 
	Lcd_WriteData(0x3f); 
	Lcd_WriteData(0x00); 
	Lcd_WriteData(0x07); 
	Lcd_WriteData(0x03); 
	Lcd_WriteData(0x10);  
	
	Lcd_WriteCmd(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x7f);

	Lcd_WriteCmd(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x00);
	Lcd_WriteData(0x9f);
	
	Lcd_WriteCmd(0xF0); //Enable test command  
	Lcd_WriteData(0x01); 
	Lcd_WriteCmd(0xF6); //Disable ram power save mode 
	Lcd_WriteData(0x00); 
	
	Lcd_WriteCmd(0x3A); //65k mode 
	Lcd_WriteData(0x05); 
	
	
	Lcd_WriteCmd(0x29);//Display on	 
    
    LCD_LED_SET();
	Lcd_Clear(WHITE);
}

/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
void Lcd_SetRegion(uint16_t x_start,uint16_t y_start,uint16_t x_end,uint16_t y_end)
{		
	Lcd_WriteCmd(0x2a);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_start);//Lcd_WriteData(x_start+2);
	Lcd_WriteData(0x00);
	Lcd_WriteData(x_end);

	Lcd_WriteCmd(0x2b);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_start);
	Lcd_WriteData(0x00);
	Lcd_WriteData(y_end);
	
	Lcd_WriteCmd(0x2c);

}

/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(uint16_t x,uint16_t y)
{
  	Lcd_SetRegion(x,y,x,y);
}


/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：无
返回值：无
*************************************************/
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	Lcd_SetRegion(x,y,x+1,y+1);
	LCD_WriteData_16Bit(Data);

}  

/*****************************************
 函数功能：读TFT某一点的颜色                          
 出口参数：color  点颜色值                                 
******************************************/
uint16_t Lcd_ReadPoint(uint16_t x,uint16_t y)
{
  uint16_t Data;
  Lcd_SetXY(x,y);

  //Lcd_ReadData();//丢掉无用字节
  //Data=Lcd_ReadData();
  Lcd_WriteData(Data);
  return Data;
}

/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Clear(uint16_t Color)               
{	
   uint16_t i,m;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
   Lcd_WriteCmd(0x2C);
   for(i=0;i<X_MAX_PIXEL;i++)
    for(m=0;m<Y_MAX_PIXEL;m++)
    {	
	  	LCD_WriteData_16Bit(Color);
    }   
}


void Lcd_Fill(uint16_t x_star,uint16_t y_star,uint16_t x_end,uint16_t y_end,uint16_t color)
{
	Lcd_SetRegion(x_star,y_star,x_end,y_end);
	
	uint8_t x_len = x_end - x_star + 1;
	uint8_t y_len = y_end - y_star + 1;
	Lcd_WriteCmd(0x2C);
	for(uint8_t i=0;i<x_len;i++)
    for(uint8_t m=0;m<y_len;m++)
    {	
	  	LCD_WriteData_16Bit(color);
    }  
	
}

void Lcd_test(void)
{
	Lcd_Clear(RED);
    delay_ms(500);
    Lcd_Clear(GREEN);
    delay_ms(500);
    Lcd_Clear(BLUE);
    delay_ms(500);
    Lcd_Clear(WHITE);
    delay_ms(500);
    Lcd_Clear(BLACK);
    delay_ms(500);
    
    // 测试绘制方块
    Lcd_Clear(WHITE);
    Lcd_Fill(10, 10, 50, 50, RED);
    delay_ms(500);
    Lcd_Fill(60, 60, 100, 100, GREEN);
    delay_ms(500);
    Lcd_Fill(80, 80, 120, 120, BLUE);
		Lcd_Clear(WHITE);
}


void LCD_ShowChar(uint16_t x, uint16_t y, char chr, uint16_t color, uint16_t bg)
{
    uint8_t i, j, temp;
    const uint8_t *p;

    p = ascii_font_8x16[chr - 0x20];

    for (i = 0; i < 16; i++)  // 每行16字节
    {
        temp = p[i];
        for (j = 0; j < 8; j++) // 每字节8像素
        {
            if (temp & (0x80 >> j))
                Gui_DrawPoint(x + j, y + i, color);
            else
                Gui_DrawPoint(x + j, y + i, bg);
        }
    }
}

void LCD_ShowString(uint16_t x, uint16_t y, char *str, uint16_t color, uint16_t bg)
{
    while (*str != '\0') // 遍历字符串直到结束符
    {
        LCD_ShowChar(x, y, *str, color, bg);
        x += 8;  // 每个字符宽度 8 像素（8x16 字模）

        // 自动换行
        if (x > X_MAX_PIXEL - 8) 
        {
            x = 0;
            y += 16; 
        }

        // 超出屏幕范围则退出
        if (y > Y_MAX_PIXEL - 16) 
            break;

        str++;
    }
}


void LCD_Show_Single_Chinese(uint16_t x, uint16_t y, uint8_t Index, uint16_t color, uint16_t bg)
{
    uint8_t i, j;
    uint8_t temp;
    const uint8_t *hz = chinese_font_16x16[Index*2];

    for (i = 0; i < 16; i++)  // 16行
    {
        temp = hz[i * 2];  // 每行1个字节 = 8像素
        for (j = 0; j < 8; j++)
        {
            if (temp & (0x80 >> j))
                Gui_DrawPoint(x + j, y + i, color);
            else
                Gui_DrawPoint(x + j, y + i, bg);
        }

        temp = hz[i * 2 + 1];  // 第二列
        for (j = 0; j < 8; j++)
        {
            if (temp & (0x80 >> j))
                Gui_DrawPoint(x + 8 + j, y + i, color);
            else
                Gui_DrawPoint(x + 8 + j, y + i, bg);
        }
    }
}

void LCD_Show_Chinese_String(uint16_t x, uint16_t y, const uint8_t *index_array, uint8_t count, uint16_t color, uint16_t bg)
{
    for (uint8_t k = 0; k < count; k++)
    {
       LCD_Show_Single_Chinese(x, y, index_array[k], color, bg);
        x += 16;  // 每个汉字占16像素宽度
    }
}

void LCD_Show_Single_Chinese2(uint16_t x, uint16_t y, uint8_t Index, uint16_t color, uint16_t bg)
{
    uint8_t i, j;
    uint8_t temp;
    const uint8_t *hz = chinese_Tile2_font_16x16[Index*2];

    for (i = 0; i < 16; i++)  // 16行
    {
        temp = hz[i * 2];  // 每行1个字节 = 8像素
        for (j = 0; j < 8; j++)
        {
            if (temp & (0x80 >> j))
                Gui_DrawPoint(x + j, y + i, color);
            else
                Gui_DrawPoint(x + j, y + i, bg);
        }

        temp = hz[i * 2 + 1];  // 第二列
        for (j = 0; j < 8; j++)
        {
            if (temp & (0x80 >> j))
                Gui_DrawPoint(x + 8 + j, y + i, color);
            else
                Gui_DrawPoint(x + 8 + j, y + i, bg);
        }
    }
}

void LCD_Show_Chinese_String2(uint16_t x, uint16_t y, const uint8_t *index_array, uint8_t count, uint16_t color, uint16_t bg)
{
    for (uint8_t k = 0; k < count; k++)
    {
       LCD_Show_Single_Chinese2(x, y, index_array[k], color, bg);
        x += 16;  // 每个汉字占16像素宽度
    }
}

void LCD_Show_Single_Chinese3(uint16_t x, uint16_t y, uint8_t Index, uint16_t color, uint16_t bg)
{
    uint8_t i, j;
    uint8_t temp;
    const uint8_t *hz = chinese_Tile3_font_16x16[Index*2];

    for (i = 0; i < 16; i++)  // 16行
    {
        temp = hz[i * 2];  // 每行1个字节 = 8像素
        for (j = 0; j < 8; j++)
        {
            if (temp & (0x80 >> j))
                Gui_DrawPoint(x + j, y + i, color);
            else
                Gui_DrawPoint(x + j, y + i, bg);
        }

        temp = hz[i * 2 + 1];  // 第二列
        for (j = 0; j < 8; j++)
        {
            if (temp & (0x80 >> j))
                Gui_DrawPoint(x + 8 + j, y + i, color);
            else
                Gui_DrawPoint(x + 8 + j, y + i, bg);
        }
    }
}

void LCD_Show_Chinese_String3(uint16_t x, uint16_t y, const uint8_t *index_array, uint8_t count, uint16_t color, uint16_t bg)
{
    for (uint8_t k = 0; k < count; k++)
    {
       LCD_Show_Single_Chinese3(x, y, index_array[k], color, bg);
        x += 16;  // 每个汉字占16像素宽度
    }
}




