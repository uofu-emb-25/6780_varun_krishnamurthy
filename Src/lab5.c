#include <stm32f0xx_hal.h>
#include "main.h"
#include "stm32f072xb.h"

uint8_t sensor_slave_Address = 0x69;
uint8_t who_am_i_register = 0x0F;
uint8_t who_am_i_value = 0xD3;

void init_lab5(void){
    HAL_Init();
    SystemClock_Config();
    GPIO_InitTypeDef initStr = {GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
        GPIO_MODE_OUTPUT_PP,
        GPIO_SPEED_FREQ_LOW,
        GPIO_NOPULL};

    // Enable the GPIOC clock
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN| RCC_AHBENR_GPIOCEN;
    // Set PB11 and PB13 to alternate function mode
    GPIOB->MODER |= ((1 << 23) | (1 << 27));
    // PB11 to AF1 and set to open drain
    GPIOB->AFR[1] |= (1 << 12);
    GPIOB->OTYPER |= (1 << 11);
    // PB13 to AF5 and set to open drain
    GPIOB->AFR[1] |= (0x5 << 20);
    GPIOB->OTYPER |= (1 << 13);
    // PB14 configuration
    GPIOB->MODER |= (1 << 28);
    // PB14 to push-pull
    GPIOB->OTYPER &= ~(1 << 14);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
    // PC0 configuration
    GPIOC->MODER |= (1 << 0);
    // PC0 to push-pull
    GPIOC->OTYPER &= ~(1 << 0);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET);

    //Set PB15 to input mode
    GPIOB->MODER &= ~(3 << 30);

    // Enable the I2C2 peripheral clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;
    // Set the I2C2 clock to 100KhZ
    I2C2->TIMINGR =  0x00010402;
    // Enable the I2C2 peripheral
    I2C2->CR1 |= I2C_CR1_PE;

    HAL_GPIO_Init(GPIOC, &initStr);
}

void i2c_paramters(char operation){
    I2C2->CR2 &= ~I2C_CR2_START; // Clear the start bit
    I2C2->CR2 = (sensor_slave_Address << 1);
    I2C2->CR2 |= (1 << 16); // Set NBYTES to 1
    if (operation == 'r'){
        I2C2->CR2 |= (1 << 10); // Configure RD_WRN for read operation
    }
    if (operation == 'w'){
        I2C2->CR2 &= ~(1 << 10); // Configure RD_WRN for write operation
    }
    I2C2->CR2 |= I2C_CR2_START; // Set the start bit  
}

int lab5_main(void){
    init_lab5();
    i2c_paramters('w');

    // Checking flags TXIS or NACKF
    while (!(I2C2->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF))) {}

    // If NACKF
    if (I2C2->ISR & I2C_ISR_NACKF) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
        while(1) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
            HAL_Delay(1000);
        }
    }

    //Send the register address
    I2C2->TXDR = who_am_i_register;
    //Waiting for TC flag to be set
    while (!(I2C2->ISR & I2C_ISR_TC)) {}

    i2c_paramters('r');

    // Checking flags RXNE or NACKF
    while (!(I2C2->ISR & (I2C_ISR_RXNE | I2C_ISR_NACKF))) {}

    // If NACKF 
    if (I2C2->ISR & I2C_ISR_NACKF) {
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
        while(1) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
        }
    }

    // Checking TC Flag
    while (!(I2C2->ISR & I2C_ISR_TC)) {}

    if(I2C2->RXDR == who_am_i_value){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
        int i = 0;
        while(i < 10){
            i++;
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_8);
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_9);
            HAL_Delay(1000);
        }
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

    }
    else{
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    } 

    // Stop the I2C communication
    I2C2->CR2 |= I2C_CR2_STOP;

    return 0;
}