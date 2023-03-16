/*
 * seg_display.c
 *
 *  Created on: Dec 31, 2022
 *      Author: oneju
 */


#include "main.h"
#include "seg_display.h"
#include "stdio.h"


// Static Functions declaration
static void charMap(char order[], uint8_t SPIdata[], uint8_t myChar[]);
static void shiftArray(uint8_t buf[], char new);

// Data Size being transfered by SPI
#define BYTE_SIZE 1
#define ARRAYSIZE 95

// Used to print out string
String_Display_HandleTypeDef string;
TIM_HandleTypeDef * timer;

// Register and Segment Display ports and pins
uint16_t displayCathode[4];
GPIO_TypeDef * displayPort[4];
Register_HanldeTypeDef * reg;
uint16_t displayCathode[4];


uint8_t SPIdata[ARRAYSIZE];		// Decoded ASCII Data

// Mapped ASCII Segments		a - dp, a=msb and dp =lsb
uint8_t myChar[ARRAYSIZE]= {	0x00,	// space = 0
								0x41,	// ! = b,dp
								0x44,	// " = b,f
								0x00,	// # = null
								0x00,	// $ = null
								0x00,	// % = null
								0x00,	// & = null
								0x04,	// ' = f
								0x9c,	// ( = a,d,e,f
								0xf0,	// ) = a,b,c,d
								0x00,	// * = null
								0x00,	// + = null
								0x08,	// , = e
								0x02,	// - = g
								0x01,	// . = dp
								0x4a,	// / = b,d,e,g
								0xfc,	// 0
								0x60,	// 1
								0xda,	// 2
								0xf2,	// 3
								0x66,	// 4
								0xb6,   // 5
								0xbe,	// 6
								0xe0,	// 7
								0xfe,	// 8
								0xf6,	// 9
								0x00,	// : = null
								0x00,	// ; = null
								0x86,	// < = a,f,g
								0x12,	// = = d,g
								0xc2,	// > = a,b,g
								0xca,	// ? = a,b,g,e
								0x00,	// @ = null
								0xee,	// A = a,b,c,e,f,g
								0xfe,	// B = a,b,c,d,e,f,g
								0x9c,	// C = a,d,e,f
								0x7a,	// D = b,c,d,e,g
								0x9e,	// E = a,d,e,f,g
								0x8e,	// F = a,e,f,g
								0xbe,	// G = a,d,e,f,g
								0x6e,	// H = b,c,e,f,g
								0x0c,	// I = f,e
								0x78,	// J = b,c,d,e
								0x9c,	// K = a,d,e,f
								0x1c,	// L = d,e,f
								0x00,	// M = null
								0xec,	// N = a,b,c,e,f
								0xfc,	// O = a,b,c,d,e,f
								0xce,	// P = a,b,e,f,g
								0xe6,	// Q = a,b,c,f,g
								0x0a,	// R = e,g
								0xb6,	// S = a,c,d,f,g
								0xe0,	// T = a,b,c
								0x7c,	// U = b,c,d,e,f
								0x7c,	// V = b,c,d,e,f
								0x00,	// W = null
								0x6e,	// X = b,c,e,f,g
								0x76,	// Y = b,c,d,f,g
								0xda,	// Z = a,b,d,e,g
								0x9c,	// [ = a,d,e,f
								0x26,	// \ = c,f,g
								0xf0,	// ] = a,b,c,d
								0xc4,	// ^ = a,b,f
								0x10,	// _ = d
								0x04,	// ` = f
								0xee,	// a = a,b,c,e,f,g
								0x3e,	// b = c,d,e,f,g
								0x1a,	// c = d,e,g
								0x7a,	// d = b,c,d,e,g
								0xde,	// e = a,b,d,e,f,g
								0x8e,	// f = a,e,f,g
								0xf6,	// g = a,b,c,d,f,g
								0x2e,	// h = c,e,f,g
								0x08,	// i = e
								0x78,	// j = b,c,d,e
								0x1a,	// k = d,e,g
								0x0c,	// l = e,f
								0x00,	// m = null
								0x2a,	// n = c,e,g
								0x3a,	// o = c,d,e,g
								0xce,	// p = a,b,e,f,g
								0xe6,	// q = a,b,c,f,g
								0x0a,	// r = e,g
								0xb6,	// s = a,c,d,f,g
								0x62,	// t = b,c,g
								0x38,	// u = c,d,e
								0x38,	// v = c,d,e
								0x00,	// w = null
								0x6e,	// x = b,c,d,e,f
								0x76,	// y = b,c,d,f,g
								0xda,	// z = a,b,d,e,g
								0x9c,	// { = a,d,e,f
								0x0c,	// | = f,e
								0xf0,	// } = a,b,c,d
								0x80};	// ~ = a;

