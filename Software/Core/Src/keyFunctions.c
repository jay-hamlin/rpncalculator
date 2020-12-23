/*
 * keyFunctions.c
 *
 *  Created on: Dec 13, 2020
 *      Author: jayhamlin
 */



#include "main.h"
#include <stdio.h>
#include <string.h>

#include "rpnHwDefs.h"
#include "display.h"
#include "keypad.h"
#include "calcMaths.h"
#include "calcUtilities.h"
#include "calculator.h"
#include "keyFunctions.h"

uint8_t	functionPending;

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *  DoKeyFunction - Handles commands.
 *
 *  INPUT:  keycode
 *  OUTPUT: none
 *  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void	DoKeyFunction(uint8_t keycode)
{

	switch(keycode){
	case KEY_DIV:		// divide
		CalcDivide(&registerX,&registerX,&registerY);		// *result, *X,*Y
															// X/Y, put result in X
		break;
	case KEY_X:			// multiply
		CalcMultiply(&registerX,&registerX,&registerY);		// *result, *X,*Y
															// X*Y, put result in X
		break;
	case KEY_MINUS:		// minus
		CalcSubtract(&registerX,&registerX,&registerY);		// *result, *X,*Y
															// X-Y, put result in X
		break;
	case KEY_PLUS:		// add
		CalcAdd(&registerX,&registerX,&registerY);			// *result, *X,*Y
															// X+Y, put result in X
		break;
	case KEY_SEND:		// send to computer
		break;
	case KEY_CHS:		// change sign

		if(registerX.sign&DECIMAL_SIGN_NEGATIVE){
			registerX.sign &= (-1^DECIMAL_SIGN_NEGATIVE);
		}else {
			registerX.sign |= DECIMAL_SIGN_NEGATIVE;
		}
	    break;
	case KEY_COPY:		// copy to computer
		break;
	case KEY_ROL:		// roll stack
		CalcROLStack();
		break;
	case KEY_PASTE:		// paste to computer
		break;
	case KEY_X_Y:		// X,Y exchange
		CalcSwapXY();
		break;
	case KEY_MOD:		// modulus
		break;
	case KEY_BASE:		// DEC,OCT,HEX
		break;
	case KEY_DSP:		// set display mode
		break;
	case KEY_FUNC:		// shift function
		SetIndicatorLED(INDICATOR_D4, 1);
		functionPending=true;
		break;
	case KEY_DWN:		// rol down ???
		break;
	case KEY_DEL:		// delete (backspace)
		break;
	case KEY_ENTER:		// ENTER
		break;
	default:
		break;

	}

}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *  DoShiftedFunction - Handles commands of the "Func" key.
 *
 *  INPUT:  keycode
 *  OUTPUT: none
 *  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void	DoShiftedFunction(uint8_t keycode)
{
	uint16_t		val=-1;

	SetIndicatorLED(INDICATOR_D4, 0);
	functionPending=false;

	switch(keycode){
	case KEY_A:
		val = adcValues[ADC_INDEX_POT];
		sprintf(notificationString,"POT=%d" ,val);
		break;
	case KEY_B:
		val = adcValues[ADC_INDEX_USB];
		sprintf(notificationString,"USB=%d" ,val);
		break;
	case KEY_C:
		val = adcValues[ADC_INDEX_BATT];
		sprintf(notificationString,"BATTERY=%d" ,val);
		break;
	case KEY_D:
		val = adcValues[ADC_INDEX_TEMP];
		sprintf(notificationString,"TEMP=%d" ,val);
		break;
	case KEY_E:
		val = adcValues[ADC_INDEX_VREF];
		sprintf(notificationString,"VREF=%d" ,val);
		break;
	case KEY_F:
		sprintf(notificationString,"UNIMPLEMENTED");
		break;
	default:
		break;
	}
	if(val!=-1){
		DisplayNotification(notificationString, 2000);
	}
}
