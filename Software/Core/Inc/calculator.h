/*
 * calculator.h
 *
 *  Created on: Dec 5, 2020
 *      Author: jayhamlin
 */

#ifndef INC_CALCULATOR_H_
#define INC_CALCULATOR_H_

void	DoInitializeCalculator(void);
void	DoRunCalculator(uint8_t	keycode);
void	DisplayNotification(char	*str, int16_t	timoutms);

void DecimalNumberFromString(decimal_t *dec,char *str);
void DecimalNumberToDisplayString(char *str,decimal_t *dec);

extern	decimal_t	registerT;
extern	decimal_t	registerZ;
extern	decimal_t	registerX;
extern	decimal_t	registerY;

extern	char		notificationString[];

#define		RADIX_BINARY		2
#define		RADIX_OCTAL			8
#define		RADIX_DECIMAL		10
#define		RADIX_HEXADECIMAL	16

#define		DISP_FORMAT_FIXED	1		// fixed point or integer
#define		DISP_FORMAT_SCI		2		// scientific notation
#define		DISP_FORMAT_ENG		3		// engineering notation.


typedef struct DisplayMode {
	char		radix;		//2,8,10,16
	char		format;
	char		points;
	char		editMode;
	char		update;
} display_t;

extern display_t	display;

#endif /* INC_CALCULATOR_H_ */
