#include <stm32f0xx_hal.h>
#include "main.h"
#include "hal_gpio.h"
#include "stm32f0xx.h" 

void EXTI0_1_IRQHandler(void){
    EXTI->PR = (1 << 0); //Clearing the flag
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);

    //TASK2
    volatile uint32_t delay_count = 1500000;
    while (delay_count--) {
    }
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
}

int lab2_main(void){
    HAL_Init();
    SystemClock_Config();
    __HAL_RCC_GPIOC_CLK_ENABLE(); 

    GPIO_InitTypeDef initStr = {GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
                                GPIO_MODE_OUTPUT_PP,
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_NOPULL};

    My_HAL_GPIO_Init(GPIOC, &initStr);

    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);

    //Button Interrupt
    My_EXTI_INRPT();

    //Task 3
    NVIC_EnableIRQ(SysTick_IRQn);
    NVIC_SetPriority(SysTick_IRQn, 2);
    while (1)
    {
        HAL_Delay(400);
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
    }

    return 0;
} 