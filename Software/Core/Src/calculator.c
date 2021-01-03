/*
 * calculator.c
 *
 *  Created on: Dec 5, 2020
 *      Author: jayhamlin
 */

#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>

#include "rpnHwDefs.h"
#include "display.h"
#include "keypad.h"
#include "calcMaths.h"
#include "calcUtilities.h"
#include "keyFunctions.h"
#include "calculator.h"


#define NUMBER_KEY_COUNT	12
const char numberKeycodes[NUMBER_KEY_COUNT] = {
		KEY_0,KEY_1,KEY_2,KEY_3,
		KEY_4,KEY_5,KEY_6,KEY_7,
		KEY_8,KEY_9,KEY_DP,KEY_CHS }; // we made dp and CHS number keys

#define ALPHA_KEY_COUNT	6
const char alphaKeycodes[ALPHA_KEY_COUNT] ={
		KEY_A,KEY_B,KEY_C,
		KEY_D, KEY_E,KEY_F };

#define FUNCTION_KEY_COUNT	16
const char functionKeycodes[FUNCTION_KEY_COUNT] ={
		KEY_BASE,KEY_DSP,KEY_DIV,KEY_SEND,
		KEY_X,KEY_COPY,KEY_ROL,
		KEY_MINUS,KEY_PASTE,KEY_X_Y,KEY_MOD,
		KEY_PLUS,KEY_FUNC,KEY_CLX,KEY_DEL,
		KEY_ENTER };

char	IsNumberKey(char keycode);
char	IsAlphaKey(char keycode);
char	IsFunctionKey(char keycode);
char	IsNumberKeyInRadix(char keycode);
void	AppendDisplayString(char ch);

#define	EDIT_STRING_SIZE	24
#define	DISP_STR_LENGTH	(DISPLAY_DIGIT_COUNT+1)

decimal_t	registerT;
decimal_t	registerZ;
decimal_t	registerX;
decimal_t	registerY;
char		dispEditString[EDIT_STRING_SIZE];
char		notificationString[EDIT_STRING_SIZE];
display_t	display;
uint16_t	notificationTimeout;

void DisplayNumber(decimal_t *dec);

