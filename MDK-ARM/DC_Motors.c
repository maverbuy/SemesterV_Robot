#include "DC_Motors.h"
#include "stm32f3xx_hal.h"

char modCCR_str[5];

void DC_Motor_GPIO_Init (void)
{
	 GPIO_InitTypeDef GPIO_InitStruct;

 /* GPIO Ports Clock Enable */
 __HAL_RCC_GPIOC_CLK_ENABLE();
 __HAL_RCC_GPIOF_CLK_ENABLE();
 __HAL_RCC_GPIOA_CLK_ENABLE();
 __HAL_RCC_GPIOB_CLK_ENABLE();

 /*Configure GPIO pin Output Level */
 HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|LD2_Pin, GPIO_PIN_RESET);

 /*Configure GPIO pin Output Level */
 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);

 /*Configure GPIO pin : B1_Pin */
 GPIO_InitStruct.Pin = B1_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

 /*Configure GPIO pins : PA0 PA1 LD2_Pin */
 GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|LD2_Pin;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

 /*Configure GPIO pins : PB0 PB1 */
 GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
 GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	
}



void moveMotor (int16_t joystickYCoord, int rorl, TIM_HandleTypeDef *htim1)
{
	// Who knows what this random letter could do?
	int16_t modCCR;
	
	
	// Determine DIRA ports
	// Right Wheel
	if (joystickYCoord >= 0 && rorl == RIGHT)	// Forward
	{
		HAL_GPIO_WritePin(DCM2_DIR_PORT, DCM2_DIRA_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(DCM2_DIR_PORT, DCM2_DIRB_Pin, GPIO_PIN_SET);
	}
	if (joystickYCoord < 0 && rorl == RIGHT)	// Backwards
	{
		HAL_GPIO_WritePin(DCM2_DIR_PORT, DCM2_DIRA_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(DCM2_DIR_PORT, DCM2_DIRB_Pin, GPIO_PIN_RESET);
	}
	
	// Left Wheel
	if (joystickYCoord >= 0 && rorl == LEFT)	// Forward
	{
		HAL_GPIO_WritePin(DCM1_DIR_PORT, DCM1_DIRA_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(DCM1_DIR_PORT, DCM1_DIRB_Pin, GPIO_PIN_SET);
	}
	if (joystickYCoord < 0 && rorl == LEFT)	// Backwards
	{
		HAL_GPIO_WritePin(DCM1_DIR_PORT, DCM1_DIRA_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(DCM1_DIR_PORT, DCM1_DIRB_Pin, GPIO_PIN_RESET);
	}	
	
	// Convert to positive only
	joystickYCoord = abs(joystickYCoord);
	
	if(joystickYCoord == 0){
		modCCR = 0;
	}
	else {
		 modCCR = ((joystickYCoord * CCR_SLOPE) + CCR_OFFSET);
		// modCCR = (((joystickYCoord * joystickYCoord ) * 0.00001118) - (joystickYCoord * 0.09184) + 1000 );
		
		//modCCR = 4000;
		
		sprintf(modCCR_str, "%d", modCCR);
		LCD_Clear();
		LCD_disp_str(modCCR_str, strlen(modCCR_str));
	}
	
	if (rorl == RIGHT)	// Right Wheel
	{
		htim1->Instance->CCR1 = modCCR;
	}
	if (rorl == LEFT)	// Left Wheel
	{
		htim1->Instance->CCR2 = modCCR;
	}
		
}
	
