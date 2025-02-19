#include <stm32f0xx_hal.h>
#include "main.h"
#include "stm32f072xb.h"

// Global variables
volatile uint8_t received_data[2];
volatile uint8_t flag = 0;

// Function to initialize the lab
void init_lab4(void){
    HAL_Init();
    SystemClock_Config();
    __HAL_RCC_GPIOC_CLK_ENABLE(); 

    GPIO_InitTypeDef initStr = {GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
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

    //Enable the RX interrupt
    USART3->CR1 |= USART_CR1_RXNEIE;
    NVIC_EnableIRQ(USART3_4_IRQn);
    NVIC_SetPriority(USART3_4_IRQn, 0);

    //Enable the transmitter and receiver
    USART3->CR1 |= USART_CR1_TE | USART_CR1_RE;

    //Enable the USART
    USART3->CR1 |= USART_CR1_UE;
}

// Function to transmit a character
void transmit_char(char c){
    while(!(USART3->ISR & USART_ISR_TXE)){
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

//USART3_4_IRQHandler
void USART3_4_IRQHandler(void){
    if (USART3->ISR & USART_ISR_RXNE) {
        static uint8_t i = 0;
        received_data[i] = USART3->RDR;
        i++;
        if(i == 1){
            transmit_string("Pin Action Number(0:OFF, 1:ON, 2:TOGGLE)\r\n");
        }
        if (i == 2){
            i = 0;
            flag = 1;
        }
    }
}

// Function to turn on the LED based on the input
void led_on(void){
    if (USART3->ISR & USART_ISR_RXNE){
    char received = USART3->RDR;
    switch (received){
        case 'r':
        case 'R':
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
            break;

        case 'b':
        case 'B':
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
            break;

        case 'o':
        case 'O':
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
            break;

        case 'g':
        case 'G':
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
            break;
        
        default:
            transmit_string("Not a valid input\r\n");
            break;
        }
    }
}

// Function to toggle the LED based on input
uint8_t toggling(uint8_t number){
    if (number == 0){
        return GPIO_PIN_RESET;
    }
    if (number == 1){
        return GPIO_PIN_SET;
    }  
}

// Function to turn on the LED based on the input
void led_operation(char c, uint8_t operation){
    switch (c){
        case 'r':
        case 'R':
            if(operation != 2){
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, toggling(operation));
            }
            else{
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
            }
            break;

        case 'b':
        case 'B':
            if(operation != 2){
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, toggling(operation));
            }
            else{
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
            }
            break;

        case 'o':
        case 'O':
            if(operation != 2){
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, toggling(operation));
            }
            else{
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
            }
            break;

        case 'g':
        case 'G':
            if(operation != 2){
            HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, toggling(operation));
            }
            else{
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
            }
            break;
        
        default:
            transmit_string("Not a valid input\r\n");
            break;
        }
}

int lab4_main(void){
    init_lab4();

    //For Task1 TA
    #if(0)
    while(1){
        led_on();
    }
    #endif

    transmit_string("Select colour (r,g,b,o)\r\n");
    while(1){
        if(flag){
            flag = 0;
            led_operation(received_data[0], (received_data[1] - '0'));
            transmit_string("Select colour (r,g,b,o)\r\n");
        } 
    }
    return 0;  
}