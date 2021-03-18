#include <time.h>
#include <stdlib.h>
#include "SysClock.h"
#include "led_matrix.h"
#include "UART.h"
#include "I2C.h"
int game_Symbols [50]; //don't expect anyone to get to 51 points
void printScore(int); //prints the current score of the game (always shows)
void playRound(int); //handles one round
void startGame(); //start the game
void gameOver(int); //displays final score
void addRandomSymbol(int); //adds another symbol to the current list for user to remember
int userInput(int); //function to handle user input
void printXXXX(); //prints XXXX
void printMistakes(); //prints the x's on the board
void userMistake(); //handles the behavior when user puts in the wrong input
void initNunchuk(); //initializes the nunchuk
void initUSART(); //for testing purposes and interfacing with thermite
void printSuccess(); //Print success if user input is correct
int mistakes = 0;
void randfunc();

int main(){
	System_Clock_Init();
	SPI1_GPIO_Init();
	SPI_Init();
	SysTick_Init();
	initializeLEDMatrix();
  clearAllLEDS();	
	initUSART();
	I2C_GPIO_Init();
	I2C_Initialization();
	initNunchuk();
	randfunc();
	
	uint16_t noop = 0;
	int roundNum = 1;
	int numMistakes = 0;
//	time_t t;
//	srand((unsigned) time(&t));
	//srand());
	startGame();
	
	addRandomSymbol(1);
	
	int game = 1;
	int successfulRound = 0;
	
	while(game){
		playRound(roundNum);
		
		successfulRound = userInput(roundNum);
		

		if(successfulRound == -1){
			userMistake();
		}
		else{
			roundNum += 1;
			addRandomSymbol(roundNum);
			clearAllLEDS();
			scrollingText("SUCCESS");
			
		}
		
		
		if (mistakes == 3) {
			gameOver(roundNum);
			game = 0;
		}
	}
		
}
void randfunc(){
		uint8_t Data_Send;
	uint8_t Data_Receive[6];
	char message[6];
	uint8_t slaveAddress = 82UL << 1;
	int i;
	
	Data_Send = 0x00;

	I2C_SendData(I2C1, slaveAddress, &Data_Send, 1);
		
		for(i = 0; i < 10000; i++){}
		
		I2C_ReceiveData(I2C1, slaveAddress, &*Data_Receive, 6);
		
		 int a = Data_Receive[0];
		 int b = Data_Receive[1];
		 int c = Data_Receive[2] ;
		 int d = Data_Receive[5] ;

		 int seed = (a+b*c)*d; 

		 srand(seed);
}
void startGame(){
	scrollingText("SIMON SAYS GAME");	
	
		scrollingText("START");
}

void playRound(int roundNum){
	int delayTime = 500;
		uint16_t noop = 0;
	
	
	
		for (int k = 0; k < roundNum; k++){
			for(int i = 0; i < 8; i++){
					uint8_t addr = MAX7219_Dot_Matrix_row[i];
					uint16_t fullData = addr << 8 | MAX7219_game_symbols[game_Symbols[k]][i];  //goes through each symbol in the game symbols
					printFirst(noop,fullData);
			}

			printMistakes();
			printScore(roundNum);
			
			delay(delayTime);
			clearFirst();
			delay(delayTime);
	}
		
	clearAllLEDS();	
	for(int i = 0; i < 8; i++){
		uint8_t addr = MAX7219_Dot_Matrix_row[i];
		uint16_t fullData = addr << 8 | MAX7219_Dot_Matrix_font[17][i]; //G
		printSecond(noop, fullData);
	}
	
	for(int i = 0; i < 8; i++){
					uint8_t addr = MAX7219_Dot_Matrix_row[i];
					uint16_t fullData = addr << 8 | MAX7219_Dot_Matrix_font[25][i];  //O
					printThird(noop,fullData);
			}
	delay(delayTime);
	clearAllLEDS();	
	printMistakes();
	printScore(roundNum);
}


void printScore(int roundNum){
	uint16_t noop = 0;
	if (roundNum<10){
		for(int i = 0; i < 8; i++){
				uint8_t addr = MAX7219_Dot_Matrix_row[i];
				uint16_t fullData = addr << 8 | MAX7219_Dot_Matrix_font[roundNum][i];
				printFourth(noop,fullData);

		}
	}
	else{
		for(int i = 0; i < 8; i++){
				uint8_t addr = MAX7219_Dot_Matrix_row[i];
				uint16_t fullData3 = addr << 8 | MAX7219_Dot_Matrix_font[roundNum/10][i];
				uint16_t fullData4 = addr << 8 | MAX7219_Dot_Matrix_font[roundNum%10][i];
				printThird(noop,fullData3);
				printFourth(noop,fullData4);

		}
	}
}




