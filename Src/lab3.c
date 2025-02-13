#include <stm32f0xx_hal.h>
#include "main.h"
#include "hal_gpio.h"
#include "stm32f0xx.h" 
#include "stm32f072xb.h"
#include <assert.h>


void TIM2_IRQHandler(void){
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);
    //Clear the flag
    TIM2->SR &= ~0x0001;
}

void tim_2(void){
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
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    // Set the registers to the calulated value for 4Hz
    TIM2->PSC = 7999;
    TIM2->ARR = 250;
    //Enable the update interrupt
    TIM2->DIER |= 0x0001;
    //Enable the timer
    TIM2->CR1 |= 0x0001; 
    //NVIC Enable interrupt handler
    NVIC_EnableIRQ(TIM2_IRQn);
}

void init(void){
    HAL_Init();
    SystemClock_Config();
    __HAL_RCC_GPIOC_CLK_ENABLE(); 

    GPIO_InitTypeDef initStr = {GPIO_PIN_8 | GPIO_PIN_9,
                                GPIO_MODE_OUTPUT_PP,
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_NOPULL};

    HAL_GPIO_Init(GPIOC, &initStr);
}

void tim_3(){
    //Enable timer 3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 99;
    TIM3->ARR = 100;

    //CCMR Register for ch1 and ch2
    //CC1S & CC2S
    
    /* work on it */
    //TIM3->CCMR1 |= (TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);

    //Clear and set ch1 to PWM output mode 2
    TIM3->CCMR1 &= ~(0b111 << 4);  
    TIM3->CCMR1 |= (0b111 << 4);
    assert(TIM3->CCMR1 == 0x70);

    //Clear and set ch2 to PWM output mode 1
    TIM3->CCMR1 &= ~(0b111 << 12);
    TIM3->CCMR1 |= (0b110 << 12);
    assert(TIM3->CCMR1 == 0x6070);


    //Output compare preload 1 and 2 enabled
    TIM3->CCMR1 &= ~( (1 << 3) | (1 << 11) );  
    TIM3->CCMR1 |= ( (1 << 3) | (1 << 11) );
    assert(TIM3->CCMR1 == 0x6878);


    //CCER Register
    TIM3->CCER &= ~( (1 << 0) | (1 << 4) );
    TIM3->CCER |= ( (1 << 0) | (1 << 4) );
    assert(TIM3->CCER == 0x11);
    


    //20% of ARR(1250) = 250
    TIM3->CCR1 = 99;
    TIM3->CCR2 = 100;

    //Clear and set to alternate function mode
    //GPIOC->MODER &= ~(0xF << 12);
    //GPIOC->MODER |= (0xA << 12);
    GPIOC->MODER |= ((1 << 15 | (1 << 13)));

    //Selecting AF0
    // GPIOC->AFR[0] &= ~(0xFF << 24);
    // GPIOC->AFR[0] |= (0x00 << 24); 
}
//main function
int lab3_main(void){
    init();
    //tim_2();
    tim_3();
    while(1){
        __WFI();
    }
    return 0;
}