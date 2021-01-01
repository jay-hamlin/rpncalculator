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
//		Debug_PrintStack("DIV");
		CalcPopStack(&rX);
		CalcPopStack(&rY);
		CalcDivide(&resX,&rY,&rX);		// *result, *Y/*X
		CalcPushStack(&resX);
		break;
	case KEY_X:			// multiply
//		Debug_PrintStack("MUL");
		CalcPopStack(&rX);
		CalcPopStack(&rY);
		CalcMultiply(&resX,&rX,&rY);	// X*Y, put result in X
		CalcPushStack(&resX);
		break;
	case KEY_MINUS:		// minus
//		Debug_PrintStack("SUB");
		CalcPopStack(&rX);
		CalcPopStack(&rY);
		CalcSubtract(&resX,&rY,&rX);	// Y-X, put result in X
		CalcPushStack(&resX);
		break;
	case KEY_PLUS:		// add
//		Debug_PrintStack("ADD");
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
	case KEY_MOD:		// modulus
		break;
	case KEY_BASE:		// DEC,OCT,HEX
		break;
	case KEY_DSP:		// set display mode
		SetIndicatorLED(INDICATOR_D3, 1);
		dspModePending = true;
		break;
	case KEY_FUNC:		// shift function
		SetIndicatorLED(INDICATOR_D4, 1);
		functionPending=true;
		break;
	case KEY_CLX:		// clear X
		DecimalNumberFromString(&registerX,"0.00");
		break;
	case KEY_DEL:		// delete (backspace)
		break;
	case KEY_ENTER:		// ENTER
		Debug_PrintStack("ENTER");
		//CalcPushStack((decimal_t*)&registerX);
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

	SetIndicatorLED(INDICATOR_D4, 0);
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
 *  DoDSPKeyFunction - Handles commands of the "Func" key.
 *
 *  INPUT:  keycode
 *  OUTPUT: none
 *  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void	DoDSPKeyFunction(uint8_t keycode)
{
	extern	char	IsNumberKey(char keycode);
	char	ch;

	if((ch = IsNumberKey(keycode))){
		display.points = ch-'0';
		display.update=true;
		dspModePending = false;
		SetIndicatorLED(INDICATOR_D3, 0);
	}
}

