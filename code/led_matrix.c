#include "led_matrix.h"
#include "SPI.h"

#define LED_CS_LOW   GPIOE->ODR &= ~GPIO_ODR_OD12;    
#define LED_CS_HIGH  GPIOE->ODR |=  GPIO_ODR_OD12;

void initializeLEDMatrix(void){
	//based on the MAX7219 datasheet 
	//first 8bits are teh address 
	//last 8 bits determine funcitonality
	uint16_t noDecode = 0b0000100100000000;
	uint16_t intensity = 0b0000101000000111;//half intensity
	uint16_t scanLimit = 0b0000101100000111; 
	uint16_t normalMode = 0b0000110000000001;
	uint16_t displayTest = 0b0000111100000000;
	uint16_t shutdown1 = 0b0000110000000000;

	LED_CS_LOW;
	SPI_Write(SPI1,&shutdown1,1);
	SPI_Write(SPI1,&shutdown1,1);
	SPI_Write(SPI1,&shutdown1,1);
	SPI_Write(SPI1,&shutdown1,1);
	LED_CS_HIGH;
	
  LED_CS_LOW;
	SPI_Write(SPI1, &noDecode, 1);
	SPI_Write(SPI1, &noDecode, 1);
	SPI_Write(SPI1, &noDecode, 1);
	SPI_Write(SPI1, &noDecode, 1);
	LED_CS_HIGH;
	
	LED_CS_LOW;
	SPI_Write(SPI1, &intensity, 1);
	SPI_Write(SPI1, &intensity, 1);
	SPI_Write(SPI1, &intensity, 1);
	SPI_Write(SPI1, &intensity, 1);
	LED_CS_HIGH;
	
	LED_CS_LOW;
	SPI_Write(SPI1, &scanLimit, 1);
	SPI_Write(SPI1, &scanLimit, 1);
	SPI_Write(SPI1, &scanLimit, 1);
	SPI_Write(SPI1, &scanLimit, 1);
	LED_CS_HIGH;
	
	LED_CS_LOW;
	SPI_Write(SPI1, &normalMode, 1);
	SPI_Write(SPI1, &normalMode, 1);
	SPI_Write(SPI1, &normalMode, 1);
	SPI_Write(SPI1, &normalMode, 1);
	LED_CS_HIGH;
	
	LED_CS_LOW;
	SPI_Write(SPI1, &displayTest, 1);
	SPI_Write(SPI1, &displayTest, 1);
	SPI_Write(SPI1, &displayTest, 1);
	SPI_Write(SPI1, &displayTest, 1);
	LED_CS_HIGH;

}
void clearAllLEDS(void){
	uint16_t noop = 0b0000000000000000;
	for(int i = 0 ; i < 8; i++){
		uint8_t data = MAX7219_Dot_Matrix_row[i];
		uint16_t fullData = data << 8 | 0b00000000;
		LED_CS_LOW;
		SPI_Write(SPI1, &noop,1);
		SPI_Write(SPI1, &noop,1);
		SPI_Write(SPI1, &noop,1);
		SPI_Write(SPI1, &fullData,1);
		LED_CS_HIGH;
	}
	
	for(int i = 0 ; i < 8; i++){
		uint8_t data = MAX7219_Dot_Matrix_row[i];
		uint16_t fullData = data << 8 | 0b00000000;
		LED_CS_LOW;
		SPI_Write(SPI1, &noop,1);
		SPI_Write(SPI1, &noop,1);
		SPI_Write(SPI1, &fullData,1);
		SPI_Write(SPI1, &noop,1);
		LED_CS_HIGH;
	}
	
	for(int i = 0 ; i < 8; i++){
		uint8_t data = MAX7219_Dot_Matrix_row[i];
		uint16_t fullData = data << 8 | 0b00000000;
		LED_CS_LOW;
		SPI_Write(SPI1, &noop,1);
		SPI_Write(SPI1, &fullData,1);
		SPI_Write(SPI1, &noop,1);
		SPI_Write(SPI1, &noop,1);
		LED_CS_HIGH;
	}
	
	for(int i = 0 ; i < 8; i++){
		uint8_t data = MAX7219_Dot_Matrix_row[i];
		uint16_t fullData = data << 8 | 0b00000000;
		LED_CS_LOW;
		SPI_Write(SPI1, &fullData,1);
		SPI_Write(SPI1, &noop,1);
		SPI_Write(SPI1, &noop,1);
		SPI_Write(SPI1, &noop,1);
		LED_CS_HIGH;
	}
}

