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
#include "calcTrig.h"
#include "calcUtilities.h"
#include "calculator.h"
#include "keyFunctions.h"

uint8_t	functionPending;
uint8_t	dspModePending;

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *  DoKeyFunction - Handles commands.
 *
 *  INPUT:  keycode
 *  OUTPUT: none
 *  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void	DoKeyFunction(uint8_t keycode)
{
	decimal_t	resX,rX,rY;

	switch(keycode){
	case KEY_DIV:		// divide
		CalcPopStack(&rX);
		CalcPopStack(&rY);
		CalcDivide(&resX,&rY,&rX);		// *result, *Y/*X
		if(resX.sign == NOT_A_NUMBER){
			DisplayNotification("NOT A NUMBER", 2000);
		}else{
			DecimalNumberFromString(&registerX,"0.00");
		}
		break;
	case KEY_X:			// multiply
		CalcPopStack(&rX);
		CalcPopStack(&rY);
		CalcMultiply(&resX,&rX,&rY);	// X*Y, put result in X
		CalcPushStack(&resX);
		break;
	case KEY_MINUS:		// minus
		CalcPopStack(&rX);
		CalcPopStack(&rY);
		CalcSubtract(&resX,&rY,&rX);	// Y-X, put result in X
		CalcPushStack(&resX);
		break;
	case KEY_PLUS:		// add
		CalcPopStack(&rX);
		CalcPopStack(&rY);
		CalcAdd(&resX,&rX,&rY);			// X+Y, put result in X
		CalcPushStack(&resX);
		break;
	case KEY_SEND:		// send to computer
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
	case KEY_MOD:		// modulo
		CalcPopStack(&rX);
		CalcPopStack(&rY);
		CalcModulo(&resX,&rY,&rX);		// *result, *Y%*X
		CalcPushStack(&resX);
		break;
	case KEY_BASE:		// DEC,OCT,HEX
		break;
	case KEY_DSP:		// set display mode
		SetIndicatorLED(INDICATOR_D1, 1);
		dspModePending = true;
		break;
	case KEY_FUNC:		// shift function
		SetIndicatorLED(INDICATOR_D1, 1);
		functionPending=true;
		break;
	case KEY_CLX:		// clear X
		DecimalNumberFromString(&registerX,"0.00");
		break;
	case KEY_DEL:		// delete (backspace)
		break;
	case KEY_ENTER:		// ENTER
		CalcPushStack((decimal_t*)&registerX);
		break;
	default:
		break;

	}

	Debug_PrintStack("RES");

}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *  DoShiftedFunction - Handles commands of the "Func" key.
 *
 *  INPUT:  keycode
 *  OUTPUT: none
 *  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void	DoShiftedFunction(uint8_t keycode)
{
	uint16_t		val;
	char			notify;

	SetIndicatorLED(INDICATOR_D1, 0);
	functionPending=false;
	notify=false;

	switch(keycode){
	case KEY_A:
		val = adcValues[ADC_INDEX_POT];
		sprintf(notificationString,"POT=%d" ,val);
		notify=true;
		break;
	case KEY_B:
		val = adcValues[ADC_INDEX_USB];
		sprintf(notificationString,"USB=%d" ,val);
		notify=true;
		break;
	case KEY_C:
		val = adcValues[ADC_INDEX_BATT];
		sprintf(notificationString,"BATTERY=%d" ,val);
		notify=true;
		break;
	case KEY_D:
		val = adcValues[ADC_INDEX_TEMP];
		sprintf(notificationString,"TEMP=%d" ,val);
		notify=true;
		break;
	case KEY_E:
		val = adcValues[ADC_INDEX_VREF];
		sprintf(notificationString,"VREF=%d" ,val);
		notify=true;
		break;
	case KEY_F:
		CalcPushStack((decimal_t*)&pi);
		display.update=true;
		val=-1;
		break;
	default:
		break;
	}
	if(notify ){
		DisplayNotification(notificationString, 2000);
	}
}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *  DoDSPKeyFunction - Handles commands of the "DSP" key.
 *  	numbers from 0-9 set the number of displayed decimal points
 *  	Alpha characters set the display modes
 *  		A = OCTAL
 *  		B = DECIMAL
 *  		C = HEXADECIMAL
 *  		D = FIXED POINT
 *  		E = SCIENTIFIC NOTATION
 *  		F = ENGINEERING NOTATION
 *
 *  INPUT:  keycode
 *  OUTPUT: none
 *  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void	DoDSPKeyFunction(uint8_t keycode)
{
	extern	char	IsNumberKey(char keycode);
	extern	char	IsAlphaKey(char keycode);
	char	ch;



	if((ch = IsNumberKey(keycode))){
		display.points = ch-'0';
	} else {
		ch = IsAlphaKey(keycode);
		switch(ch){
			case 'A':
				display.radix = RADIX_OCTAL;
				break;
			case 'B':
				display.radix = RADIX_DECIMAL;
				break;
			case 'C':
				display.radix = RADIX_HEXADECIMAL;
				break;
			case 'D':
				display.format = DISP_FORMAT_FIXED;
				break;
			case 'E':
				display.format = DISP_FORMAT_SCI;
				break;
			case 'F':
				display.format = DISP_FORMAT_ENG;
				break;
			default:
				break;
		}
	}

	SetIndicatorLED(INDICATOR_D1, 0);
	display.update=true;
	dspModePending = false;
}

