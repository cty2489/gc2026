#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "Delay.h"
#include "OLED.h"
#include "OLED_Font.h"
#include <string.h>

#define OLED_CMD				0U
#define OLED_DATA				1U
#define OLED_WIDTH				128U
#define OLED_PAGE_COUNT			8U
#define OLED_CHAR_WIDTH			8U
#define OLED_CHAR_HEIGHT		16U
#define OLED_TEST_CHAR_WIDTH	16U

static uint8_t OLED_GRAM[OLED_WIDTH][OLED_PAGE_COUNT];

#define OLED_RST_LOW()			GPIO_ResetBits(OLED_RST_GPIOx, OLED_RST_Pin)
#define OLED_RST_HIGH()			GPIO_SetBits(OLED_RST_GPIOx, OLED_RST_Pin)
#define OLED_DC_LOW()			GPIO_ResetBits(OLED_DC_GPIOx, OLED_DC_Pin)
#define OLED_DC_HIGH()			GPIO_SetBits(OLED_DC_GPIOx, OLED_DC_Pin)
#define OLED_SCLK_LOW()			GPIO_ResetBits(OLED_SCLK_GPIOx, OLED_SCLK_Pin)
#define OLED_SCLK_HIGH()		GPIO_SetBits(OLED_SCLK_GPIOx, OLED_SCLK_Pin)
#define OLED_SDIN_LOW()			GPIO_ResetBits(OLED_SDIN_GPIOx, OLED_SDIN_Pin)
#define OLED_SDIN_HIGH()		GPIO_SetBits(OLED_SDIN_GPIOx, OLED_SDIN_Pin)

static void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	uint8_t i;

	if(cmd == OLED_DATA)
	{
		OLED_DC_HIGH();
	}
	else
	{
		OLED_DC_LOW();
	}

	for(i = 0; i < 8; i++)
	{
		OLED_SCLK_LOW();
		if(dat & 0x80)
		{
			OLED_SDIN_HIGH();
		}
		else
		{
			OLED_SDIN_LOW();
		}
		OLED_SCLK_HIGH();
		dat <<= 1;
	}

	OLED_DC_HIGH();
}

static void OLED_Refresh(void)
{
	uint8_t page;
	uint8_t column;

	for(page = 0; page < OLED_PAGE_COUNT; page++)
	{
		OLED_WR_Byte(0xB0 + page, OLED_CMD);
		OLED_WR_Byte(0x00, OLED_CMD);
		OLED_WR_Byte(0x10, OLED_CMD);
		for(column = 0; column < OLED_WIDTH; column++)
		{
			OLED_WR_Byte(OLED_GRAM[column][page], OLED_DATA);
		}
	}
}

static void OLED_ClearBuffer(void)
{
	memset(OLED_GRAM, 0, sizeof(OLED_GRAM));
}

static uint8_t OLED_NormalizeAscii(char chr)
{
	if(chr < ' ' || chr > '~')
	{
		return 0;
	}

	return (uint8_t)(chr - ' ');
}

static void OLED_DrawCharCell(uint8_t line, uint8_t column, char chr)
{
	uint8_t x;
	uint8_t top_page;
	uint8_t bottom_page;
	uint8_t font_index;
	uint8_t char_column;

	if(line < 1 || line > 4 || column < 1 || column > 16)
	{
		return;
	}

	x = (uint8_t)((column - 1) * OLED_CHAR_WIDTH);
	top_page = (uint8_t)((line - 1) * (OLED_CHAR_HEIGHT / 8));
	bottom_page = (uint8_t)(top_page + 1);
	font_index = OLED_NormalizeAscii(chr);

	for(char_column = 0; char_column < OLED_CHAR_WIDTH; char_column++)
	{
		OLED_GRAM[x + char_column][top_page] = OLED_ASCII[font_index][char_column];
		OLED_GRAM[x + char_column][bottom_page] = OLED_ASCII[font_index][char_column + 8];
	}
}

static uint8_t OLED_TestCharIndex(char chr)
{
	if(chr >= '1' && chr <= '3')
	{
		return (uint8_t)(chr - '1');
	}

	return 3;
}

