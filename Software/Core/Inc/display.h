/*
 * dsky.h
 *
 *  Created on: Nov 27, 2020
 *      Author: jayhamlin
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

// Defines for the display

// IS31FL3730 digit driver I2C addresses
// On I2C1
#define	LD1_LD2_ADDR	0xC0	// 1100 0000
#define	LD3_LD4_ADDR	0xC6	// 1100 0110
#define	LD5_LD6_ADDR	0xC2	// 1100 0010
// On I2C2
#define	LD7_LD8_ADDR	0xC0	// 1100 0000
#define	LD9_LD10_ADDR	0xC6	// 1100 0110
#define	LD11_LD12_ADDR	0xC2	// 1100 0010

// IS31FL3730 digit driver registers
#define IS31_REG_CONFIG			0x00
#define IS31_REG_UPDATE			0x0C
#define IS31_REG_OPTIONS		0x0D
#define IS31_REG_MATRIX_1		0x01
#define IS31_REG_MATRIX_2		0x0E
#define IS31_REG_BRIGHTNESS		0x19

#define IS31_REG_CONFIG_VALUE	0x18	//0b00011000	// turn on both matrix 1 and matrix 2 and 8x8 matrix mode
#define IS31_REG_OPTS_VALUE		0x0E	//0b00001110  // 1110 = 35mA, 0000 = 40mA
#define IS31_REG_BRIGHTNESS_VALUE	127  // 0-127


#define MATRIX_1 				1
#define MATRIX_2 				2
#define I2C_1	 				1
#define I2C_2	 				2

#define INDICATOR_D4	6 	//	D4 - digit 6
#define INDICATOR_D3	4 	//	D3 - digit 4
#define INDICATOR_D2	2 	//	D2 - digit 2
#define INDICATOR_D1	0 	//	D1 - digit 0


typedef struct DigitAddresses {
	uint8_t		i2caddr;
	uint8_t		i2cbus;
	uint8_t		matrix;
	uint8_t		empt;
} DigitAddress;

#define	DIGIT_COUNT		12

// Exported prototypes for display
void	DisplayInitialize(void);
void	DisplayWriteChar(int16_t digit, char ch);
void	DisplayWriteString(char *pStr);
void	DisplaySetBrightness(int16_t digit, int8_t br);
void 	SetDecimalPoint(int16_t digit,char clearOthers);
void	SetIndicatorLED(int16_t led, char onoff);

#endif /* INC_DISPLAY_H_ */
