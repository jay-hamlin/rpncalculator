/*
 * dsky.c
 *
 *  Created on: Nov 27, 2020
 *      Author: jayhamlin
 */
#include "main.h"
#include "rpnHwDefs.h"
#include "display.h"
#include "charMatrix.h"
#include "string.h"

#define PROTO_DISP_ROTATE	1

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	DigitAddress: digitAddress
 * 	an array containg parameters for each digit.
 * 	uint8_t		i2caddr;  --> I2C address of the driver chip
 *	uint8_t		i2cbus;	  --> bus 1 or bus 2
 *	uint8_t		matrix;   --> IS31FL3730 matrix A or matrix B
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
DigitAddress digitAddress[DISPLAY_DIGIT_COUNT] = {
	{LD1_LD2_ADDR,I2C_1,IS31_REG_MATRIX_2,0},
	{LD1_LD2_ADDR,I2C_1,IS31_REG_MATRIX_1,0},
	{LD3_LD4_ADDR,I2C_1,IS31_REG_MATRIX_2,0},
	{LD3_LD4_ADDR,I2C_1,IS31_REG_MATRIX_1,0},
	{LD5_LD6_ADDR,I2C_1,IS31_REG_MATRIX_2,0},
	{LD5_LD6_ADDR,I2C_1,IS31_REG_MATRIX_1,0},
	{LD1_LD2_ADDR,I2C_2,IS31_REG_MATRIX_2,0},
	{LD1_LD2_ADDR,I2C_2,IS31_REG_MATRIX_1,0},
	{LD3_LD4_ADDR,I2C_2,IS31_REG_MATRIX_2,0},
	{LD3_LD4_ADDR,I2C_2,IS31_REG_MATRIX_1,0},
	{LD5_LD6_ADDR,I2C_2,IS31_REG_MATRIX_2,0},
	{LD5_LD6_ADDR,I2C_2,IS31_REG_MATRIX_1,0}
};

// global variables;
int16_t	gDecimalPoints;
int16_t	gIndicators;

// Private prototypes for display
I2C_HandleTypeDef *I2CHandlePtr(int16_t	digit);
int16_t MatrixFlip(uint8_t *pData,uint8_t *pMatrix);
char 	GetDecimalPoint(int16_t digit);
char 	GetIndicatorLED(int16_t digit);


