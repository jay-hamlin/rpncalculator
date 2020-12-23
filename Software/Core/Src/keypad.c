/*
 * keypad.c
 *
 *  Created on: Nov 30, 2020
 *      Author: jayhamlin
 */

#include "main.h"
#include <stdio.h>
#include "rpnHwDefs.h"
#include "keypad.h"

#define	KEY_QUEUE_SIZE	16
uint8_t	keycodequeue[KEY_QUEUE_SIZE];
int16_t	queueHead;
int16_t	queueTail;
char	prevPushbutton;

uint8_t		gKeyInterrupt;

void KeypadISRHandler(void);
HAL_StatusTypeDef	ReadTCA9418Register(uint8_t reg, uint8_t *value);
uint8_t	PopKeyEventQueue(void);
void	PushKeyEventQueue(uint8_t	value);

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	KeyboardInitialize - call from main to set up the TCA8418 keypad scanner.
 *  INPUT: none
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	KeyboardInitialize(void)
{
	uint16_t 	size;
	uint8_t 	pData[4];

	gKeyInterrupt = 0;
	prevPushbutton=0;

	queueHead=0;
	queueTail=0;

	KEYPAD_RESET(0); 		 // release reset on keypad

	HAL_Delay(2); // wait 2 ms

    // setup configuration register  tca8418 datasheet page 28
    // 1 auto increment ON
    // 0 GPI tracked when locked
    // 0 overflow OFF
    // 0 INT remains asserted
    // 0 overflw INT disabled
    // 0 LOCK disabled
    // 0 GPI disabled
    // 1 Key Events INT enabled
    pData[0] = TCA_REG_CFG; //
    pData[1] = 0x99;	//0x81; //
    size = 2;
    HAL_I2C_Master_Transmit(&hi2c1, TCA8418_ADDR, pData, size, 1000);

    // setup which pins are keys.  tca8418 datasheet page 32
    // we use COLs 0-6 and ROWs 0-4
	pData[0] = TCA_REG_KP_GPIO1; // register
	pData[1] = 0x1F; // TCA_REG_KP_GPIO1 --> 00011111
	size = 2;
	HAL_I2C_Master_Transmit(&hi2c1, TCA8418_ADDR, pData, size, 1000);

	pData[0] = TCA_REG_KP_GPIO2; // register
	pData[1] = 0x7F; // TCA_REG_KP_GPIO2 --> 01111111

	size = 2;
	HAL_I2C_Master_Transmit(&hi2c1, TCA8418_ADDR, pData, size, 1000);

	pData[0] = TCA_REG_KP_GPIO3; // register
	pData[1] = 0x00; // TCA_REG_KP_GPIO3 --> 00000000
	size = 2;
	HAL_I2C_Master_Transmit(&hi2c1, TCA8418_ADDR, pData, size, 1000);

	uint8_t value;

	ReadTCA9418Register(TCA_REG_CFG, &value);

}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	KeypadISRHandler - called from stm32f0xx_it.c on falling edge of PA10
 * 	PA10 == KEYBD_INT from tca8418
 *  INPUT: none
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void KeypadISRHandler(void)
{
	gKeyInterrupt = 1;
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	ReadKeypad - called periodically from our main loop
 * 	Checks for a keypad interrupt then reads key events and stuffs the key codes into a queue
 *  INPUT: none
 *  OUTPUT: returns true if there is new data
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
uint8_t	ReadKeypad(void)
{
	HAL_StatusTypeDef ret;
	uint8_t 	pData[8];
	uint8_t value;


	if(READ_PUSHBUTTON()){
		if(prevPushbutton!=1){
			prevPushbutton = 1;
	    	value = (0x80|KEY_PWR_ON_OFF); // button down
	    	PushKeyEventQueue(value);
		}

	} else {
		if(prevPushbutton!=2){
			prevPushbutton = 2;
	    	value = KEY_PWR_ON_OFF;// button up
	    	PushKeyEventQueue(value);
		}
	}

	if(gKeyInterrupt){ // something happened


		ret = ReadTCA9418Register(TCA_REG_INT_STAT, &value); // read status to determine if a K_INT was set
		if(ret==HAL_OK){
			if(value&0x01){	// K_INT is bit 0
				value=0;
				ret = ReadTCA9418Register(TCA_REG_LCK_EC, &value);
				while((value)&&(ret==HAL_OK)){ // read key codes until zero
					ret = ReadTCA9418Register(TCA_REG_EVENT_A, &value);
					PushKeyEventQueue(value);
				}
			}
			gKeyInterrupt=0;
			// write a '1' to the K_INT bit to reset the INT.
			pData[0] = TCA_REG_INT_STAT; // register
			pData[1] = 0x01; // K_INT bit
			HAL_I2C_Master_Transmit(&hi2c1, TCA8418_ADDR, pData, 2, 1000);
		}
	}
	// see if there is data in the queue
	if(queueHead != queueTail){
		value =PopKeyEventQueue();
	} else {
		value = 0;
	}


	return(value);
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	ReadTCA9418Register - reads a 1 byte register from the TCA8414
 *  INPUT: register number and pointer to byte value
 *  OUTPUT: HAL status
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
HAL_StatusTypeDef	ReadTCA9418Register(uint8_t reg, uint8_t *value)
{
	uint8_t 	pData[8];
	HAL_StatusTypeDef ret;

	pData[0] = reg; // register
	HAL_I2C_Master_Transmit(&hi2c1, TCA8418_ADDR, pData, 1, 1000);


	ret = HAL_I2C_Master_Receive(&hi2c1, TCA8418_ADDR, pData, 1, 1000);
	if ( ret != HAL_OK ) {
	        // error
	} else {
		 (*value) = pData[0];
	}

	return(ret);
}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	PushKeyEventQueue - reads a 1 byte register from the TCA8414
 *  INPUT: value (keycode) to stuff onto the queue
 *  OUTPUT: none
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
void	PushKeyEventQueue(uint8_t	value)
{
	keycodequeue[queueHead]=value;
	queueHead++;
	if(queueHead==KEY_QUEUE_SIZE)
		queueHead=0;

	if(queueHead == queueTail){
		// the queue is now full

	}

}

/*	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**
 * 	PopKeyEventQueue - reads a 1 byte register from the TCA8414
 *  INPUT:  none
 *  OUTPUT: value (keycode) from the queue or zero if queue is empty
 *  **	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	**	*/
uint8_t	PopKeyEventQueue(void)
{
	uint8_t value;

	if(queueHead == queueTail){
		// the queue is empty
		value =0;
	} else {
		value = keycodequeue[queueTail];
		queueTail++;
		if(queueTail==KEY_QUEUE_SIZE)
			queueTail=0;
	}

	return(value);
}
