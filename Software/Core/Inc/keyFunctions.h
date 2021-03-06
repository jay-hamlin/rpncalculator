/*
 * keyFunctions.h
 *
 *  Created on: Dec 13, 2020
 *      Author: jayhamlin
 */

#ifndef INC_KEYFUNCTIONS_H_
#define INC_KEYFUNCTIONS_H_

void	DoKeyFunction(uint8_t keycode);
void	DoShiftedFunction(uint8_t keycode);
void	DoDSPKeyFunction(uint8_t keycode);

extern	uint8_t	functionPending;
extern	uint8_t	dspModePending;

#endif /* INC_KEYFUNCTIONS_H_ */
