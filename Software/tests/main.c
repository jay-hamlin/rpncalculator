//
//  main.c
//  
//
//  Created by Jay Hamlin on 11/22/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calcMaths.h"
#include "calcUtilities.h"
#include "keyFunctions.h"
#include "calculator.h"

#define    EDIT_STRING_SIZE    32
#define    DISP_STR_LENGTH    EDIT_STRING_SIZE

decimal_t    registerT;
decimal_t    registerZ;
decimal_t    registerX;
decimal_t    registerY;
char        dispEditString[EDIT_STRING_SIZE];
display_t    display;


// pi = 3.14159265358979323846264338327950288419716939937510
const   decimal_t    pi = {
    0,0,
    {0x31, 0x41, 0x59, 0x26, 0x53, 0x58, 0x97, 0x93}
    };

// Euler's number e = 2.71828182845904523536028747135266249775724709369995
const   decimal_t    e = {
    0,0,
    {0x27, 0x18, 0x28, 0x45, 0x90, 0x45, 0x23, 0x53}
    };



void    TestMultiplying(char *x,char *y);
void    TestDividing(char *x,char *y);
void    TestAdding(char *x,char *y,int16_t  doadd);
void    TestNibbleShifting(int16_t  shift);

void DecimalNumberFromString(decimal_t *dec,char *str);
void DecimalNumberToString(char *str,decimal_t *dec);

int main( int argc, char *argv[] )  {
    int val;
    decimal_t  regX,regY,regZ,regT;
    char    xStr[48];
    char    yStr[48];
    char    tStr[48];
    
    display.radix=RADIX_DECIMAL;
    display.format = DISP_FORMAT_FIXED;
    display.points = 16;

    if(argc>2)
        DecimalNumberFromString(&regX,argv[2]);
    if(argc>3)
        DecimalNumberFromString(&regY,argv[3]);
    
    DecimalNumberFromString(&regT,"0.00");
    
//    PrintDecimal_tDebug("X",&regX);
//    PrintDecimal_tDebug("Y",&regY);


    // argc is one more than the number of arguments.
    if(strcmp(argv[1],"pi")==0){
        memcpy(&regX,(decimal_t*)&pi,sizeof(decimal_t));
        memcpy(&regY,(decimal_t*)&e,sizeof(decimal_t));
        DecimalNumberToString(xStr,&regX);
        DecimalNumberToString(yStr,&regY);
        printf("pi=%s,e=%s\r\n",xStr,yStr);
        CalcAdd(&regT,&regX,&regY);
    } else if(strcmp(argv[1],"add")==0){
        CalcAdd(&regT,&regX,&regY);
    } else if(strcmp(argv[1],"sub")==0){
        CalcSubtract(&regT,&regX,&regY);
    } else if(strcmp(argv[1],"mul")==0){
        CalcMultiply(&regT,&regX,&regY);
    } else if(strcmp(argv[1],"div")==0){
        CalcDivide(&regT,&regX,&regY);
    } else if(strcmp(argv[1],"sft")==0){
        val = strtol(argv[3], NULL, 10);
        ShiftSigNibbles(regX.sig,val,16);
    } else if(strcmp(argv[1],"mod")==0){
        CalcModulo(&regT,&regX,&regY);
    } else {
        printf("unknown argument %s\r\n",argv[1]);
    }
    
//    PrintDecimal_tDebug("T",&regT);

    DecimalNumberToString(xStr,&regX);
    DecimalNumberToString(yStr,&regY);
    DecimalNumberToString(tStr,&regT);
    
    printf("%s,%s,%s,%s\r\n",argv[1],xStr,yStr,tStr);
 
}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *     DecimalNumberFromString - Parses a string for the decimal_t number.
 *  INPUT:  *dec = pointer to the results decimal_t
 *          *str = input string
 *  OUTPUT: none
 *  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
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
}

