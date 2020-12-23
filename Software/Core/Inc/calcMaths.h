/*
 * calcMaths.h
 *
 *  Created on: Dec 22, 2020
 *      Author: jayhamlin
 */

#ifndef INC_CALCMATHS_H_
#define INC_CALCMATHS_H_

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	Decimal number encoding
 * 	char		sign  --> bit 0 is the significand sign bit. (1=negative)
 * 	int16_t		exp --> The exponent is stored as a signed binary int.
 * 	uint8_t		sig[BCD_DIGIT_BYTES] --> these are all the significant digits done BCD style.
 * 						Each nibble represents a decimal digit  from 0-9. Big Endian.
 *
 * 	Numbers are stored as scientific notation.
 * 	Some examples:
 * 		3.141592653589793
 * 				->sig = 3141592653589793 encoded 0x31,0x41,0x59,0x26,0x53,0x58,0x97,0x93
 * 				->exp = 0
 * 				->signs = 0
 *		31415.92653589793
 * 				->sig = 3141592653589793 encoded 0x31,0x41,0x59,0x26,0x53,0x58,0x97,0x93
 * 				->exp = 4
 * 				->signs = 0
 *		0.0003141592653589793
 * 				->sig = 3141592653589793 encoded 0x31,0x41,0x59,0x26,0x53,0x58,0x97,0x93
 * 				->exp = 4
 * 				->sign = DECIMAL_SIGN_NEGATIVE
 *		-0.0003141592653589793
 * 				->sig = 3141592653589793 encoded 0x31,0x41,0x59,0x26,0x53,0x58,0x97,0x93
 * 				->exp = -4
 * 				->sign = DECIMAL_SIGN_NEGATIVE
 *
 *
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/

#define        NOT_A_NUMBER             0xFF
#define        DECIMAL_SIGN_NEGATIVE    0x01
#define        BCD_DIGIT_COUNT        16
#define        BCD_DIGIT_BYTES        (BCD_DIGIT_COUNT/2)

typedef struct DecimalNumber {
    char        sign;
    int16_t     exp;            // a signed binary
    uint8_t     sig[BCD_DIGIT_BYTES];
} decimal_t;

// pi = 3.14159265358979323846264338327950288419716939937510
extern  const   decimal_t    pi;

void    CalcAdd(decimal_t *res,decimal_t *x,decimal_t *y);
void    CalcSubtract(decimal_t *res,decimal_t *x,decimal_t *y);
void    CalcMultiply(decimal_t *res,decimal_t *x,decimal_t *y);
void    CalcDivide(decimal_t *quotient,decimal_t *x,decimal_t *y);


#endif /* INC_CALCMATHS_H_ */
