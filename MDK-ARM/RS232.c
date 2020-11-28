#include "RS232.h"

// code from page 537 in textbook
void USART_Interrupt_Init(void){
	USART1->CR1 |= USART_CR1_RXNEIE;	// Receive register not empty interrupt
	USART1->CR1 &= ~USART_CR1_TXEIE;	// Transmit register empty interrupt
	NVIC_SetPriority(USART1_IRQn, 0);	// Set highest urgency
	NVIC_EnableIRQ(USART1_IRQn);			// Enable NVIC interrupt
}

// code from textbook page 535
void USART1_Port_Init (void){
	
	//PC4 = AF7 (USART1_TX), PC5 = AF7 (USART1_RX)
	RCC->AHBENR |= RCC_AHBENR_GPIOCEN; // Enable GPIO port C clock
	
	// 00 = Input, 01 = Output, 10 = Alternate Function, 11 = Analog
	GPIOC->MODER &= ~(0xF << (2*4)); // Clear mode bits for pins 4 and 5
	GPIOC->MODER |= 	0xA << (2*4);	 // Select Alternate Function mode
	
	// Alternate function 7 = USART1
	GPIOC->AFR[0] |= 0x77 << (4*4);
	
	// GPIO Speed: 00 = Low speed, 	10 = Medium speed,
	//						 10 = Fast speed,	11 = High speed
	GPIOC->OSPEEDR |= 0xF << (2*4);
	
	// GPIO Push-Pull: 00 = No pull-up/pull-down, 01 = Pull-up
	//  							 10 = Pull-down, 11 = Reserved
	GPIOC->PUPDR &= ~(0xF << (2*4));
	GPIOC->PUPDR |=   0x5 << (2*4); // Select pull-up
	
	// GPIO Output Type: 0 = push-pull, 1 = open drain
	GPIOC->OTYPER &= ~(0x3 << 4);
	
	// Enable UART 1 clock
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
}

// code from textbook page 534
void USART_Init (USART_TypeDef* USARTx) {
	
	// Disable USART
	USARTx->CR1 &= ~USART_CR1_UE;
	
	// Set data length to 8 bits
	// 00 = 8 data bits, 01 = 9 data bits, 10 = 7 data bits
	USARTx->CR1 &= ~USART_CR1_M;
	
	// Select 1 stop bit
	// 00 = 1 Stop bit		01 = 0.5 Stop bit
	// 10 = 2 Stop bits		11 = 1.5 Stop bits
	USARTx->CR2 &= ~USART_CR2_STOP;
	
	// Set parity control as no parity
	// 0 = no parity,
	// 1 = parity enabled (must then be set for Even or Odd)
	USARTx->CR1 &= ~USART_CR1_PCE;
	
	// Oversampleing by 16
	// 0 = oversampling by 16, 1 = oversampling by 8
	USARTx->CR1 &= ~USART_CR1_OVER8;
	
	// Set Baud rate to 115200
	// BRR = (1 + OVER8)*(processor clock rate)/(Baud rate)
	// BRR = (1 + 0)*(72000000)/115200
	int BRR_value = 72000000/115200;
	USARTx->BRR = BRR_value;
	
	// Enable transmission and reception
	USARTx->CR1 |= (USART_CR1_TE | USART_CR1_RE);
	
	// Enable USART
	USARTx->CR1 |= USART_CR1_UE;
	
	// Verify that USART is ready for transmission
	// TEACK: Transmit enable acknowledge flag. Hardware sets or resets it.
	while ((USARTx->ISR & USART_ISR_TEACK) == 0);
	
	// Verify that USART is ready for reception
	// REACK: Receive enable acknowledge flag. Hardware sets or resets it.
	while ((USARTx->ISR & USART_ISR_REACK) == 0);
}