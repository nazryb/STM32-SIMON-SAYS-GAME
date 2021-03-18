#include "UART.h"

void UART1_Init(void) {
	// [TODO]
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	
	RCC->CCIPR &= ~RCC_CCIPR_USART1SEL;
	RCC->CCIPR |= RCC_CCIPR_USART1SEL_0; //select the system clock as USART1 clock source (01)
	
}

void UART2_Init(void) {
	// [TODO]
	//step 1
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;
	
	RCC->CCIPR &= ~RCC_CCIPR_USART2SEL;
	RCC->CCIPR |= RCC_CCIPR_USART2SEL_0; //select the system clock as USART2 clock source (01)
	
	
	
}

void UART1_GPIO_Init(void) {
	// [TODO]
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN; //enabling gpio B
	//configuring pins as UART transmitters and receivers
	GPIOB->MODER &= ~GPIO_MODER_MODE6;
	GPIOB->MODER &= ~GPIO_MODER_MODE7;
	
	GPIOB->MODER |= GPIO_MODER_MODE6_1; //set PB6 as alternative function (tx)
	GPIOB->MODER |= GPIO_MODER_MODE7_1;//set PB7 as alternative function	(rx)

	
	GPIOB->AFR[0] &= ~GPIO_AFRL_AFSEL6 & ~GPIO_AFRL_AFSEL7;
	
	//Pin 6  (TX) and pin 7(RX) AF7 (0111)
	GPIOB->AFR[0] |= GPIO_AFRL_AFSEL6_2 | GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL6_0 | GPIO_AFRL_AFSEL7_2 | GPIO_AFRL_AFSEL7_1 | GPIO_AFRL_AFSEL7_0; 

	
	//set speed for pin PB6 and PB7 to very high (11)
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED6 | GPIO_OSPEEDR_OSPEED7; 
	
	//set output type as push-pull (0) for PB6, PB7
	GPIOB->OTYPER &= ~GPIO_OTYPER_OT6 & ~GPIO_OTYPER_OT7;
	
	//use pull-up resistors for I/O (01)
	GPIOB->PUPDR &= ~GPIO_PUPDR_PUPD6 & ~GPIO_PUPDR_PUPD7;	
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD6_0 | GPIO_PUPDR_PUPD7_0;
	
}

void UART2_GPIO_Init(void) {
	// [TODO]
	//step 2
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIODEN; //enabling gpio D
	//configuring pins as UART transmitters and receivers
	GPIOD->MODER &= ~GPIO_MODER_MODE5;
	GPIOD->MODER &= ~GPIO_MODER_MODE6;
	
	GPIOD->MODER |= GPIO_MODER_MODE5_1; //set PD5 as alternative function (tx)
	GPIOD->MODER |= GPIO_MODER_MODE6_1;//set PD6 as alternative function	(rx)

	
	GPIOD->AFR[0] &= ~GPIO_AFRL_AFSEL5 & ~GPIO_AFRL_AFSEL6;
	
	//PIN 5 (TX) and PIN 6(RX) AF7 (0111)
	GPIOD->AFR[0] |= GPIO_AFRL_AFSEL5_2 | GPIO_AFRL_AFSEL5_1 | GPIO_AFRL_AFSEL5_0 | GPIO_AFRL_AFSEL6_2 | GPIO_AFRL_AFSEL6_1 | GPIO_AFRL_AFSEL6_0; 

	
	//set speed for pin PD5 and PD6 to very high (11)
	GPIOD->OSPEEDR |= GPIO_OSPEEDR_OSPEED5 | GPIO_OSPEEDR_OSPEED6; 
	
	//set output type as push-pull (0) for PD5, PD6
	GPIOD->OTYPER &= ~GPIO_OTYPER_OT5 & ~GPIO_OTYPER_OT6;
	
	//use pull-up resistors for I/O (01)
	GPIOD->PUPDR &= ~GPIO_PUPDR_PUPD5 & ~GPIO_PUPDR_PUPD6;	
	GPIOD->PUPDR |= GPIO_PUPDR_PUPD5_0 | GPIO_PUPDR_PUPD6_0;
	
	
	
}

void USART_Init(USART_TypeDef* USARTx) {
	// [TODO]
	//step 3
	
	USARTx->CR1 &= ~USART_CR1_UE; //disable USART so we can configure it
	
	//set words length to 8 bits (00)
	USARTx->CR1 &= USART_CR1_M1 & USART_CR1_M0; 
	
	//set oversampling mode to oversample by 16 (0)
	USARTx->CR1 &= ~USART_CR1_OVER8;
	
	//set number of stop bits to 1 (00)
	USARTx->CR2 &= ~USART_CR2_STOP;
	
	//set baud rate to 9600 --> baud rate = Fclk/(USART DIV)
	//USARTDIV = 80MHZ/9600 = 8333
	USARTx->BRR = 80000000 / 9600;
	
	//enable the transmitter and the receiver and enable USART
	USARTx->CR1 |= USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
	
	//enable USART
	//USARTx->CR1 |= USART_CR1_UE;
	
}

void USART_IRQHandler(USART_TypeDef* USARTx, uint8_t* buffer, uint32_t* counter){

    if( USARTx->ISR & USART_ISR_RXNE ){
        buffer[*counter] = USARTx->RDR;
        counter++;
        if(*counter >= BufferSize)
            *counter = 0;
    }
}


uint8_t USART_Read (USART_TypeDef * USARTx) {
	// SR_RXNE (Read data register not empty) bit is set by hardware
	while (!(USARTx->ISR & USART_ISR_RXNE));  // Wait until RXNE (RX not empty) bit is set
	// USART resets the RXNE flag automatically after reading DR
	return ((uint8_t)(USARTx->RDR & 0xFF));
	// Reading USART_DR automatically clears the RXNE flag 
}

void USART_Write(USART_TypeDef * USARTx, uint8_t *buffer, uint32_t nBytes) {
	int i;
	// TXE is cleared by a write to the USART_DR register.
	// TXE is set by hardware when the content of the TDR 
	// register has been transferred into the shift register.
	for (i = 0; i < nBytes; i++) {
		while (!(USARTx->ISR & USART_ISR_TXE));   	// wait until TXE (TX empty) bit is set
		// Writing USART_DR automatically clears the TXE flag 	
		USARTx->TDR = buffer[i] & 0xFF;
		USART_Delay(300);
	}
	while (!(USARTx->ISR & USART_ISR_TC));   		  // wait until TC bit is set
	USARTx->ISR &= ~USART_ISR_TC;
}   

void USART_Delay(uint32_t us) {
	uint32_t time = 100*us/7;    
	while(--time);   
}