void NumberToStringBIN(char *str,decimal_t *dec);
void NumberToStringOCT(char *str,decimal_t *dec);
void NumberToStringDEC_FIXED(char *str,decimal_t *dec);
void NumberToStringDEC_SCI(char *str,decimal_t *dec);
void NumberToStringHEX(char *str,decimal_t *dec);
void MakeOctalTriple(char *str,uint8_t bin);

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	IsNumberKey - Returns an ascii value if this is a number key else zero
 *
 *  INPUT:  keycode
 *  OUTPUT: ascii   - 0,1,2,3,4,5,6,7,8,9 and  '. for KEY_DP and '-' for KEY_CHS
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
char	IsNumberKey(char keycode)
{
	uint16_t	i;
	char 		ascii;

	ascii=0;

	if(keycode == KEY_DP){
		ascii=('.');
	} else if(keycode == KEY_CHS){
		ascii=('-');
	}else {
		i=0;
		while((i<NUMBER_KEY_COUNT)&& (ascii==0)){
			if(keycode == numberKeycodes[i]){
				ascii=(i+'0'); // index + ascii zero
			}
			i++;
		}
	}

	return(ascii);
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	IsAlphaKey - Returns an ascii value if this is a alpha key else zero
 *
 *  INPUT:  keycode
 *  OUTPUT: ascii   - A,B,C,D,E,F
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
char	IsAlphaKey(char keycode)
{
	uint16_t	i;
	char 		ascii;

	ascii=0;
	i=0;
	while((i<ALPHA_KEY_COUNT)&& (ascii==0)){
		if(keycode == alphaKeycodes[i]){
			ascii=(i+'A'); // index + ascii zero
		}
		i++;
	}

	return(ascii);
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	IsFunctionKey - Returns a keycode if this is a function key else zero
 *
 *  INPUT:  keycode
 *  OUTPUT: keycode
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
char	IsFunctionKey(char keycode)
{
	uint16_t	i;
	char 		ascii;

	ascii=0;
	i=0;
	while((i<FUNCTION_KEY_COUNT)&& (ascii==0)){
		if(keycode == functionKeycodes[i]){
			ascii=keycode; // Actually not sure how best to handle this one
		}
		i++;
	}

	return(ascii);
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	IsNumberKeyInRadix - The same as IsNumberKey but takes into account the radix display mode
 *
 *  INPUT:  keycode
 *  OUTPUT: ascii   - 0,1,2,3,4,5,6,7,8,9 and  '. for KEY_DP and '-' for KEY_CHS
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
char	IsNumberKeyInRadix(char keycode)
{
	char 		ch,ascii=0;

	if(display.radix == RADIX_OCTAL){
		ch = IsNumberKey(keycode);
		if((ch>='0') && (ch<='7')){
			ascii = ch;
		}
	}else if(display.radix == RADIX_DECIMAL){
		ch = IsNumberKey(keycode);
		if((ch>='0') && (ch<='9')){
			ascii = ch;
		}else if((ch=='.') || (ch<='-')){
			ascii = ch;
		}
	}
	if(display.radix == RADIX_HEXADECIMAL){
		ch = IsNumberKey(keycode);
		if((ch>='0') && (ch<='9')){
			ascii = ch;
		}else {
			ch = IsAlphaKey(keycode);
			ascii = ch;
		}
	}

	return(ascii);
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	DoInitializeCalculator - Called from main() once at the begining of time
 *
 *  INPUT:  none
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	DoInitializeCalculator(void){

	DecimalNumberFromString(&registerT,"0.00");
	DecimalNumberFromString(&registerZ,"0.00");
	DecimalNumberFromString(&registerY,"0.00");
	DecimalNumberFromString(&registerX,"0.00");

	// global calculator options
	display.radix = RADIX_DECIMAL;		//2,8,10,16
	display.format = DISP_FORMAT_FIXED;
	display.points = 2;			// 4 decimal points
	display.editMode = false;
	display.update	 = true;

	dispEditString[0]=0;
	notificationString[0]=0;
	notificationTimeout=0;
	functionPending=false;
	dspModePending= false;

	DisplayNotification("   JFH-97   ", 1000);

}


/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	DoRunCalculator - Called from main() once per millisecond to service
 * 					  all calculator functions.
 *  INPUT:  keycode - will be zero if no key have been pressed.
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	DoRunCalculator(uint8_t	keycode){
	char	ch;

#if 0
	if(keycode){
		if(keycode&KEYDOWN_EVENT)
			Debug_Printf("DWN keycode=%d\r\n",(keycode-KEYDOWN_EVENT));
		else
			Debug_Printf("UP keycode=%d\r\n",keycode);
	}
#endif
	if(KEYDOWN_EVENT&keycode){
		keycode &= (-1^KEYDOWN_EVENT);

		// Is a shift key function pending? (meaning we are waiting for the next key)
		if(functionPending){
			DoShiftedFunction(keycode);
		} else if(dspModePending){
			DoDSPKeyFunction(keycode);
		} else {
			// A number key?

			if((ch = IsNumberKeyInRadix(keycode))){
				if(display.editMode == false){
					dispEditString[0]=0; // zero string
					display.editMode= true; // begin editing new string
				}
				AppendDisplayString(ch);
				display.update=true;

			} else if((ch = IsAlphaKey(keycode))){

				sprintf(dispEditString, "ALPHA=  ");
				dispEditString[6]=(ch);
				display.update=true;

			} else if((ch = IsFunctionKey(keycode))){
				if(display.editMode == true){
					// move what we have been editing and put it into X
					DecimalNumberFromString(&registerX,dispEditString);
					display.editMode=false;
				}

				DoKeyFunction(keycode);
				display.update=true;
			}
		}
	}else if(keycode){
		// key up events

	} // end key code events

	if(notificationTimeout){
		notificationTimeout--;
		if(notificationTimeout==0)
			DisplayWriteString(dispEditString); // put back what was there previously
	}else {
		if(display.update){
			if(display.editMode){
				DisplayWriteString(dispEditString);
			}else{
				DisplayNumber(&registerX);
			}
		}
	}
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	AppendDisplayString - Adds the input char to the display string while editing.
 *
 *  INPUT:  ch  = input character
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	 AppendDisplayString(char ch)
{
	int16_t	len,i,j;

	len = strlen(dispEditString);

	// if we are at the maximum string length, remove the oldest char
	if(len>(DISPLAY_DIGIT_COUNT-1)){
		i=0;
		j=1;
		while(j<len){
			dispEditString[i++]=dispEditString[j++];
		}
		len--;
	}

	// minus sign goes at the front of the line, make room for it
	if(ch=='-'){
		i=len-1;
		j=len;
		while(j>0){
			dispEditString[j--]=dispEditString[i--];
		}
		dispEditString[0]=ch;
		len++;
	} else {
		dispEditString[len++]=ch;
	}

	dispEditString[len]=0;

}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	DisplayNotification - Writes a string to the display and holds it for a timeout period
 * 						  The display gets overwritten with the original content when the
 * 						  timeout expires.
 *  INPUT:  *str  = input string
 *  		timeoutms = timeout in milliseconds
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	DisplayNotification(char	*str, int16_t	timoutms)
{
	DisplayWriteString(str);
	notificationTimeout=timoutms;
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	DisplayNumber - Formats the display string with a decimal_t number and writes it to the display
 *  INPUT:  *dec = pointer to the decimal_t to write to the display
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void DisplayNumber(decimal_t *dec)
{
	int16_t	len;

//	PrintDecimal_tDebug("X",dec);
	DecimalNumberToDisplayString(dispEditString,dec);

	len = strlen(dispEditString);
    Debug_Printf("DISPLAY %d [%s]\r\n",len,dispEditString);

	DisplayWriteString(dispEditString);


	display.update=false;
}



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

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	DecimalNumberFromString - Parses a string for the decimal_t number.
 *  INPUT:  *dec = pointer to the results decimal_t
 *  		*str = input string
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void DecimalNumberFromString(decimal_t *dec,char *str)
{
    uint8_t        sig_temp[BCD_DIGIT_BYTES];
    int16_t        i;
    int16_t        strLen=strlen(str);
    int16_t        digitCount,firstNonZeroIndex,dpIndex,sig_index;
    int16_t        foundExponent;
    int16_t        foundExponentValue;
    int16_t        calculatedExp;
    int8_t        num;

    dec->sign    =0;

    calculatedExp=0;
    foundExponent=0;
    foundExponentValue=0;
    firstNonZeroIndex=-1;
    dpIndex=-1;
    digitCount=0;
    memset(sig_temp,0,BCD_DIGIT_BYTES);
    sig_index =0;

    // parse and validate the string
    i=0;
    while((i<strLen)&&(sig_index<BCD_DIGIT_COUNT)){
        if(str[i]=='-'){
            if(foundExponent){
                foundExponent=-1;
            } else {
                dec->sign = DECIMAL_SIGN_NEGATIVE; // minus
            }
        }else if(str[i]=='.') { // decimal point
            if(dpIndex == -1){
                dpIndex=i;// dp
            }
        }else if(str[i]=='E'){  // exponent
            foundExponent = 1;
        }
        else if((str[i]>='0')&&(str[i]<='9')) { // number
            num = str[i]-'0';
            if(foundExponent){
                foundExponentValue*=10;
                foundExponentValue+=(num);
            } else {
                    // first, we need to skip over leading zeros
                if(firstNonZeroIndex == -1){
                    if(num>0){ // zero after the dp are significant
                        firstNonZeroIndex=i;
                        digitCount++;
                        SetBCDNibble(sig_temp,num,sig_index++);// store then next nibble
                    }
                } else {
                    digitCount++;
                    SetBCDNibble(sig_temp,num,sig_index++);// store then next nibble
                }
            }
        }
        i++;
    }
    // if there was a minus sign our indexes are wrong
    if(dec->sign == DECIMAL_SIGN_NEGATIVE){
        if(firstNonZeroIndex != -1)
            firstNonZeroIndex--;
        if(dpIndex != -1)
            dpIndex--;
    }

    if(dpIndex == -1){  // then there was no dp
        calculatedExp = digitCount-1;
    } else {
        if(dpIndex>firstNonZeroIndex){  // dp is after the first digit
                                        // ie: 3.14159
            calculatedExp = (dpIndex-firstNonZeroIndex-1);
        } else{                         // dp is before the first digit
                                        // ie: 0.0314159   exp=-2
            calculatedExp = (dpIndex-firstNonZeroIndex);
        }
    }
    
    if(foundExponent){
        if(foundExponent==-1){
            foundExponentValue = -1*foundExponentValue;
        }
        calculatedExp = (calculatedExp+foundExponentValue);
    }
    
    dec->exp = calculatedExp;

    memcpy(dec->sig,sig_temp, BCD_DIGIT_BYTES);

    FixResultSignIfZero(dec);

}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	DecimalNumberToString - Converts a decimat_t number to an ascii string
 *  INPUT:  str = results string pointer for the representation of the input decimal_t
 *  		*dec = pointer to the decimal_t input
 *  		radix = formatting radix.
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/


void DecimalNumberToDisplayString(char *str,decimal_t *dec)
{
    int16_t     digits,i;

    if(dec->sign==NOT_A_NUMBER){
    	sprintf(str,"NOT A NUMBER");
    }else {
		switch(display.radix){
			case RADIX_BINARY:
				NumberToStringBIN(str,dec);
				break;
			case RADIX_OCTAL:
				NumberToStringOCT(str,dec);
				break;
			case RADIX_DECIMAL:
				if(display.format == DISP_FORMAT_FIXED){
					// Will this number fit into the 12 digit display?
					// if not we need to switch to SCI or ENG mode
					// -3.14E1 == -31.40
					digits = 1 + 1 + 1 + display.points;
					if(dec->exp<0){
						digits -= dec->exp;
						i = 0 - dec->exp;
					}else{
						digits += dec->exp;
						i = 0 + dec->exp;
					}
					if(dec->sign==DECIMAL_SIGN_NEGATIVE) digits++;
					if(i>10) digits++;
					if(i>100) digits++;
					if(i>100) digits++;

					if(digits>=12){
						NumberToStringDEC_SCI(str,dec);
					}else{
						NumberToStringDEC_FIXED(str,dec);
					}
				}
				break;
			case RADIX_HEXADECIMAL:
				NumberToStringHEX(str,dec);
				break;
			default:
				break;
		}
    }
}

void NumberToStringBIN(char *str,decimal_t *dec)
{
    sprintf(str,"BIN unimplemented");
}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *     NumberToStringOCT - Formats a decimat_t into a octal string for display
 *                         Integer only, factions are truncated.
 *                         32 bit word maximum.
 *
 *  INPUT: display string pointer and decimal_t pointer
 *  OUTPUT: none
 *  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void NumberToStringOCT(char *str,decimal_t *dec)
{
    char        tstr[DISP_STR_LENGTH];
    int16_t     i,digitIndex;
    int16_t     strIndex;
    int16_t     binWord;

    binWord=0;  // sum is zero

    memset(tstr,0,DISP_STR_LENGTH);

    strIndex= 0;        // zero string length


    // The first thing we do is figure out the binary value we want to display.
    // get a pointer to the least significant integer digit
    digitIndex = dec->exp;  // yep, easy
    if(digitIndex>16){
        sprintf(tstr," Too large ");
    } else if(digitIndex<0){
        sprintf(tstr," Too small ");
    } else {
        // Insert the leading negative sign if there is one.
        if(dec->sign == DECIMAL_SIGN_NEGATIVE){
            tstr[strIndex++]='-';        // negative
        }
        digitIndex++;
        i=0;
        while(i<digitIndex){
            binWord *= 10;
            binWord += GetBCDNibble(dec->sig,i);
            i++;
        }

    // assume a 16 bit word
    // 32 bits o 377 377 377
    // 16 bits OCT 377 377
    // 'cause that's all we can fit in the display
        tstr[strIndex++]='O'; // start with "OCT"
        tstr[strIndex++]='C';
        tstr[strIndex++]='T';
        tstr[strIndex++]=' ';
        MakeOctalTriple(&tstr[strIndex],(binWord&0x00FF));
        strIndex+=3;
        binWord>>=8;
        tstr[strIndex++]=' ';
        MakeOctalTriple(&tstr[strIndex],(binWord&0x00FF));
        strIndex+=3;
   }
    tstr[strIndex++]=' ';
    tstr[strIndex] = 0; // END

    // copy it out.
    strIndex++; //so we copy the null too
    memcpy(str,tstr,strIndex);


}

void    MakeOctalTriple(char *str,uint8_t bin)
{
    uint8_t  b2=bin;

    str[2] = (b2&0x07)+'0';
    b2>>=3;
    str[1] = (b2&0x07)+'0';
    b2>>=3;
    str[0] = (b2&0x03)+'0';

}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *     NumberToStringDEC_FIXED - Formats a decimat_t into a string for display
 *                             on the calculator in fixed point notation.
 *
 *  INPUT: display string pointer and decimal_t pointer
 *  OUTPUT: none
 *  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void NumberToStringDEC_FIXED(char *str,decimal_t *dec)
{
    char        tstr[2*DISP_STR_LENGTH];
    uint8_t     ch;
    int16_t     sig_index;
    int16_t     strIndex;
    int16_t     firstDigitIndex,dpIndex,chopIndex;

    /* display.format can be one of:
        DISP_FORMAT_FIXED
        DISP_FORMAT_SCI
        DISP_FORMAT_ENG
    */

    memset(tstr,0,(2*DISP_STR_LENGTH));

    strIndex= 0;    // zero string length
    dpIndex = 0;    // dp at zero is not valid because such a string would be "0.123" making dpIndex=1

    // Insert the leading negative sign if there is one.
    if(dec->sign == DECIMAL_SIGN_NEGATIVE){
        tstr[strIndex++]='-';        // negative
        strIndex=1;
    }

    // what about the decimal point? Do we have to pad the string with leading zeros?
    if(dec->exp<0){
        tstr[strIndex++] = '0';
        dpIndex = strIndex;
        tstr[strIndex++] = '.';
    } else {
        dpIndex=(strIndex+dec->exp+1);
    }
    // dpIndex points to the decimal point.

    // we are doing fixed point... so where is the first non-zero digit?
    if(dec->exp<0){
        firstDigitIndex = dpIndex - dec->exp;
     } else {
         firstDigitIndex = strIndex;
     }

    // if we aren't there yet we have to pad with zeros.
    while((strIndex<firstDigitIndex)&&(strIndex<DISP_STR_LENGTH)){
        tstr[strIndex++] = '0';
    }

    // so we build up the string one nibble at a time
    sig_index=0;
    while((sig_index<BCD_DIGIT_COUNT)&&(strIndex<DISPLAY_DIGIT_COUNT)){
        if(strIndex==dpIndex){
            tstr[strIndex++] = '.'; // insert dp
        } else {
            ch=GetBCDNibble(dec->sig,sig_index);
            tstr[strIndex++] = ch+'0'; // convert BCD to ascii
            sig_index++;
        }
    }

    if((strIndex == dpIndex)&&(strIndex<DISPLAY_DIGIT_COUNT)){  // we still need to add dp
        tstr[strIndex++] = '.';
    }

//    Debug_PrintDecimal_t("1strIndex=%d\r\n",strIndex);

    // fill to the end with zeros
    while(strIndex<DISPLAY_DIGIT_COUNT){
        tstr[strIndex++] = '0';
    }

    // now we chop off so many places after the dp and fill the string with spaces.
    chopIndex = dpIndex+display.points;
    dpIndex++;
    while(dpIndex<DISPLAY_DIGIT_COUNT){
        if(chopIndex<dpIndex)
            tstr[dpIndex] = ' ';
        dpIndex++;
    }

    tstr[strIndex] = 0; // END

 //   Debug_PrintDecimal_t("2strIndex=%d\r\n",strIndex);

    // copy it out.
    strIndex++; //so we copy the null too
    memcpy(str,tstr,strIndex);
}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *     NumberToStringDEC_SCI - Formats a decimat_t into a string for display
 *                             on the calculator in scientific notation.
 *
 *  INPUT: display string pointer and decimal_t pointer
 *  OUTPUT: none
 *  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void NumberToStringDEC_SCI(char *str,decimal_t *dec)
{
    char        tstr[DISP_STR_LENGTH];
    int16_t     workingDigit,stopHere;
    int16_t     strIndex;
    int16_t     i,reserve, exponent;

    /* display.format can be one of:
        DISP_FORMAT_FIXED
        DISP_FORMAT_SCI
        DISP_FORMAT_ENG
    */

    memset(tstr,0,DISP_STR_LENGTH);

    strIndex= 0;        // zero string length
    workingDigit = 0;   // which sig digit wwe are working on

    // Insert the leading negative sign if there is one.
    if(dec->sign == DECIMAL_SIGN_NEGATIVE){
        tstr[strIndex++]='-';        // negative
    }

    // for SCI notation there is always 1 significant digit then the dp.
    tstr[strIndex++] = GetBCDNibble(dec->sig,workingDigit++)+'0';
    tstr[strIndex++] = '.';

    // how many digits do we need to reserve for the exponent?
    reserve = 1;        // 'E'
    exponent = dec->exp;
    if(exponent<0)  // negative
        exponent = 0-exponent; // change sign

    do{ // count the number of exponent digits.
        reserve++;
        exponent /= 10; // div 10;
    } while(exponent);

    // add digits until we get to the number of places or end of string.
    stopHere = DISP_STR_LENGTH - reserve;
    if(stopHere > (strIndex+display.points))
        stopHere = strIndex+display.points;

    while(strIndex<stopHere){
        tstr[strIndex++] = GetBCDNibble(dec->sig,workingDigit++)+'0';
    }

    // Now the exponent
    tstr[strIndex++] = 'E';
    exponent = dec->exp;
    if(exponent<0) { // negative
        tstr[strIndex++] = '-';
        exponent = 0-exponent; // change sign
    }

    reserve--;
    i= strIndex+reserve-1;
    while(reserve){
        tstr[i--] = (exponent%10)+'0';
        exponent /= 10;
        reserve--;
        strIndex++;
    }

    tstr[strIndex] = 0; // END

    // copy it out.
    strIndex++; //so we copy the null too
    memcpy(str,tstr,strIndex);
}


/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *     NumberToStringHEX - Formats a decimat_t into a hexadecimal string for display
 *                         Integer only, factions are truncated.
 *                         32 bit word maximum.
 *
 *  INPUT: display string pointer and decimal_t pointer
 *  OUTPUT: none
 *  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void NumberToStringHEX(char *str,decimal_t *dec)
{
    char        tstr[DISP_STR_LENGTH];
    uint8_t     ch;
    int16_t     i,digitIndex;
    int16_t     strIndex;
    int32_t     binWord;

    binWord=0;  // sum is zero

    memset(tstr,0,DISP_STR_LENGTH);

    strIndex= 0;        // zero string length


    // The first thing we do is figure out the binary value we want to display.
    // get a pointer to the least significant integer digit
    digitIndex = dec->exp;  // yep, easy
    if(digitIndex>16){
        sprintf(tstr," Too large ");
    } else if(digitIndex<0){
        sprintf(tstr," Too small ");
    } else {
        // Insert the leading negative sign if there is one.
        if(dec->sign == DECIMAL_SIGN_NEGATIVE){
            tstr[strIndex++]='-';        // negative
        }
        digitIndex++;
        i=0;
        while(i<digitIndex){
 //           printf("i=%d, binword=0x%.8x\r\n",i,binWord);
            binWord *= 10;
            binWord += GetBCDNibble(dec->sig,i);
            i++;
        }

    // assume a full 32 bit word
        tstr[strIndex++]='0'; // why not start with "0x"
        tstr[strIndex++]='x';
        i=strIndex+8;
        while(i>strIndex){
            i--;
            ch = (binWord&0x0000000F);
            if(ch<10)
                ch = (ch+'0');
            else
                ch = (ch + 'A' - 10);
            tstr[i]=ch;
            binWord>>=4;
        }
        strIndex+=8;
   }

    tstr[strIndex] = 0; // END

    // copy it out.
    strIndex++; //so we copy the null too
    memcpy(str,tstr,strIndex);
}

