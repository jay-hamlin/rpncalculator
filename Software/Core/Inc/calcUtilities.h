/*
 * calcUtilities.h
 *
 *  Created on: Dec 8, 2020
 *      Author: jayhamlin
 */

#ifndef INC_CALCUTILITIES_H_
#define INC_CALCUTILITIES_H_

// stack manipulation
void	CalcPushStack(decimal_t *dec);
void	CalcPopStack(decimal_t *dec);
void	CalcSwapXY(void);
void	CalcROLStack(void);

// printing utils
void 	PrintDecimal_tDebug(char *str,decimal_t *dec);
void    PrintNibbles(uint8_t *sigs,int16_t size);

// digit manipulation
uint8_t GetBCDNibble(uint8_t *sigs,int16_t    nib);
void    SetBCDNibble(uint8_t *sigs,uint8_t value,int16_t    nib);
void    ShiftSigNibbles(uint8_t *sigs,int16_t    shift,int16_t    size);

#endif /* INC_CALCUTILITIES_H_ */
