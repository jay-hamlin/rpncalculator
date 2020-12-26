/*
 * calcMaths.c
 *
 *  Created on: Dec 22, 2020
 *      Author: jayhamlin
 */
#ifdef MAKE_FOR_MAC
#include "tests.h"
#else
#include "main.h"
#endif

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#include <stdio.h>
#include <string.h>

#include "calcMaths.h"
#include "calcUtilities.h"

// define add and subtract for CalcAddSubtract
#define REQUEST_ADD         1
#define REQUEST_SUBTRACT     0

void    CalcAddSubtract(decimal_t *res,decimal_t *x,decimal_t *y,char requestAdd);
int16_t BCDSubtractor(uint8_t   *sum, uint8_t   *x, uint8_t   *y,int16_t    size);
int16_t BCDAdder(uint8_t   *sum, uint8_t   *x, uint8_t   *y,int16_t    size);
int16_t BCDCompare(uint8_t   *x, uint8_t   *y,int16_t    size);


void    CalcAdd(decimal_t *res,decimal_t *x,decimal_t *y)
{
	CalcAddSubtract(res,x,y,REQUEST_ADD);
}

void    CalcSubtract(decimal_t *res,decimal_t *x,decimal_t *y)
{
	CalcAddSubtract(res,x,y,REQUEST_SUBTRACT);
}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *  CalcAddSubtract - Adds or subtracts 2 numbers.
 *  INPUT:  requestAdd -  0=subtract, 1=add
            *x - pointer to first number
 *          *y - pointer to second number
 *          *res - pointer to result number
 *
 *  OUTPUT: none
