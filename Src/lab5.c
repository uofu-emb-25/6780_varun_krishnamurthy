#include <stm32f0xx_hal.h>
#include "main.h"
#include "stm32f072xb.h"

#define X_THRESHOLD 0x1000   
#define Y_THRESHOLD 0x1000 
// Address
uint8_t sensor_slave_Address = 0x69;
uint8_t who_am_i_register = 0x0F;
uint8_t who_am_i_value = 0xD3;
uint8_t ctrl_reg1 = 0x20;
uint8_t status_reg = 0x27;
uint8_t out_x_l = 0x28;
uint8_t out_x_h = 0x29;
uint8_t out_y_l = 0x2A;
uint8_t out_y_h = 0x2B;

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

void i2c_paramters(char operation, uint8_t n_bytes){
    I2C2->CR2 &= ~I2C_CR2_START; // Clear the start bit
    I2C2->CR2 = (sensor_slave_Address << 1);
    I2C2->CR2 &= ~I2C_CR2_NBYTES;
    
    if (operation == 'r'){
        I2C2->CR2 |= (n_bytes << 16); // Set NBYTES to 1
        I2C2->CR2 |= (1 << 10); // Configure RD_WRN for read operation
    }
    if (operation == 'w'){
        I2C2->CR2 |= (n_bytes << 16); // Set NBYTES to 2
        I2C2->CR2 &= ~(1 << 10); // Configure RD_WRN for write operation
    }
    I2C2->CR2 |= I2C_CR2_START; // Set the start bit  
}

void write_gyro_i2c(uint8_t register_address, uint8_t value) {
    // Configure I2C for write operation (once at the beginning)
    i2c_paramters('w', 2);

    // Send the register address
    I2C2->TXDR = register_address;


    while (!(I2C2->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF))) {}

    // If NACKF
    if (I2C2->ISR & I2C_ISR_NACKF) {
        while(1) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
            HAL_Delay(1000);
        }
    }


    HAL_Delay(100);
    // Send the data to the register
    I2C2->TXDR = value;

    while (!(I2C2->ISR & I2C_ISR_TC)) {}

}


uint8_t read_gyro_i2c(uint8_t register_address){
    i2c_paramters('w', 1);
    I2C2->TXDR = register_address;
    //Waiting for TC flag to be set
    while (!(I2C2->ISR & I2C_ISR_TC)) {}
    i2c_paramters('r', 1);

    while (!(I2C2->ISR & (I2C_ISR_RXNE | I2C_ISR_NACKF))) {}

    // If NACKF 
    if (I2C2->ISR & I2C_ISR_NACKF) {
        while(1) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
        }
    }

    while (!(I2C2->ISR & I2C_ISR_TC)) {}
    uint8_t data = I2C2->RXDR;
    return data;
}

void read_axes(void) {
    uint8_t x_data[2], y_data[2];

    // Read the X-axis data (0x28 is the low byte, 0x29 is the high byte)
    x_data[0] = read_gyro_i2c(0x28);  
    HAL_Delay(10);                    
    x_data[1] = read_gyro_i2c(0x29);  

    // Read the Y-axis data (0x2A is the low byte, 0x2B is the high byte)
    y_data[0] = read_gyro_i2c(0x2A); 
    HAL_Delay(10);                    
    y_data[1] = read_gyro_i2c(0x2B);  

    // Combine the low and high bytes for both axes
    int16_t x = (x_data[1] << 8) | x_data[0]; 
    int16_t y = (y_data[1] << 8) | y_data[0];  

    if (x > 0x7FFF) {  
        x -= 0x10000; 
    }
    if (y > 0x7FFF) {
        y -= 0x10000;   
    }

    if (x > X_THRESHOLD) { 
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET); 
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET); 
    } else if (x < -X_THRESHOLD) { 
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
    }


    if (y > Y_THRESHOLD) {  
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET); 
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET); 
    } else if (y < -Y_THRESHOLD) {  
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET); 
    }
}



int lab5_main(void){
    init_lab5();
    i2c_paramters('w', 1);

    // Task 1
    // Checking flags TXIS or NACKF
    while (!(I2C2->ISR & (I2C_ISR_TXIS | I2C_ISR_NACKF))) {}

    // If NACKF
    if (I2C2->ISR & I2C_ISR_NACKF) {
        while(1) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_6);
            HAL_Delay(1000);
        }
    }

    //Send the register address
    I2C2->TXDR = who_am_i_register;
    //Waiting for TC flag to be set
    while (!(I2C2->ISR & I2C_ISR_TC)) {}

    i2c_paramters('r', 1);

    // Checking flags RXNE or NACKF
    while (!(I2C2->ISR & (I2C_ISR_RXNE | I2C_ISR_NACKF))) {}

    // If NACKF 
    if (I2C2->ISR & I2C_ISR_NACKF) {
        while(1) {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_7);
        }
    }

    // Checking TC Flag
    while (!(I2C2->ISR & I2C_ISR_TC)) {}

    if(I2C2->RXDR == who_am_i_value){
        int i = 0;
        //Blinks the LED 5 times after successful communication
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

    // Task 2
    // Gyroscope LED Code
    // Default x and y axis are enabled so we will be skipping that part and put the sensor into normal/sleep mode
    uint8_t read_data = 0;
    read_data = read_gyro_i2c(ctrl_reg1);
    write_gyro_i2c(ctrl_reg1, read_data | 0x08);
    HAL_Delay(100);

    while(1){
        read_axes();
        HAL_Delay(100);
    }
    I2C2->CR2 |= I2C_CR2_STOP;
    return 0;
}