#include "SPI.h"
#include "SysTimer.h"

// Note: When the data frame size is 8 bit, "SPIx->DR = byte_data;" works incorrectly. 
// It mistakenly send two bytes out because SPIx->DR has 16 bits. To solve the program,
// we should use "*((volatile uint8_t*)&SPIx->DR) = byte_data";

// LSM303C eCompass (a 3D accelerometer and 3D magnetometer module) SPI Interface: 
//   MAG_DRDY = PC2   MEMS_SCK  = PD1 (SPI2_SCK)   XL_CS  = PE0             
//   MAG_CS   = PC0   MEMS_MOSI = PD4 (SPI2_MOSI)  XL_INT = PE1       
//   MAG_INT  = PC1 
//
// L3GD20 Gyro (three-axis digital output) SPI Interface: 
//   MEMS_SCK  = PD1 (SPI2_SCK)    GYRO_CS   = PD7 (GPIO)
//   MEMS_MOSI = PD4 (SPI2_MOSI)   GYRO_INT1 = PD2
//   MEMS_MISO = PD3 (SPI2_MISO)   GYRO_INT2 = PB8

extern uint8_t Rx1_Counter;
extern uint8_t Rx2_Counter;

void SPI1_GPIO_Init(void) {
	// [TODO]
	//WILL USE PIN PE13 FOR SPI1_SCK
	//PE15 FOR SPI1_MOSI 
	//BOTH ARE AF[5]
	
	//initializing pins
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOEEN; //enabling gpio E
	
	GPIOE->MODER &= ~GPIO_MODER_MODE13 & ~GPIO_MODER_MODE15 & ~GPIO_MODER_MODE12;
	
	//alternate function mode (101) PE13, PE15
	GPIOE->MODER |= GPIO_MODER_MODE13_1 | GPIO_MODER_MODE15_1 | GPIO_MODER_MODE12_0; 
	
	GPIOE->AFR[1] &= ~GPIO_AFRH_AFSEL13 & ~GPIO_AFRH_AFSEL15;
	
	GPIOE->AFR[1] |= GPIO_AFRH_AFSEL13_2 | GPIO_AFRH_AFSEL13_0 | GPIO_AFRH_AFSEL15_2 | GPIO_AFRH_AFSEL15_0;
	
	//set output type as push-pull (0) for PE13, PE15
	GPIOE->OTYPER &= ~GPIO_OTYPER_OT13 & ~GPIO_OTYPER_OT15 & ~GPIO_OTYPER_OT12;
	
	//set speed for pin PE13, PE15 to very high (11)
	GPIOE->OSPEEDR |= GPIO_OSPEEDR_OSPEED13 | GPIO_OSPEEDR_OSPEED15 | GPIO_OSPEEDR_OSPEED12; 
	
	//use no pull up no pull down (00)
	GPIOE->PUPDR &= ~GPIO_PUPDR_PUPD13 & ~GPIO_PUPDR_PUPD15 & ~GPIO_PUPDR_PUPD12;	
}

void SPI_Init(void){
	// [TODO] 
	//step 1
	
	//quad spi memory interface clock enable (1)
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
	
	//reset SPI2 (1). clear the bits so that spi2 does not remain in a reset state (0)
	RCC->APB2RSTR |= RCC_APB2RSTR_SPI1RST;
	RCC->APB2RSTR &= ~RCC_APB2RSTR_SPI1RST;
	
	//step 2
	
	//disable SPI before modifying the registers (0)
	SPI1->CR1 &= ~SPI_CR1_SPE;
	
	//a) set full-duplex communication (0)
	SPI1->CR1 &= ~SPI_CR1_RXONLY;
	
	//b) configure comm for 2-line unidirectional data mode (0)
	SPI1->CR1 &= ~SPI_CR1_BIDIMODE;
	
	//c) disable output in bidirectional mode (0)
	SPI1->CR1 &= ~SPI_CR1_BIDIOE;
	
	//d) set the frame format for receiving the MSB first when data is transmitted (0)
	//set data length to 8 bits (1111) and set the frame format in SPI Motorola mode (0)
	SPI1->CR1 &= ~SPI_CR1_LSBFIRST;
	//SPI1->CR2 &= ~SPI_CR2_DS;
	//SPI1->CR2 |= SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
	SPI1->CR2 |= SPI_CR2_DS;
	SPI1->CR2 &= ~SPI_CR2_FRF;
	
	//e) set clock polarity to 0 (goes to 0 when idle) (0)
	//set the clock phase such that the first clock transition is the first data capture edge (0)
	SPI1->CR1 &= ~SPI_CR1_CPOL;
	SPI1->CR1 &= ~SPI_CR1_CPHA;
	
	//f) set the baud rate prescaler to 16 (011)
	SPI1->CR1 &= ~SPI_CR1_BR;
	SPI1->CR1 |= SPI_CR1_BR_1 | SPI_CR1_BR_0;
	
	//g) disable CRC calculation (0)
	SPI1->CR1 &= ~SPI_CR1_CRCEN;
	
	//h) set to operate in master mode (1)
	//enable software peripheral(slave) management (1) and NSS pulse managements (1)
	SPI1->CR1 |= SPI_CR1_MSTR | SPI_CR1_SSM;//
	SPI1->CR2 |= SPI_CR2_NSSP;
	
	//i) set the internal peripheral(slave) select bit 
	SPI1->CR1 |= SPI_CR1_SSI;
	
	//j) set the FIFO reception threshold to 1/4 (8bit) (1)
	SPI1->CR2 &= ~SPI_CR2_FRXTH;   //****this was changed
	
	//k) enable SPI
	SPI1->CR1 |= SPI_CR1_SPE;
}
 
void SPI_Write(SPI_TypeDef * SPIx, uint16_t *txBuffer,  int size) {
	//uint8_t * rxBuffer = 0;
	volatile uint16_t tmpreg; 
	int i = 0;
	for (i = 0; i < size; i++) {
		while( (SPIx->SR & SPI_SR_TXE ) != SPI_SR_TXE );  // Wait for TXE (Transmit buffer empty)
		*((volatile uint16_t*)&SPIx->DR) = txBuffer[i];
	}
	while( (SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY ); // Wait for BSY flag cleared
}

void SPI_Delay(uint32_t us) {
	uint32_t i, j;
	for (i = 0; i < us; i++) {
		for (j = 0; j < 18; j++) // This is an experimental value.
			(void)i;
	}
}

void SPIx_IRQHandler(SPI_TypeDef * SPIx, uint16_t *buffer, uint16_t *counter) {
	if(SPIx->SR & SPI_SR_RXNE) {        //	SPI Busy
		buffer[*counter] = SPIx->DR;   
		// Reading SPI_DR automatically clears the RXNE flag 
		(*counter)++;  
		if( (*counter) >= BufferSize )  {
			(*counter) = 0;
		}  
	}
}