// ***************************************** Public functions ******************************************
Seg_Display_HandleTypeDef createDisplay(GPIO_TypeDef * d1_port, uint16_t d1_pin,
										GPIO_TypeDef * d2_port, uint16_t d2_pin,
										GPIO_TypeDef * d3_port, uint16_t d3_pin,
										GPIO_TypeDef * d4_port, uint16_t d4_pin,
										char * order, TIM_HandleTypeDef * timer)
{
	Seg_Display_HandleTypeDef display;

	display.d1_port = d1_port;
	display.d1_pin = d1_pin;

	display.d2_port = d2_port;
	display.d2_pin = d2_pin;

	display.d3_port = d3_port;
	display.d3_pin = d3_pin;

	display.d4_port = d4_port;
	display.d4_pin = d4_pin;


	display.order = order;
	display.timer = timer;

	return display;
}

Register_HanldeTypeDef registerCreate(SPI_HandleTypeDef hspi1,
										GPIO_TypeDef * stcp_port, uint16_t stcp_pin,
										GPIO_TypeDef * mr_port, uint16_t mr_pin)
/* SPI_HandleTypeDef structure that contains the configuration information for SPI module
 * stcp_port is the Storage register clock port
 * stcp_pin is the Storage register clock pin
 * mr_port is the master clear port
 * mr_pin is the master clear pin
 */
{
	Register_HanldeTypeDef reg;
	reg.hspi1 = hspi1;

	reg.stcp_port = stcp_port;
	reg.stcp_pin = stcp_pin;

	reg.mr_port = mr_port;
	reg.mr_pin = mr_pin;

	return reg;
}