void addRandomSymbol(int roundNum){
	game_Symbols[roundNum-1] = rand() % 6; //random num in range 0 to 5
}


int userInput(int roundNum){
	for(int k = 0; k < roundNum; k++){
	clearFirst();
	

	uint8_t Data_Send;
	uint8_t Data_Receive[6];
	char message[6];
	uint8_t slaveAddress = 82UL << 1;
	int i;
	
	Data_Send = 0x00;
	
	int centerX = 128;
	int centerY = 124;
	int upY = 218;
	int downY = 36;
	int rightX = 226;
	int leftX = 29;
	
	int xCoord;
	int yCoord = Data_Receive[1];
	int buttonZ = Data_Receive[5] & 0x00000001;
	int buttonC = Data_Receive[5] & 0x00000002;
	int errorMargin = 10;
		
	int validInput = 0;
		
	int actionIndex = 0; //the index to access the gameSymbols; 0 = left. 1 = right; 2 = up; 3 = down; 4 = C; 5 = Z	
	
		I2C_SendData(I2C1, slaveAddress, &Data_Send, 1);
		
		for(i = 0; i < 10000; i++){}
		
		I2C_ReceiveData(I2C1, slaveAddress, &*Data_Receive, 6);
		
		 xCoord = Data_Receive[0];
		 yCoord = Data_Receive[1];
		 buttonZ = Data_Receive[5] & 0x00000001;
		 buttonC = Data_Receive[5] & 0x00000002;
		 errorMargin = 10;
			
		 validInput = 0;
			
		 actionIndex = 0; //the index to access the gameSymbols; 0 = left. 1 = right; 2 = up; 3 = down; 4 = C; 5 = Z	
			
		while(!validInput){
		I2C_SendData(I2C1, slaveAddress, &Data_Send, 1);
		
		for(i = 0; i < 10000; i++){}
		
		I2C_ReceiveData(I2C1, slaveAddress, &*Data_Receive, 6);
		
		 xCoord = Data_Receive[0];
		 yCoord = Data_Receive[1];
		 buttonZ = Data_Receive[5] & 0x00000001;
		 buttonC = Data_Receive[5] & 0x00000002;
		 errorMargin = 10;
			
				//left
			if( (xCoord > (leftX - errorMargin)) && (xCoord < (leftX + errorMargin))){
				if( (yCoord > (centerY - errorMargin)) && (yCoord < (centerY + errorMargin))){
						validInput = 1;
						actionIndex = 0;
				}
			}
			
			//right
			if( (xCoord > (rightX - errorMargin)) && (xCoord < (rightX + errorMargin))){
				if( (yCoord > (centerY - errorMargin)) && (yCoord < (centerY + errorMargin))){
						validInput = 1;
						actionIndex = 1;
				}
			}		
			
			//up
			if( (xCoord > (centerX - errorMargin)) && (xCoord < (centerX + errorMargin))){
				if( (yCoord > (upY - errorMargin)) && (yCoord < (upY + errorMargin))){
						validInput = 1;
						actionIndex = 2;
				}
			}
				
			//down
			if( (xCoord > (centerX - errorMargin)) && (xCoord < (centerX + errorMargin))){
				if( (yCoord > (downY - errorMargin)) && (yCoord < (downY + errorMargin))){
						validInput = 1;
						actionIndex = 3;
				}
			}

			//C button
			if(buttonC == 0){
						validInput = 1;
						actionIndex = 4;
				}
			
			//Z button
			if(buttonZ == 0){
						validInput = 1;
						actionIndex = 5;
				}
		}

		int validCenter = 0;
		
		while(!validCenter){
		I2C_SendData(I2C1, slaveAddress, &Data_Send, 1);
		
		for(i = 0; i < 10000; i++){}
		
		I2C_ReceiveData(I2C1, slaveAddress, &*Data_Receive, 6);
		
		 xCoord = Data_Receive[0];
		 yCoord = Data_Receive[1];
		 buttonZ = Data_Receive[5] & 0x00000001;
		 buttonC = Data_Receive[5] & 0x00000002;
		 errorMargin = 10;
			
			if( (buttonC == 1 )|| (buttonZ == 1)){
				validCenter = 1;
			}
			
			if( (xCoord > (centerX - errorMargin)) && (xCoord < (centerX + errorMargin))){
				if( (yCoord > (centerY - errorMargin)) && (yCoord < (centerY + errorMargin))){
						validCenter = 1;
				}
			}
			

		}

//print user input	
	uint16_t noop = 0;
	for(int i = 0; i < 8; i++){
				uint8_t addr = MAX7219_Dot_Matrix_row[i];
				uint16_t fullData = addr << 8 | MAX7219_game_symbols[actionIndex][i];  //goes through each symbol in the game symbols
				printFirst(noop,fullData);
	}
	
	int delayTime = 500;
	
	//testing*****
			printf("-------\n");
	for(int d = 0; d < roundNum; d++){
 		sprintf((char*)message, "%d", game_Symbols[d]);
		printf(message);
		printf("\n");
	}
	
	//k goes through each symbol of the game_Symbols
	if(actionIndex == game_Symbols[k]){ //means user input is correct
		delay(delayTime);
		clearFirst();
	}
	else{
		return -1;
	}

}
return 0;

//		printf("--------------\n");
// 		sprintf((char*)message, "%d", Data_Receive[0]);
//		printf(message);
//		printf("\n");
//		sprintf((char*)message, "%d", Data_Receive[1]);
//		printf(message);
//		printf("\n");

//		sprintf((char*)message, "%d", buttonZ);
//		printf(message);
//		printf("\n");
//		sprintf((char*)message, "%d", buttonC);
//		printf(message);
//		printf("\n");
}

