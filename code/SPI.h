#ifndef __STM32L476G_DISCOVERY_SPI_H
#define __STM32L476G_DISCOVERY_SPI_H

#include "stm32l476xx.h"

#define BufferSize 32

void SPI1_GPIO_Init(void);
void SPI_Init(void);
void SPI_Write(SPI_TypeDef * SPIx, uint16_t *txBuffer, int size);
void SPI_Delay(uint32_t us);

#endif 
