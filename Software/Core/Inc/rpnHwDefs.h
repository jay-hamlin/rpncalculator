/*
 * rpnHwDefs.h
 *
 *  Created on: Dec 3, 2020
 *      Author: jayhamlin
 */

#ifndef INC_RPNHWDEFS_H_
#define INC_RPNHWDEFS_H_

// signals to the MCU
// PORT A
#define	NC_PA0			GPIO_PIN_0
#define	NC_PA1			GPIO_PIN_1
#define	PIN_UART_TX		GPIO_PIN_2
#define	PIN_UART_RX		GPIO_PIN_3
#define	PIN_ADC4_POT	GPIO_PIN_4
#define	PIN_ADC5_USB	GPIO_PIN_5
#define	PIN_ADC6_BAT	GPIO_PIN_6
#define	NC_PA7			GPIO_PIN_7
#define	PIN_TP404		GPIO_PIN_8
#define	PIN_KEYBD_RST	GPIO_PIN_9
#define	PIN_KEYBD_INT	GPIO_PIN_10
#define	PIN_USB_DM		GPIO_PIN_11
#define	PIN_USB_DP		GPIO_PIN_12
#define	PIN_SWDIO		GPIO_PIN_13
#define	PIN_SWCLK		GPIO_PIN_14
#define	PIN_LED1		GPIO_PIN_15
// PORT B
#define	PIN_CHGR_PG		GPIO_PIN_0
#define	PIN_CHGR_STAT2	GPIO_PIN_1
#define	PIN_CHGR_STAT1	GPIO_PIN_2
#define	PIN_SPI_SCLK	GPIO_PIN_3
#define	PIN_SPI_MISO	GPIO_PIN_4
#define	PIN_SPI_MOSI	GPIO_PIN_5
#define	PIN_I2C1_SCL	GPIO_PIN_6
#define	PIN_I2C1_SDA	GPIO_PIN_7
#define	PIN_EM1_CS		GPIO_PIN_8
#define	PIN_EM2_CS		GPIO_PIN_9
#define	NC_PB10			GPIO_PIN_10
#define	NC_PB11			GPIO_PIN_11
#define	PIN_BUTTON_DN	GPIO_PIN_12
#define	PIN_I2C2_SCL	GPIO_PIN_13
#define	PIN_I2C2_SDA	GPIO_PIN_14
#define	PIN_PWR_HOLD	GPIO_PIN_15

// some useful macros
#define POWER_SUPPLY_HOLD(a) 	HAL_GPIO_WritePin(GPIOB,PIN_PWR_HOLD,a)
#define KEYPAD_RESET(a)			HAL_GPIO_WritePin(GPIOA,PIN_KEYBD_RST,(a^1))  // bit flip to make positive true
#define LED(a) 					HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15, (a^1)) // bit flip to make positive true

#define READ_PUSHBUTTON()		HAL_GPIO_ReadPin(GPIOB, PIN_BUTTON_DN) // button is positive true
#define READ_CHARGER_STATUS(a)	HAL_GPIO_ReadPin(GPIOB, a) // PIN_CHGR_PG,PIN_CHGR_STAT1,PIN_CHGR_STAT2

// ADC channels -
//		The values in adcValues are updated once per second
#define		ADC_CHANNEL_COUNT		5
#define		ADC_INDEX_POT			0
#define		ADC_INDEX_USB			1
#define		ADC_INDEX_BATT			2
#define		ADC_INDEX_TEMP			3
#define		ADC_INDEX_VREF			4



#endif /* INC_RPNHWDEFS_H_ */