void gameOver(int roundNum){
	int tens = roundNum / 10;
	int ones = roundNum%10;
	
	char tensStr = tens + '0';
	char onesStr = ones + '0';
	
	scrollingText("GAME OVER");
	char scoreString [] = "YOUR FINAL SCORE IS ";
	if(tens != 0){
	strcat(scoreString,&tensStr);
	}
	strcat(scoreString,&onesStr);
	strcat(scoreString, " POINTS");
	scrollingText(scoreString);
}

void printXXXX(){
		uint16_t noop = 0;
		for(int i = 0; i < 8; i++){
				uint8_t data = MAX7219_Dot_Matrix_row[i];
				uint16_t fullData = data << 8 | MAX7219_mistake_symbols[0][i];
				uint16_t fullData2 = data << 8 | MAX7219_mistake_symbols[0][i];
				uint16_t fullData3 = data << 8 | MAX7219_mistake_symbols[0][i];
			  uint16_t fullData4 = data << 8 | MAX7219_mistake_symbols[0][i];
				printFirst(noop,fullData);
				printSecond(noop,fullData2);
				printThird(noop,fullData3);
				printFourth(noop,fullData4);
		}
		delay(500);
		clearAllLEDS();
}

void printMistakes(){
	uint16_t noop = 0;
	
	if(mistakes == 1){
	for(int i = 0; i < 8; i++){
		uint8_t addr = MAX7219_Dot_Matrix_row[i];
		uint16_t fullData = addr << 8 | MAX7219_mistake_symbols[1][i];
		printSecond(noop, fullData);
	}
	}
	else if (mistakes == 2){
		for(int i = 0; i < 8; i++){
			uint8_t addr = MAX7219_Dot_Matrix_row[i];
			uint16_t fullData = addr << 8 | MAX7219_mistake_symbols[2][i];
			printSecond(noop, fullData);
		}
	}
	
}

void userMistake(){
	printXXXX();
	mistakes += 1;
}



void initNunchuk(){
	uint8_t initData[3];
	initData[0] = 0xF0;
	initData[1] = 0x55;

		//uint8_t slaveAddress = 0x52 << 1;
	uint8_t slaveAddress = 82UL << 1;
	
	I2C_SendData(I2C1, slaveAddress, &*initData, 2); 

	
		initData[0] = 0xFB;
		initData[1] = 0x00;


		I2C_SendData(I2C1, slaveAddress, &*initData, 2); 
		printf("initialized successfully\n");
						
		char message[6];
		uint8_t Data_Receive[6];		
		I2C_ReceiveData(I2C1, slaveAddress, &*Data_Receive, 6);	
		sprintf((char*)message, "%d", Data_Receive[0]);
		printf(message);
		printf("\n");
		sprintf((char*)message, "%d", Data_Receive[1]);
		printf(message);
		printf("\n");
		sprintf((char*)message, "%d", Data_Receive[2]);
		printf(message);
		printf("\n");
		sprintf((char*)message, "%d", Data_Receive[3]);
		printf(message);
		printf("\n");
		sprintf((char*)message, "%d", Data_Receive[4]);
		printf(message);
		printf("\n");
		sprintf((char*)message, "%d", Data_Receive[5]);
		printf(message);
		printf("\n");
}

void initUSART(){
		UART2_Init();
		UART2_GPIO_Init();
		USART_Init(USART2);
}
