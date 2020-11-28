#include "LCD.h"

#define DISP_LINES 1			// LCD display lines: 0 for 1 line, 1 for 2 lines
#define FONT_SIZE 1				// LCD font size: 0 for 5x8, 1 for 5x11
#define LCD_Port GPIOB		// LCD port is GPIOB

/*************************************************************
LCD Pin structure - GPIOB
PB4		LCD_D4	1st output pin
PB5		LCD_D5	2nd output pin
PB6		LCD_D6	3rd output pin
PB7		LCD_D7	4th output pin

PB12	LCD_E		Enable (pulse) pin
PB13	LCD_RS	Register Select (0=command, 1=data)
PB15	LCD_RW	Read/Write Select (0=write, 1=read) SHOULD ALWAYS BE 0 (write)
**************************************************************/

void LCD_PIN_set(void){
	
	// Create a struct to hold the GPIO settings
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// Set GPIOB pins 4-7, 12, 13, 15: Push/Pull, No Pullup, Speed low
	GPIO_InitStruct.Pin = GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	// Run HAL function to set GPIOB settings
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// Set GPIOA pin 10 as PWM signal for backlight: TIM1 ch3
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Alternate = GPIO_AF6_TIM1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	
	// Run HAL function to set GPIOA settings
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void LCD_backlight_init(void){
	
	// Code from p.394 in text
	
	// Enable TIMER 1 clock
	RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
	
	//Counting direction: 0 = Up-counting, 1 = Down-counting
	TIM1->CR1 &= ~TIM_CR1_DIR;
	
	// Prescaler: 72MHz/72 = 1KHz
	TIM1->PSC = 72;
	
	// Auto Reload: 1/1KHz = 0.001s
	TIM1->ARR = 999;
	
	// Clear output compare mode bits for channel 1
	TIM1->CCMR2 &= ~TIM_CCMR2_OC3M;
	
	// Select PWM Mode 1 output on channel 1 (OC3M = 110)
	TIM1->CCMR2 |= TIM_CCMR2_OC3M_1 | TIM_CCMR2_OC3M_2;
	
	// Output 1 preload enable
	TIM1->CCMR2 |= TIM_CCMR2_OC3PE;
	
	// Select output polarity: 0 = Active high, 1 = Active low
	TIM1->CCER &= ~TIM_CCER_CC3P;
	
	// Enable output of channel 3 (CH3)
	TIM1->CCER |= TIM_CCER_CC3E;
	
	//Main output enable (MOE): 0 = Disable, 1 = Enable
	TIM1->BDTR |= TIM_BDTR_MOE;
	
	// Output Compare Register for channel 3
	TIM1->CCR3 = 500; 	// Initial duty cycle 50%
	
	// Enable counter
	TIM1->CR1 |= TIM_CR1_CEN;
}

void LCD_CMD_mode(void){
	
	// Set Register Select to receive commands (NOT data)
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_Delay(1);
}

void LCD_DATA_mode(void){
	
	// Set Register Select to receive data (characters)
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
	HAL_Delay(1);
}

void LCD_Pulse(void){
	
	// Send one falling edge pulse to LCD chip
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_Delay(1);
}

void LCD_Power_Sequence(void){
	
	// Sequence for initial power-up of LCD display
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	LCD_Pulse();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	LCD_Pulse();
}

void LCD_PowerOn(void){
	
	// Power-on sequence as per p.449 in Embedded Systems textbook
	HAL_Delay(30);
	LCD_Power_Sequence();
	HAL_Delay(5);
	LCD_Power_Sequence();
	HAL_Delay(1);
	LCD_Power_Sequence();
	HAL_Delay(1);
}

void LCD_4bit_init(void){
	
	// Set 4-bit mode on LCD display (0010)
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	LCD_Pulse();
	HAL_Delay(1);
}

void LCD_line_size(bool lines, bool size){
	
	// Lines and font size defined as constants above
	
	LCD_4bit_init(); // requered to be sent again before lines and size
	
	if(lines){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	}
	else {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	}
	
	if(size){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	}
	else{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	}
	LCD_Pulse();
	
}

void LCD_Entry_Set(void){
	
	// Sets Entry mode to write L>R, display shift off (0000 0110)
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	LCD_Pulse();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	LCD_Pulse();
	HAL_Delay(1);
}

void LCD_Clear(void){
	
	// Clear LCD display (0000 0001)
	LCD_CMD_mode();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	LCD_Pulse();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	LCD_Pulse();
	HAL_Delay(3);
}

void LCD_Disp_ON(void){
	
	// Turn LCD Display ON (0000 1111)
	LCD_CMD_mode();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	LCD_Pulse();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	LCD_Pulse();
	HAL_Delay(1);
	TIM1->CCER |= TIM_CCER_CC3E;
}

void LCD_Disp_OFF(void){
	
	// Turn LCD Display OFF (0000 1000)
	LCD_CMD_mode();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	LCD_Pulse();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	LCD_Pulse();
	HAL_Delay(1);
	TIM1->CCER &= ~TIM_CCER_CC3E;
}

void LCD_reset(void){
	LCD_Clear();
	LCD_Entry_Set();
	LCD_Disp_ON();
}

void LCD_curser(short spaces){
	
	// Move curser L (neg value) or R (pos value) spaces
	LCD_CMD_mode();
	
	// Move Left (spaces x 0001 0000)
	if(spaces <= 0){
		int value = abs(spaces);
		for(int i = 0; i < value; i++){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
			LCD_Pulse();
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
			LCD_Pulse();
			HAL_Delay(1);
		}
	}
	// Move Right (spaces x 0001 0100)
	else{
		for(int i = 0; i < spaces; i++){
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
			LCD_Pulse();
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
			LCD_Pulse();
			HAL_Delay(1);
		}
	}
}

// from textbook p.446
void LCD_PutNibble(uint8_t c){
	
	if(c & 0x8){	// 4th bit on pin 7
		LCD_Port->ODR |= 1<<7;
	}
	else{
		LCD_Port->ODR &= ~(1<<7);
	}
	
	if(c & 0x4){	// 3rd bit on pin 6
		LCD_Port->ODR |= 1<<6;
	}
	else{
		LCD_Port->ODR &= ~(1<<6);
	}
	
	if(c & 0x2){	// 2nd bit on pin 5
		LCD_Port->ODR |= 1<<5;
	}
	else{
		LCD_Port->ODR &= ~(1<<5);
	}
	
	if(c & 0x1){	// 1st bit on pin 4
		LCD_Port->ODR |= 1<<4;
	}
	else{
		LCD_Port->ODR &= ~(1<<4);
	}
}


// Function to display character
void LCD_disp_char(uint8_t c){
	LCD_DATA_mode();
	
	// Send upper 4 bits
	LCD_PutNibble(c >> 4);
	LCD_Pulse();
	
	// Send lower 4 bits
	LCD_PutNibble(c & 0xF);
	LCD_Pulse();
}


// Function to display string
void LCD_disp_str(char* string, int strlen){
	
	// Sent each character to the display
	for(int i=0; i < strlen; i++){
		LCD_disp_char(string[i]);
	}
}

// Function to change output to line 1
void LCD_line_1(void){
	
	LCD_CMD_mode();
	
	// write 0xC0 to command input (0000 0010)
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	LCD_Pulse();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	LCD_Pulse();
	HAL_Delay(1);
	
}

// Function to change output to line 2
void LCD_line_2(void){
	
	LCD_CMD_mode();
	
	// write 0xC0 to command input (1100 0000)
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	LCD_Pulse();
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
	LCD_Pulse();
	HAL_Delay(1);
	
}

// initialize LCD
void LCD_Init(void){
	
	LCD_PIN_set();
	LCD_CMD_mode();
	LCD_PowerOn();
	LCD_4bit_init();
	LCD_line_size(DISP_LINES, FONT_SIZE);
	LCD_Clear();
	LCD_Entry_Set();
	LCD_Disp_ON();
	LCD_backlight_init();
	
}
