/*
 * calcTrig.c
 *
 *  Created on: Dec 23, 2020
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
#include <calcTrig.h>


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


/*    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **
 *  CalcRadToDegrees - converts radias to degrees
 *  INPUT:  *rad - pointer to radians inout
 *          *deg - pointer to degree result
 *
 *     2pi (radians) = 360 degrees
 *
 *  OUTPUT: none
*  **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    **    */
void    CalcRadToDegrees(decimal_t *deg,decimal_t *rad)
{
	const   decimal_t    dr180 = {0,1,{0x57, 0x29, 0x57, 0x79, 0x51, 0x30, 0x82, 0x30}  //
		};

	// degrees  = 360/(2pi) = radians* 180/pi =
	CalcMultiply(deg,(decimal_t*)&dr180,rad);

}

void    CalcDegreesToRad(decimal_t *rad,decimal_t *deg)
{

	const   decimal_t    dr180 = {0,-2,{0x17, 0x45, 0x32, 0x92, 0x51, 0x99, 0x43, 0x29}  //
		};

	// radians  = (degrees*(2pi)/360) = degrees* pi/180 =
	CalcMultiply(rad,(decimal_t*)&dr180,deg);

}

// calculating cos
// https://web.eecs.utk.edu/~azh/blog/cosine.html
