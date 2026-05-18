#ifndef __PLANTSYSTEM_H__
#define __PLANTSYSTEM_H__

#include "sys.h"
#include <stdio.h>
#include "delay.h"
#include "ts.h"
#include "dht11.h"
#include "fan.h"
#include "bump.h"
#include "ldr.h"
#include "led.h"
#include "key.h"
#include "lcd.h"
#include "esp8266.h"
#include "onenet.h"

extern uint16_t current_humi;
extern uint16_t current_light;


void LCD_Show_Page1(void);
void LCD_Show_Page2(void);
void LCD_Show_Page3(void);
void Fan_Bump_LED_status(void);
void LCD_ShowDHT11Data(void);
void uart_printf(void);
void PlantSystem_UpdateSensors(void);
void PlantSystem_HandleCloudPayload(uint8_t *payload);
uint16_t PlantSystem_GetSoilHumidity(void);
uint16_t PlantSystem_GetLightIntensity(void);
uint8_t PlantSystem_GetFanState(void);
uint8_t PlantSystem_GetBumpState(void);
uint8_t PlantSystem_GetLedState(void);
uint8_t PlantSystem_IsAutoMode(void);

#endif
