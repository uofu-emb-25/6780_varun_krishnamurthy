#include <stm32f0xx_hal.h>
#include "main.h"
#include "stm32f072xb.h"

void init_lab4(void){
    HAL_Init();
    SystemClock_Config();
    __HAL_RCC_GPIOC_CLK_ENABLE(); 

    GPIO_InitTypeDef initStr = {GPIO_PIN_6,
                                GPIO_MODE_OUTPUT_PP,
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_NOPULL};

    HAL_GPIO_Init(GPIOC, &initStr);

    //Set the pins PC4 and PC5 to alternate function 
    GPIOC->MODER |= ((1 << 11) | (1 << 9));
    //Set the alternate function to AF1
    GPIOC->AFR[0] |= ((1 << 20) | (1 << 16));


    //Clock enable to USART3
    RCC->APB1ENR |= RCC_APB1ENR_USART3EN;

    //Set the baud rate to 115200
    USART3->BRR = HAL_RCC_GetHCLKFreq() / 115200;

    //Enable the transmitter and receiver
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;

    //Enable the USART
    USART3->CR1 |= USART_CR1_UE;
}

// Function to transmit a character
void transmit_char(char c){
    while(!(USART3->ISR & USART_ISR_TXE)){
        //empty loop
    }
    USART3->TDR = c;
}

// Function to transmit a string of characters
void  transmit_string(char *str) {
    while (*str != '\0') {
        transmit_char(*str);
        str++; 
    }
}

int lab4_main(void){
    init_lab4();
    while(1){
        transmit_string("Testing String\r\n");
        HAL_Delay(100);
    }
}