void scrollingText(char* c){
	int delayVal = 200;
	uint16_t noop = 0b0000000000000000;
	char *str = c;
	int length = strlen(str);
	int value,value2, value3,value4;
	int k = 0;
	
		value = (int)(str[k]) - 54;
		value2 = (int)(str[k+1]) - 54;
		value3 = (int)(str[k+2]) - 54;
		value4 = (int)(str[k+3]) - 54;
		for(int i = 0; i < 8; i++){
				uint8_t data = MAX7219_Dot_Matrix_row[i];
				uint16_t fullData = data << 8 | MAX7219_Dot_Matrix_font[value][i];
				printFourth(noop,fullData);
			}
			delay(delayVal);
		for(int i = 0; i < 8; i++){
				uint8_t data = MAX7219_Dot_Matrix_row[i];
				uint16_t fullData = data << 8 | MAX7219_Dot_Matrix_font[value][i];
				uint16_t fullData2 = data << 8 | MAX7219_Dot_Matrix_font[value2][i];
				printThird(noop,fullData);
				printFourth(noop,fullData2);
		}
		
		delay(delayVal);
		for(int i = 0; i < 8; i++){
				uint8_t data = MAX7219_Dot_Matrix_row[i];
				uint16_t fullData = data << 8 | MAX7219_Dot_Matrix_font[value][i];
				uint16_t fullData2 = data << 8 | MAX7219_Dot_Matrix_font[value2][i];
				uint16_t fullData3 = data << 8 | MAX7219_Dot_Matrix_font[value3][i];
				printSecond(noop,fullData);
				printThird(noop,fullData2);
				printFourth(noop,fullData3);
		}
		
		delay(delayVal);
		int j = 0;
		while(j < strlen(str)){
		for(int i = 0; i < 8; i++){
				uint8_t data = MAX7219_Dot_Matrix_row[i];
				uint16_t fullData = data << 8 | MAX7219_Dot_Matrix_font[value][i];
				uint16_t fullData2 = data << 8 | MAX7219_Dot_Matrix_font[value2][i];
				uint16_t fullData3 = data << 8 | MAX7219_Dot_Matrix_font[value3][i];
			  uint16_t fullData4 = data << 8 | MAX7219_Dot_Matrix_font[value4][i];
				printFirst(noop,fullData);
				printSecond(noop,fullData2);
				printThird(noop,fullData3);
				printFourth(noop,fullData4);
		}
		delay(delayVal);
		clearFirst();
		j++;
		value = value2;
		value2 = value3;
		value3 = value4;
		value4 = (int)(str[j+3]) - 54;
		if(value4 == -22 || (j+3 >= length))
		{
			value4 = 37;
		}
		
		if(value4 >= -6 && value4 <= 3){
			switch(value4){
				case -6: value4 = 0;
				break;
				case -5: value4 = 1;
				break;
				case -4: value4 = 2;
				break;
				case -3: value4 = 3;
				break;
				case -2: value4 = 4;
				break;
				case -1: value4 = 5;
				break;
				case 0: value4 = 6;
				break;
				case 1: value4 = 7;
				break;
				case 2: value4 = 8;
				break;
				case 3: value4 = 9;
				break;
			}

		}			
	}
}

void printFirst(uint16_t noop, uint16_t fullData){
			LED_CS_LOW;
			SPI_Write(SPI1,&noop,1);
			SPI_Write(SPI1,&noop,1);
			SPI_Write(SPI1,&noop,1);
			SPI_Write(SPI1,&fullData,1);
			LED_CS_HIGH;
}
void printSecond(uint16_t noop, uint16_t fullData){
			LED_CS_LOW;
			SPI_Write(SPI1,&noop,1);
			SPI_Write(SPI1,&noop,1);
			SPI_Write(SPI1,&fullData,1);
			SPI_Write(SPI1,&noop,1);
			LED_CS_HIGH;
}
void printThird(uint16_t noop, uint16_t fullData){
			LED_CS_LOW;
			SPI_Write(SPI1,&noop,1);
			SPI_Write(SPI1,&fullData,1);
			SPI_Write(SPI1,&noop,1);
			SPI_Write(SPI1,&noop,1);
			LED_CS_HIGH;
}
void printFourth(uint16_t noop, uint16_t fullData){
			LED_CS_LOW;
			SPI_Write(SPI1,&fullData,1);
			SPI_Write(SPI1,&noop,1);
			SPI_Write(SPI1,&noop,1);
			SPI_Write(SPI1,&noop,1);
			LED_CS_HIGH;
}

void clearFirst(void){
	for(int i = 0; i < 8; i++){
		uint16_t noop = 0b0000000000000000;
		uint8_t clearRow = 0b00000000;
		uint16_t fullData = MAX7219_Dot_Matrix_row[i] << 8 | clearRow;
		LED_CS_LOW;
		SPI_Write(SPI1,&noop,1);
		SPI_Write(SPI1,&noop,1);
		SPI_Write(SPI1,&noop,1);
		SPI_Write(SPI1,&fullData,1);
		LED_CS_HIGH;
	}
}
