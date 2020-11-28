#include "stm32f3xx_hal.h"
#include <stdbool.h>
#include <stdlib.h>

void LCD_PIN_set(void);
void LCD_backlight_init(void);
void LCD_CMD_mode(void);
void LCD_DATA_mode(void);
void LCD_Pulse(void);
void LCD_Power_Sequence(void);
void LCD_PowerOn(void);
void LCD_4bit_init(void);
void LCD_line_size(bool lines, bool size);
void LCD_Entry_Set(void);
void LCD_Clear(void);
void LCD_Disp_ON(void);
void LCD_Disp_OFF(void);
void LCD_reset(void);
void LCD_curser(short spaces);
void LCD_PutNibble(uint8_t c);
void LCD_disp_char(uint8_t c);
void LCD_disp_str(char* string, int strlen);
void LCD_line_1(void);
void LCD_line_2(void);
void LCD_Init(void);
