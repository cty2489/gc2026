#ifndef __OLED_H
#define __OLED_H

#include <stdint.h>
#include "stm32f4xx.h"

#define OLED_GPIO_RCC		RCC_AHB1Periph_GPIOD
#define OLED_GPIOx			GPIOD

#define OLED_RST_GPIOx		GPIOD
#define OLED_RST_Pin		GPIO_Pin_12
#define OLED_DC_GPIOx		GPIOD
#define OLED_DC_Pin			GPIO_Pin_11
#define OLED_SCLK_GPIOx		GPIOD
#define OLED_SCLK_Pin		GPIO_Pin_14
#define OLED_SDIN_GPIOx		GPIOD
#define OLED_SDIN_Pin		GPIO_Pin_13

void OLED_Init(void);											//OLED初始化
void OLED_Clean(void);											//OLED清屏
void OLED_Clear(void);											//兼容C30D清屏函数
void OLED_ShowChar(uint8_t Line,uint8_t Column,char Char);		//OLED显示字符
void OLED_ShowString(uint8_t Line,uint8_t Column,char *String);	//OLED显示字符串
void OLED_ShowTestNum(char *String);							//OLED显示任务码
void OLED_Show(char *String);									//OLED显示字符串并清屏

#endif