*  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void    CalcAddSubtract(decimal_t *res,decimal_t *x,decimal_t *y,char requestAdd)
{
    decimal_t A,B;
    int16_t    i,j,k;
    char       aIsBigger,resultSign,resultExp;
    uint8_t    sum[BCD_DIGIT_BYTES];

    // make copies so we don't screw with the original numbers
    memcpy(&A,x,sizeof(decimal_t));
    memcpy(&B,y,sizeof(decimal_t));

    // zero the sum
    memset(sum,0,BCD_DIGIT_BYTES);

    // in order to add 2 numbers, we have to align the places (1's, 10's, 100's etc.)
    // we start with finding the largest number in magnitude (sign doesn't count) then
    // we shift the smaller number to get the places to align.
    if((A.exp)>(B.exp)){
        // A is greater than B
        ShiftSigNibbles(B.sig,(A.exp - B.exp),BCD_DIGIT_COUNT); // smaller shifts right this many places.
        resultExp = A.exp;
    }else{
        // Either B is greater than A or they are the same.
        ShiftSigNibbles(A.sig,(B.exp - A.exp),BCD_DIGIT_COUNT); // smaller shifts right this many places.
        resultExp = B.exp;
    }

    // we have all the places lined up but which number is bigger now?
    // start at the big end and work out way down.
    // the first number we hit which is larger makes that number the winner
    aIsBigger=0;
    i=0;
    while((i<BCD_DIGIT_COUNT)&&(aIsBigger==0)){
        j = GetBCDNibble(A.sig,i);
        k = GetBCDNibble(B.sig,i);
        if(j>k)
            aIsBigger = 1;
        else if(j<k)
            aIsBigger = -1;
        i++;
    }

   // Are we adding or subtracting?
   // What is the sign of the result?
   //    4 possible forms of addition:
   // case 0:    C = (A + B)
   // case 1:    C = -A + B is done as C = (B - A) if A<B else -(A - B)
   // case 2:    C = A + -B is done as C = (A - B) if B<A else -(B - A)
   // case 3:    C = -A + -B is done as C = -(A + B)
   //    4 possible forms of subtraction:
   // case 4:    C = A - B if B<A else -(B - A)
   // case 5:    C = -A - B is done as C = -(B + A)
   // case 6:    C = A - -B is done as C = (A + B)
   // case 7:    C = -A - -B is done as C = (B - A) if A<B else -(A - B)

   if(requestAdd==REQUEST_ADD)
       k = 0;
   else // subtract
       k = 4;
    if(A.sign) // A is negative
        k |= 0x01;
    if(B.sign) // B is negative
        k |= 0x02;
    // now k shoud look like the cases listed above
//    printf("switch(k)=%d\r\n",k);
    switch(k){
        case 0:   // case 0:    C = (A + B)
            requestAdd = REQUEST_ADD;
            resultSign = 0;
            break;
        case 1:   // case 1:    C = -A + B is done as C = (B - A) if A<B else -(A - B)
            requestAdd = REQUEST_SUBTRACT;
            if(aIsBigger)
                resultSign = DECIMAL_SIGN_NEGATIVE;
            else
                resultSign = 0;
            break;
        case 2:   // case 2:    C = A + -B is done as C = (A - B) if B<A else -(B - A)
            requestAdd = REQUEST_SUBTRACT;
            if(aIsBigger)
                resultSign = 0;
            else
                resultSign = DECIMAL_SIGN_NEGATIVE;
            break;
        case 3:   // case 3:    C = -A + -B is done as C = -(A + B)
            requestAdd = REQUEST_ADD;
            resultSign = DECIMAL_SIGN_NEGATIVE;
            break;
        case 4:   // case 4:    C = A - B if B<A else -(B - A)
            requestAdd = REQUEST_SUBTRACT;
            if(aIsBigger)
                resultSign = 0;
            else
                resultSign = DECIMAL_SIGN_NEGATIVE;
            break;
        case 5:   // case 5:    C = -A - B is done as C = -(B + A)
            requestAdd = REQUEST_ADD;
            resultSign = DECIMAL_SIGN_NEGATIVE;
            break;
        case 6:   // case 6:    C = A - -B is done as C = (A + B)
            requestAdd = REQUEST_ADD;
            resultSign = 0;
            break;
        case 7:   // case 7:    C = -A - -B is done as C = (B - A) if A<B else -(A - B)
            requestAdd = REQUEST_SUBTRACT;
            if(aIsBigger)
                resultSign = DECIMAL_SIGN_NEGATIVE;
            else
                resultSign = 0;
            break;
   }

    if(requestAdd == REQUEST_ADD)
    	// BCDAdder returns carry.  Do we ever need it?
        (void)BCDAdder(sum,A.sig,B.sig,BCD_DIGIT_COUNT);
    else{
        if(aIsBigger)
            BCDSubtractor(sum,A.sig,B.sig,BCD_DIGIT_COUNT);
        else
            BCDSubtractor(sum,B.sig,A.sig,BCD_DIGIT_COUNT);

        // It is possible for the MSD to be zero in which case we need to left shift one place and add to the exponent.
        if(GetBCDNibble(sum,0)==0){
            ShiftSigNibbles(sum,(-1),BCD_DIGIT_COUNT);
            resultExp-=1;
        }
    }

    // now copy the result
    res->sign = resultSign;
    res->exp = resultExp;
    memcpy(res->sig,sum,BCD_DIGIT_BYTES);
}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *  BCDAdder - Adds 2 BCD strings of any size.
 *             numbers are unsigned (both positive)
 *  INPUT:  requestAdd -  0=subtract, 1=add
 *          *sum - pointer to result number
 *          *x - pointer to first number
 *          *y - pointer to second number
 *          size = the number of BCD digits. (2x the number of bytes)
 *
 *  OUTPUT: carry
*  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
int16_t    BCDAdder(uint8_t   *sum, uint8_t   *x, uint8_t   *y,int16_t    size)
{
    int16_t    i,j,carry;

    carry = 0;
    i=size; // start at the LSB and work our way down
    while(i>0){
        i--;
        j = GetBCDNibble(x,i) + GetBCDNibble(y,i)+carry;
        SetBCDNibble(sum,(j % 10),i);
        carry = (j/10);
    }
    // if carry is set now it means that the entire thing needs to be shifted to make room for carry in the msb
    if(carry){
        ShiftSigNibbles(sum,1,size);
        SetBCDNibble(sum,carry,0);
        // we have to bump the exponent of the result by 1 also which is done by adding carry (pBigger->exp+carry)
    }

    return(carry);
}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *  BCDSubtractor - Subtracts 2 BCD strings of any size.
 *                  Numbers are unsigned (both positive) and left justified
 *                  sum - X - Y
 *                  X is larger than Y by definition.
 *  INPUT:  *sum - pointer to result number
 *          *x - pointer to first number
 *          *y - pointer to second number
 *          size = the number of BCD digits. (2x the number of bytes)
 *
 *  OUTPUT: borrow
