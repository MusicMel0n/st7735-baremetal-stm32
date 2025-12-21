#include "gpio.h"

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
    uint32_t mode_value = Port->MODER;
    mode_value &= ~(3u << (BitNumber * 2));
    mode_value |=  (Mode << (BitNumber * 2));
    Port->MODER = mode_value;
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
    Port->PUPDR &= ~(3u << (BitNumber * 2));
    Port->PUPDR |=  (1u << (BitNumber * 2));
}

void delay(volatile uint32_t time)
{
    while (time--);
}