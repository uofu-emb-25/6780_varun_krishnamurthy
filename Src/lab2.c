#include <stm32f0xx_hal.h>
#include "main.h"
#include "hal_gpio.h"

int lab2_main(void){
    HAL_Init();
    SystemClock_Config();
    __HAL_RCC_GPIOC_CLK_ENABLE(); 

    GPIO_InitTypeDef initStr = {GPIO_PIN_6 | GPIO_PIN_9,
                                GPIO_MODE_OUTPUT_PP,
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_NOPULL};

    My_HAL_GPIO_Init(GPIOC, &initStr);

    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    while (1)
    {
        HAL_Delay(400);
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
    }

    return 0;
} 