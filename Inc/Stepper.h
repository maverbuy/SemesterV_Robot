//Stepper header file

//Includes
#include "stm32f3xx_hal.h"
#include "stm32f3xx_hal_gpio_ex.h"
#include "stm32f3xx_hal_gpio.h"



//Prototypes
void step_init(void);
void StepLeftHome(int delay);
int StepRightHome(int delay);
void StepHome(void);
void StepMoveLeft(int nbrSteps);
void StepMoveRight(int nbrSteps);
