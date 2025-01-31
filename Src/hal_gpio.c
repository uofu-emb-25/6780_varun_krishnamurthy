#include <stdint.h>
#include <stm32f0xx_hal.h>
#include <stm32f0xx_hal_gpio.h>


void My_HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init)
{
    uint32_t pinMask;
    uint32_t pinPosition;

    // Loop through each pin in the GPIO_Init structure
    for (pinPosition = 0; pinPosition < 16; pinPosition++) {
        pinMask = (1 << pinPosition);

        // Only process the pins that are enabled in the GPIO_Init structure
        if (GPIO_Init->Pin & pinMask) {

            // Set MODER for the pin to 01 (General Purpose Output Mode)
            GPIOx->MODER &= ~(0x03 << (pinPosition * 2));  // Clear the bits for the pin
            GPIOx->MODER |= (0x01 << (pinPosition * 2));   // Set pin to general purpose output mode

            // Set OTYPER for the pin to 0 (Push-pull output type)
            GPIOx->OTYPER &= ~(0x01 << pinPosition);  // Clear the bit for the pin (push-pull)

            // Set OSPEEDR for the pin to LOW SPEED (00)
            GPIOx->OSPEEDR &= ~(0x03 << (pinPosition * 2));  // Clear speed bits for the pin
            GPIOx->OSPEEDR |= (0x00 << (pinPosition * 2));   // Set pin to low speed

            // Set PUPDR for the pin to 00 (No pull-up, no pull-down)
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

/*
GPIO_PinState My_HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    return -1;
}
*/


void My_HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    if (PinState == GPIO_PIN_SET)
    {
        // Set the pin to high (write 1 to the ODR register)
        GPIOx->ODR |= GPIO_Pin;  // Set the bit corresponding to GPIO_Pin
    }
    else
    {
        // Reset the pin to low (write 0 to the ODR register)
        GPIOx->ODR &= ~GPIO_Pin;  // Clear the bit corresponding to GPIO_Pin
    }
}




void My_HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    // Toggle the pin using ODR register
    GPIOx->ODR ^= GPIO_Pin;  // XOR the pin's bit with 1 to toggle it
}


