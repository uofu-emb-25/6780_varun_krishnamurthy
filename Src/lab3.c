#include <stm32f0xx_hal.h>
#include "main.h"
#include "hal_gpio.h"
#include "stm32f0xx.h" 


void TIM2_IRQHandler(void){
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
    //Clear the flag
    TIM2->SR &= ~0x0001;
}

//main function
int lab3_main(void){
    //LED's
    HAL_Init();
    SystemClock_Config();
    __HAL_RCC_GPIOC_CLK_ENABLE(); 

    GPIO_InitTypeDef initStr = {GPIO_PIN_8 | GPIO_PIN_9,
                                GPIO_MODE_OUTPUT_PP,
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_NOPULL};

    HAL_GPIO_Init(GPIOC, &initStr);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

    //Enable timer 2
    RCC->APB1ENR = 0x0001;
    // Set the registers to the calulated value for 4Hz
    TIM2->PSC = 7999;
    TIM2->ARR = 250;
    //Enable the update interrupt
    TIM2->DIER = 0X0001;
    //Enable the timer
    TIM2->CR1 |= 0x0001; 
    //NVIC Enable interrupt handler
    NVIC_EnableIRQ(TIM2_IRQn);
    return 0;
}