// Private prototypes for keyboard

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	I2CHandlePtr - returns the i2c handle for the I2C buss of this digit
 * 	The left 6 digits are on bus 2, the right 6 digits are on bus 1
 *  INPUT: digit number.  0 is the right most digit(LSB), 11 is the left digit.
 *  OUTPUT a I2C_HandleTypeDef of the I2C bus.
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
I2C_HandleTypeDef *I2CHandlePtr(int16_t	digit)
{
	I2C_HandleTypeDef *phi2c;

	phi2c=0L;

	if(digitAddress[digit].i2cbus == I2C_1){
		phi2c = &hi2c1;
	}	else if(digitAddress[digit].i2cbus == I2C_2){
		phi2c = &hi2c2;
	}
	return(phi2c);
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	GetDecimalPoint - returns true if this digit has he decimal point set
 *  INPUT: digit number.  0 is the right most digit(LSB), 11 is the left digit.
 *  OUTPUT: true or false
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
char GetDecimalPoint(int16_t digit)
{
	char	isdp;
	uint16_t	mask;

	mask = 0x0001;
	mask<<= digit;

	if(gDecimalPoints&mask)
		isdp = 1;
	else
		isdp = 0;

	return(isdp);
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	SetDecimalPoint - returns true if this digit has he decimal point set
 *  INPUT: digit number.  0 is the right most digit(LSB), 11 is the left digit.
 *  		clearOthers.  set to true if you want only 1 dp set at a time.
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void SetDecimalPoint(int16_t digit,char clearOthers)
{
	uint16_t	mask;

	mask = 0x0001;
	mask<<= digit;
	mask &= 0x0FFF;

	if(clearOthers)
		gDecimalPoints = mask;
	else
		gDecimalPoints |= mask;

}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	SetIndicatorLED - Turns on one of the 4 indicator LEDs below the display
 *  INPUT: led number.  may be on of the following:
 *  	INDICATOR_D4	6
 *  	INDICATOR_D3	4
 *  	INDICATOR_D2	2
 *  	INDICATOR_D1	0
 *  	also input true or false to turn the LED on or off.
 *
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	SetIndicatorLED(int16_t led, char onoff)
{
	// byte 6, bit 0 of the  selected digit
	// led is the digit number associated with the led
	//	D4 - digit 6
	//	D3 - digit 4
	//	D2 - digit 2
	//	D1 - digit 0
	uint16_t	mask;

	mask = 0x0001;
	mask<<= led;
	mask &= 0x0FFF;

	if(onoff)
		gIndicators |= mask;
	else
		gIndicators &= (~mask);

#if 0
	// Need to work out how to set the indicators because the matrix has to be rewritten
	// and we don't have an offscreen bitmap. Need to make one.
	// until then, the indicators get set the next time the digit is written.
		I2C_Transmit(&hi2c, addr, pData,size, 1000);

		pData[0] = IS31_REG_UPDATE;
		pData[1] = 0x01;
		I2C_Transmit(&hi2c, addr, pData,2, 1000);
#endif
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	GetIndicatorLED - returns true if this digit has the indicator LED set
 *  INPUT: digit number.  0 is the right most digit(LSB), 11 is the left digit.
 *  OUTPUT: true or false
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
char GetIndicatorLED(int16_t digit)
{
	char	isdp;
	uint16_t	mask;

	mask = 0x0001;
	mask<<= digit;

	if(gIndicators&mask)
		isdp = 1;
	else
		isdp = 0;

	return(isdp);
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	DisplayInitialize - call from main to set up the IS31FL3730 digit drivers.
 *  INPUT: none
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	DisplayInitialize(void)
{
	I2C_HandleTypeDef *phi2c;
	int16_t		digit;
	uint16_t	addr;
	uint16_t 	size;
	uint8_t 	pData[4];

	gDecimalPoints =0;
	gIndicators =0;

	digit = 0;
	while(digit < DISPLAY_DIGIT_COUNT){
		phi2c = I2CHandlePtr(digit);
		if((phi2c) && (digitAddress[digit].matrix == IS31_REG_MATRIX_1)){
			// this actually inits 1 and 2 so we skip all MATRIX_2 calls
			addr = digitAddress[digit].i2caddr;
			pData[0] = IS31_REG_CONFIG;
			pData[1] = IS31_REG_CONFIG_VALUE;
			size = 2;
			HAL_I2C_Master_Transmit(phi2c, addr, pData, size, 1000);
			pData[0] = IS31_REG_OPTIONS;
			pData[1] = IS31_REG_OPTS_VALUE;
			size = 2;
			HAL_I2C_Master_Transmit(phi2c, addr, pData, size, 1000);
			pData[0] = IS31_REG_BRIGHTNESS;
			pData[1] = IS31_REG_BRIGHTNESS_VALUE;
			size = 2;
			HAL_I2C_Master_Transmit(phi2c, addr, pData, size, 1000);
		}
		digit++;		// 2 digits per chip
	}
}



/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	DisplayWriteChar - writes an ascii char to the display digit
 *  INPUT: digit and char
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	DisplayWriteChar(int16_t digit, char ch)
{
	I2C_HandleTypeDef *phi2c;
	uint16_t	addr;
	uint8_t 	pData[12];

	uint8_t		*chPtr;
	int16_t		chr,size;

#if PROTO_DISP_ROTATE
	digit = 11-digit;
#endif

	phi2c = I2CHandlePtr(digit);
	if(phi2c){
		addr = digitAddress[digit].i2caddr;

		chr = ch;

		if((chr>=ASCII_START) && (chr<=ASCII_END)){
			chr = (chr-ASCII_START)*BYTES_PER_CHAR;
			chPtr = (uint8_t*)&asciiFont[chr];

			pData[0] = digitAddress[digit].matrix;
			size = MatrixFlip((uint8_t *)pData,chPtr);

			if(GetDecimalPoint(digit))
				pData[7] |= 0x40;
			else
				pData[7] &= 0xBF;

#if PROTO_DISP_ROTATE
			if((digit==0)|| (digit==2)|| (digit==4)|| (digit==6)){
#else
			{
#endif
				if(GetIndicatorLED(digit))
					pData[7] |= 0x01;
				else
					pData[7] &= 0xFE;
			}

			HAL_I2C_Master_Transmit(phi2c, addr, pData,size, 1000);

			pData[0] = IS31_REG_UPDATE;
			pData[1] = 0x01;
			HAL_I2C_Master_Transmit(phi2c, addr, pData,2, 1000);
		}
	}
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	DisplayWriteStr - writes a string to the display
 * 					- clears unused digits
 * 					- truncates string if longer than display area
 *  INPUT: pointer to a string
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	DisplayWriteString(char *pStr)
{
	int16_t		len = strlen(pStr);
	int16_t		i,digit;
	char		ch;

	digit=12;
	i=0;
	while(i < digit){
		if(i<len)
			ch=pStr[i];
		else
			ch = ' ';

		DisplayWriteChar(i,ch);

		i++;
	}
}

#if PROTO_DISP_ROTATE
uint8_t		BitFlipper(uint8_t in);
uint8_t		BitFlipper(uint8_t in)
{
	uint8_t	out;
	int16_t	i;

	out =0;
	i=0;
	while(i<7){
		out<<=1;
		out |= (in&0x01);
		in>>=1;
		i++;
	}

	return(out);
}
#endif

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	MatrixFlip - swaps the rows and columns for digits on MATRIX_1
 *  INPUT: output data pointer and charMatrix pointer
 *  OUTPUT: size = number of byte to write on I2C
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
int16_t MatrixFlip(uint8_t *pData,uint8_t *pMatrix){
	int16_t i,size;
	uint8_t	*bPtr;
	uint8_t	c,k;
	int16_t bit,byt;

	if(pData[0]==IS31_REG_MATRIX_2){
		// This is a vertical digit.
		// each byte in pMatrix is a column
		// there are 5 columns
		size = 8;  // data bytes + 1 for CMD byte
		i=0;
		while(i<size){
			if(i<BYTES_PER_CHAR)
#if PROTO_DISP_ROTATE
				pData[i+1] = BitFlipper(pMatrix[4-i]);
#else
			pData[i+1] = pMatrix[i];
#endif
			else
				pData[i+1] = 0;
			i++;
		}

	} else {
		// This is a horizontal digit.
		// each byte in pMatrix is a column
		// there are 5 columns
		size = 8;  // data bytes + 1 for CMD byte

		bPtr = &pData[1];
		i=0;
		while(i<7){	// clear all
			bPtr[i++]=0;
		}

		byt=0;
		while(byt<5){
#if PROTO_DISP_ROTATE
			c = pMatrix[byt];
			bit =0;
			while(bit<7){
				if(c&0x01){
					// set bit number byt in byte number bit.  What?
					k = 0x01;
					k<<=(4-byt);
					bPtr[6-bit] |= k;
				}
				c>>=1;
				bit++;
			}
			byt++;
		}
#else
			c = pMatrix[byt];
			bit =0;
			while(bit<7){
				if(c&0x01){
					// set bit number byt in byte number bit.  What?
					k = 0x01;
					k<<=byt;
					bPtr[bit] |= k;
				}
				c>>=1;
				bit++;
			}
			byt++;
		}
#endif
	}

	return(size);
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	DisplaySetBrightness - swaps the rows and columns for digits on MATRIX_1
 *  INPUT: digit and brightness value.
 *    digit from 0-11
 *    brightness from 0-127
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	DisplaySetBrightness(int16_t digit, int8_t br)
{
	I2C_HandleTypeDef *phi2c;
	uint16_t	addr;
	uint16_t 	size;
	uint8_t 	pData[4];

	phi2c = I2CHandlePtr(digit);
	if(phi2c){
		addr = digitAddress[digit].i2caddr;

		pData[0] = IS31_REG_BRIGHTNESS;
		pData[1] = IS31_REG_BRIGHTNESS_VALUE;
		size = 2;
		HAL_I2C_Master_Transmit(phi2c, addr, pData, size, 1000);
	}
}
