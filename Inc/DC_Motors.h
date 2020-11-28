#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "LCD.h"
#include "stm32f3xx_hal.h"



// Port Definitions
#define DCM_PWM_PORT GPIOA
#define DCM1_PWM_Pin GPIO_PIN_8
#define DCM2_PWM_Pin GPIO_PIN_9

#define DCM1_DIR_PORT GPIOB
#define DCM1_DIRA_Pin GPIO_PIN_0
#define DCM1_DIRB_Pin GPIO_PIN_1

#define DCM2_DIR_PORT GPIOA
#define DCM2_DIRA_Pin GPIO_PIN_0
#define DCM2_DIRB_Pin GPIO_PIN_1

#define RIGHT 1
#define LEFT 0

#define MINWHEELSPEED 1200
#define MAXWHEELSPEED 9999

#define CCR_SLOPE 0.035137
#define CCR_OFFSET 49

void DC_Motor_GPIO_Init (void);
void moveMotor (int16_t joystickYCoord, int rorl, TIM_HandleTypeDef *htim1);