*  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
int16_t    BCDSubtractor(uint8_t   *sum, uint8_t   *x, uint8_t   *y,int16_t    size)
{
    int16_t    i,j,borrow;

    borrow = 0;
    i=size; // start at the LSB and work our way down
    while(i>0){
        i--;
        j = GetBCDNibble(x,i) - GetBCDNibble(y,i) - borrow;
        if(j<0){
            j+=10;
            borrow=1;
        }else{
            borrow=0;
        }
        SetBCDNibble(sum,(j % 10),i);
    }
    return(borrow);
}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *  BCDCompare -    compares 2 BCD strings of any size.
 *                  Numbers are unsigned (both positive) and left justified
 *                  sum - X - Y
 *                  X is larger than Y by definition.
 *  INPUT:  *x - pointer to first number
 *          *y - pointer to second number
 *          size = the number of BCD digits. (2x the number of bytes)
 *
 *  OUTPUT: 1 if x>y
 *          0 if x=y
 *         -1 if x<y
*  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
int16_t    BCDCompare(uint8_t   *x, uint8_t   *y,int16_t    size)
{
    int16_t    i,j,compare;

    compare = 0;
    i=0; // start at the MSB , first non-zero result gives us our answer
    while((i<size)&&(compare==0)){
        j=(GetBCDNibble(x,i) - GetBCDNibble(y,i));
        i++;
        if(j>0){
            compare=1;
        }else if(j<0){
            compare=-1;
        }
    }

 //   printf("BCDCompare=%d\r\n",compare);

    return(compare);
}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *  CalcMultiply - Multiplies 2 numbers.
 *  INPUT:  requestAdd -  0=subtract, 1=add
            *x - pointer to first number
 *          *y - pointer to second number
 *          *res - pointer to result number
 *
 *  OUTPUT: none
*  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void    CalcMultiply(decimal_t *res,decimal_t *x,decimal_t *y)
{
    decimal_t   A,B;
    int16_t    aIndex,bIndex,sumIndex,carry;
    int16_t     productDigit,bcdValue;
    char       resultSign,resultExp;
    uint8_t    sum[(2*BCD_DIGIT_BYTES)]; // we keep a double sized sum

    // make copies so we don't screw with the original numbers
    memcpy(&A,x,sizeof(decimal_t));
    memcpy(&B,y,sizeof(decimal_t));

    // zero the sum
    memset(sum,0,(2*BCD_DIGIT_BYTES));

    // we can figure the sign and exponent of the result now.
    if((A.sign^B.sign)==0) // both are the same sign
        resultSign = 0;
    else
        resultSign = DECIMAL_SIGN_NEGATIVE;

    resultExp = B.exp+A.exp; // exponent

    // Get a digit from A
    // Multiply with B
    // store in sum.

    bIndex = BCD_DIGIT_COUNT; // starting at the LSD of B
    while(bIndex>0){
        carry =0;
        bIndex--;
        //bIndex from 15 to 0
        aIndex = BCD_DIGIT_COUNT; // starting at the LSD of A
        while(aIndex>0){
            aIndex--;
           // aIndex from 15 to 0
            sumIndex=(1+aIndex+bIndex); // 31....0
            productDigit = GetBCDNibble(A.sig,aIndex) * GetBCDNibble(B.sig,bIndex)+ GetBCDNibble(sum,sumIndex)+carry;
            bcdValue = (productDigit%10);
            carry = (productDigit/10);
            SetBCDNibble(sum,bcdValue,sumIndex);
        }
        if(carry){
            sumIndex=(aIndex+bIndex); // 31....0
            SetBCDNibble(sum,carry,sumIndex);
        }
    }

    // if the MSD is not zero then we have to bump the exponent.
    // if the MSD IS zero then we have to left shift the result
    if(GetBCDNibble(sum,0)==0){
        // left shift 1
        ShiftSigNibbles(sum,-1,(BCD_DIGIT_COUNT+1));
    } else {
        resultExp++;
    }

    // now copy the result
    res->sign = resultSign;
    res->exp = resultExp;
    memcpy(res->sig,sum,BCD_DIGIT_BYTES);
}

