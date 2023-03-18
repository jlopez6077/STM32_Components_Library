/*
 * seg_display.h
 *
 *  Created on: Dec 31, 2022
 *      Author: oneju
 */

#ifndef __SEG_DISPLAY_H_
#define __SEG_DISPLAY_H_



/************************************** Seg_Display typedefs **************************************/
typedef struct {
	GPIO_TypeDef * d1_port;
	uint16_t d1_pin;

	GPIO_TypeDef * d2_port;
	uint16_t d2_pin;

	GPIO_TypeDef * d3_port;
	uint16_t d3_pin;

	GPIO_TypeDef * d4_port;
	uint16_t d4_pin;

	TIM_HandleTypeDef * timer;

	char * order;
}Seg_Display_HandleTypeDef;

/************************************** String_Display typedefs **************************************/
typedef struct{
	char * stringPointer;		// points to next char
	char * basePointer;			// points to the first char in a new line
	char * originalPointer;		// points to the first char
	uint8_t buffer[4];			// what is being displayed

	uint8_t printed;			// printed = 1 if char is done printing

	uint8_t characterCount;		// used to determine number of spaces for tab
	uint8_t delayNum;			// used to determine number of iterations between changing stringPointer

	uint16_t timerVal;			// stores the last time buffer was changed milliseconds
	uint16_t tDelay;			// time between buffer changed in milliseconds

}String_Display_HandleTypeDef;
// ****************************************** Register typedef ******************************************
typedef struct{
	SPI_HandleTypeDef hspi1;

	GPIO_TypeDef * stcp_port;
	uint16_t stcp_pin;

	GPIO_TypeDef * mr_port;
	uint16_t mr_pin;
}Register_HanldeTypeDef;


/*************************************** Public functions ***************************************/
Seg_Display_HandleTypeDef createDisplay(GPIO_TypeDef * d1_port, uint16_t d1_pin,
										GPIO_TypeDef * d2_port, uint16_t d2_pin,
										GPIO_TypeDef * d3_port, uint16_t d3_pin,
										GPIO_TypeDef * d4_port, uint16_t d4_pin,
										char * order, TIM_HandleTypeDef * timer);
Register_HanldeTypeDef registerCreate(SPI_HandleTypeDef hspi1,
										GPIO_TypeDef * stcp_port, uint16_t stcp_pin,
										GPIO_TypeDef * mr_port, uint16_t mr_pin);

void sendSPIdata(Register_HanldeTypeDef * reg, uint8_t data[]);
void seg_display_init(Seg_Display_HandleTypeDef * display, Register_HanldeTypeDef * regi);
void displayInt(uint8_t * buffer);
uint8_t printChar(char * str);

#endif /* __SEG_DISPLAY_H_ */
