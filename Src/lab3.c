#include <stm32f0xx_hal.h>
#include "main.h"
#include "stm32f0xx.h" 
#include "stm32f072xb.h"

void init(void){
    HAL_Init();
    //SystemClock_Config();
    __HAL_RCC_GPIOC_CLK_ENABLE(); 

    GPIO_InitTypeDef initStr = {GPIO_PIN_8 | GPIO_PIN_9,
                                GPIO_MODE_OUTPUT_PP,
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_NOPULL};

   HAL_GPIO_Init(GPIOC, &initStr);
}


void TIM2_IRQHandler(void){
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);

    //Clear the flag
    TIM2->SR &= ~0x0001;


    //This is used to set the PWM duty cycle(from reference solution)
    #if(0)
    static uint8_t dir = 1;
    static uint8_t value = 0;

    if(dir) { // Going up
        if(++value > 100) {
            dir = 0;
        }
    } else {
        if(--value < 1) {
            dir = 1;
        }
    }
    TIM3->CCR1 = value;
    TIM3->CCR2 = value;
    #endif
}


void tim_2(void){
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);

    //Enable timer 2
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Set the registers to the calulated value for 4Hz
    TIM2->PSC = 7999;
    TIM2->ARR = 250;
    //Enable the update interrupt
    TIM2->DIER |= TIM_DIER_UIE;

    //Enable the timer
    TIM2->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN; 

    //NVIC Enable interrupt handler
    NVIC_EnableIRQ(TIM2_IRQn);
}

void tim_3(){
    //Enable timer 3
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
    TIM3->PSC = 99;
    TIM3->ARR = 100;

    //Set ch1 to PWM output mode 2
    TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0;

    //Set ch2 to PWM output mode 1
    TIM3->CCMR1 |= TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;


    //Output compare preload 1 and 2 enabled
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE;

    //CCER Register
    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
    
    //Enable preload and start timer
    TIM3->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN;

    //Assigning this value so that both LED's are dim(According to TA)
    TIM3->CCR1 = 1;
    TIM3->CCR2 = 1;

    //Clear and set to alternate function mode 
    GPIOC->MODER |= ((1 << 15) | (1 << 13));
}


//main function
int lab3_main(void){
    init();
    tim_2();
    tim_3();
    while(1){
        __WFI();
    }
    return 0;
}