/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *  CalcDivide - Divides 2 numbers by long division. (like in grade school)
 *                  res = x / y   == quotient = (dividend/divisor);
 *  INPUT:  *x - dividend
 *          *y - divisor
 *          *res - quotient
 *
 *  OUTPUT: none
*  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void    CalcDivide(decimal_t *res,decimal_t *x,decimal_t *y)
{
    int16_t    quotientIndex,quote,zeros;
    char       resultSign,resultExp;
    uint8_t    quotient[BCD_DIGIT_BYTES];
    uint8_t    divisor[(2*BCD_DIGIT_BYTES)];
    uint8_t    remainder[(2*BCD_DIGIT_BYTES)]; // we keep a double sized working remainder
    
    // quotient is zeroed
    memset(quotient,0,BCD_DIGIT_BYTES);

    // Make sure we are not about to divide by zero.
    // we only need to look at the first digit since all numbers are left justified
    if(GetBCDNibble(x->sig,0)==0){ // x is the divisor
        resultSign=NOT_A_NUMBER;
        resultExp=0;
    } else {
        // copy the divisor significant digits into a double sized buffer
        // The reason is that we are going to be shifting it right as we go along
        memcpy(divisor,y->sig,BCD_DIGIT_BYTES);
        memset(&divisor[BCD_DIGIT_BYTES],0,BCD_DIGIT_BYTES); // zero to the end of the double buffer

        // copy the dividend into the remainder. At the begining the entire dvidend remains!
        // the remainder is also double sized so that we can subtract the divisor as we go along (they have to match)
        memcpy(remainder,x->sig,BCD_DIGIT_BYTES);
        memset(&remainder[BCD_DIGIT_BYTES],0,BCD_DIGIT_BYTES);// zero to the end of the double buffer

        // Note that both dividend and divisor are left justified and in scientific notation.
        // we can figure the sign and exponent of the result now.
        if(((x->sign)^(y->sign))==0) // both are the same sign
            resultSign = 0;
        else
            resultSign = DECIMAL_SIGN_NEGATIVE;

        resultExp = x->exp-y->exp; // exponent

        quotientIndex=0;
        while(quotientIndex<BCD_DIGIT_COUNT){
            // how many times can the divisor go into the remainder?
            // HINT: Its a number between 0 and 9
            quote = 0;
            while(BCDCompare(remainder, divisor,(2*BCD_DIGIT_COUNT))>=0){
                // subtract 1 divisor worth
                BCDSubtractor(remainder, remainder, divisor,(2*BCD_DIGIT_COUNT));
                quote++;
            }

            // we have the first digit of the quotient!
            SetBCDNibble(quotient,quote,quotientIndex);
            // now we shift the divisor right one place (divide by 10)
            ShiftSigNibbles(divisor,1,(2*BCD_DIGIT_COUNT));

            quotientIndex++;
        }

        // The quotient may have some leading zeros. we have to left justify to get
        // it back to scientific notation format.
        quotientIndex=0;
        zeros = 0;
        while(quotientIndex<BCD_DIGIT_COUNT){
            if(GetBCDNibble(quotient,quotientIndex)==0){
                quotientIndex++;
                zeros++;
            }else{
                quotientIndex = BCD_DIGIT_COUNT;
            }
        }
        // zeros contains the number of leading zeros
        if(zeros){
            ShiftSigNibbles(quotient,(0-zeros),(BCD_DIGIT_COUNT));
            resultExp--;
       } else {
            //resultExp++;
        }
    }

    // now copy the result
    res->sign = resultSign;
    res->exp = resultExp;
    memcpy(res->sig,quotient,BCD_DIGIT_BYTES);
}
