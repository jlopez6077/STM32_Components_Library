/*
 * register.h
 *
 *  Created on: Dec 25, 2022
 *      Author: jlopez6077
 *
 * This is for the 74HC595 Shift Register
 * Utilizes SPI
 * Mode = Transmit Only Master
 * Data Size = 8 bits
 * First Bit = MSB First
 */


#ifndef REGISTER_H
#define REGISTER_H

// Data Size being transfered by SPI
#define BYTE_SIZE 1




// ****************************************** Register typedef ******************************************
typedef struct{
	SPI_HandleTypeDef hspi1;

	GPIO_TypeDef * stcp_port;
	uint16_t stcp_pin;

	GPIO_TypeDef * mr_port;
	uint16_t mr_pin;
}Register_HanldeTypeDef;



// ***************************************** Public functions ******************************************
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

#endif /* __REGISTER_H_ */
