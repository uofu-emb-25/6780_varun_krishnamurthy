#include <stm32f0xx_hal.h>
#include "main.h"
#include "stm32f072xb.h"

#define ADC 0
#define DAC 1

const uint8_t triangle_table[32] = {0,15,31,47,63,79,95,111,127,142,158,174,
    190,206,222,238,254,238,222,206,190,174,158,142,127,111,95,79,63,47,31,15};   

const uint8_t sine_table[32] = {127,151,175,197,216,232,244,251,254,251,244,
    232,216,197,175,151,127,102,78,56,37,21,9,2,0,2,9,21,37,56,78,102};

void led_init(){
    GPIO_InitTypeDef initStr = {GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9,
        GPIO_MODE_OUTPUT_PP,
        GPIO_SPEED_FREQ_LOW,
        GPIO_NOPULL};
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
    HAL_GPIO_Init(GPIOC, &initStr);
}

void adc_init(){
    //Configuring to use ADC_IN10 channel
    GPIOC->MODER |= ((1 << 0) | (1 << 1)); 

    //Clock Enable to ADC1
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    
    //Calibration
    ADC1->CFGR1 |= ADC_CFGR1_CONT;
    ADC1->CFGR1 &= ~ADC_CFGR1_RES; 
    ADC1->CFGR1 |= ADC_CFGR1_RES_0; 
    ADC1->CHSELR = ADC_CHSELR_CHSEL10;

    if ((ADC1->CR & ADC_CR_ADEN) != 0)
    {
        ADC1->CR |= ADC_CR_ADDIS; 
    }

    while ((ADC1->CR & ADC_CR_ADEN) != 0){}

    ADC1->CR |= ADC_CR_ADCAL;

    while ((ADC1->CR & ADC_CR_ADCAL) != 0){}

    ADC1->CFGR1 &= ~ADC_CFGR1_DMAEN;

    if ((ADC1->ISR & ADC_ISR_ADRDY) != 0) 
    {
        ADC1->ISR |= ADC_ISR_ADRDY;
    }

    ADC1->CR |= ADC_CR_ADEN;

    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0){}

    //Start ADC
    ADC1->CR |= ADC_CR_ADSTART;
}


void adc_led(uint32_t adc_value){
    if (adc_value < 20){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET); 
    } 
    if (adc_value >= 20 && adc_value < 64){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    }
    if (adc_value >= 64 && adc_value < 128){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    }
    if (adc_value >= 128 && adc_value < 192){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);
    }
    if (adc_value >= 192){
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_SET);
    }
}

void dac_init(){
    //PA4 Pin for DAC_OUT1
    GPIOA->MODER |= ((1 << 8) | (1 << 9));
    RCC->APB1ENR |= RCC_APB1ENR_DACEN; 
    DAC1->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
    DAC1->CR |= DAC_CR_EN1;
}



int lab6_main(void){
    HAL_Init();
    SystemClock_Config();
    led_init();
    adc_init();

    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_7, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_9, GPIO_PIN_RESET);

    #if ADC
    uint32_t adc_value = 0;
    while(1){
        adc_value = ADC1->DR;
        HAL_Delay(100);
        adc_led(adc_value);
    }
    #endif

    #if DAC
    dac_init();
    uint8_t i = 0;
    while(1){
        DAC1->DHR8R1 = triangle_table[i];
        HAL_Delay(1);
        i++;
        if (i==32){
            i = 0;
        }
    }
    #endif

    return 0;
}
