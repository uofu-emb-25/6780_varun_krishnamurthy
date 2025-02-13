#include <stm32f0xx_hal.h>
#include "main.h"
#include "hal_gpio.h"
#include "stm32f0xx.h" 
#include "stm32f072xb.h"
#include <assert.h>

#define _BV(x) (1 << x)

void TIM2_IRQHandler(void) {
    TIM2->SR &= ~TIM_SR_UIF; // Clear interrupt flag
    GPIOC->ODR ^= (1 << 7) | (1 << 6); // Toggle LEDs

    static uint8_t dir = 1; // Only initialized once
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
    TIM3->CCR3 = value; // Set PWM duty cycle
    TIM3->CCR4 = value;
}

void ref(void){
    HAL_Init();
    SystemClock_Config();

    /* Enable Peripheral Clocks in RCC ---------------------------------------*/
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN ;

    /* Configure GPIOC (LED) Pins --------------------------------------------*/
    // (Safe to clear register) Set PC7 & PC6 to output, set PC9 & PC8 to alternate function
    GPIOC->MODER = _BV(19) | _BV(17) | _BV(14) | _BV(12);
    // PC8 has TIM3_CH3 as AF0, PC8 has TIM3_CH4 as AF0 (no need to explicitly set AFR)
    GPIOC->BSRR = _BV(6);

    /* Configure TIM2 for UEV Interrupt --------------------------------------*/
    TIM2->PSC = 7999;           // Want 1 kHz frequency
    TIM2->ARR = 250;             // 20 ms UEV period
    TIM2->DIER = TIM_DIER_UIE;  // Enable update interrupt
        TIM2->EGR |= TIM_EGR_UG;
        TIM2->RCR = 0;
    NVIC_SetPriority(TIM2_IRQn, 2);
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM2->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN; // Enable preload and start timer (default upcounter)


    /* Configure TIM3 for PWM  -----------------------------------------------*/
    TIM3->PSC = 99;   // 80 kHz frequency
    TIM3->ARR = 100;  // 1.25 ms UEV period (800 Hz)

    // Using a local var to make setting register clearer, will be cleaned up by compiler
    uint32_t CCMR = TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1 | TIM_CCMR2_OC4M_0;    // Set TIM3_CH4 to PWM Mode 2
             CCMR |= TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;                      // Set TIM3_CH3 to PWM Mode 1
             CCMR |= TIM_CCMR2_OC3PE  | TIM_CCMR2_OC3PE;                       // Enable preload on both CCRx registers
    TIM3->CCMR2 = CCMR;
    //TIM3->CCMR2 = 0x78D0;

    TIM3->CCER = TIM_CCER_CC4E | TIM_CCER_CC3E;

    TIM3->CR1 = TIM_CR1_ARPE | TIM_CR1_CEN; // Enable preload and start timer (default upcounter)

    TIM3->CCR1 = 25;
    TIM3->CCR2 = 25;
}