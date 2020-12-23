//
//  main.c
//  
//
//  Created by Jay Hamlin on 11/22/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "numbers.h"
#include "calculator.h"
#include "calcUtilities.h"
#include "keyFunctions.h"
#include "calcUtilities.h"
#include "calcMaths.h"

#define    EDIT_STRING_SIZE    24
#define    DISP_STR_LENGTH    EDIT_STRING_SIZE

decimal_t    registerT;
decimal_t    registerZ;
decimal_t    registerX;
decimal_t    registerY;
char        dispEditString[EDIT_STRING_SIZE];
display_t    display;

void    TestMultiplying(char *x,char *y);
void    TestDividing(char *x,char *y);
void    TestAdding(char *x,char *y,int16_t  doadd);
void    TestNibbleShifting(int16_t  shift);

void DecimalNumberFromString(decimal_t *dec,char *str);
void DecimalNumberToString(char *str,int16_t strLength,decimal_t *dec,char radix);

int main( int argc, char *argv[] )  {
    int val;

    // argc is one more than the number of arguments.
    if(strcmp(argv[1],"add")==0){
        TestAdding(argv[2],argv[3],1);
    } else if(strcmp(argv[1],"sub")==0){
        TestAdding(argv[2],argv[3],0);
    } else if(strcmp(argv[1],"mul")==0){
        TestMultiplying(argv[2],argv[3]);
    } else if(strcmp(argv[1],"div")==0){
        TestDividing(argv[2],argv[3]);
    } else if(strcmp(argv[1],"sft")==0){
        val = strtol(argv[2], NULL, 10);
        TestNibbleShifting(val);
    } else {
        printf("unknown argument %s\r\n",argv[1]);
    }
}

void    TestMultiplying(char *x,char *y)
{
    decimal_t  regX,regY,regZ,regT;
    char    xStr[64];
    char    yStr[64];
    char    resStr[64];

    DecimalNumberFromString(&regX,x);
    DecimalNumberFromString(&regY,y);
    DecimalNumberFromString(&regT,"0.00");

//    PrintDecimal_tDebug("regX",&regX);
//    PrintDecimal_tDebug("regY",&regY);

    CalcMultiply(&regT,&regX,&regY);

    DecimalNumberToString(xStr,16,&regX,RADIX_DECIMAL);
    DecimalNumberToString(yStr,16,&regY,RADIX_DECIMAL);
    DecimalNumberToString(resStr,24,&regT,RADIX_DECIMAL);
    
    printf("MULTIPLY  X=%sX Y=%s =%s\r\n",xStr,yStr,resStr);

}

void    TestDividing(char *x,char *y)
{
    decimal_t  regX,regY,regZ,regT;
    char    xStr[64];
    char    yStr[64];
    char    resStr[64];

    DecimalNumberFromString(&regX,x);
    DecimalNumberFromString(&regY,y);
    DecimalNumberFromString(&regT,"0.00");
    
//    PrintDecimal_tDebug("regX",&regX);
//    PrintDecimal_tDebug("regY",&regY);

    CalcDivide(&regT,&regX,&regY);

    DecimalNumberToString(xStr,16,&regX,RADIX_DECIMAL);
    DecimalNumberToString(yStr,16,&regY,RADIX_DECIMAL);
    DecimalNumberToString(resStr,24,&regT,RADIX_DECIMAL);

    printf("DIVIDE  X=%s/ Y=%s=%s\r\n",xStr,yStr,resStr);

}

void    TestAdding(char *x,char *y,int16_t  doadd)
{
    decimal_t  regX,regY,regZ,regT;
    char    xStr[64];
    char    yStr[64];
    char    resStr[64];

    DecimalNumberFromString(&regX,x);
    DecimalNumberFromString(&regY,y);
    DecimalNumberFromString(&regT,"0.00");

    if(doadd)
        CalcAdd(&regT,&regX,&regY);
    else
        CalcSubtract(&regT,&regX,&regY);

    DecimalNumberToString(xStr,16,&regX,RADIX_DECIMAL);
    DecimalNumberToString(yStr,16,&regY,RADIX_DECIMAL);
    DecimalNumberToString(resStr,24,&regT,RADIX_DECIMAL);

    if(doadd)
        printf("ADD  X=%s+ Y=%s=%s\r\n",xStr,yStr,resStr);
    else
        printf("SUBTRACT  X=%s= Y=%s=%s\r\n",xStr,yStr,resStr);

}

void    TestNibbleShifting(int16_t  shift)
{
    decimal_t   regX;
    
    printf("Shifting = %d\n", shift);

    DecimalNumberFromString(&regX,"123456789123456789");

    PrintDecimal_tDebug("before",&regX);
    
    ShiftSigNibbles(regX.sig,shift,16);

    PrintDecimal_tDebug(" after",&regX);
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

    if((strIndex == dpIndex)&&(strIndex<strLength)){  // we still need to add dp
        tstr[strIndex++] = '.';
    }

    // fill to the end with zeros
    while(strIndex<strLength){
        tstr[strIndex++] = '0';
    }

    // now we chop off so many places after the dp and fill the string with spaces.
    places = 10;
    chopIndex = dpIndex+places;
    dpIndex++;
    while(dpIndex<strLength){
        if(chopIndex<dpIndex)
            tstr[dpIndex] = ' ';
        dpIndex++;
    }

    tstr[strIndex] = 0; // END
    
    // copy it out.
    strIndex++; //so we copy the null too
    memcpy(str,tstr,strIndex);
}


