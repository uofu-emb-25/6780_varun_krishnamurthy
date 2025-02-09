#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>
#include "hal_gpio.h"
#include "assert.h"

void My_HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    uint32_t pinMask;
    uint32_t pinPosition;

    // Loop through each pin in the GPIO_Init structure
    for (pinPosition = 0; pinPosition < 16; pinPosition++) 
    {
        pinMask = (1 << pinPosition);

        // Only process the pins that are enabled in the GPIO_Init structure
        if (GPIO_Init->Pin & pinMask) 
        {

            GPIOx->MODER &= ~(0x03 << (pinPosition * 2));  // Clear the bits for the pin
            GPIOx->MODER |= (0x01 << (pinPosition * 2));   // Set pin to general purpose output mode

            GPIOx->OTYPER &= ~(0x01 << pinPosition);  // Clear the bit for the pin (push-pull)

            GPIOx->OSPEEDR &= ~(0x03 << (pinPosition * 2));  // Clear speed bits for the pin
            GPIOx->OSPEEDR |= (0x00 << (pinPosition * 2));   // Set pin to low speed

            GPIOx->PUPDR &= ~(0x03 << (pinPosition * 2));  // Clear pull-up/down bits for the pin
            GPIOx->PUPDR |= (0x00 << (pinPosition * 2));   // Set pin to no pull-up, no pull-down
        }
    }
}



/*
void My_HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin)
{
}
*/


GPIO_PinState My_HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    // Reads and checks the state of the pin
    if (GPIOx->IDR & GPIO_Pin)
    {
        return GPIO_PIN_SET;
    }
    else 
    {
        return GPIO_PIN_RESET;
    }
}




void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    if (PinState == GPIO_PIN_SET)
    {
        GPIOx->ODR |= GPIO_Pin;  // Set the bit corresponding to GPIO_Pin
    }
    else
    {
        GPIOx->ODR &= ~GPIO_Pin;  // Clear the bit corresponding to GPIO_Pin
    }
}




void My_HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->ODR ^= GPIO_Pin;  // XOR the pin's bit with 1 to toggle it
}



void My_EXTI_INRPT(void){
    //Button
    // __HAL_RCC_GPIOA_CLK_ENABLE(); 
    
    // GPIO_InitTypeDef initStr = {GPIO_PIN_0,
    //                             GPIO_MODE_INPUT,
    //                             GPIO_SPEED_FREQ_LOW,
    //                             GPIO_NOPULL};

    // My_HAL_GPIO_Init(GPIOA, &initStr); 
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER &= ~(0x03 << (0 * 2)); // Clear the bits
    GPIOA->MODER &= ~(0x03 << (0 * 2)); // Set PA0 to input mode

    GPIOA->OSPEEDR &= ~(0x03 << (0 * 2));  // Clear the bits for PA0 and set to low speed

    GPIOA->PUPDR &= ~(0x03 << (0 * 2));  // Clear the bits
    GPIOA->PUPDR |= (0x02 << (0 * 2));   // Enable pull-down resistor
    

    assert(EXTI->IMR == 0x7F840000); // Default Values
    assert(EXTI->RTSR == 0x00000000);

    SYSCFG->EXTICR[1] &= (uint16_t)~SYSCFG_EXTICR1_EXTI0_PA;
    EXTI->IMR = 0x0001;
    EXTI->RTSR = 0x0001;

    assert(EXTI->IMR == 0x0001);
    assert(EXTI->RTSR == 0x0001);

    RCC->APB2ENR |= (1 << 0); //Enable clock to SYSCFG peripheral

    assert(SYSCFG->EXTICR[0] == 0x0000); //by default its enabled
    SYSCFG->EXTICR[0] = 0x0000; //Still changing and checking
    assert(SYSCFG->EXTICR[0] == 0x0000); 

    NVIC_EnableIRQ(EXTI0_1_IRQn);
    NVIC_SetPriority(EXTI0_1_IRQn, 1);
}