static void OLED_DrawTestChar(uint8_t line, uint8_t column, char chr)
{
	uint8_t x;
	uint8_t font_index;
	uint8_t page;
	uint8_t char_column;

	if(line != 1 || column < 1 || column > 8)
	{
		return;
	}

	x = (uint8_t)((column - 1) * OLED_TEST_CHAR_WIDTH);
	font_index = OLED_TestCharIndex(chr);

	for(page = 0; page < OLED_PAGE_COUNT; page++)
	{
		for(char_column = 0; char_column < OLED_TEST_CHAR_WIDTH; char_column++)
		{
			OLED_GRAM[x + char_column][page] = OLED_TestNum[font_index * OLED_PAGE_COUNT + page][char_column];
		}
	}
}

static void OLED_GPIOInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(OLED_GPIO_RCC, ENABLE);

	GPIO_InitStructure.GPIO_Pin = OLED_RST_Pin | OLED_DC_Pin | OLED_SCLK_Pin | OLED_SDIN_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(OLED_GPIOx, &GPIO_InitStructure);

	OLED_RST_HIGH();
	OLED_DC_HIGH();
	OLED_SCLK_HIGH();
	OLED_SDIN_HIGH();
}

void OLED_Clean(void)
{
	OLED_ClearBuffer();
	OLED_Refresh();
}

void OLED_Clear(void)
{
	OLED_Clean();
}

void OLED_Init(void)
{
	OLED_GPIOInit();

	OLED_RST_LOW();
	Delay_ms(100);
	OLED_RST_HIGH();

	OLED_WR_Byte(0xAE, OLED_CMD);
	OLED_WR_Byte(0xD5, OLED_CMD);
	OLED_WR_Byte(0x80, OLED_CMD);
	OLED_WR_Byte(0xA8, OLED_CMD);
	OLED_WR_Byte(0x3F, OLED_CMD);
	OLED_WR_Byte(0xD3, OLED_CMD);
	OLED_WR_Byte(0x00, OLED_CMD);
	OLED_WR_Byte(0x40, OLED_CMD);
	OLED_WR_Byte(0x8D, OLED_CMD);
	OLED_WR_Byte(0x14, OLED_CMD);
	OLED_WR_Byte(0x20, OLED_CMD);
	OLED_WR_Byte(0x02, OLED_CMD);
	OLED_WR_Byte(0xA1, OLED_CMD);
	OLED_WR_Byte(0xC8, OLED_CMD);
	OLED_WR_Byte(0xDA, OLED_CMD);
	OLED_WR_Byte(0x12, OLED_CMD);
	OLED_WR_Byte(0x81, OLED_CMD);
	OLED_WR_Byte(0xEF, OLED_CMD);
	OLED_WR_Byte(0xD9, OLED_CMD);
	OLED_WR_Byte(0xF1, OLED_CMD);
	OLED_WR_Byte(0xDB, OLED_CMD);
	OLED_WR_Byte(0x30, OLED_CMD);
	OLED_WR_Byte(0xA4, OLED_CMD);
	OLED_WR_Byte(0xA6, OLED_CMD);
	OLED_WR_Byte(0xAF, OLED_CMD);

	OLED_Clean();
}

void OLED_ShowChar(uint8_t line, uint8_t column, char chr)
{
	OLED_DrawCharCell(line, column, chr);
	OLED_Refresh();
}

void OLED_ShowString(uint8_t line, uint8_t column, char *string)
{
	if(string == 0)
	{
		return;
	}

	while(*string != '\0' && line <= 4)
	{
		if(column > 16)
		{
			column = 1;
			line++;
			if(line > 4)
			{
				break;
			}
		}

		OLED_DrawCharCell(line, column, *string);
		column++;
		string++;
	}

	OLED_Refresh();
}

void OLED_ShowTestNum(char *string)
{
	uint8_t i;

	OLED_ClearBuffer();
	if(string != 0)
	{
		for(i = 0; i < 4; i++)
		{
			if(i < 3 && string[i] == '\0')
			{
				break;
			}
			OLED_DrawTestChar(1, (uint8_t)(1 + i), (i == 3) ? '+' : string[i]);
		}
	}
	OLED_Refresh();
}

void OLED_Show(char *string)
{
	OLED_Clean();
	OLED_ShowString(1, 1, string);
}