#if 1
void NumberToStringBIN(char *str,decimal_t *dec);
void NumberToStringOCT(char *str,decimal_t *dec);
void NumberToStringDEC_FIXED(char *str,decimal_t *dec);
void NumberToStringDEC_SCI(char *str,decimal_t *dec);
void NumberToStringHEX(char *str,decimal_t *dec);
void MakeOctalTriple(char *str,uint8_t bin);

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *     DecimalNumberToString - Converts a decimat_t number to an ascii string
 *  INPUT:  str = results string pointer for the representation of the input decimal_t
 *          *dec = pointer to the decimal_t input
 *          radix = formatting radix.
 *  OUTPUT: none
 *  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */

void DecimalNumberToString(char *str,decimal_t *dec)
{
    uint8_t     digits;

    if(dec->sign==(int8_t)NOT_A_NUMBER){
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
                    digits = 1 + 1 + 1 + dec->exp + display.points;
                    if(digits>=24){
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
                printf("Unknown display radix\r\n");

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
    char        tstr[DISP_STR_LENGTH];
    uint8_t     ch;
    int16_t     sig_index;
    int16_t     strIndex;
    int16_t     firstDigitIndex,dpIndex,chopIndex;

    /* display.format can be one of:
        DISP_FORMAT_FIXED
        DISP_FORMAT_SCI
        DISP_FORMAT_ENG
    */
 
    memset(tstr,0,DISP_STR_LENGTH);

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
    while((sig_index<BCD_DIGIT_COUNT)&&(strIndex<DISP_STR_LENGTH)){
        if(strIndex==dpIndex){
            tstr[strIndex++] = '.'; // insert dp
        } else {
            ch=GetBCDNibble(dec->sig,sig_index);
            tstr[strIndex++] = ch+'0'; // convert BCD to ascii
            sig_index++;
        }
    }
#if 0
    if((strIndex == dpIndex)&&(strIndex<DISP_STR_LENGTH)){  // we still need to add dp
        tstr[strIndex++] = '.';
    }

    // fill to the end with zeros
    while(strIndex<DISP_STR_LENGTH){
        tstr[strIndex++] = '0';
    }

    // now we chop off so many places after the dp and fill the string with spaces.
    chopIndex = dpIndex+display.points;
    dpIndex++;
    while(dpIndex<DISP_STR_LENGTH){
        if(chopIndex<dpIndex)
            tstr[dpIndex] = ' ';
        dpIndex++;
    }
#endif
    tstr[strIndex] = 0; // END

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
    if(stopHere>15)
        stopHere=15;

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



#else

void DecimalNumberToString(char *str,int16_t strLength,decimal_t *dec,char radix)
{
    char        tstr[64];
    uint8_t     ch;
    int16_t     sig_index;
    int16_t     strIndex,places;
    int16_t     firstDigitIndex,dpIndex,chopIndex;

    /* display.format can be one of:
        DISP_FORMAT_FIXED
        DISP_FORMAT_SCI
        DISP_FORMAT_ENG
    */

     memset(tstr,0,strLength+1);
    
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
    while((strIndex<firstDigitIndex)&&(strIndex<strLength)){
        tstr[strIndex++] = '0';
    }
    
    // so we build up the string one nibble at a time
    sig_index=0;
    while((sig_index<BCD_DIGIT_COUNT)&&(strIndex<strLength)){
        if(strIndex==dpIndex){
            tstr[strIndex++] = '.'; // insert dp
        } else {
            ch=GetBCDNibble(dec->sig,sig_index);
            tstr[strIndex++] = ch+'0'; // convert BCD to ascii
            sig_index++;
        }
    }
    
#if 0

    if((strIndex == dpIndex)&&(strIndex<strLength)){  // we still need to add dp
        tstr[strIndex++] = '.';
    }

    // fill to the end with zeros
    while(strIndex<strLength){
        tstr[strIndex++] = '0';
    }
    // now we chop off so many places after the dp and fill the string with spaces.
    places = 16;
    chopIndex = dpIndex+places;
    dpIndex++;
    while(dpIndex<strLength){
        if(chopIndex<dpIndex)
            tstr[dpIndex] = ' ';
        dpIndex++;
    }
#endif
    tstr[strIndex] = 0; // END
    
    // copy it out.
    strIndex++; //so we copy the null too
    memcpy(str,tstr,strIndex);
}
#endif