void sendSPIdata(Register_HanldeTypeDef * reg, uint8_t data[])
/* Register_HandleTypeDef structure that contains the configuration information for register module.
 * Data address to data buffer
 */
{
	// clear the shift register buffer
	HAL_GPIO_WritePin(reg->mr_port, reg->mr_pin, GPIO_PIN_RESET);	// Low Active

	// open shift register buffer
	HAL_GPIO_WritePin(reg->mr_port, reg->mr_pin, GPIO_PIN_SET);

	// transmit data
	HAL_SPI_Transmit(&reg->hspi1, &data[0], BYTE_SIZE, 10); 		// Controls SPI1_MOSI and SPI1_SCLK

	// parallel load into storage register
	HAL_GPIO_WritePin(reg->stcp_port, reg->stcp_pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(reg->stcp_port, reg->stcp_pin, GPIO_PIN_RESET);
}

void seg_display_init(Seg_Display_HandleTypeDef * display, Register_HanldeTypeDef * regi){
	displayCathode[0] = display->d1_pin;
	displayCathode[1] = display->d2_pin;
	displayCathode[2] = display->d3_pin;
	displayCathode[3] = display->d4_pin;

	displayPort[0] = display->d1_port;
	displayPort[1] = display->d2_port;
	displayPort[2] = display->d3_port;
	displayPort[3] = display->d4_port;

	timer = display->timer;
	HAL_TIM_Base_Start(timer);

	for(int i = 0; i < 4; i++){
		HAL_GPIO_WritePin(displayPort[i], displayCathode[i], GPIO_PIN_SET);
	}
	reg = regi;

	charMap(display->order, &SPIdata[0], &myChar[0]);

	return;
}

void displayInt(uint8_t * buffer)
/* Takes Array Size 4
 * Sends data to SPI to print onto 4-digit 7-Segment Display */
{
	for(uint8_t i = 0; i < 4; i++){
			sendSPIdata(reg,&buffer[3-i]);											// index (3-i) in array is displayed
			HAL_GPIO_WritePin(displayPort[i], displayCathode[i], GPIO_PIN_RESET);	// index (3-i) in cathode is GND
			HAL_Delay(4); 															// 4 ms each digit = 16 ms cycle = 60 Hz
			HAL_GPIO_WritePin(displayPort[i], displayCathode[i], GPIO_PIN_SET);		// index (3-i) in cathode is Set
		}
	return;
}

uint8_t printChar(char * str)
/* Takes Array of Char
 * Returns 1 if Entire Array was printed once */
{
	//String initialization
	if(string.originalPointer != str){	// if the str isn't orignalPointer then it doesn't need to be initialized
		string.stringPointer = str;		// points to current char
		string.basePointer = str;		// used if char is '\t'
		string.originalPointer = str;	// used to check if function parameter has changed

		string.printed = 0;
		string.characterCount =0;

		string.timerVal = __HAL_TIM_GET_COUNTER(timer);
		string.tDelay =2000;			// initial Delay is 2 seconds

		//clears buffer
		for(int i = 0; i < 4; i++){
			string.buffer[i] = 0;
		}

		// Fills the buffer with While loop. While buffer not full and the string isn't finished.
		while(string.characterCount < 4 && string.printed != 1)
		{
			uint8_t zeroBuffer;
			switch(*string.stringPointer){
			case '\0' :
				string.printed = 1;
				break;
			case '\t' :
				zeroBuffer = (4-(string.characterCount)%4);
				string.characterCount+= zeroBuffer;
				string.stringPointer++;
				for(int i = 0; i < zeroBuffer; i++){
					shiftArray(string.buffer, ' ');
				}
				break;
			case '\n' :
				string.characterCount = 4;
				break;
			case '.' :
				if(string.characterCount != 0 && (string.buffer[3]&SPIdata[(int)'.'-32]) != SPIdata[(int)'.'-32]) // if previous buffer has period then break
					string.buffer[3] |= SPIdata[(int)'.'-32];
				else{
					shiftArray(string.buffer, '.');
					string.characterCount++;
				}
				string.stringPointer++;
				break;

			default :
				shiftArray(string.buffer, *string.stringPointer);
				string.stringPointer++;
				string.characterCount++;
			}
		}

	}

	if((string.characterCount > 4 || *(string.stringPointer) != '\0') &&
		(__HAL_TIM_GET_COUNTER(timer) - string.timerVal > string.tDelay))
	{
		//case statement
		switch(*string.stringPointer){
		case '\0' :
			if(string.delayNum == 2){
				string.stringPointer = string.basePointer;
				string.delayNum = 0;
				string.characterCount = 0;
				string.printed = 1;
			}
			else{
			shiftArray(string.buffer,' ');
			string.delayNum++;
			}
			break;

		case '\n' :
			string.stringPointer++;
			string.basePointer = string.stringPointer;
			string.characterCount = 0;
			break;

		case '\t' :
			if(4-(string.characterCount)%4 == string.delayNum){
				string.delayNum = 0;
				string.stringPointer++;
			}
			else{
				shiftArray(string.buffer,' ');
				string.delayNum++;
				string.characterCount++;
			}
			break;

		case '.' :
			if((string.buffer[3]&SPIdata[(int)'.'-32]) != SPIdata[(int)'.'-32]) // if previous buffer has period then break
				string.buffer[3] |= SPIdata[(int)'.'-32];
			else{
				shiftArray(string.buffer, '.');
				string.characterCount++;
			}
			string.stringPointer++;
			break;
		default :
			shiftArray(string.buffer,*string.stringPointer);
			string.stringPointer++;
			string.characterCount++;
		}
		string.tDelay = 400;
		if(string.characterCount != 0)
			string.timerVal = __HAL_TIM_GET_COUNTER(timer);
	}
	displayInt(string.buffer);
	return string.printed;
}


// ***************************************** Static Functions ******************************************
static void charMap(char order[], uint8_t SPIdata[], uint8_t myChar[]){

	// Converting order
	int numOrder[8];
	int num;
	for(int i =0; i < 8; i++){
        num = (int) order[i] - 97;
        switch(num){
            case 0:
                numOrder[i] = 7;
                break;
            case 1:
                numOrder[i] = 6;
                break;
            case 2:
                numOrder[i] = 5;
                break;
            case 3:
                numOrder[i] = 4;
                break;
            case 4:
                numOrder[i] = 3;
                break;
            case 5:
                numOrder[i] = 2;
                break;
            case 6:
                numOrder[i] = 1;
                break;
            case 15:
                numOrder[i] = 0;
                break;
            default:
                numOrder[i] = -1;
        }
    }

	// Mapping array
	int temp = 0;
	int a = 0;
	int b = 0;
	for(int i = 0; i < ARRAYSIZE; i++){
			temp = 0;
			for(int j = 0; j < 8; j++){
				a = 1;
				a = a << numOrder[j];
				b = myChar[i];
				if((b&=a) == a){
					temp = temp << 1;
					temp += 1;
				}
				else
					temp = temp << 1;
			}
			SPIdata[i] = temp;
	}

	return;
}

static void shiftArray(uint8_t buf[], char new){
	for(int i = 0; i < 3; i++){
		buf[i] = buf[i+1];
	}
	buf[3] = SPIdata[(uint8_t)new - 32];
	return;
}



