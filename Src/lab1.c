#include <stm32f0xx_hal.h>
#include "main.h"
#include "assert.h"
#include "hal_gpio.h"

// Comment this to revert the function back to use the original HAL functions
#define CUSTOM_HAL_FUNCTION

//Comment this to revert back to ORAGNE & GREEN LED.
//#define NEW_LED

/**Enable this or disable to check assert statements.
Applicable only to OLD LEDS**/
#define ASSERT 1

//Enable this to Deactivate button
#define LED

uint16_t GPIO_PIN_A;
uint16_t GPIO_PIN_B;

#ifdef LED
int lab1_main(void) 
{
    HAL_Init();
    SystemClock_Config();

    #ifdef NEW_LED
    GPIO_PIN_A = GPIO_PIN_6;
    GPIO_PIN_B = GPIO_PIN_7;
    #else
    GPIO_PIN_A = GPIO_PIN_8;
    GPIO_PIN_B = GPIO_PIN_9;
    #endif


    #ifdef CUSTOM_HAL_FUNCTION
    My_HAL_RCC_GPIOC_CLK_ENABLE();
    #else
    __HAL_RCC_GPIOC_CLK_ENABLE(); // Enable the GPIOC clock in the RCC
    #endif
    

    // Set up a configuration struct to pass to the initialization function
    GPIO_InitTypeDef initStr = {GPIO_PIN_A | GPIO_PIN_B,
                                GPIO_MODE_OUTPUT_PP,
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_NOPULL};
    //Checking the inital state
    #if ASSERT
    assert(GPIOC->MODER == 0x00000000);
    #endif

    #ifdef CUSTOM_HAL_FUNCTION
    My_HAL_GPIO_Init(GPIOC, &initStr);
    #else
    HAL_GPIO_Init(GPIOC, &initStr); // Initialize pins PC8 & PC9
    #endif


    //Checking if the pins are in General purpose output mode
    //Pin 8 and Pin 9 accordingly
    #if ASSERT
    assert(((GPIOC->MODER >> 16) & 0x03) == 0x01);
    assert(((GPIOC->MODER >> 18) & 0x03) == 0x01);

    //Checking if the pins are in low speed
    assert(((GPIOC->OSPEEDR >> 16) & 0x03) == 0x00); 
    assert(((GPIOC->OSPEEDR >> 18) & 0x03) == 0x00); 

    // Checking if pins are actually in no_pull-up or pull down
    assert(((GPIOC->PUPDR >> 16) & 0x03) == 0x00);
    assert(((GPIOC->PUPDR >> 18) & 0x03) == 0x00);
    #endif

    #ifdef CUSTOM_HAL_FUNCTION
    My_HAL_GPIO_WritePin(GPIOC, GPIO_PIN_A, GPIO_PIN_SET);
    #else
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_A, GPIO_PIN_SET); // Start PC8 high
    #endif

    //Checking if the bit is actually set
    #if ASSERT
    assert(((GPIOC->ODR >> 8) & 0x01) == 0x01);

    int count = 0;
    #endif

    while (1) 
    {
        HAL_Delay(200); // Delay 200ms
        // Toggle the output state of both PC8 and PC9

        #ifdef CUSTOM_HAL_FUNCTION
        My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_A | GPIO_PIN_B);
        #else
        HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_A | GPIO_PIN_B);
        #endif
        
        #if ASSERT
        count++;
        if (count % 2 == 0){
            assert(((GPIOC->ODR >> 8) & 0x01) == 0x01);
            assert(((GPIOC->ODR >> 9) & 0x01) == 0x00);
        }

        else  {
            assert(((GPIOC->ODR >> 8) & 0x01) == 0x00);
            assert(((GPIOC->ODR >> 9) & 0x01) == 0x01);
        }
        #endif
    }
}

#else

int lab1_main(void) 
{
    HAL_Init();
    SystemClock_Config();
    My_HAL_RCC_GPIOC_CLK_ENABLE();
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; //Port A CLK enable

    GPIO_InitTypeDef initStr = {GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
                                GPIO_MODE_OUTPUT_PP,
                                GPIO_SPEED_FREQ_LOW,
                                GPIO_NOPULL};
    My_HAL_GPIO_Init(GPIOC, &initStr);

    //Button
    GPIOA->MODER &= ~(0x03 << (0 * 2)); // Clear the bits
    GPIOA->MODER &= ~(0x03 << (0 * 2)); // Set PA0 to input mode

    GPIOA->OSPEEDR &= ~(0x03 << (0 * 2));  // Clear the bits for PA0 and set to low speed

    GPIOA->PUPDR &= ~(0x03 << (0 * 2));  // Clear the bits
    GPIOA->PUPDR |= (0x02 << (0 * 2));   // Enable pull-down resistor

    uint32_t debouncer = 0;
    uint8_t leg_high = 0;  // Flag to track LED high
    uint8_t led_low = 0;   // Flag to track LED low

    while(1) 
    {
        debouncer = (debouncer << 1); // Always shift every loop iteration

        if (My_HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)) 
        {     
            debouncer |= 0x01;  // Set the lowest bit of bit-vector if button is pressed (high)
        }

        // When debouncer is 0xFFFFFFFF, the button is steady high
        if (debouncer == 0xFFFFFFFF) 
        {
            if (!leg_high)  // Ensure LEDs toggle only once
            {
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6 | GPIO_PIN_7);  // Toggle LEDs on pins 6 and 7
                leg_high = 1;  // Set the flag
            }
        }

        // When debouncer is 0x00000000, the button is steady low
        if (debouncer == 0x00000000) 
        {
            if (!led_low)  // Ensure LEDs toggle only once
            {
                My_HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8 | GPIO_PIN_9);  // Toggle LEDs on pins 8 and 9
                led_low = 1;  // Set the flag
            }
        }

        // When debouncer is 0x7FFFFFFF, it means transition to steady high (button released)
        if (debouncer == 0x7FFFFFFF) 
        {
            // This code triggers only once when transitioning to steady high!
            // Reset Flag
            leg_high = 0;  
            led_low = 0;   
        }
    }

    return 0;
}

#endif