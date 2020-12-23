/*
 * calcUtilities.c
 *
 *  Created on: Dec 8, 2020
 *      Author: jayhamlin
 */
#ifdef MAKE_FOR_MAC
#include "tests.h"
#else
#include "main.h"
#endif

#include <stdio.h>
#include <string.h>

#include "calcMaths.h"
#include "calcUtilities.h"
#include "calculator.h"


/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	CalcPushStack - push a new value onto the stack. Everything gets bumped up 1
 *  INPUT:  pointer to a decimal_t with the new value for X
 *  OUTPUT:
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	CalcPushStack(decimal_t *dec)
{
	//memcpy(*destination, *source, size);
    memcpy(&registerT,&registerZ,sizeof(decimal_t));
    memcpy(&registerZ,&registerY,sizeof(decimal_t));
    memcpy(&registerY,&registerX,sizeof(decimal_t));
    memcpy(&registerX,dec,sizeof(decimal_t));

}


/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	CalcPopStack - pops X off the stack, shifts everything else down one.
 *  INPUT:  pointer to a decimal_t to receive the value
 *  OUTPUT:
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	CalcPopStack(decimal_t *dec)
{
	//memcpy(*destination, *source, size);
    memcpy(dec,&registerX,sizeof(decimal_t));
    memcpy(&registerX,&registerY,sizeof(decimal_t));
    memcpy(&registerY,&registerZ,sizeof(decimal_t));
    memcpy(&registerZ,&registerT,sizeof(decimal_t));

}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	CalcSwapXY - exchanges x and y registers
 * 		Y<-->X
 *  INPUT:  none
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	CalcSwapXY(void)
{
	decimal_t	temp;

	//memcpy(*destination, *source, size);
    memcpy(&temp,&registerY,sizeof(decimal_t));
    memcpy(&registerX,&temp,sizeof(decimal_t));

}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	CalcROLStack - does a stack roll
 * 		Y-->X, Z-->Y, T-->Z, X-->T,
 *  INPUT:  none
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	CalcROLStack(void)
{
	decimal_t	temp;

	//memcpy(*destination, *source, size);
    memcpy(&temp,&registerX,sizeof(decimal_t));
    memcpy(&registerX,&registerY,sizeof(decimal_t));
    memcpy(&registerY,&registerZ,sizeof(decimal_t));
    memcpy(&registerZ,&registerT,sizeof(decimal_t));
    memcpy(&registerT,&temp,sizeof(decimal_t));

}


/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	GetBCDNibble - reads a BCD digits from sig string
 *  INPUT:  a pointer to the digits and index.
 *  		index 0 is the MSD, index=15 is the LSD
 *  OUTPUT: BCD value
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
uint8_t GetBCDNibble(uint8_t *sigs,int16_t    nib)
{
    uint8_t value;

    if(nib&0x0001){ // LSB
        value= (0x0F&sigs[nib>>1]);
    } else {        // MSB
        value= (0x0F&(sigs[nib>>1]>>4));
    }
    return(value);
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	SetBCDNibble - writes a BCD digits into the sig string
 *  INPUT:  a pointer to the digits, value of the digit and index.
 *  		index 0 is the MSD, index=15 is the LSD
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void SetBCDNibble(uint8_t *sigs,uint8_t value,int16_t    nib)
{
    uint8_t byt;

    byt =sigs[nib>>1];

    if(nib&0x0001){ // LSB
        value &= 0x0F;
        byt &= 0xF0;
        byt |= value;
    } else {        // MSB
        value<<=4;
        byt &= 0x0F;
        byt |= value;
    }

    sigs[nib>>1]=byt;
}


/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	ShiftSigNibbles - shifts a string of BCD digits left or right
 *  INPUT:  a pointer to the digits, length, and the number of places to shift
 *  		a negative shift is left, positive, right
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void    ShiftSigNibbles(uint8_t *sigs,int16_t    shift,int16_t    size)
{
    int16_t     from,to;

    if(shift>0){  // shift right
        size--;
        from = size-shift;
        to   = size;
        while(from>=0){
            SetBCDNibble(sigs,GetBCDNibble(sigs,from),to);
            from--;
            to--;
        }
        while(to>=0){
            SetBCDNibble(sigs,0,to);
            to--;
        }
    } else if(shift<0){  // shift left
        shift = 0-shift; // change sign
        size--;
        from = shift;
        to   = 0;
        while(to<=(size-shift)){
            SetBCDNibble(sigs,GetBCDNibble(sigs,from),to);
            from++;
            to++;
        }
        while(to<=size){
            SetBCDNibble(sigs,0,to);
            to++;
        }
    }
}


/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	PrintNibbles - Prints a debug string of significant digits to the console
 *  INPUT:  a pointer to the digits and the length. (some functions have double length digits)
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void    PrintNibbles(uint8_t *sigs,int16_t size)
{
    int16_t i;
    printf("Nibbles: ");
    i=0;
    while(i<size){
        printf("%d,",GetBCDNibble(sigs,i));
        i++;
    }
    printf("\r\n");
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	PrintDecimal_tDebug - Prints a debug version of a decimal_t to the console
 *  INPUT: a label string and a pointer to a decimal_t
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void PrintDecimal_tDebug(char *str,decimal_t *dec)
{
    char    pStr[64];
    uint8_t    ch;
    int16_t    sig_index,j;

    sprintf(pStr, "  %s sign=%.1d, exponent=%d ",str,dec->sign,dec->exp);
#ifdef MAKE_FOR_MAC
    printf("%s",pStr);
#else
    HAL_UART_Transmit(&huart2,(uint8_t*)pStr, strlen(pStr), 1000);
#endif

    sprintf(pStr, "      sig=x x x x x x x x x x x x x x x x \n\r");

    j=10;
    sig_index=0;
    while(sig_index<BCD_DIGIT_COUNT){
        ch=GetBCDNibble(dec->sig,sig_index);
        sig_index++;
        pStr[j] = ch+'0';
        j+=2;
    }

#ifdef MAKE_FOR_MAC
    printf("%s",pStr);
#else
    HAL_UART_Transmit(&huart2,(uint8_t*)pStr, strlen(pStr), 1000);
#endif

}

