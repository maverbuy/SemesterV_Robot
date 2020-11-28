/**
  ******************************************************************************
  *          : Stepper.c
  ******************************************************************************

  Stepper motor must hit a limit switch, and home to the center position.
	
	
	
  ******************************************************************************
  */
	
/*************************Includes*********************/
#include "Stepper.h"


/*************************Defines*********************/
#define STEP_A GPIO_PIN_6
#define STEP_NOT_A GPIO_PIN_7
#define STEP_B GPIO_PIN_8
#define STEP_NOT_B GPIO_PIN_9
#define LMTLEFT GPIO_PIN_2



/*************************Global Variables*********************/
int triggerRight;
int triggerLeft;


/*************************Pseudocode - Initialization*********************/
//Initialize stepper ports
//Enable GPIOC clock (C6,C7,C8,C9)
//Set mode to output
//Set output type to push pull
//Set PUPDR to no pull up, no pull down

void step_init(void){


	GPIO_InitTypeDef GPIO_InitStruct;

	//Set up C6
	GPIO_InitStruct.Pin = STEP_A;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //set to output push pull
	GPIO_InitStruct.Pull = GPIO_NOPULL; //set to no pull up, no pull down
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //set speed to low
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//Set up C7
	GPIO_InitStruct.Pin = STEP_NOT_A;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //set to output push pull
	GPIO_InitStruct.Pull = GPIO_NOPULL; //set to no pull up, no pull down
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //set speed to low
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
		//Set up C8
	GPIO_InitStruct.Pin = STEP_B;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //set to output push pull
	GPIO_InitStruct.Pull = GPIO_NOPULL; //set to no pull up, no pull down
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //set speed to low
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	//Set up C9
	GPIO_InitStruct.Pin = STEP_NOT_B;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP; //set to output push pull
	GPIO_InitStruct.Pull = GPIO_NOPULL; //set to no pull up, no pull down
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW; //set speed to low
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	/*Configure GPIO pins : PB2 PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_TSC_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_TSC_IRQn);
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}


void StepMoveLeft(int nbrSteps)
{
	
	unsigned char HalfStep[8] = {0x9, 0x8, 0xA, 0x2, 0x6, 0x4, 0x5, 0x1};
	uint8_t An, Bn, B, A;
	int i,k;
	int delay = 10;
	
	// reset limit switch
	triggerLeft = 0;
	
	for (k = 0; k<nbrSteps;k++){
		if(triggerLeft == 1){
			StepMoveRight(1);
			break;
		}
		for (i = 0; i< 8; i++){
			HAL_Delay(delay);
				
				//Set the proper windings
				A = (HalfStep[i] & 0x8) >> 3;
				An = (HalfStep[i] & 0x4) >> 2;
				B = (HalfStep[i] & 0x2) >> 1;
				Bn = HalfStep[i] & 0x1;
				
				//Shift winding outputs to register
				GPIOC->ODR &= 0xFC3F;
				
				GPIOC->ODR |= (A << 6);
				GPIOC->ODR |= (An << 7);
				GPIOC->ODR |= (B << 8);
				GPIOC->ODR |= (Bn << 9);	


			}
						GPIOC->ODR &= 0xFC3F;
			
		}
}


void StepMoveRight(int nbrSteps)
{
	
	unsigned char HalfStep[8] = {0x1, 0x5, 0x4, 0x6, 0x2, 0xA, 0x8, 0x9};
	uint8_t An, Bn, B, A;
	int i,k;
	int delay = 10;
	
	// reset limit switch
	triggerRight = 0;
	
	for (k = 0; k<nbrSteps;k++){
		// check if currently at right limit
		if(triggerRight == 1){
			StepMoveLeft(1);
			break;
		}
		for (i = 0; i< 8; i++){
			HAL_Delay(delay);
				
				//Set the proper windings
				A = (HalfStep[i] & 0x8) >> 3;
				An = (HalfStep[i] & 0x4) >> 2;
				B = (HalfStep[i] & 0x2) >> 1;
				Bn = HalfStep[i] & 0x1;
				
				//Shift winding outputs to register
				GPIOC->ODR &= 0xFC3F;
				
				GPIOC->ODR |= (A << 6);
				GPIOC->ODR |= (An << 7);
				GPIOC->ODR |= (B << 8);
				GPIOC->ODR |= (Bn << 9);	


			}
		
							GPIOC->ODR &= 0xFC3F;
		}
}




void StepLeftHome(int delay)
{
		
	unsigned char HalfStep[8] = {0x9, 0x8, 0xA, 0x2, 0x6, 0x4, 0x5, 0x1};
	uint8_t An, Bn, B, A;
	int i,k;
	
	triggerLeft = 0;
	
	while(triggerLeft == 0){
		for (i = 0; i< 8; i++){
			HAL_Delay(delay);
				
				//Set the proper windings
				A = (HalfStep[i] & 0x8) >> 3;
				An = (HalfStep[i] & 0x4) >> 2;
				B = (HalfStep[i] & 0x2) >> 1;
				Bn = HalfStep[i] & 0x1;
				
				//Shift winding outputs to register
				GPIOC->ODR &= 0xFC3F;
				
				GPIOC->ODR |= (A << 6);
				GPIOC->ODR |= (An << 7);
				GPIOC->ODR |= (B << 8);
				GPIOC->ODR |= (Bn << 9);	

			
			}
						GPIOC->ODR &= 0xFC3F;
		}
	
}

int StepRightHome(int delay)
{
						
	unsigned char HalfStepRight[8] = {0x1, 0x5, 0x4, 0x6, 0x2, 0xA, 0x8, 0x9};
	uint8_t An, Bn, B, A;
	int i;
	int StepCount = 0;
	
	triggerRight = 0;
	while(triggerRight == 0){
		
		StepCount++;
		for (i = 0; i< 8; i++){
			HAL_Delay(10);
				
				//Set the proper windings
				A = (HalfStepRight[i] & 0x8) >> 3;
				An = (HalfStepRight[i] & 0x4) >> 2;
				B = (HalfStepRight[i] & 0x2) >> 1;
				Bn = HalfStepRight[i] & 0x1;
				
				//Shift winding outputs to register
				GPIOC->ODR &= 0xFC3F;
				
				GPIOC->ODR |= (A << 6);
				GPIOC->ODR |= (An << 7);
				GPIOC->ODR |= (B << 8);
				GPIOC->ODR |= (Bn << 9);	

			
			}
		
							GPIOC->ODR &= 0xFC3F;
		}	
	
		return StepCount;

}



void StepHome(void)
{
	
	int StepCount = 0;
	int StepHome = 0;
	int delay = 10;
	
	StepLeftHome(delay);
	StepCount = StepRightHome(delay);
	
	StepHome = StepCount/2;
	
	StepMoveLeft(StepHome);
		

	
}
// EOF
