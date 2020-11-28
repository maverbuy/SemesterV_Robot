#include "stm32f3xx_hal.h"
#include <stdbool.h>
#include <stdlib.h>

void USART1_Port_Init (void);
void USART_Init (USART_TypeDef* USARTx);
void USART_Interrupt_Init(void);